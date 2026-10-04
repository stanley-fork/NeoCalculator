// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/CalculationEngine.h"
#include "math/VariableManager.h"
#include "math/ToolboxCatalog.h"
#include "math/CursorController.h"
#include <cstdio>
#include <cstdlib>
using namespace numos; using namespace vpam;
namespace {
unsigned checks=0;
void check(bool ok,const char* message){++checks;if(!ok){std::printf("FAIL %s\n",message);std::exit(1);}}
bool compact(const MathNode* n){while(n && n->type()==NodeType::Row && n->childCount()==1)n=n->child(0);return n && n->type()==NodeType::SpecialValue && static_cast<const NodeSpecialValue*>(n)->specialKind()==SpecialValueKind::BothInfinities;}
NodePtr authored(uint8_t variant){auto row=makeRow();static_cast<NodeRow*>(row.get())->appendChild(toolbox::prepare(*toolbox::find({410,variant})).node);return row;}
}
int main(){
    check(GiacEngine::instance().begin(),"engine init");
    auto& engine=CalculationEngine::instance();
    auto input=authored(3);auto result=engine.evaluate(input.get());
    check(result.ok() && result.exactText=="[-infinity, +infinity]","canonical pair remains a list");
    check(compact(result.exactAST.get()) && result.presentation==ResultPresentation::BothInfinities,"explicit object presents compactly");
    std::string machine,error;check(CalculationEngine::serializeForGiac(result.exactAST.get(),machine,error) && machine=="[-infinity,infinity]","display AST retains both alternatives");
    check(compact(cloneNode(result.exactAST.get()).get()),"history display clone");
    check(compact(engine.evaluate(cloneNode(input.get()).get()).exactAST.get()),"recalled input reevaluates with provenance");
    engine.noteAnsRotated(result.exactText,false,result.presentation);
    auto ans=makeVariable(VAR_ANS);auto ansResult=engine.evaluate(ans.get());
    check(ansResult.ok() && ansResult.exactText==result.exactText && compact(ansResult.exactAST.get()),"direct Ans retains mathematics and provenance");
    check(engine.storeAns('A'),"session STO of pair");
    check(compact(engine.evaluate(makeVariable('A').get()).exactAST.get()),"direct stored pair retains provenance");
    VariableManager::instance().setVariable('A',ExactVal::fromInt(7));
    auto stale=engine.evaluate(makeVariable('A').get());check(stale.exactText=="7" && !compact(stale.exactAST.get()),"changed variable invalidates provenance");
    auto derived=makeRow();auto* row=static_cast<NodeRow*>(derived.get());row->appendChild(makeVariable(VAR_ANS));row->appendChild(makeOperator(OpKind::Add));row->appendChild(makeNumber("1"));
    auto changed=engine.evaluate(row);check(changed.ok() && !compact(changed.exactAST.get()),"arithmetic does not blindly propagate hint");
    engine.noteAnsRotated(changed.exactText,false,changed.presentation);
    check(!compact(engine.evaluate(ans.get()).exactAST.get()),"derived Ans is not relabelled");
    for(const char* text:{"[-infinity,infinity]","[-1,1]","1/0","0/0"}) {
        auto raw=GiacEngine::instance().evaluateStructured(text,true);
        check(raw.hasTree && !compact(CalculationEngine::resultTreeToAST(raw.tree).get()),"generic engine results are never compacted");
    }
    EngineResultNode interval;interval.kind=EngineNodeKind::Interval;
    interval.children={{EngineNodeKind::MinusInfinity},{EngineNodeKind::PlusInfinity}};
    check(!compact(CalculationEngine::resultTreeToAST(interval).get()),"interval endpoints stay an interval");
    for(uint8_t v:{uint8_t(0),uint8_t(1),uint8_t(2)}) {
        auto ev=engine.evaluate(authored(v).get());check(ev.ok() && ev.exactText==(v==2?"-infinity":"+infinity") && !compact(ev.exactAST.get()),"single infinity unchanged");
    }
    auto power=authored(2);CursorController cursor;cursor.init(static_cast<NodeRow*>(power.get()));cursor.moveRight();cursor.insertPower();cursor.insertDigit('2');
    check(power->child(0)->child(0)->child(0)->type()==NodeType::Paren,"real delimiters around signed base");
    check(engine.evaluate(power.get()).exactText=="+infinity","negative infinity squared");
    std::printf("PASS %u infinity provenance checks\n",checks);
}
