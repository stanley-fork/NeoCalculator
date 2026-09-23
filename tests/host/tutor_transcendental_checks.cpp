// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/giac/GiacEngine.h"
#include "math/tutor/TeachingPlan.h"
#include <functional>
#include <iostream>
#include <stdexcept>
bool setting_complex_enabled=false;
using namespace numos;using namespace numos::tutor;
unsigned checks=0,mutations=0;
void require(bool value,const char* name){++checks;if(!value)throw std::runtime_error(name);}
Derivation solve(const std::string& lhs,const std::string& rhs,bool complex=false){
    auto& engine=GiacEngine::instance();Snapshot s;s.variables={"x"};s.authored={{lhs,rhs}};s.inputEpoch=21;s.complex=complex;
    auto answer=engine.solveStructured({lhs,rhs},"x",complex?SolveDomainPolicy::RealAndComplex:SolveDomainPolicy::RealOnly);
    return engine.explainEquations(s,answer);
}
size_t rule(const Derivation& d,Rule id){for(size_t i=0;i<d.steps.size();++i)if(d.steps[i].rule==id)return i;throw std::runtime_error("missing mutation target");}
void reject(const char* name,const Derivation& original,const std::function<void(Derivation&)>& mutation){
    auto d=original;mutation(d);for(auto& state:d.states)state.fingerprint=fingerprint(state);
    auto v=GiacEngine::instance().verifyDerivation(d,d.input);
    std::cout<<"mutation|"<<name<<'|'<<unsigned(v)<<'\n';require(v==Verdict::Rejected,name);++mutations;
}
int main(){try{
    auto& engine=GiacEngine::instance();require(engine.begin(),"begin");require(validateCatalogs(),"catalog schemas");
    for(const auto& e:std::initializer_list<Equation>{{"2*exp(x)+1","7"},{"3*ln(x)-6","0"},{"-2*exp(x)+1","-5"}}) {
        const auto d=solve(e.lhs,e.rhs);const auto unchanged=replayJson(d);
        require(d.status==Status::Complete,"balancing caption fixture");
        for(const auto& s:d.steps)if(teachingNeedsOperand(s)) {
            const bool divide=s.rule==Rule::DivideBoth;
            const bool negative=s.operand.front()=='-';
            const auto amount=!divide && negative?s.operand.substr(1):s.operand;
            const std::string expected=divide?"Divide both sides by "+amount+".":
                negative?"Subtract "+amount+" from both sides.":"Add "+amount+" to both sides.";
            require(teachingOperationText(s,Locale::English)==expected,"checked sign/amount caption");
            for(auto locale:{Locale::Spanish,Locale::French,Locale::Pseudo})
                require(teachingOperationText(s,locale).find(amount)!=std::string::npos,"localized checked amount");
        }
        require(replayJson(d)==unchanged,"caption projection preserves every proof field");
    }
    require(!validateMessage(Message::ViewSubtractAmount,{{ParameterKind::Variable,"3"}}),"view operand typing");
    auto log=solve("ln(x-1)","2"),pair=solve("ln(x)","ln(5)"),exp=solve("exp(x)","5"),inject=solve("exp(2*x-1)","exp(3)"),power=solve("2^x","8"),base=solve("logb(x,2)","3"),empty=solve("exp(x)","-1");
    for(const auto& equation:std::initializer_list<Equation>{{"2^(x+0)","8"},{"exp(x+0)","5"},{"ln(x+0)","2"},{"logb(x+0,2)","3"},{"exp(2*x-x)","5"},{"ln(x+x-x)","2"}}){
        auto collected=solve(equation.lhs,equation.rhs);
        require(collected.status==Status::Complete,"authored affine identity completes");
        require(engine.verifyDerivation(collected,collected.input)==Verdict::Verified,"affine collection independently replayed");
        require(collected.states.back().branches[0].equations[0].lhs=="x","candidate is structurally isolated");
        require(collected.states.back().branches[0].originalCheck==Verdict::Verified,"collected candidate checked in original");
        const auto at=rule(collected,Rule::Collect);
        require(at<rule(collected,Rule::CheckOriginal),"collection precedes candidate validation");
        reject("wrong affine collection",collected,[at](auto& d){d.states[d.steps[at].after].branches[0].equations[0].rhs="-123";});
    }
    for(const auto* d:{&log,&pair,&exp,&inject,&power,&base,&empty}){
        require(d->status==Status::Complete,"Complete fixture");require(engine.verifyDerivation(*d,d->input)==Verdict::Verified,"independent replay");
        const auto hash=fingerprint(d->states.back());
        for(auto locale:{Locale::English,Locale::Spanish,Locale::French,Locale::Pseudo}){
            require(!replayJson(*d,locale).empty(),"locale replay");require(hash==fingerprint(d->states.back()),"locale graph invariance");
            for(const auto& step:d->steps)require(validateMessage(step.explanation,step.parameters),"typed message");
        }
        for(bool guided:{false,true}){unsigned pages=0;visitTeachingPages(*d,guided,[&](unsigned,TeachingPage p){require(p.first<=p.last&&p.last<d->steps.size(),"page provenance");++pages;});require(pages>0,"pages exist");}
    }
    reject("missing logarithm positivity",log,[](auto& d){auto i=rule(d,Rule::PositiveDomain);d.states[d.steps[i].after].conditions.clear();d.steps[i].introduced.clear();});
    reject("missing second log domain",pair,[](auto& d){auto& step=d.steps[1];d.states[step.after].conditions.pop_back();step.introduced.clear();});
    reject("log negative argument at injectivity",pair,[](auto& d){auto i=rule(d,Rule::LogInjective);d.states[d.steps[i].before].branches[0].equations[0].rhs="ln(-1)";d.steps[i].operand="ln(-1)";});
    reject("negative exponential target",exp,[](auto& d){auto i=rule(d,Rule::ExpInverse);d.states[d.steps[i].before].branches[0].equations[0].rhs="-5";d.steps[i].operand="-5";});
    reject("zero exponential target",exp,[](auto& d){auto i=rule(d,Rule::ExpInverse);d.states[d.steps[i].before].branches[0].equations[0].rhs="0";d.steps[i].operand="0";});
    for(const char* invalid:{"1","-2"})reject(invalid,power,[invalid](auto& d){auto i=rule(d,Rule::ExpInjective);d.states[d.steps[i].before].branches[0].equations={{std::string(invalid)+"^x",std::string(invalid)+"^3"}};d.steps[i].auxiliaries={invalid};});
    reject("wrong logarithm base",base,[](auto& d){d.steps[rule(d,Rule::LogInverse)].auxiliaries={"10"};});
    reject("negated injective exponent",inject,[](auto& d){auto i=rule(d,Rule::ExpInjective);d.states[d.steps[i].after].branches[0].equations[0].rhs="-3";});
    reject("wrong exact power exponent",power,[](auto& d){d.steps[rule(d,Rule::ExpExactPower)].operand="4";});
    reject("minimum integer exponent cannot overflow bound",power,[](auto& d){d.steps[rule(d,Rule::ExpExactPower)].operand="-2147483648";});
    reject("dropped final condition",log,[](auto& d){d.states.back().conditions.clear();});
    reject("negated inverse exponent",log,[](auto& d){auto i=rule(d,Rule::LogInverse);d.states[d.steps[i].after].branches[0].equations[0].rhs="exp(-2)";});
    reject("approximation replaces exact log",exp,[](auto& d){auto i=rule(d,Rule::ExpInverse);d.states[d.steps[i].after].branches[0].equations[0].rhs="1.6094379124341";});
    reject("wrong positivity provenance",log,[](auto& d){for(auto& state:d.states)for(auto& c:state.conditions)c.source.side=1;});
    reject("false Complete prefix",log,[](auto& d){d.steps.resize(2);d.states.resize(3);d.status=Status::Complete;});
    reject("skip original validation",exp,[](auto& d){auto i=rule(d,Rule::CheckOriginal);d.states[d.steps[i].after].branches[0].originalCheck=Verdict::Unknown;});
    reject("wrong explanatory operand",exp,[](auto& d){d.steps[rule(d,Rule::ExpInverse)].operand="7";});
    reject("wrong relation",inject,[](auto& d){d.steps[rule(d,Rule::ExpInjective)].relation=Relation::Candidates;});
    auto stale=log.input;++stale.inputEpoch;require(engine.verifyDerivation(log,stale)==Verdict::Rejected,"stale epoch");
    stale=log.input;++stale.engineGeneration;require(engine.verifyDerivation(log,stale)==Verdict::Rejected,"stale generation");
    // 03B deliberately adds this formerly unsupported quadratic composition.
    const auto composed=solve("exp(x)+exp(2*x)","3");
    require(composed.status==Status::Complete && composed.composition &&
            composed.states.back().branches.size()==1 &&
            engine.verifyDerivation(composed,composed.input)==Verdict::Verified,
            "quadratic exponential now has a checked complete preimage");
    for(const auto& e:std::initializer_list<Equation>{{"ln(x)","ln(-1)"},{"ln((x^2-1)/(x-1))","0"},{"exp((x^2-1)/(x-1))","5"},{"ln(x)+ln(x-1)","2"},{"ln(exp(x))","2"},{"exp(ln(x))","2"},{"x^x","4"},{"1^x","1"},{"(-2)^x","4"},{"exp(x)","x"},{"exp(x^3)","2"},{"ln(x^2)","2"},{"exp(x)","exp(x)"},{"logb(x,1)","2"}})
        require(solve(e.lhs,e.rhs).status==Status::Unsupported,"unsupported boundary");
    require(solve("exp(x)","2",true).status==Status::Unsupported,"complex exponential refusal");require(solve("ln(x)","2",true).status==Status::Unsupported,"complex logarithm refusal");
    engine.evaluateStructured("A:=2");auto stored=solve("exp(A*x)","1");require(stored.status==Status::Complete,"stored exact exponent");
    require(engine.tutorSnapshotCurrent(stored.input,21,false),"stored current");engine.evaluateStructured("A:=3");require(!engine.tutorSnapshotCurrent(stored.input,21,false),"stored invalidation");engine.evaluateStructured("purge(A)");
    std::cout<<checks<<" checks; "<<mutations<<" mutations\n";return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
