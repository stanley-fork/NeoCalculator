// SPDX-License-Identifier: GPL-3.0-or-later
// Deterministic finite affine inverse plan. Balancing is checked by the original
// AddBoth/DivideBoth kernel, with exact symbolic constants kept on the right.
void Planner::transcendental() {
    auto form=transForm(a,current().branches[0].equations[0]);
    if(form.reversed){const auto e=current().branches[0].equations[0];equation(Rule::SwapSides,a.parse(e.rhs),a.parse(e.lhs));}
    form=transForm(a,current().branches[0].equations[0]);
    if(!a.zero(form.remainder)){const auto e=current().branches[0].equations[0];equation(Rule::AddBoth,preservingNormal(a,a.parse(e.lhs)-form.remainder),a.normal(a.parse(e.rhs)-form.remainder),-form.remainder);}
    form=transForm(a,current().branches[0].equations[0]);
    if(!a.same(form.coefficient,gen(1))){const auto e=current().branches[0].equations[0];equation(Rule::DivideBoth,preservingNormal(a,a.parse(e.lhs)/form.coefficient),a.normal(a.parse(e.rhs)/form.coefficient),form.coefficient);}
    auto e=current().branches[0].equations[0];form=transForm(a,e);const auto function=form.function;gen rhs=a.parse(e.rhs);auto other=transFunction(rhs);
    bool injective=other.kind!=TransKind::None&&other.logarithm()==function.logarithm()&&a.same(other.base,function.base);
    if(!function.logarithm()&&!injective){
        if(!exactPositive(a,rhs)){State next=current();next.conclusion=Conclusion::Empty;Step s;s.rule=Rule::PositiveImpossible;s.operand=e.rhs;add(s,std::move(next));return;}
        State range=current();Path path;path.side=1;range.conditions.push_back(transCondition(a,rhs,path,ConditionKind::Positive,ConditionRole::ExponentialTarget,uint16_t(d.states.size()-1)));Step s;s.rule=Rule::ExpRange;s.operand=e.rhs;add(s,std::move(range));
        gen exponent;
        if(function.kind==TransKind::Power&&exactPower(a,function.base,rhs,exponent)){
            State power=current();power.branches[0].equations[0].rhs=a.print(giac::symbolic(giac::at_pow,giac::makevecteur(function.base,exponent)));Step s;s.rule=Rule::ExpExactPower;s.operand=a.print(exponent);add(s,std::move(power));e=current().branches[0].equations[0];rhs=a.parse(e.rhs);other=transFunction(rhs);injective=true;
        }
    }
    State next=current();Step theorem;theorem.operand=e.rhs;gen target;
    if(injective){theorem.rule=function.logarithm()?Rule::LogInjective:Rule::ExpInjective;theorem.auxiliaries={a.print(function.base)};target=other.inner;}
    else if(function.logarithm()){theorem.rule=Rule::LogInverse;theorem.auxiliaries={function.natural()?"ln":a.print(function.base)};target=function.natural()?gen(giac::symbolic(giac::at_exp,rhs)):gen(giac::symbolic(giac::at_pow,giac::makevecteur(function.base,rhs)));target=a.normal(target);}
    else {theorem.rule=Rule::ExpInverse;theorem.auxiliaries={function.natural()?"exp":a.print(function.base)};target=giac::symbolic(giac::at_ln,rhs);if(!function.natural())target=target/giac::symbolic(giac::at_ln,function.base);target=a.normal(target);}
    next.branches[0].origin=1;next.branches[0].equations={{a.print(function.inner),a.print(target)}};++group;add(theorem,std::move(next));
    e=current().branches[0].equations[0];gen l=a.parse(e.lhs),r=a.parse(e.rhs);const gen x=a.parse(a.input.variables[0]);
    if(!solveVariable(l,a.input)&&solveVariable(r,a.input)){equation(Rule::SwapSides,r,l);std::swap(l,r);}
    auto lc=a.coefficients(l,x);auto rc=solveVariable(r,a.input)?a.coefficients(r,x):std::array<gen,3>{r,gen(0),gen(0)};
    if(!a.zero(lc[2])||!a.zero(rc[2]))throw UnsupportedInput{__LINE__,"affine child required"};
    const gen coefficient=a.normal(lc[1]-rc[1]);
    if(a.zero(coefficient)){
        if(a.same(l,r))throw UnsupportedInput{__LINE__,"conditional infinite transcendental family"};
        terminal(Rule::Contradiction,Conclusion::Empty);return;
    }
    if(!a.zero(rc[1])){const gen term=-rc[1]*x;equation(Rule::AddBoth,a.normal(l+term),a.normal(r+term),term);l=a.normal(l+term);r=a.normal(r+term);}
    if(!a.zero(lc[0])){equation(Rule::AddBoth,a.normal(l-lc[0]),a.normal(r-lc[0]),-lc[0]);l=a.normal(l-lc[0]);r=a.normal(r-lc[0]);}
    if(!a.same(coefficient,gen(1))){equation(Rule::DivideBoth,a.normal(l/coefficient),a.normal(r/coefficient),coefficient);r=a.normal(r/coefficient);}
    // WHY: an authored affine term such as x+0 can have unit coefficient and
    // zero constant without being the isolated variable required by CheckOriginal.
    // Retain the inverse theorem's authored operand, then explicitly replay the
    // existing collection rule instead of silently certifying an unisolated row.
    e=current().branches[0].equations[0];
    if(e.lhs!=a.input.variables[0]) {
        equation(Rule::Collect,a.normal(a.parse(e.lhs)),a.normal(a.parse(e.rhs)));
        r=a.parse(current().branches[0].equations[0].rhs);
    }
    auto reason=candidateFailure(a,current(),r);State checked=current();Step check;check.operand=a.print(r);
    if(reason.empty()){check.rule=Rule::CheckOriginal;checked.branches[0].originalCheck=Verdict::Verified;}
    else {check.rule=Rule::RejectOriginal;check.auxiliaries={reason};checked.branches[0].status=BranchStatus::Rejected;checked.branches[0].originalCheck=Verdict::Rejected;}
    ++group;add(check,std::move(checked));terminal(Rule::Finish,reason.empty()?Conclusion::Finite:Conclusion::Empty);
}
