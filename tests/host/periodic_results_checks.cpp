// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/giac/GiacEngine.h"
#include "math/MathEvaluator.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
bool setting_complex_enabled=false;
using namespace numos;
unsigned checks=0;
void require(bool value,const char* why){++checks;if(!value)throw std::runtime_error(why);}
AffinePeriodicFamily family(const char* o,const char* p){
    AffinePeriodicFamily f;f.variable=f.lhs="x";f.offset=o;f.period=p;f.binderScope=7;return f;
}
int main(){try{
    auto& e=GiacEngine::instance();require(e.begin(),"begin");
    for(bool degrees:{false,true}){
        vpam::g_angleMode=degrees?vpam::AngleMode::DEG:vpam::AngleMode::RAD;
        for(const char* function:{"sin","cos","tan"})for(const char* target:{"0","1/2","1","-1","2","1/3"})for(const char* argument:{"x","2*x","3*x-1","-2*x+1"}){
            SolveEquation input{std::string(function)+"("+argument+")",target};
            auto answer=e.solveStructured(input,"x",SolveDomainPolicy::RealOnly);
            std::cout<<"case|"<<degrees<<'|'<<input.lhs<<'='<<target<<'|'<<unsigned(answer.status)<<'|'<<unsigned(answer.coverage)<<'|'<<answer.rawExactText<<'\n';
            require(answer.ok(),"ordinary status");require(answer.origin==SolveOrigin::GiacAllTrig,"ordinary independent producer");
            require(answer.coverage==SolveCoverage::PeriodicComplete,"complete conversion");
            require(e.periodicAnswerCurrent(answer),"current answer");
            const bool empty=std::string(function)!="tan"&&std::string(target)=="2";
            require(answer.setKind==(empty?SolutionSetKind::NoSolution:SolutionSetKind::Periodic),"result kind");
            require(answer.families.size()<=PeriodicLimits::families,"family cap");
            for(const auto& f:answer.families){require(f.binderScope==answer.binderScope&&f.binderId==1,"bound identity");require(f.degrees==degrees,"family units");require(f.periodValue.kind!=EngineNodeKind::Unsupported,"structured period");}
            require(answer.restrictions.size()==(std::string(function)=="tan"?1u:0u),"tangent condition");
            // Only now construct the independently checked tutor. Ordinary data
            // above was produced with no derivation in existence.
            tutor::Snapshot s;s.authored={{input.lhs,input.rhs}};s.variables={"x"};s.inputEpoch=31;s.degrees=degrees;
            auto d=e.explainEquations(s,answer);
            require(d.status==tutor::Status::Complete,d.diagnostic.c_str());
            require(e.verifyDerivation(d,d.input)==tutor::Verdict::Verified,"independent theorem replay");
            if(d.reconciliation!=tutor::Verdict::Verified){for(const auto& f:d.states.back().families)std::cout<<"tutor|"<<f.lhs<<'|'<<f.offset<<'|'<<f.period<<'\n';for(const auto& f:answer.families)std::cout<<"ordinary|"<<f.lhs<<'|'<<f.offset<<'|'<<f.period<<'\n';}
            require(d.reconciliation==tutor::Verdict::Verified,"set equality");
            auto again=e.solveStructured(input,"x",SolveDomainPolicy::RealOnly);
            require(again.rawExactText==answer.rawExactText,"counter restored and tutor independent");
        }
    }
    vpam::g_angleMode=vpam::AngleMode::RAD;
    auto cmp=[&](std::vector<AffinePeriodicFamily> a,std::vector<AffinePeriodicFamily> b,SetComparison expected,const char* why){require(e.comparePeriodicSets(a,b)==expected,why);};
    cmp({family("0","pi")},{family("0","2*pi"),family("pi","2*pi")},SetComparison::Equivalent,"sine zero residue proof");
    cmp({family("pi/6","2*pi")},{family("13*pi/6","2*pi")},SetComparison::Equivalent,"integer shift");
    cmp({family("pi/6","-2*pi")},{family("pi/6","2*pi")},SetComparison::Equivalent,"sign reparameterization");
    cmp({family("0","pi")},{family("0","2*pi")},SetComparison::Different,"wrong period");
    cmp({family("0","pi")},{family("pi/2","pi")},SetComparison::Different,"noninteger offset");
    cmp({family("0","pi")},{family("0","65*pi")},SetComparison::Unknown,"bounded expansion");
    cmp({family("0","pi")},{family("0","sqrt(2)")},SetComparison::Unknown,"unproved commensurability");
    cmp({family("0","pi")},{family("n_0","pi")},SetComparison::Unknown,"arbitrary identifier not binder");
    auto renamed=family("pi/6","2*pi");renamed.binderId=7;renamed.binderScope=991;
    cmp({family("pi/6","2*pi")},{renamed},SetComparison::Equivalent,"alpha-renamed binder");
    cmp({family("0","2*pi"),family("pi","2*pi")},{family("0","2*pi")},SetComparison::Different,"deleted family");
    cmp({family("pi","2*pi"),family("0","2*pi")},{family("0","pi")},SetComparison::Equivalent,"branch order irrelevant");
    cmp({family("0","pi"),family("0","pi"),family("0","pi")},{family("0","pi")},SetComparison::Unknown,"family cap cannot be bypassed");
    // Public entries synchronize the selected product angle before entering
    // the scoped request. Establish that expected boundary state first.
    e.evaluate("0");
    for(unsigned fault=1;fault<=13;++fault){
        const auto context=e.debugPeriodicContextState();
        e.debugPeriodicFault(fault);
        // Keep the fault active across a second attempt; neither may publish a
        // half-answer as a complete periodic set.
        for(unsigned attempt=0;attempt<2;++attempt){
            auto a=e.solveStructured({fault==5?"tan(x)":"sin(x)","1/2"},"x",SolveDomainPolicy::RealOnly);
            require(a.coverage!=SolveCoverage::PeriodicComplete,"corruption/failure not complete");
            require(a.families.empty(),"failed publication is transactional");
            require(e.debugPeriodicContextState()==context,"flags/quotes restored after failure");
        }
        e.debugPeriodicFault(0);
        require(e.solveStructured({"sin(x)","1/2"},"x",SolveDomainPolicy::RealOnly).coverage==SolveCoverage::PeriodicComplete,"healthy recovery without reset");
    }
    for(const char* rhs:{"0*x/x","A*x/x","0/(x-1)"}){
        e.evaluate("A:=0");auto a=e.solveStructured({"sin(x)",rhs},"x",SolveDomainPolicy::RealOnly);
        require(a.coverage!=SolveCoverage::PeriodicComplete,"authored domain exclusion");
    }
    for(const char* lhs:{"sin(x^2)","sin(x)+cos(x)"})require(e.solveStructured({lhs,"1/2"},"x",SolveDomainPolicy::RealOnly).coverage!=SolveCoverage::PeriodicComplete,"unsupported input");
    auto answer=e.solveStructured({"sin(x)","1/2"},"x",SolveDomainPolicy::RealOnly);
    vpam::g_angleMode=vpam::AngleMode::DEG;require(!e.periodicAnswerCurrent(answer),"stale angle");
    vpam::g_angleMode=vpam::AngleMode::RAD;require(e.periodicAnswerCurrent(answer),"restored angle");
    e.evaluate("k:=19");e.evaluate("n_0:=27");auto collision=e.solveStructured({"sin(x)","1/2"},"x",SolveDomainPolicy::RealOnly);
    require(collision.coverage==SolveCoverage::PeriodicComplete,"user generated-name collision");
    require(e.evaluate("k").exactText=="19"&&e.evaluate("n_0").exactText=="27","bindings untouched");
    require(e.comparePeriodicSets({collision.families.begin(),collision.families.end()},{answer.families.begin(),answer.families.end()})==SetComparison::Equivalent,"bound parameter not substituted");
    e.evaluate("A:=1/2");auto stored=e.solveStructured({"sin(x)","A"},"x",SolveDomainPolicy::RealOnly);
    require(stored.coverage==SolveCoverage::PeriodicComplete&&e.periodicAnswerCurrent(stored),"stored rational");
    e.evaluate("A:=1/3");require(!e.periodicAnswerCurrent(stored),"stale stored input");
    e.evaluate("x:=3");require(!e.periodicAnswerCurrent(answer),"stale solve-variable binding");
    e.reset();require(!e.periodicAnswerCurrent(answer),"engine generation invalidation");
    std::cout<<"PASS "<<checks<<" checks\n";return 0;
}catch(const std::exception& error){std::cerr<<"FAIL after "<<checks<<": "<<error.what()<<'\n';return 1;}}
