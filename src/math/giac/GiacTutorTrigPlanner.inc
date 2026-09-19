// SPDX-License-Identifier: GPL-3.0-or-later
void Planner::trigonometric() {
    const auto f=trigForm(a);
    auto append=[&](Rule rule,State next,std::string operand={},Vector<std::string> aux={}){
        Step s;s.rule=rule;s.operand=std::move(operand);s.auxiliaries=std::move(aux);
        s.explanation=trigMessage(rule,f.kind);
        if(rule==Rule::TrigFamilies)s.relation=Relation::ExhaustiveSplit;
        else if(rule!=Rule::FamilyShift&&rule!=Rule::FamilyDivide)s.relation=Relation::Terminal;
        if((rule==Rule::TrigRange||rule==Rule::TrigImpossible)&&!next.conditions.empty())s.introduced={0};
        ++group;add(std::move(s),std::move(next));
    };
    State range=current();captureTrig(a,range.conditions);
    if(!trigInRange(a,f)){range.periodicStage=7;range.conclusion=Conclusion::Empty;append(Rule::TrigImpossible,std::move(range),a.print(f.target));return;}
    range.periodicStage=1;append(Rule::TrigRange,std::move(range),a.print(f.target));
    const gen alpha=trigPrincipal(a,f),half=trigHalfTurn(a),period=a.normal(f.kind==2?half:2*half);
    State principal=current();principal.periodicStage=2;append(Rule::TrigPrincipal,std::move(principal),a.print(f.target),{a.print(alpha)});
    State split=current();split.periodicStage=3;
    for(unsigned i=0;i<(f.kind==2?1u:2u);++i){PeriodicFamily family;family.variable=a.input.variables[0];family.lhs=a.print(f.argument);
        family.offset=a.print(a.normal(i?(f.kind==0?half-alpha:-alpha):alpha));family.period=a.print(period);
        family.binderScope=fingerprint(a.input);family.theoremStep=uint16_t(d.steps.size());family.theoremBranch=uint8_t(i);family.degrees=a.input.degrees;split.families.push_back(std::move(family));}
    append(Rule::TrigFamilies,std::move(split));
    if(!a.zero(f.constant)){State next=current();for(auto& family:next.families){family.lhs=a.print(a.normal(f.argument-f.constant));family.offset=a.print(a.normal(a.parse(family.offset)-f.constant));}next.periodicStage=4;append(Rule::FamilyShift,std::move(next),a.print(f.constant));}
    if(!a.same(f.coefficient,gen(1))){State divided=current();for(auto& family:divided.families){family.lhs=a.input.variables[0];family.offset=a.print(a.normal(a.parse(family.offset)/f.coefficient));family.period=a.print(a.normal(a.parse(family.period)/f.coefficient));}
    divided.periodicStage=5;append(Rule::FamilyDivide,std::move(divided),a.print(f.coefficient));}
    State normalized=current();for(auto& family:normalized.families)family.lhs=a.input.variables[0];normalizeFamilies(a,normalized.families);normalized.periodicStage=6;append(Rule::FamilyNormalize,std::move(normalized));
    State final=current();for(auto& family:final.families)family.originalCheck=Verdict::Verified;
    final.periodicStage=7;final.conclusion=Conclusion::Periodic;append(Rule::FamilyFinish,std::move(final));
}
