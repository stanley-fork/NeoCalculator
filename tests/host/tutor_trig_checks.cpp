// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/giac/GiacEngine.h"
#include "math/MathEvaluator.h"
#include "math/tutor/TeachingPlan.h"
#include <iostream>
#include <functional>
#include <stdexcept>
bool setting_complex_enabled=false;
using namespace numos;using namespace numos::tutor;
unsigned checks=0,mutations=0;
void require(bool x,const char* why){++checks;if(!x)throw std::runtime_error(why);}
Derivation solve(std::string left,std::string right,bool degrees=false){
    vpam::g_angleMode=degrees?vpam::AngleMode::DEG:vpam::AngleMode::RAD;
    auto& e=GiacEngine::instance();Snapshot s;s.authored={{left,right}};s.variables={"x"};s.inputEpoch=71;s.degrees=degrees;
    auto answer=e.solveStructured({left,right},"x",SolveDomainPolicy::RealOnly);return e.explainEquations(s,answer);
}
void reject(const char* name,const Derivation& original,std::function<void(Derivation&)> change){
    auto d=original;change(d);for(auto& s:d.states)s.fingerprint=fingerprint(s);
    auto v=GiacEngine::instance().verifyDerivation(d,d.input);
    require(v!=Verdict::Verified,name);++mutations;std::cout<<"mutation|"<<name<<'|'<<unsigned(v)<<'\n';
}
int main(){try{
    auto& engine=GiacEngine::instance();require(engine.begin(),"begin");require(validateCatalogs(),"catalog");
    for(bool degrees:{false,true})for(const char* function:{"sin","cos","tan"})for(const char* target:{"0","1","-1","1/2","-1/2","1/3","2"}){
        auto d=solve(std::string(function)+"(x)",target,degrees);
        require(d.status==Status::Complete,d.diagnostic.c_str());require(engine.verifyDerivation(d,d.input)==Verdict::Verified,"replay");
        const auto& s=d.states.back();const bool impossible=std::string(function)!="tan"&&std::string(target)=="2";
        require(s.conclusion==(impossible?Conclusion::Empty:Conclusion::Periodic),"range conclusion");
        require(s.families.size()<3,"bounded union");
        for(const auto& f:s.families){require(f.domain==IntegerDomain::AllIntegers&&f.binderId==1&&f.binderScope==fingerprint(d.input),"bound integer");require(f.degrees==degrees,"unit");require(f.originalCheck==Verdict::Verified,"universal validation");}
        auto stale=d.input;stale.degrees=!stale.degrees;require(engine.verifyDerivation(d,stale)==Verdict::Rejected,"angle snapshot");
        stale=d.input;++stale.inputEpoch;require(engine.verifyDerivation(d,stale)==Verdict::Rejected,"input epoch snapshot");
        stale=d.input;++stale.engineGeneration;require(engine.verifyDerivation(d,stale)==Verdict::Rejected,"engine generation snapshot");
        vpam::g_angleMode=degrees?vpam::AngleMode::RAD:vpam::AngleMode::DEG;
        require(!engine.tutorSnapshotCurrent(d.input,71,false),"angle change invalidates reopening");
    }
    auto sine=solve("sin(3*x-1)","1/3"),cosine=solve("cos(x)","1/2"),tangent=solve("tan(3*x)","1"),endpoint=solve("sin(x)","1"),negative=solve("sin(-2*x+1)","1/2"),deg=solve("sin(2*x)","1/2",true);
    for(const auto* d:{&sine,&cosine,&tangent,&endpoint,&negative,&deg}){
        require(d->status==Status::Complete,"mutation fixtures");
        const auto original=replayJson(*d);
        for(auto locale:{Locale::English,Locale::Spanish,Locale::French,Locale::Pseudo}){
            for(const auto& s:d->steps)require(validateMessage(s.explanation,s.parameters),"typed parameters");
            require(!replayJson(*d,locale).empty(),"locale projection");
        }require(replayJson(*d)==original,"immutable graph");
        reject("period",*d,[](auto& d){d.states.back().families[0].period="123";});
        reject("offset",*d,[](auto& d){d.states.back().families[0].offset="99";});
        reject("missing family",*d,[](auto& d){d.states.back().families.pop_back();});
        reject("spurious family",*d,[](auto& d){d.states.back().families.push_back(d.states.back().families[0]);});
        reject("binder scope",*d,[](auto& d){for(auto& s:d.states)for(auto& f:s.families)++f.binderScope;});
        reject("binder identity",*d,[](auto& d){for(auto& s:d.states)for(auto& f:s.families)f.binderId=0;});
        reject("missing integer domain",*d,[](auto& d){for(auto& s:d.states)for(auto& f:s.families)f.domain=static_cast<IntegerDomain>(9);});
        reject("branch identity swap",*d,[](auto& d){for(auto& s:d.states)for(auto& f:s.families)f.theoremBranch^=1;});
        reject("wrong theorem provenance",*d,[](auto& d){for(auto& s:d.states)for(auto& f:s.families)++f.theoremStep;});
        reject("degree mismatch",*d,[](auto& d){for(auto& s:d.states)for(auto& f:s.families)f.degrees=!f.degrees;});
        reject("finite samples not proof",*d,[](auto& d){d.states.back().conclusion=Conclusion::Finite;});
        reject("false Complete prefix",*d,[](auto& d){d.steps.pop_back();d.states.pop_back();});
        reject("original verification missing",*d,[](auto& d){d.states.back().families[0].originalCheck=Verdict::Unknown;});
        for(size_t i=0;i<d->steps.size();++i){
            reject("wrong operand",*d,[i](auto& x){x.steps[i].operand="19";});
            reject("wrong relation",*d,[i](auto& x){x.steps[i].relation=Relation::Approximation;});
        }
    }
    require(endpoint.states.back().families.size()==1,"endpoint deduplication");
    require(solve("cos(x)","-1").states.back().families.size()==1,"cos endpoint exact integer shift");
    require(solve("sin(x)","0").states.back().families.size()==2,"no unproved period compression");
    auto impossible=solve("sin(x)","2");reject("false range acceptance",impossible,[](auto& d){d.steps[0].rule=Rule::TrigRange;});
    reject("wrong principal inverse",sine,[](auto& d){d.steps[1].auxiliaries={"0"};});
    reject("lost tangent pole condition",tangent,[](auto& d){d.states.back().conditions.clear();});
    reject("wrong supplementary angle",sine,[](auto& d){d.states[3].families[1].offset="pi+asin(1/3)";});
    reject("duplicate endpoint retained",endpoint,[](auto& d){d.states.back().families.push_back(d.states.back().families[0]);});
    reject("sine half-period instead of full turn",sine,[](auto& d){d.states[3].families[0].period="pi";});
    reject("tangent full-period loses odd solutions",tangent,[](auto& d){d.states[3].families[0].period="2*pi";});
    reject("wrong principal acos",cosine,[](auto& d){d.steps[1].auxiliaries={"pi/6"};});
    reject("false no-solution for valid sine",sine,[](auto& d){d.states.back().conclusion=Conclusion::Empty;});
    reject("wrong degree period",deg,[](auto& d){d.states[3].families[0].period="180";});
    reject("binder replaced by stored variable",sine,[](auto& d){for(auto& s:d.states)for(auto& f:s.families)f.variable="A";});
    reject("wrong negative-period normalization",negative,[](auto& d){d.states.back().families[0].period="-pi";});
    for(const auto rule:{Rule::FamilyShift,Rule::FamilyDivide}) {
        for(size_t i=0;i<sine.steps.size();++i)if(sine.steps[i].rule==rule){
            reject("affine offset alone changed",sine,[i](auto& d){d.states[d.steps[i].after].families[0].offset="0";});
            reject("affine period alone changed",sine,[i](auto& d){d.states[d.steps[i].after].families[0].period="pi";});
        }
    }
    engine.evaluate("k:=27");auto collision=solve("sin(x)","1/2");require(collision.status==Status::Complete,"stored k collision");require(engine.evaluate("k").exactText=="27","binder did not assign user k");engine.evaluate("purge(k)");
    engine.evaluate("A:=3");auto stored=solve("sin(A*x-1)","1/3");require(stored.status==Status::Complete,"stored rational");engine.evaluate("A:=2");require(!engine.tutorSnapshotCurrent(stored.input,71,false),"stored invalidation");engine.evaluate("purge(A)");
    for(const char* lhs:{"sin(x)+cos(x)","sin(x)*sin(x)","sin(sin(x))","sin(x^2)","asin(x)","sin(x)/x","sin(0*x)"})require(solve(lhs,"1/2").status==Status::Unsupported,"unsupported admission");
    // Authored target domains must be checked before zero/cancellation hides them.
    for(bool degrees:{false,true})for(const char* rhs:{"0*x/x","x/x","0*ln(-1)","0*sqrt(-1)","(x-1)/(x-1)"}){
        require(solve("sin(x)",rhs,degrees).status==Status::Unsupported,"domain-bearing target admission");
        require(solve(rhs,"sin(x)",degrees).status==Status::Unsupported,"reversed domain-bearing target admission");
    }
    require(solve("sin(x*x/x)","0").status==Status::Unsupported,"argument cancellation retains domain");
    require(solve("sin(x)","(1+1)/4").status==Status::Complete,"constant arithmetic target");
    engine.evaluate("A:=0");require(solve("sin(x)","A*x/x").status==Status::Unsupported,"stored value cannot erase target domain");
    require(solve("sin(x+A*x/x)","0").status==Status::Unsupported,"stored zero cannot erase argument domain");
    require(solve("sin(x+0/A)","0").status==Status::Unsupported,"undefined bound argument is not a trig node");
    require(solve("sin(x+1/A)","0").status==Status::Unsupported,"infinite bound argument is not a trig node");
    require(solve("sin(x)","0/A").status==Status::Unsupported,"stored zero denominator");
    engine.evaluate("A:=1/2");require(solve("sin(x)","A").status==Status::Complete,"stored rational target");engine.evaluate("purge(A)");
    auto request=sine.input;request.complex=true;auto answer=engine.solveStructured({"sin(3*x-1)","1/3"},"x",SolveDomainPolicy::RealAndComplex);require(engine.explainEquations(request,answer).status==Status::Unsupported,"complex refusal");
    std::cout<<checks<<" checks; "<<mutations<<" mutations; sizeof family="<<sizeof(PeriodicFamily)<<'\n';return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
