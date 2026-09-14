// SPDX-License-Identifier: GPL-3.0-or-later
// Bounded lifting of the existing polynomial planner. No second solver and no
// guessed intermediate states: every child primitive is independently replayed.
uint8_t liftedId(uint8_t initial,uint8_t local) { return local?uint8_t(2*initial+local):initial; }
State liftState(const State& parent,const State& child,uint8_t origin,uint8_t initial) {
    State result=parent;
    Vector<Branch> replacements;
    for(auto b:child.branches) {
        b.id=liftedId(initial,b.id);b.origin=origin;
        if(child.conclusion==Conclusion::Empty)b.status=BranchStatus::Empty;
        replacements.push_back(std::move(b));
    }
    size_t at=0;
    for(auto& b:result.branches)if(b.origin==origin) {
        if(at>=replacements.size())throw CheckFailure{};
        b=replacements[at++];
    }
    while(at<replacements.size())result.branches.push_back(replacements[at++]);
    if(result.branches.size()>Limits::branches)throw LimitReached{__LINE__};
    result.fingerprint=fingerprint(result);return result;
}
Verdict verifyLifted(Algebra& a,const Derivation& d,const Step& s) {
    if(s.caseStart==0 || s.caseStart>s.before || s.caseStart>=d.states.size() || s.caseStart>d.steps.size())return a.reject(__LINE__);
    const auto& theorem=d.steps[s.caseStart-1];
    if(theorem.after!=s.caseStart || (theorem.rule!=Rule::AbsCases && theorem.rule!=Rule::RadicalCandidates))return a.reject(__LINE__);
    const auto& cases=d.states[s.caseStart];
    if(s.caseBranch>=cases.branches.size() || s.row || s.otherRow || !s.discharged.empty() || !s.introduced.empty())return a.reject(__LINE__);
    const auto& base=cases.branches[s.caseBranch];
    const auto& before=d.states[s.before];const auto& after=d.states[s.after];
    if(before.conclusion!=Conclusion::None || after.conclusion!=Conclusion::None || s.branch>=before.branches.size() ||
       before.branches[s.branch].origin!=base.origin || !base.origin || !sameConditions(before.conditions,after.conditions) ||
       s.prerequisites.size()!=before.conditions.size())return a.reject(__LINE__);
    for(size_t i=0;i<s.prerequisites.size();++i)if(s.prerequisites[i]!=i)return a.reject(__LINE__);
    switch(s.rule) {
    case Rule::Expand:case Rule::Collect:case Rule::SwapSides:case Rule::AddBoth:case Rule::DivideBoth:
    case Rule::ClearDenominators:case Rule::Factor:case Rule::SquareRoots:case Rule::QuadraticFormula:
    case Rule::ZeroProduct:case Rule::Contradiction:break;
    default:return a.reject(__LINE__);
    }
    Derivation local;local.input=d.input;local.input.authored=base.equations;local.input.bindings.clear();
    auto project=[&](const State& state,bool output) {
        State projected;
        for(auto b:state.branches)if(b.origin==base.origin) {
            if(b.originalCheck!=Verdict::Unknown)throw CheckFailure{};
            if(b.id==base.id)b.id=0;
            else if(b.id==2*base.id+1)b.id=1;
            else if(b.id==2*base.id+2)b.id=2;
            else throw CheckFailure{};
            b.origin=0;
            if(output && b.status==BranchStatus::Empty){projected.conclusion=Conclusion::Empty;b.status=BranchStatus::Active;}
            if(b.status!=BranchStatus::Active)throw CheckFailure{};
            projected.branches.push_back(std::move(b));
        }
        projected.fingerprint=fingerprint(projected);return projected;
    };
    local.states.push_back(project(before,false));local.states.push_back(project(after,true));
    Step primitive=s;primitive.before=0;primitive.after=1;primitive.branch=0;
    for(size_t i=0;i<s.branch;++i)if(before.branches[i].origin==base.origin)++primitive.branch;
    primitive.caseStart=UINT16_MAX;primitive.caseBranch=0;primitive.prerequisites.clear();primitive.substeps.clear();
    // Each primitive validates its own message, operands, relation and equation
    // operation; the outer replay also checks every unrelated case unchanged.
    Algebra child{a.ctx,local.input};child.calls=a.calls;
    auto verdict=verifyStep(child,local,primitive);a.calls=child.calls;
    if(verdict!=Verdict::Verified){a.rejectedLine=child.rejectedLine;return verdict;}
    const State expected=liftState(before,local.states.back(),base.origin,base.id);
    if(!sameBranches(expected.branches,after.branches) || after.fingerprint!=fingerprint(after) || before.fingerprint!=fingerprint(before))return a.reject(__LINE__);
    return Verdict::Verified;
}
void Planner::nonlinear() {
    auto form=isolatedForm(a,current().branches[0].equations[0]);
    if(form.reversed) {
        const auto e=current().branches[0].equations[0];
        equation(Rule::SwapSides,a.parse(e.rhs),a.parse(e.lhs));
    }
    form=isolatedForm(a,current().branches[0].equations[0]);
    if(!a.zero(form.remainder)) {
        const auto e=current().branches[0].equations[0];
        equation(Rule::AddBoth,preservingNormal(a,a.parse(e.lhs)-form.remainder),a.normal(a.parse(e.rhs)-form.remainder),-form.remainder);
    }
    form=isolatedForm(a,current().branches[0].equations[0]);
    if(!a.same(form.coefficient,gen(1))) {
        const auto e=current().branches[0].equations[0];
        equation(Rule::DivideBoth,preservingNormal(a,a.parse(e.lhs)/form.coefficient),a.normal(a.parse(e.rhs)/form.coefficient),form.coefficient);
    }
    form=isolatedForm(a,current().branches[0].equations[0]);
    const auto e=current().branches[0].equations[0];const gen rhs=a.parse(e.rhs);
    if(a.rational(a.normal(rhs)) && a.negative(a.normal(rhs))) {
        State next=current();next.conclusion=Conclusion::Empty;
        Step s;s.rule=Rule::NonnegativeImpossible;s.auxiliaries={form.special.radical?"sqrt":"abs"};
        add(s,std::move(next));return;
    }
    {
        State next=current();Path path;path.side=1;
        next.conditions.push_back(realCondition(a,rhs,path,ConditionRole::IsolatedRange,uint16_t(d.states.size()-1)));
        Step s;s.rule=Rule::RangeCondition;s.operand=e.rhs;s.auxiliaries={form.special.radical?"sqrt":"abs"};
        ++group;add(s,std::move(next));
    }
    {
        State next=current();next.branches.clear();
        const unsigned count=form.special.radical || a.zero(rhs)?1:2;
        for(unsigned i=0;i<count;++i) {
            Branch b;b.id=uint8_t(i+1);b.origin=uint8_t(i+1);
            b.equations={{a.print(form.special.inner),form.special.radical?a.print(giac::symbolic(giac::at_pow,giac::makevecteur(rhs,gen(2)))):a.print(i?a.normal(-rhs):rhs)}};
            next.branches.push_back(std::move(b));
        }
        Step s;s.rule=form.special.radical?Rule::RadicalCandidates:Rule::AbsCases;s.operand=e.rhs;s.auxiliaries={a.print(form.special.inner)};
        ++group;add(s,std::move(next));
    }
    const uint16_t start=uint16_t(d.states.size()-1);
    const unsigned caseCount=current().branches.size();
    for(unsigned index=0;index<caseCount;++index) {
        const Branch base=d.states[start].branches[index];
        Derivation child;child.input=d.input;child.input.authored=base.equations;child.input.bindings.clear();
        child.states.push_back(authoredState(child.input));
        Algebra algebra{a.ctx,child.input};algebra.calls=a.calls;
        Planner planner{algebra,child};planner.run();a.calls=algebra.calls;
        const auto conclusion=child.states.back().conclusion;
        if(conclusion!=Conclusion::Finite && conclusion!=Conclusion::Empty)
            throw UnsupportedInput{__LINE__,"conditional infinite branch needs an inequality/family method"};
        for(const auto& original:child.steps) {
            if(original.rule==Rule::Finish || original.rule==Rule::AlreadySolved)continue;
            Step step=original;step.before=0;step.after=0;step.caseStart=start;step.caseBranch=uint8_t(index);
            step.prerequisites.clear();step.affected.clear();step.substeps.clear();step.branch=0;
            unsigned seen=0;bool found=false;
            for(size_t i=0;i<current().branches.size();++i)if(current().branches[i].origin==base.origin) {
                if(seen++==original.branch){step.branch=uint8_t(i);found=true;break;}
            }
            if(!found)throw CheckFailure{};
            auto next=liftState(current(),child.states[original.after],base.origin,base.id);
            ++group;add(step,std::move(next));
        }
    }
    for(size_t i=0;i<current().branches.size();++i) {
        if(current().branches[i].status!=BranchStatus::Active)continue;
        const auto value=current().branches[i].equations[0].rhs;
        const auto reason=candidateFailure(a,current(),a.parse(value));
        State next=current();Step step;step.branch=uint8_t(i);step.operand=value;
        if(reason.empty()) {step.rule=Rule::CheckOriginal;next.branches[i].originalCheck=Verdict::Verified;}
        else {step.rule=Rule::RejectOriginal;step.auxiliaries={reason};next.branches[i].status=BranchStatus::Rejected;next.branches[i].originalCheck=Verdict::Rejected;}
        ++group;add(step,std::move(next));
        if(!reason.empty())continue;
        for(size_t j=0;j<i;++j)if(current().branches[j].status==BranchStatus::Active && a.same(a.parse(current().branches[j].equations[0].rhs),a.parse(value))) {
            State unique=current();unique.branches[i].status=BranchStatus::Duplicate;
            Step duplicate;duplicate.rule=Rule::DuplicateCandidate;duplicate.branch=uint8_t(i);duplicate.operand=value;
            ++group;add(duplicate,std::move(unique));break;
        }
    }
    unsigned active=0;for(const auto& b:current().branches)active+=b.status==BranchStatus::Active;
    ++group;terminal(Rule::Finish,active?Conclusion::Finite:Conclusion::Empty);
}
