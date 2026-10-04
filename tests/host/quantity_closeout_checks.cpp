// Focused closeout boundaries; independent expectations, no new unit capability.
#include "math/CalculationEngine.h"
#include "math/VariableManager.h"
#include "math/Quantity.h"
#include "lvgl.h"
#include <cstdio>
#include <cstdlib>
using namespace vpam;
using namespace numos;
bool setting_complex_enabled=false;
static unsigned checks=0;
static void check(bool ok,const char* label){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",label);std::exit(1);}}
static NodePtr metre(){return makeUnit({units::UnitId(1),units::PrefixId(0)});}
static NodePtr op(OpKind kind){return makeOperator(kind);}
static void append(NodePtr& r,NodePtr n){static_cast<NodeRow*>(r.get())->appendChild(std::move(n));}
static NodePtr row(NodePtr a,NodePtr b,NodePtr c){auto r=makeRow();append(r,std::move(a));append(r,std::move(b));append(r,std::move(c));return r;}
static void exact(NodePtr n,const char* text,int power=0){auto result=CalculationEngine::instance().evaluate(n.get());check(result.ok()&&result.exactText==text,"closed exact coefficient");check(power?(result.quantity&&result.quantity->dimension.powers[0]==power):!result.quantity,"quantity versus scalar identity");}
int main(){
 lv_init();check(GiacEngine::instance().begin(),"Giac initialized");
 auto identity=[](){return makeFraction(metre(),metre());};
 auto expression=row(makeNumber("2"),op(OpKind::Add),makeNumber("3"));
 append(expression,op(OpKind::Mul));append(expression,makeNumber("4"));append(expression,identity());exact(std::move(expression),"14");
 auto division=row(makeNumber("6"),op(OpKind::Div),op(OpKind::Sub));append(division,makeNumber("2"));append(division,identity());exact(std::move(division),"-3");
 exact(row(makeNumber("2"),op(OpKind::Mul),makePower(metre(),makeNumber("2"))),"2",2);
 exact(makePower(makeParen(row(makeNumber("2"),op(OpKind::Mul),metre())),makeNumber("2")),"4",2);
 auto incomplete=row(makeNumber("2"),op(OpKind::Mul),metre());append(incomplete,op(OpKind::Div));
 check(CalculationEngine::instance().evaluate(incomplete.get()).quantityError==quantity::Error::Incomplete,"incomplete division retains typed diagnostic");
 auto deep=metre();for(unsigned i=0;i<24;++i)deep=makeParen(std::move(deep));
 check(quantity::evaluate(deep.get(),nullptr,nullptr).error==quantity::Error::ExpressionLimit,"depth bound precedes recursive descent");
 auto overflow=makePower(makePower(metre(),makeNumber("64")),makeNumber("2"));
 check(quantity::evaluate(overflow.get(),nullptr,nullptr).error==quantity::Error::ExponentLimit,"dimension overflow rejected");
 auto oversized=makeRow();for(unsigned i=0;i<192;++i)append(oversized,op(OpKind::Sub));append(oversized,metre());
 const auto bounded=quantity::evaluate(oversized.get(),nullptr,nullptr);
 std::printf("NODE_LIMIT error=%s calls=%u\n",quantity::errorName(bounded.error),bounded.calls);
 check(bounded.error==quantity::Error::ExpressionLimit&&bounded.calls==0,"operator nodes count toward the node limit before Giac");
 std::printf("PASS %u focused closeout assertions\n",checks);
}
