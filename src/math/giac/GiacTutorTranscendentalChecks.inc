// SPDX-License-Identifier: GPL-3.0-or-later
// Independent theorem replay: no generator calls and no final-answer oracle.
case Rule::ExpRange: {
    const auto f=transForm(a,eq);ok=!f.reversed&&!f.function.logarithm()&&lhs==f.function.node&&s.operand==eq.rhs&&s.auxiliaries.empty();
    for(const auto& c:before.conditions)if(c.role==ConditionRole::ExponentialTarget)ok=false;
    ok=ok&&s.introduced==Vector<uint8_t>{uint8_t(before.conditions.size())};
    Path path;path.side=1;expected.conditions.push_back(transCondition(a,rhs,path,ConditionKind::Positive,ConditionRole::ExponentialTarget,s.before));break;
}
case Rule::PositiveImpossible:case Rule::ExpExactPower:case Rule::ExpInverse:
case Rule::ExpInjective:case Rule::LogInverse:case Rule::LogInjective: {
    const auto f=transForm(a,eq);const auto& function=f.function;
    validTransBase(a,function);
    ok=before.branches.size()==1&&!f.reversed&&lhs==function.node;
    auto condition=[&](const gen& value,ConditionKind kind,ConditionRole role){for(const auto& c:before.conditions)if(c.kind==kind&&c.role==role&&a.same(a.parse(c.nonzero),value))return true;return false;};
    if(!function.natural())ok=ok&&condition(function.base,ConditionKind::Positive,function.logarithm()?ConditionRole::LogarithmBase:ConditionRole::ExponentialBase)&&condition(function.base,ConditionKind::NotOne,function.logarithm()?ConditionRole::LogarithmBase:ConditionRole::ExponentialBase);
    if(s.rule==Rule::PositiveImpossible){ok=ok&&!function.logarithm()&&a.rational(a.normal(rhs))&&!exactPositive(a,rhs)&&s.operand==eq.rhs&&s.auxiliaries.empty();expected.conclusion=Conclusion::Empty;break;}
    if(s.rule==Rule::ExpExactPower){
        const gen exponent=a.normal(a.parse(s.operand));gen p=exponent,q(1);
        if(exponent.type==giac::_FRAC){p=exponent._FRACptr->num;q=exponent._FRACptr->den;}
        // Compare bounds directly: abs(INT_MIN) can overflow for a forged operand.
        ok=ok&&function.kind==TransKind::Power&&p.type==giac::_INT_&&q.type==giac::_INT_&&p.val>=-16&&p.val<=16&&q.val>0&&q.val<=8&&s.auxiliaries.empty()&&exactPositive(a,rhs)&&condition(rhs,ConditionKind::Positive,ConditionRole::ExponentialTarget);
        if(ok)ok=a.same(giac::pow(function.base,p.val,a.ctx),giac::pow(rhs,q.val,a.ctx));
        expected.branches[0].equations={{eq.lhs,a.print(giac::symbolic(giac::at_pow,giac::makevecteur(function.base,exponent)))}};break;
    }
    gen result;
    if(s.rule==Rule::ExpInjective||s.rule==Rule::LogInjective){
        const auto other=transFunction(rhs);validTransBase(a,other);
        ok=ok&&other.kind!=TransKind::None&&function.logarithm()==other.logarithm()&&(function.logarithm()==(s.rule==Rule::LogInjective))&&a.same(function.base,other.base)&&s.operand==eq.rhs&&s.auxiliaries==Vector<std::string>{a.print(function.base)};
        if(function.logarithm())ok=ok&&condition(function.inner,ConditionKind::Positive,ConditionRole::LogArgument)&&condition(other.inner,ConditionKind::Positive,ConditionRole::LogArgument);
        result=other.inner;
    }else if(s.rule==Rule::LogInverse){
        ok=ok&&function.logarithm()&&condition(function.inner,ConditionKind::Positive,ConditionRole::LogArgument)&&!solveVariable(rhs,a.input)&&exactAlgebraicTarget(a,a.normal(rhs))&&s.operand==eq.rhs&&s.auxiliaries==Vector<std::string>{function.natural()?"ln":a.print(function.base)};
        result=function.natural()?gen(giac::symbolic(giac::at_exp,rhs)):gen(giac::symbolic(giac::at_pow,giac::makevecteur(function.base,rhs)));result=a.normal(result);
    }else {
        ok=ok&&!function.logarithm()&&condition(rhs,ConditionKind::Positive,ConditionRole::ExponentialTarget)&&!solveVariable(rhs,a.input)&&exactPositive(a,rhs)&&s.operand==eq.rhs&&s.auxiliaries==Vector<std::string>{function.natural()?"exp":a.print(function.base)};
        result=giac::symbolic(giac::at_ln,rhs);if(!function.natural())result=result/giac::symbolic(giac::at_ln,function.base);result=a.normal(result);
    }
    expected.branches[0].origin=1;expected.branches[0].equations={{a.print(function.inner),a.print(result)}};break;
}
