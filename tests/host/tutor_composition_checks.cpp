// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/giac/GiacEngine.h"
#include "math/tutor/TeachingPlan.h"
#include "math/MathEvaluator.h"
#include <iostream>
#include <functional>
bool setting_complex_enabled=false;
using namespace numos;using namespace numos::tutor;
unsigned checks=0,failed=0;
void require(bool value,const char* name){++checks;if(!value){++failed;std::cerr<<"FAIL "<<name<<'\n';}}
Derivation proof(const std::string& text){auto& engine=GiacEngine::instance();const auto split=text.find('=');
 Snapshot input;input.authored={{text.substr(0,split),text.substr(split+1)}};input.variables={"x"};input.inputEpoch=7;input.engineGeneration=engine.generation();
 auto ordinary=engine.solveStructured({input.authored[0].lhs,input.authored[0].rhs},"x",SolveDomainPolicy::RealOnly);
 auto d=engine.explainEquations(input,ordinary);require(d.status==Status::Complete,text.c_str());
 if(d.status==Status::Complete)require(engine.verifyDerivation(d,d.input)==Verdict::Verified,"independent replay");return d;
}
Derivation copy(const Derivation& d){Derivation m=d;if(d.composition)m.composition=std::make_shared<Composition>(*d.composition);return m;}
void refresh(Derivation& d){for(auto& state:d.states)state.fingerprint=fingerprint(state);if(d.composition)for(auto& child:d.composition->children)refresh(child);}
void mutation(const Derivation& d,const char* name,std::function<void(Derivation&)> change){auto m=copy(d);change(m);refresh(m);require(GiacEngine::instance().verifyDerivation(m,m.input)!=Verdict::Verified,name);}
int main(){auto& e=GiacEngine::instance();e.begin();
 auto square=proof("x^4-5*x^2+4=0"),exp=proof("exp(2*x)-3*exp(x)+2=0"),trig=proof("6*sin(x)^2-5*sin(x)+1=0");
 if(!square.composition||!exp.composition||!trig.composition)return 2;
 mutation(square,"changed substitution",[](auto& d){d.composition->definition="(x+1)^2";});
 mutation(square,"changed coefficient",[](auto& d){d.composition->coefficients[0]="5";});
 mutation(square,"lost range",[](auto& d){d.composition->auxiliaryConditions.clear();});
 mutation(square,"wrong auxiliary scope",[](auto& d){++d.composition->scope;});
 mutation(square,"wrong child scope",[](auto& d){++d.composition->children[1].input.parentScope;});
 mutation(square,"auxiliary confused with original unknown",[](auto& d){d.composition->children[0].input.variables={"x"};});
 mutation(square,"missing auxiliary root",[](auto& d){d.composition->preimages.pop_back();});
 mutation(square,"wrong pullback",[](auto& d){d.composition->preimages[1].value=d.composition->preimages[0].value;});
 mutation(square,"missing preimage sign",[](auto& d){d.composition->children[1].states.back().branches.pop_back();});
 mutation(square,"candidate hidden pole",[](auto& d){d.composition->children[1].states.back().branches[0].equations[0].rhs="1+0/(x-1)";});
 mutation(square,"candidate hidden invalid function",[](auto& d){d.composition->children[1].states.back().branches[0].equations[0].rhs="1+0*ln(-1)";});
 mutation(square,"false Complete",[](auto& d){d.steps.pop_back();d.states.pop_back();});
 mutation(square,"aggregate child step budget cannot be bypassed",[](auto& d){auto& steps=d.composition->children[0].steps;while(steps.size()<=Limits::steps)steps.push_back(steps.front());});
 mutation(square,"aggregate retained payload cannot be bypassed",[](auto& d){d.composition->children[0].diagnostic.assign(Limits::retainedBytes,'q');});
 mutation(square,"false child Verified",[](auto& d){d.composition->children[0].steps[0].verification=Verdict::Unknown;});
 mutation(square,"wrong parent message",[](auto& d){d.steps[0].explanation=Message::Finish;});
 mutation(square,"wrong parent provenance",[](auto& d){d.steps[0].affected[0].side=0;});
 mutation(trig,"missing periodic family",[](auto& d){d.states.back().families.pop_back();});
 mutation(trig,"wrong period",[](auto& d){d.states.back().families[0].period="pi";});
 mutation(trig,"wrong binder",[](auto& d){++d.states.back().families[0].binderScope;});
 mutation(trig,"child RAD DEG mix",[](auto& d){d.composition->children[1].input.degrees=true;});
 mutation(trig,"child offset divided without period",[](auto& d){d.composition->children[1].states.back().families[0].offset="pi/12";});
 mutation(trig,"sine target outside range accepted",[](auto& d){d.composition->preimages[0].value="2";});
 mutation(exp,"double exponent rewritten as first power",[](auto& d){d.composition->coefficients[2]="0";d.composition->coefficients[1]="-2";});
 mutation(exp,"wrong child explanation operand",[](auto& d){auto& child=d.composition->children[1];for(auto& s:child.steps)if(!s.operand.empty()){s.operand="17";break;}});
 auto rejected=proof("exp(2*x)+exp(x)=0");
 mutation(rejected,"range rejection removed",[](auto& d){d.composition->preimages[0].range=Verdict::Verified;});
 mutation(rejected,"zero admitted as exponential value",[](auto& d){for(auto& p:d.composition->preimages)if(p.value=="0")p.range=Verdict::Verified;});
 auto logarithm=proof("ln(x-1)^2-3*ln(x-1)+2=0");
 mutation(logarithm,"original restriction removed",[](auto& d){d.composition->originalConditions.clear();});
 mutation(logarithm,"condition provenance changed",[](auto& d){d.composition->originalConditions[0].source.side=1;});
 mutation(logarithm,"false positive logarithm range",[](auto& d){Condition c;c.nonzero="t";c.kind=ConditionKind::Positive;c.role=ConditionRole::AuxiliaryRange;c.verification=Verdict::Verified;d.composition->auxiliaryConditions.push_back(c);});
 auto tangent=proof("tan(2*x)^2=3");
 mutation(tangent,"composed tangent pole lost",[](auto& d){d.composition->originalConditions.clear();});
 // Deliberate cycle must be rejected BEFORE recursive retained-byte accounting.
 auto cycle=copy(square);cycle.composition->children[0].composition=cycle.composition;
 require(e.verifyDerivation(cycle,cycle.input)!=Verdict::Verified,"nested/cyclic composition");cycle.composition->children[0].composition.reset();
 auto child=square.composition->children[1];child.input.authored[0].lhs="x^4";child.states.front().branches[0].equations=child.input.authored;refresh(child);
 require(e.verifyDerivation(child,child.input)!=Verdict::Verified,"square theorem cannot accept fourth power");
 auto stale=square.input;++stale.inputEpoch;require(e.verifyDerivation(square,stale)==Verdict::Rejected,"stale epoch");++stale.engineGeneration;require(!e.tutorSnapshotCurrent(stale,stale.inputEpoch,false),"stale generation");
 for(const auto text:{"(x^4-5*x^2+4)/(x-1)=0","sin(x)=0*x/x","-6*2^x+(2+0*ln(x))^(2*x)+8=0","(2+0*ln(x))^(2*x)-6*2^x+8=0","ln(x)^2-ln(x)^2=0","sin(x^2)^2=1","sin(x)+cos(x)=0"}){
  const std::string value=text;const auto split=value.find('=');Snapshot s;s.authored={{value.substr(0,split),value.substr(split+1)}};s.variables={"x"};
  auto answer=e.solveStructured({s.authored[0].lhs,s.authored[0].rhs},"x",SolveDomainPolicy::RealOnly);auto d=e.explainEquations(s,answer);require(d.status==Status::Unsupported,text);
 }
 for(const auto name:{"t","u","k","n_0"}){e.assign(name,"37");auto d=proof("exp(2*x)-3*exp(x)+2=0");require(e.evaluate(name).exactText=="37","user variable unchanged");}
 const auto pages=teachingPageCount(trig,true);bool lastPair=false,aux=false;
 for(unsigned i=0;i<pages;++i){auto p=teachingPageAt(trig,true,i);lastPair|=p.section==1;aux|=p.child==1;auto q=teachingPageAt(trig,false,teachingPageFor(trig,false,p));require(q.child==p.child&&q.section==p.section,"guided summary provenance");}
 require(lastPair&&aux,"all four families and auxiliary pages");
 // A valid but deliberately repetitive authored tree exhausts the shared
 // symbolic budget. Ordinary Giac still solves it; no partial proof is Complete.
 e.assign("A","1/2");Snapshot budget;budget.variables={"x"};budget.inputEpoch=91;
 const std::string atom="logb(A*x+1,2)";
 budget.authored={{atom+"^2+"+atom+"^2+"+atom+"^2+"+atom+"^2-12*"+atom+"+8","0"}};
 auto ordinary=e.solveStructured({budget.authored[0].lhs,"0"},"x",SolveDomainPolicy::RealOnly);
 auto exhausted=e.explainEquations(budget,ordinary);
 require(ordinary.ok()&&exhausted.status==Status::Partial&&exhausted.metrics.symbolicCalls>=Limits::symbolicCalls,
         "symbolic exhaustion is Partial and preserves ordinary answer");
 e.assign("A","2");require(proof("x^4-5*x^2+4=0").status==Status::Complete,"healthy after exhausted construction");
 std::cout<<"composition checks="<<checks<<" failed="<<failed<<'\n';return failed?1:0;
}
