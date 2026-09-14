// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/giac/GiacEngine.h"
#include "math/tutor/TeachingPlan.h"
#include <iostream>
#include <functional>
#include <stdexcept>
bool setting_complex_enabled=false;
using namespace numos;
using namespace numos::tutor;
unsigned checks=0,mutations=0;
void require(bool b,const char* why){++checks;if(!b)throw std::runtime_error(why);}
Derivation solve(const std::string& lhs,const std::string& rhs,bool complex=false) {
    auto& engine=GiacEngine::instance();Snapshot s;s.variables={"x"};s.authored={{lhs,rhs}};s.inputEpoch=17;s.complex=complex;
    auto answer=engine.solveStructured({lhs,rhs},"x",complex?SolveDomainPolicy::RealAndComplex:SolveDomainPolicy::RealOnly);
    return engine.explainEquations(s,answer);
}
size_t rule(const Derivation& d,Rule id) {
    for(size_t i=0;i<d.steps.size();++i)if(d.steps[i].rule==id)return i;
    throw std::runtime_error("missing mutation target rule");
}
void reject(const char* name,const Derivation& original,const std::function<void(Derivation&)>& mutation) {
    auto changed=original;mutation(changed);
    // Recompute fingerprints: mutation rejection must inspect the theorem,
    // operands, conditions and candidate checks, not just stale hash fields.
    for(auto& s:changed.states)s.fingerprint=fingerprint(s);
    const auto verdict=GiacEngine::instance().verifyDerivation(changed,changed.input);
    std::cout<<"mutation|"<<name<<"|"<<unsigned(verdict)<<'\n';
    require(verdict==Verdict::Rejected,name);++mutations;
}
int main() {try {
    auto& engine=GiacEngine::instance();require(engine.begin(),"engine begin");require(validateCatalogs(),"catalog schemas");
    const auto abs=solve("abs(2*x-3)","5"),zero=solve("abs(x)","0"),rad=solve("sqrt(x+1)","x-1");
    for(const auto* d:{&abs,&zero,&rad}) {
        require(d->status==Status::Complete,"acceptance Complete");
        require(engine.verifyDerivation(*d,d->input)==Verdict::Verified,"independent replay");
        const auto hash=fingerprint(d->states.back());
        for(auto locale:{Locale::English,Locale::Spanish,Locale::French,Locale::Pseudo}) {
            auto json=replayJson(*d,locale);require(!json.empty(),"localized replay");
            require(fingerprint(d->states.back())==hash,"locale invariant graph");
            for(const auto& step:d->steps)require(validateMessage(step.explanation,step.parameters),"typed message");
        }
        for(bool guided:{false,true}) {
            unsigned seen=0;
            visitTeachingPages(*d,guided,[&](unsigned,TeachingPage p){require(p.first<=p.last&&p.last<d->steps.size(),"projection provenance");++seen;});
            require(seen>0,"nonempty pages");
        }
    }
    reject("abs missing nonnegative requirement",abs,[](auto& d){auto i=rule(d,Rule::RangeCondition);d.states[d.steps[i].after].conditions.clear();d.steps[i].introduced.clear();});
    reject("abs missing sign branch",abs,[](auto& d){auto i=rule(d,Rule::AbsCases);d.states[d.steps[i].after].branches.pop_back();});
    reject("abs repeats positive branch",abs,[](auto& d){auto i=rule(d,Rule::AbsCases);auto& b=d.states[d.steps[i].after].branches;b[1].equations=b[0].equations;});
    reject("abs wrong negative rhs",abs,[](auto& d){auto i=rule(d,Rule::AbsCases);d.states[d.steps[i].after].branches[1].equations[0].rhs="-4";});
    reject("abs duplicated zero",zero,[](auto& d){auto i=rule(d,Rule::AbsCases);auto& b=d.states[d.steps[i].after].branches;b.push_back(b[0]);b.back().id=2;b.back().origin=2;});
    reject("abs invalid original candidate",abs,[](auto& d){auto i=rule(d,Rule::CheckOriginal);d.states[d.steps[i].before].branches[d.steps[i].branch].equations[0].rhs="99";d.steps[i].operand="99";});
    reject("radical square only right",rad,[](auto& d){auto i=rule(d,Rule::RadicalCandidates);d.states[d.steps[i].after].branches[0].equations[0].lhs="sqrt(x+1)";});
    reject("radical missing rhs square",rad,[](auto& d){auto i=rule(d,Rule::RadicalCandidates);d.states[d.steps[i].after].branches[0].equations[0].rhs="x-1";});
    reject("radical missing sign",rad,[](auto& d){auto i=rule(d,Rule::RangeCondition);d.states[d.steps[i].after].conditions.pop_back();d.steps[i].introduced.clear();});
    reject("radical unconditional equivalence",rad,[](auto& d){d.steps[rule(d,Rule::RadicalCandidates)].relation=Relation::Equivalent;});
    reject("radical retains extraneous candidate",rad,[](auto& d){auto i=rule(d,Rule::RejectOriginal);auto& b=d.states[d.steps[i].after].branches[d.steps[i].branch];b.status=BranchStatus::Active;b.originalCheck=Verdict::Verified;});
    reject("radical lies about sign rejection",rad,[](auto& d){auto i=rule(d,Rule::RejectOriginal);d.steps[i].auxiliaries={"radicand"};d.steps[i].explanation=Message::RadicandFails;});
    reject("radical rejects valid candidate",rad,[](auto& d){auto i=rule(d,Rule::CheckOriginal);auto& s=d.steps[i];s.rule=Rule::RejectOriginal;s.relation=Relation::Rejection;s.auxiliaries={"original"};s.explanation=Message::OriginalFails;auto& b=d.states[s.after].branches[s.branch];b.status=BranchStatus::Rejected;b.originalCheck=Verdict::Rejected;});
    reject("radical skips original check",rad,[](auto& d){auto i=rule(d,Rule::CheckOriginal);auto& s=d.steps[i];s.rule=Rule::Finish;s.explanation=Message::Finish;d.states[s.after].conclusion=Conclusion::Finite;});
    reject("wrong condition provenance",rad,[](auto& d){for(auto& state:d.states)for(auto& c:state.conditions)if(c.role==ConditionRole::Radicand)c.source.side=1;});
    reject("wrong condition relation",rad,[](auto& d){for(auto& state:d.states)for(auto& c:state.conditions)if(c.role==ConditionRole::Radicand)c.kind=ConditionKind::Nonzero;});
    reject("wrong typed condition tree",rad,[](auto& d){for(auto& state:d.states)for(auto& c:state.conditions)if(c.expression){auto tree=std::make_shared<EngineResultNode>(*c.expression);tree->kind=EngineNodeKind::Integer;tree->text="99";tree->children.clear();c.expression=tree;}});
    reject("case identity swap",abs,[](auto& d){auto i=rule(d,Rule::AbsCases);auto& b=d.states[d.steps[i].after].branches;std::swap(b[0].origin,b[1].origin);});
    reject("lifted primitive wrong case",abs,[](auto& d){auto i=rule(d,Rule::AddBoth);d.steps[i].caseBranch^=1;});
    reject("lifted balancing wrong sign",abs,[](auto& d){auto i=rule(d,Rule::AddBoth);d.steps[i].operand="-3";});
    reject("false Complete prefix",rad,[](auto& d){d.steps.resize(3);d.states.resize(4);d.status=Status::Complete;});
    auto stale=rad.input;++stale.inputEpoch;require(engine.verifyDerivation(rad,stale)==Verdict::Rejected,"stale input rejected");
    const auto duplicate=solve("abs(2*x)","x");require(duplicate.status==Status::Complete,"duplicate candidate family");
    require(duplicate.steps[rule(duplicate,Rule::DuplicateCandidate)].verification==Verdict::Verified,"duplicate checked");
    reject("duplicate counted twice",duplicate,[](auto& d){auto i=rule(d,Rule::DuplicateCandidate);d.states[d.steps[i].after].branches[d.steps[i].branch].status=BranchStatus::Active;});
    reject("duplicate rejection step removed entirely",duplicate,[](auto& d){
        const auto i=rule(d,Rule::DuplicateCandidate);const auto branch=d.steps[i].branch;
        d.states.erase(d.states.begin()+d.steps[i].after);d.steps.erase(d.steps.begin()+i);
        for(size_t j=i;j<d.steps.size();++j){--d.steps[j].before;--d.steps[j].after;}
        for(size_t j=i;j<d.states.size();++j)d.states[j].branches[branch].status=BranchStatus::Active;
    });
    for(const auto& e:std::initializer_list<Equation>{{"abs(abs(x))","3"},{"abs(x)+abs(x-1)","3"},{"sqrt(sqrt(x))","3"},{"sqrt(x)+sqrt(x-1)","3"},{"surd(x,3)","2"},{"sqrt(x^4+1)","x"},{"sqrt(x^2)","x"},{"abs(x)","x"},{"1/sqrt(x)","2"}})
        require(solve(e.lhs,e.rhs).status==Status::Unsupported,"unsupported boundary");
    require(solve("abs(x)","3",true).status==Status::Unsupported,"complex abs refusal");
    require(solve("sqrt(x)","3",true).status==Status::Unsupported,"complex radical refusal");
    require(solve("abs(x)>2","0").status==Status::Unsupported,"absolute inequality is not an equation method");
    require(solve("abs(x)*abs(x-1)","2").status==Status::Unsupported,"multiple absolute product refusal");
    require(solve("abs(x^3)","2").status==Status::Unsupported,"absolute branch degree limit");
    engine.evaluateStructured("A:=2");auto stored=solve("abs(A*x-3)","5");require(stored.status==Status::Complete,"stored rational abs");
    engine.evaluateStructured("B:=1/2");auto storedRoot=solve("sqrt(B*x+1)","3");require(storedRoot.status==Status::Complete,"stored rational radical");
    require(engine.evaluateStructured("B").base.exactText=="1/2","radical explanation preserves stored value");engine.evaluateStructured("purge(B)");
    require(engine.tutorSnapshotCurrent(stored.input,17,false),"stored snapshot current");engine.evaluateStructured("A:=3");require(!engine.tutorSnapshotCurrent(stored.input,17,false),"stored edit invalidation");engine.evaluateStructured("purge(A)");
    // Dedicated stale-context test; lifecycle/fault/latency loops do not reset.
    engine.reset();require(!engine.tutorSnapshotCurrent(rad.input,17,false),"radical stale engine generation");
    auto current=solve("sqrt(x+1)","x-1");
    require(engine.verifyDerivation(rad,current.input)==Verdict::Rejected,"old nonlinear proof rejected against current snapshot");
    std::cout<<"PASS checks="<<checks<<" mutations="<<mutations<<'\n';return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
