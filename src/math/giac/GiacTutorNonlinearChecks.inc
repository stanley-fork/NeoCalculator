// SPDX-License-Identifier: GPL-3.0-or-later
// Cases in verifyStep's independent replay switch.
case Rule::RangeCondition: {
    const auto f=isolatedForm(a,eq);
    ok=!f.reversed && lhs==f.special.node && s.operand==eq.rhs &&
       s.auxiliaries==Vector<std::string>{f.special.radical?"sqrt":"abs"} &&
       s.introduced==Vector<uint8_t>{uint8_t(before.conditions.size())};
    for(const auto& c:before.conditions)if(c.role==ConditionRole::IsolatedRange)ok=false;
    Path path;path.side=1;
    expected.conditions.push_back(realCondition(a,rhs,path,ConditionRole::IsolatedRange,s.before));
    break;
}
case Rule::AbsCases:
case Rule::RadicalCandidates:
case Rule::NonnegativeImpossible: {
    const auto f=isolatedForm(a,eq);
    ok=before.branches.size()==1 && !f.reversed && lhs==f.special.node;
    if(s.rule==Rule::NonnegativeImpossible) {
        ok=ok && a.rational(a.normal(rhs)) && a.negative(a.normal(rhs)) &&
            s.auxiliaries==Vector<std::string>{f.special.radical?"sqrt":"abs"};
        expected.conclusion=Conclusion::Empty;break;
    }
    bool rangeFound=false,domainFound=!f.special.radical;
    for(const auto& c:before.conditions) {
        rangeFound=rangeFound || (c.kind==ConditionKind::Nonnegative && c.role==ConditionRole::IsolatedRange && a.same(a.parse(c.nonzero),rhs));
        domainFound=domainFound || (c.kind==ConditionKind::Nonnegative && c.role==ConditionRole::Radicand && a.same(a.parse(c.nonzero),f.special.inner));
    }
    ok=ok && rangeFound && domainFound && s.operand==eq.rhs &&
       s.auxiliaries==Vector<std::string>{a.print(f.special.inner)} &&
       (f.special.radical==(s.rule==Rule::RadicalCandidates));
    expected.branches.clear();
    const unsigned count=f.special.radical || a.zero(rhs)?1:2;
    for(unsigned i=0;i<count;++i) {
        Branch b;b.id=uint8_t(i+1);b.origin=uint8_t(i+1);
        b.equations={{a.print(f.special.inner), f.special.radical ? a.print(giac::symbolic(giac::at_pow,giac::makevecteur(rhs,gen(2)))) : a.print(i?a.normal(-rhs):rhs)}};
        expected.branches.push_back(std::move(b));
    }
    break;
}
case Rule::CheckOriginal:
case Rule::RejectOriginal: {
    ok=branch.origin!=0 && branch.originalCheck==Verdict::Unknown && branch.equations.size()==1 && lhs==x && s.operand==eq.rhs;
    const auto reason=candidateFailure(a,before,rhs);
    if(s.rule==Rule::CheckOriginal) {
        ok=ok && reason.empty() && s.auxiliaries.empty();
        expected.branches[s.branch].originalCheck=Verdict::Verified;
    } else {
        ok=ok && !reason.empty() && s.auxiliaries==Vector<std::string>{reason};
        expected.branches[s.branch].originalCheck=Verdict::Rejected;
        expected.branches[s.branch].status=BranchStatus::Rejected;
    }
    break;
}
case Rule::DuplicateCandidate: {
    ok=branch.origin!=0 && branch.originalCheck==Verdict::Verified && branch.equations.size()==1 && lhs==x && s.operand==eq.rhs;
    bool duplicate=false;
    for(size_t i=0;i<s.branch;++i) {
        const auto& other=before.branches[i];
        if(other.status==BranchStatus::Active && other.originalCheck==Verdict::Verified && other.equations.size()==1 &&
           a.parse(other.equations[0].lhs)==x && a.same(a.parse(other.equations[0].rhs),rhs)) duplicate=true;
    }
    ok=ok && duplicate;
    expected.branches[s.branch].status=BranchStatus::Duplicate;break;
}
