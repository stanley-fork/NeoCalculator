// SPDX-License-Identifier: GPL-3.0-or-later
// One owned composition layer. Child proofs remain complete independent traces;
// no child planner is called by the checking kernel.
gen compositionBound(Algebra& a,const gen& g,unsigned depth=0){
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(g.type==giac::_IDNT){for(const auto& b:a.input.bindings)if(g==a.parse(b.variable))return a.parse(b.value);return g;}
    if(g.type==giac::_SYMB)return giac::symbolic(g._SYMBptr->sommet,compositionBound(a,g._SYMBptr->feuille,depth+1));
    if(g.type==giac::_VECT){giac::vecteur children;for(const auto& c:*g._VECTptr)children.push_back(compositionBound(a,c,depth+1));return gen(children,g.subtype);}
    return g;
}
composition::Form compositionForm(Algebra& a) {
    if(a.input.complex||a.input.authored.size()!=1||a.input.variables.size()!=1||a.input.parentScope)
        throw UnsupportedInput{__LINE__,"one real substitution layer only"};
    OpaqueScope quoted(a.ctx);composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};
    const auto& eq=a.input.authored.front();const auto x=a.parse(a.input.variables.front());
    const auto l=a.parse(eq.lhs),r=a.parse(eq.rhs);
    try{
        composition::source(w,l,x);composition::source(w,r,x);
        return composition::admit(w,compositionBound(a,l),compositionBound(a,r),x,quoted.value);
    }catch(const composition::Declined& failure){throw UnsupportedInput{int(failure.line),"no bounded polynomial substitution certificate"};}
}
void compositionConditions(Algebra& a,const composition::Form& f,Vector<Condition>& original,Vector<Condition>& range){
    Path p;p.side=f.side;
    if(f.logarithm())original.push_back(transCondition(a,f.inner,p,ConditionKind::Positive,ConditionRole::LogArgument));
    if(f.kind==composition::Kind::Power||f.kind==composition::Kind::Log10||f.kind==composition::Kind::LogBase){
        const auto role=f.logarithm()?ConditionRole::LogarithmBase:ConditionRole::ExponentialBase;
        original.push_back(transCondition(a,f.base,p,ConditionKind::Positive,role));
        original.push_back(transCondition(a,f.base,p,ConditionKind::NotOne,role));
    }
    if(f.kind==composition::Kind::Tan)original.push_back(transCondition(a,giac::symbolic(giac::at_cos,f.inner),p,ConditionKind::Nonzero,ConditionRole::TangentPole));
    const gen t=a.parse("t");
    if(f.kind==composition::Kind::Square)range.push_back(transCondition(a,t,p,ConditionKind::Nonnegative,ConditionRole::AuxiliaryRange,1));
    if(f.kind==composition::Kind::Exp||f.kind==composition::Kind::Power)range.push_back(transCondition(a,t,p,ConditionKind::Positive,ConditionRole::AuxiliaryRange,1));
    if(f.kind==composition::Kind::Sin||f.kind==composition::Kind::Cos){
        range.push_back(transCondition(a,t+1,p,ConditionKind::Nonnegative,ConditionRole::AuxiliaryRange,1));
        range.push_back(transCondition(a,1-t,p,ConditionKind::Nonnegative,ConditionRole::AuxiliaryRange,1));
    }
}
int compositionSign(Algebra& a,const gen& value){
    composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};
    try{return composition::sign(w,value);}catch(const composition::Declined&){throw LimitReached{__LINE__};}
}
bool auxiliaryAllowed(Algebra& a,const composition::Form& f,const gen& value){
    const int sign=compositionSign(a,value);
    if(sign==2)throw UnsupportedInput{__LINE__,"exact auxiliary sign unknown"};
    if(f.kind==composition::Kind::Square)return sign>=0;
    if(f.kind==composition::Kind::Exp||f.kind==composition::Kind::Power)return sign>0;
    if(f.kind==composition::Kind::Sin||f.kind==composition::Kind::Cos){
        const int lower=compositionSign(a,value+1),upper=compositionSign(a,1-value);
        if(lower==2||upper==2)throw UnsupportedInput{__LINE__,"exact auxiliary range unknown"};
        return lower>=0&&upper>=0;
    }
    return true;
}
Equation auxiliaryEquation(Algebra& a,const composition::Form& f){
    const gen t=a.parse("t");return {a.print(a.normal(f.coefficients[2]*t*t+f.coefficients[1]*t+f.coefficients[0])),"0"};
}
Snapshot compositionChild(const Derivation& d,const Equation& equation,uint8_t role){
    Snapshot child;child.authored={equation};child.variables={role==1?"t":d.input.variables[0]};
    child.inputEpoch=d.input.inputEpoch;child.engineGeneration=d.input.engineGeneration;
    child.degrees=d.input.degrees;child.parentScope=fingerprint(d.input);child.childRole=role;
    return child;
}
Vector<std::string> auxiliaryRoots(const Derivation& child){
    Vector<std::string> values;
    if(child.states.empty()||child.status!=Status::Complete)throw CheckFailure{};
    const auto& last=child.states.back();
    if(last.conclusion==Conclusion::Empty)return values;
    if(last.conclusion!=Conclusion::Finite)throw UnsupportedInput{__LINE__,"conditional auxiliary identity is outside the finite composition contract"};
    for(const auto& b:last.branches)if(b.status==BranchStatus::Active){
        if(b.equations.size()!=1||b.equations[0].lhs!="t"||values.size()>=2)throw CheckFailure{};
        values.push_back(b.equations[0].rhs);
    }
    return values;
}
Message rejectionMessage(composition::Kind kind){
    return kind==composition::Kind::Square?Message::SubstitutionRejectSquare:
        kind==composition::Kind::Exp||kind==composition::Kind::Power?Message::SubstitutionRejectExp:Message::SubstitutionRejectTrig;
}
void completeChild(Algebra& a,Derivation& child,std::string& diagnostic){
    Scope scope(a.ctx,child.input);Algebra ca{a.ctx,child.input};ca.calls=a.calls;
    struct CountBack {Algebra& parent;Algebra& child;~CountBack(){parent.calls=child.calls;if(parent.measuredCalls)*parent.measuredCalls=parent.calls;}} counted{a,ca};
    Planner planner{ca,child};child.states.push_back(authoredState(child.input));
    try {planner.run();}catch(const CheckFailure&){diagnostic=child.diagnostic.empty()?"child checker rejected at "+std::to_string(ca.rejectedLine):child.diagnostic;throw;}
    // Every primitive passed add(). The parent independently replays the entire
    // child before publishing the corresponding auxiliary/pullback certificate.
    child.status=Status::Complete;child.validity=Verdict::Verified;
    a.calls=ca.calls;
    child.candidates=child.completeness=Verdict::Verified;
}
bool replayChild(Algebra& a,const Derivation& child,const Snapshot& expected){
    if(child.composition||fingerprint(child.input)!=fingerprint(expected)||
       child.status!=Status::Complete||child.validity!=Verdict::Verified||
       child.candidates!=Verdict::Verified||child.completeness!=Verdict::Verified)return false;
    for(const auto& step:child.steps)if(step.verification!=Verdict::Verified)return false;
    Scope scope(a.ctx,child.input);Algebra ca{a.ctx,child.input};ca.calls=a.calls;
    struct CountBack {Algebra& parent;Algebra& child;~CountBack(){parent.calls=child.calls;if(parent.measuredCalls)*parent.measuredCalls=parent.calls;}} counted{a,ca};
    const auto result=replay(ca,child);a.calls=ca.calls;return result==Verdict::Verified;
}
State compositionUnion(Algebra& a,const Derivation& d){
    const auto& cert=*d.composition;State result;result.conditions=cert.originalConditions;
    for(size_t i=0;i<cert.preimages.size();++i){const auto& p=cert.preimages[i];if(!p.child)continue;
        if(p.child>=cert.children.size())throw CheckFailure{};
        const auto& child=cert.children[p.child];const auto& state=child.states.back();
        if(state.conclusion==Conclusion::Empty)continue;
        if(state.conclusion==Conclusion::Periodic){
            for(auto family:state.families){
                family.binderScope=cert.scope;family.binderId=p.child;family.theoremBranch=uint8_t(i);
                for(size_t s=0;s<d.steps.size();++s)if(d.steps[s].rule==Rule::SubstitutionPullback&&d.steps[s].branch==i)family.theoremStep=uint16_t(s);
                bool duplicate=false;for(const auto& old:result.families)duplicate=duplicate||familyEquivalent(a,old,family);
                if(!duplicate){if(result.families.size()>=4)throw LimitReached{__LINE__};result.families.push_back(std::move(family));}
            }
        }else if(state.conclusion==Conclusion::Finite){
            for(auto branch:state.branches)if(branch.status==BranchStatus::Active){
                if(branch.equations.size()!=1)throw CheckFailure{};
                const auto candidate=a.parse(branch.equations[0].rhs);
                if(!candidateFailure(a,result,candidate).empty())throw CheckFailure{};
                bool duplicate=false;for(const auto& old:result.branches)duplicate=duplicate||a.same(a.parse(old.equations[0].rhs),candidate);
                if(!duplicate){if(result.branches.size()>=4)throw LimitReached{__LINE__};branch.id=uint8_t(result.branches.size()+1);branch.origin=uint8_t(i+1);branch.originalCheck=Verdict::Verified;result.branches.push_back(std::move(branch));}
            }
        }else throw UnsupportedInput{__LINE__,"incomplete preimage cannot establish a complete composition"};
    }
    if(!result.families.empty()){if(!result.branches.empty())throw CheckFailure{};result.conclusion=Conclusion::Periodic;result.periodicStage=7;}
    else result.conclusion=result.branches.empty()?Conclusion::Empty:Conclusion::Finite;
    result.fingerprint=fingerprint(result);return result;
}
Verdict verifyComposition(Algebra& a,const Derivation& d,const Step& s){
    if(s.before>=d.states.size()||s.after!=s.before+1||s.after>=d.states.size()||s.row||s.otherRow||
       !s.introduced.empty()||!s.discharged.empty()||!s.parameters.empty()||!s.auxiliaries.empty()||
       s.caseStart!=UINT16_MAX||!s.substeps.empty())return a.reject(__LINE__);
    const auto& before=d.states[s.before];const auto& after=d.states[s.after];
    if(before.conclusion!=Conclusion::None||before.fingerprint!=fingerprint(before)||after.fingerprint!=fingerprint(after))return a.reject(__LINE__);
    if(s.prerequisites.size()!=before.conditions.size())return a.reject(__LINE__);
    for(size_t i=0;i<s.prerequisites.size();++i)if(s.prerequisites[i]!=i)return a.reject(__LINE__);
    if(s.affected.size()!=1||s.affected[0].equation||s.affected[0].side!=2||!s.affected[0].children.empty())return a.reject(__LINE__);
    State expected=before;Message message=Message::SubstitutionDefine;Relation relation=Relation::Terminal;
    if(s.rule==Rule::SquarePreimage){
        if(d.composition||d.input.childRole!=3||!d.input.parentScope||s.before||s.branch||!s.operand.empty())return a.reject(__LINE__);
        composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};composition::Form f;
        const auto& eq=d.input.authored[0];const auto x=a.parse(d.input.variables[0]),rawTarget=a.parse(eq.rhs);
        if(!composition::closedAlgebraic(w,rawTarget))return a.reject(__LINE__);
        const auto target=a.normal(rawTarget);
        const auto authored=a.parse(eq.lhs);
        if(!composition::readAtom(w,authored,x,f)||f.kind!=composition::Kind::Square||
           authored._SYMBptr->feuille._VECTptr->back()!=gen(2)||compositionSign(a,target)<0||compositionSign(a,target)==2)return a.reject(__LINE__);
        const unsigned count=a.zero(target)?1:2;
        if(after.branches.size()!=count||after.conclusion!=Conclusion::None||!after.conditions.empty()||!after.families.empty())return a.reject(__LINE__);
        for(unsigned i=0;i<count;++i){const auto& b=after.branches[i];
            if(b.equations.size()!=1||b.id!=i+1||b.status!=BranchStatus::Active||b.originalCheck!=Verdict::Unknown||b.origin||b.equations[0].lhs!=a.print(f.inner))return a.reject(__LINE__);
            const auto value=a.parse(b.equations[0].rhs);
            w.nodes=0;if(!composition::closedAlgebraic(w,value))return a.reject(__LINE__);
            const auto image=value;

            const gen principal=giac::symbolic(giac::at_sqrt,target);
            // Replay the real-square theorem structurally. Asking the vendored
            // denester to simplify nested radicals is not part of this proof.
            if(!w.same(image,i?-principal:principal))return a.reject(__LINE__);
        }
        expected=after;message=Message::SubstitutionSquare;relation=Relation::ExhaustiveSplit;
    }else{
        if(!d.composition)return a.reject(__LINE__);const auto& c=*d.composition;
        OpaqueScope quoteT(a.ctx); // the certificate's auxiliary is additionally quoted below
        Snapshot auxScope;auxScope.variables={"t"};Scope auxiliaryQuote(a.ctx,auxScope);
        composition::Form f;
        if(s.rule==Rule::SubstitutionDefine){
            f=compositionForm(a);
            if(c.scope!=fingerprint(d.input)||c.auxiliaryId!=1||c.kind!=unsigned(f.kind)||
               c.sourceSide!=f.side||c.definition!=a.print(f.atom)||c.children.size()>3||c.preimages.size()>2)return a.reject(__LINE__);
            for(unsigned i=0;i<3;++i)if(c.coefficients[i]!=a.print(f.coefficients[i]))return a.reject(__LINE__);
            Vector<Condition> original,range;compositionConditions(a,f,original,range);
            if(!sameConditions(original,c.originalConditions)||!sameConditions(range,c.auxiliaryConditions))return a.reject(__LINE__);
        }else {
            // WHY: replay visits the checked definition first. Later rules use
            // that certificate as a premise; they do not re-extract P from the
            // original for every child primitive. Its premises are immutable
            // during replay, and every public replay starts at step zero.
            if(d.steps.empty()||d.steps[0].rule!=Rule::SubstitutionDefine)return a.reject(__LINE__);
            // The atom, coefficients and conditions were all independently
            // checked at step zero. Reusing that premise avoids re-running its
            // admission work for every range/return rule; child proofs still
            // receive full independent replay and original-equation checks.
            f.kind=static_cast<composition::Kind>(c.kind);f.side=c.sourceSide;
        }
        Equation aux;
        if(s.rule==Rule::SubstitutionRewrite||s.rule==Rule::SubstitutionAuxiliary){
            for(unsigned i=0;i<3;++i)f.coefficients[i]=a.normal(a.parse(c.coefficients[i]));
            aux=auxiliaryEquation(a,f);
        }
        if(s.rule==Rule::SubstitutionDefine){
            if(s.before||s.branch||!s.operand.empty())return a.reject(__LINE__);
            expected.conditions=c.originalConditions;expected.branches={Branch{}};expected.branches[0].equations={{"t",c.definition}};
        }else if(s.rule==Rule::SubstitutionRewrite){
            if(s.before!=1||s.branch||!s.operand.empty())return a.reject(__LINE__);
            expected.branches[0].equations={aux};message=Message::SubstitutionRewrite;
        }else if(s.rule==Rule::SubstitutionAuxiliary){
            if(s.before!=2||s.branch||!s.operand.empty()||c.children.empty()||!replayChild(a,c.children[0],compositionChild(d,aux,1)))return a.reject(__LINE__);
            (void)auxiliaryRoots(c.children[0]);expected.branches=c.children[0].states.back().branches;message=Message::SubstitutionAuxiliary;
        }else if(s.rule==Rule::SubstitutionReject||s.rule==Rule::SubstitutionPullback){
            if(c.children.empty()||s.branch>=c.preimages.size()||s.branch>=before.branches.size()||s.before!=3+s.branch)return a.reject(__LINE__);
            const auto values=auxiliaryRoots(c.children[0]);if(s.branch>=values.size())return a.reject(__LINE__);
            const auto& p=c.preimages[s.branch];if(p.value!=values[s.branch]||s.operand!=p.value)return a.reject(__LINE__);
            const bool allowed=auxiliaryAllowed(a,f,a.parse(p.value));
            if(s.rule==Rule::SubstitutionReject){
                if(allowed||p.child||p.range!=Verdict::Rejected)return a.reject(__LINE__);
                expected.branches[s.branch].status=BranchStatus::Rejected;message=rejectionMessage(f.kind);relation=Relation::Rejection;
            }else{
                if(!allowed||!p.child||p.child>=c.children.size()||p.range!=Verdict::Verified)return a.reject(__LINE__);
                const Equation equation{c.definition,p.value};
                if(!replayChild(a,c.children[p.child],compositionChild(d,equation,f.kind==composition::Kind::Square?3:2)))return a.reject(__LINE__);
                expected.branches[s.branch].equations={equation};message=Message::SubstitutionPullback;
            }
        }else if(s.rule==Rule::SubstitutionFinish){
            if(c.children.empty()||s.branch||!s.operand.empty())return a.reject(__LINE__);
            const auto values=auxiliaryRoots(c.children[0]);
            if(values.size()!=c.preimages.size()||s.before!=3+values.size())return a.reject(__LINE__);
            unsigned children=1;
            for(size_t i=0;i<values.size();++i){const auto& p=c.preimages[i];if(p.value!=values[i])return a.reject(__LINE__);
                const auto rule=d.steps[3+i].rule;
                if(p.child){if(p.child!=children++||rule!=Rule::SubstitutionPullback)return a.reject(__LINE__);}
                else if(rule!=Rule::SubstitutionReject)return a.reject(__LINE__);
            }
            if(children!=c.children.size())return a.reject(__LINE__);
            expected=compositionUnion(a,d);message=expected.conclusion==Conclusion::Empty?Message::SubstitutionFinishEmpty:Message::SubstitutionFinish;
        }else return a.reject(__LINE__);
    }
    if(s.explanation!=message||s.relation!=relation||!validateMessage(message,s.parameters)||
       expected.conclusion!=after.conclusion||expected.periodicStage!=after.periodicStage||
       !sameBranches(expected.branches,after.branches)||!sameConditions(expected.conditions,after.conditions)||!sameFamilies(expected.families,after.families))return a.reject(__LINE__);
    return Verdict::Verified;
}
void Planner::squarePreimage(){
    composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};composition::Form f;
    const auto& eq=a.input.authored[0];const auto x=a.parse(a.input.variables[0]),target=a.normal(a.parse(eq.rhs));
    if(!composition::readAtom(w,a.parse(eq.lhs),x,f)||f.kind!=composition::Kind::Square)throw UnsupportedInput{__LINE__};
    State next=current();next.branches.clear();
    const gen root=giac::symbolic(giac::at_sqrt,target);
    for(unsigned i=0;i<(a.zero(target)?1u:2u);++i){Branch b;b.id=uint8_t(i+1);b.equations={{a.print(f.inner),a.print(a.normal(i?-root:root))}};next.branches.push_back(std::move(b));}
    Step s;s.rule=Rule::SquarePreimage;s.relation=Relation::ExhaustiveSplit;s.explanation=Message::SubstitutionSquare;add(s,std::move(next));
    for(unsigned i=0;i<current().branches.size();++i){++group;auto e=current().branches[i].equations[0];
        if(!a.zero(f.intercept)){equation(Rule::AddBoth,a.normal(a.parse(e.lhs)-f.intercept),a.normal(a.parse(e.rhs)-f.intercept),-f.intercept,0,i);e=current().branches[i].equations[0];}
        if(!a.same(f.slope,gen(1)))equation(Rule::DivideBoth,a.normal(a.parse(e.lhs)/f.slope),a.normal(a.parse(e.rhs)/f.slope),f.slope,0,i);
    }
    finishCandidates();
}
void Planner::compose(){
    const auto form=compositionForm(a); // admit BEFORE replacing a failed prefix
    a.compositionMode=true;
    a.transcendentalInput=false;a.trigonometricInput=false;
    auto certificate=std::make_shared<Composition>();certificate->scope=fingerprint(d.input);certificate->kind=uint8_t(form.kind);certificate->sourceSide=form.side;certificate->definition=a.print(form.atom);
    for(unsigned i=0;i<3;++i)certificate->coefficients[i]=a.print(form.coefficients[i]);
    Snapshot auxScope;auxScope.variables={"t"};Scope auxiliaryQuote(a.ctx,auxScope);
    compositionConditions(a,form,certificate->originalConditions,certificate->auxiliaryConditions);
    d.steps.clear();d.states.clear();d.states.push_back(authoredState(d.input));d.composition=std::move(certificate);group=0;auto& c=*d.composition;
    auto emit=[&](Rule rule,Message message,State next,unsigned branch=0,const std::string& operand=std::string{}){
        Step s;s.rule=rule;s.explanation=message;s.branch=uint8_t(branch);s.operand=operand;s.relation=rule==Rule::SubstitutionReject?Relation::Rejection:Relation::Terminal;add(s,std::move(next));
    };
    State definition=current();definition.conditions=c.originalConditions;definition.branches[0].equations={{"t",c.definition}};
    emit(Rule::SubstitutionDefine,Message::SubstitutionDefine,std::move(definition));
    const auto aux=auxiliaryEquation(a,form);State rewrite=current();rewrite.branches[0].equations={aux};emit(Rule::SubstitutionRewrite,Message::SubstitutionRewrite,std::move(rewrite));
    Derivation child;child.input=compositionChild(d,aux,1);completeChild(a,child,d.diagnostic);c.children.push_back(std::move(child));
    auto roots=auxiliaryRoots(c.children[0]);State solved=current();solved.branches=c.children[0].states.back().branches;
    emit(Rule::SubstitutionAuxiliary,Message::SubstitutionAuxiliary,std::move(solved));
    for(unsigned i=0;i<roots.size();++i){
        const bool allowed=auxiliaryAllowed(a,form,a.parse(roots[i]));Preimage preimage;preimage.value=roots[i];preimage.range=allowed?Verdict::Verified:Verdict::Rejected;
        if(allowed){Derivation pullback;pullback.input=compositionChild(d,{c.definition,roots[i]},form.kind==composition::Kind::Square?3:2);preimage.child=uint8_t(c.children.size());c.children.push_back(std::move(pullback));completeChild(a,c.children.back(),d.diagnostic);}
        c.preimages.push_back(std::move(preimage));State next=current();
        if(allowed)next.branches[i].equations={{c.definition,roots[i]}};else next.branches[i].status=BranchStatus::Rejected;
        emit(allowed?Rule::SubstitutionPullback:Rule::SubstitutionReject,allowed?Message::SubstitutionPullback:rejectionMessage(form.kind),std::move(next),i,roots[i]);
    }
    auto result=compositionUnion(a,d);
    const auto message=result.conclusion==Conclusion::Empty?Message::SubstitutionFinishEmpty:Message::SubstitutionFinish;
    emit(Rule::SubstitutionFinish,message,std::move(result));
}
