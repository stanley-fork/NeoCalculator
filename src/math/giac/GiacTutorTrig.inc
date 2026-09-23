// SPDX-License-Identifier: GPL-3.0-or-later
// Periodic theorems use RAD exact scalars internally. DEG is an exact unit
// conversion (180/pi), never a change of global context or a decimal angle.
bool trigAtom(const gen& g) {
    return g.type==giac::_SYMB && (g._SYMBptr->sommet==giac::at_sin ||
        g._SYMBptr->sommet==giac::at_cos || g._SYMBptr->sommet==giac::at_tan);
}
bool trigSyntax(const gen& g,unsigned depth=0) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(trigAtom(g))return true;
    if(g.type==giac::_SYMB)return trigSyntax(g._SYMBptr->feuille,depth+1);
    if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)if(trigSyntax(c,depth+1))return true;
    return false;
}
bool trigConstant(Algebra& a,const gen& value,unsigned depth=0,bool authoredBindings=false) {
    if(depth>Limits::depth)return false;
    if(a.rational(value)||value==gen(giac::_IDNT_pi()))return true;
    if(authoredBindings&&value.type==giac::_IDNT){
        for(const auto& binding:a.input.bindings)
            if(value==a.parse(binding.variable))return trigConstant(a,a.parse(binding.value),depth+1);
        return false;
    }
    if(value.type==giac::_VECT){for(const auto& child:*value._VECTptr)if(!trigConstant(a,child,depth+1,authoredBindings))return false;return true;}
    if(value.type!=giac::_SYMB)return false;
    const auto op=value._SYMBptr->sommet;const auto& leaf=value._SYMBptr->feuille;
    if(op==giac::at_sqrt){
        if(!trigConstant(a,leaf,depth+1,authoredBindings))return false;
        composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};const int sign=composition::sign(w,authoredBindings?a.bound(leaf):leaf);
        return sign==0||sign==1;
    }
    if(op==giac::at_inv)return trigConstant(a,leaf,depth+1,authoredBindings)&&!a.zero(authoredBindings?a.bound(leaf):leaf);
    if(op==giac::at_division){return leaf.type==giac::_VECT&&leaf._VECTptr->size()==2&&trigConstant(a,leaf,depth+1,authoredBindings)&&!a.zero(authoredBindings?a.bound(leaf._VECTptr->back()):leaf._VECTptr->back());}
    return (op==giac::at_plus||op==giac::at_neg||op==giac::at_binary_minus||op==giac::at_prod)&&trigConstant(a,leaf,depth+1,authoredBindings);
}
gen compositionBound(Algebra&,const gen&,unsigned);
struct TrigForm {gen node,argument,target,coefficient,constant;unsigned kind=0,side=0;bool outer=false;};
TrigForm trigForm(Algebra& a) {
    if(a.input.complex||a.input.variables.size()!=1||a.input.authored.size()!=1)
        throw UnsupportedInput{__LINE__,"real single trigonometric equation required"};
    const auto& e=a.input.authored[0];gen left=a.parse(e.lhs),right=a.parse(e.rhs);
    TrigForm f;
    if(!trigAtom(left)){std::swap(left,right);f.side=1;}
    if(!trigAtom(left)||trigSyntax(right)){
        composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};
        const auto l=a.parse(e.lhs),r=a.parse(e.rhs),x=a.parse(a.input.variables[0]);
        try{
            composition::source(w,l,x);composition::source(w,r,x);OpaqueScope quote(a.ctx);
            const auto form=composition::admit(w,compositionBound(a,l,0),compositionBound(a,r,0),x,quote.value);
            if(!form.periodic()||!w.same(form.coefficients[2],0)||w.same(form.coefficients[1],0))throw UnsupportedInput{__LINE__};
            f.node=form.atom;f.argument=form.inner;f.target=a.normal(-form.coefficients[0]/form.coefficients[1]);
            f.coefficient=form.slope;f.constant=form.intercept;f.side=form.side;f.kind=unsigned(form.kind)-unsigned(composition::Kind::Sin);f.outer=true;return f;
        }catch(const composition::Declined&){throw UnsupportedInput{__LINE__,"single rational outer trigonometric operation required"};}
    }
    // WHY: cancellation must not erase authored target domains (0*x/x,
    // 0*ln(-1)). Periodic unions do not implement target-domain filtering.
    // Replay uses this same structural admission before any normalization.
    if(!trigConstant(a,right,0,true))throw UnsupportedInput{__LINE__,"authored trigonometric target must be constant arithmetic"};
    // Substitution itself can collapse products with stored zero coefficients.
    // Check argument domains before binding for the same reason.
    affineSource(a,left._SYMBptr->feuille);
    left=a.bound(left);right=a.bound(right);
    if(!trigAtom(left))throw UnsupportedInput{__LINE__,"bound trigonometric argument must remain defined and nonconstant"};
    f.node=left;f.argument=left._SYMBptr->feuille;f.target=a.normal(right);
    if(!a.rational(f.target)){
        composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};
        if(transExponent(f.target)||composition::sign(w,f.target)==2)throw UnsupportedInput{__LINE__,"exact real trigonometric target required"};
    }
    affineSource(a,f.argument);const gen variable=a.parse(a.input.variables[0]);
    a.tick();f.coefficient=a.normal(giac::derive(f.argument,variable,a.ctx));
    f.constant=a.normal(a.sub(f.argument,variable,gen(0)));
    if(!a.rational(f.coefficient)||a.zero(f.coefficient)||!trigConstant(a,f.constant)||
       !a.same(f.argument,f.coefficient*variable+f.constant))
        throw UnsupportedInput{__LINE__,"nonconstant affine argument with rational slope and exact rational/pi offset required"};
    f.kind=left._SYMBptr->sommet==giac::at_sin?0:left._SYMBptr->sommet==giac::at_cos?1:2;
    return f;
}
gen trigHalfTurn(Algebra& a){return a.input.degrees?gen(180):gen(giac::_IDNT_pi());}
bool trigInRange(Algebra& a,const TrigForm& f){
    if(f.kind==2)return true;
    if(a.rational(f.target))return !a.negative(f.target+1)&&!a.negative(1-f.target);
    composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};
    const int lower=composition::sign(w,f.target+1),upper=composition::sign(w,1-f.target);
    if(lower==2||upper==2)throw UnsupportedInput{__LINE__,"exact trigonometric range unknown"};
    return lower>=0&&upper>=0;
}
gen trigPrincipal(Algebra& a,const TrigForm& f) {
    // Principal domains: asin [-pi/2,pi/2], acos [0,pi], atan (-pi/2,pi/2).
    // Admission established a real rational target and the required range.
    const auto op=f.kind==0?giac::at_asin:f.kind==1?giac::at_acos:giac::at_atan;
    const gen raw=giac::symbolic(op,f.target);
    gen angle=a.normal(raw);
    if(angle!=raw) {
        // Verify named-angle simplifications independently: exact rational
        // multiple of pi inside the inverse's principal interval, plus the
        // forward identity. Unknown simplifications retain the defining inverse.
        const gen q=a.normal(angle/gen(giac::_IDNT_pi()));
        bool range=false;
        if(a.rational(q)) {
            if(f.kind==1)range=!a.negative(q)&&!a.negative(1-q);
            else range=!a.negative(q+gen(1)/2)&&!a.negative(gen(1)/2-q);
            if(f.kind==2)range=range&&!a.zero(q+gen(1)/2)&&!a.zero(gen(1)/2-q);
        }
        if(!range||!a.same(giac::symbolic(f.node._SYMBptr->sommet,angle),f.target))angle=raw;
    }
    if(a.input.degrees)angle=a.normal(angle*180/gen(giac::_IDNT_pi()));
    return angle;
}
bool familyEquivalent(Algebra& a,const PeriodicFamily& x,const PeriodicFamily& y) {
    if(x.variable!=y.variable||x.lhs!=y.lhs||x.degrees!=y.degrees||x.domain!=y.domain || !a.same(a.parse(x.period),a.parse(y.period)))return false;
    const gen shift=a.normal((a.parse(x.offset)-a.parse(y.offset))/a.parse(x.period));
    return shift.type==giac::_INT_||shift.type==giac::_ZINT;
}
void normalizeFamilies(Algebra& a,Vector<PeriodicFamily>& families) {
    for(auto& f:families){const gen p=a.parse(f.period);if(a.zero(p))throw CheckFailure{};if(a.negative(p))f.period=a.print(a.normal(-p));}
    if(families.size()==2 && familyEquivalent(a,families[0],families[1]))families.pop_back();
}
Message trigMessage(Rule rule,unsigned kind) {
    switch(rule){
    case Rule::TrigRange:return kind==2?Message::TangentDomain:Message::TrigRange;
    case Rule::TrigPrincipal:return Message::TrigPrincipal;
    case Rule::TrigFamilies:return kind==0?Message::SineFamilies:kind==1?Message::CosineFamilies:Message::TangentFamilies;
    case Rule::FamilyShift:return Message::FamilyShift;
    case Rule::FamilyDivide:return Message::FamilyDivide;
    case Rule::FamilyNormalize:return Message::FamilyNormalize;
    case Rule::FamilyFinish:return Message::FamilyFinish;
    default:return Message::TrigImpossible;
    }
}
bool captureTrig(Algebra& a,Vector<Condition>& conditions) {
    bool found=false;for(const auto& e:a.input.authored)found=found||trigSyntax(a.parse(e.lhs))||trigSyntax(a.parse(e.rhs));
    if(!found)return false;
    const auto f=trigForm(a);
    if(f.kind==2){Path p;p.side=uint8_t(f.side);auto c=realCondition(a,giac::symbolic(giac::at_cos,f.argument),p,ConditionRole::TangentPole,0);c.kind=ConditionKind::Nonzero;conditions.push_back(std::move(c));}
    return true;
}
// Independent replay of declared steps. Does not call the planner or solve().
Verdict verifyTrig(Algebra& a,const Derivation& d,const Step& s) {
    const auto& before=d.states[s.before];const auto& after=d.states[s.after];const auto f=trigForm(a);
    if(before.conclusion!=Conclusion::None||s.branch||s.row||s.otherRow||s.caseStart!=UINT16_MAX||
       !s.substeps.empty()||!s.discharged.empty()||s.affected.size()!=1||s.affected[0].side!=2||s.affected[0].equation||!s.affected[0].children.empty()||
       !sameBranches(before.branches,after.branches)||after.families.size()>2||
       s.explanation!=trigMessage(s.rule,f.kind)||!s.parameters.empty()||!validateMessage(s.explanation,s.parameters))return a.reject(__LINE__);
    State expected=before;bool ok=true;Relation relation=Relation::Terminal;
    for(const auto& family:before.families)if(family.binderScope!=fingerprint(d.input)||family.binderId!=1||family.domain!=IntegerDomain::AllIntegers||family.degrees!=d.input.degrees)return a.reject(__LINE__);
    if(s.prerequisites.size()!=before.conditions.size())return a.reject(__LINE__);
    for(size_t i=0;i<s.prerequisites.size();++i)if(s.prerequisites[i]!=i)return a.reject(__LINE__);
    switch(s.rule){
    case Rule::TrigRange:case Rule::TrigImpossible:{
        ok=before.periodicStage==0 && before.families.empty() && s.operand==a.print(f.target) && s.auxiliaries.empty();
        Vector<Condition> cs;captureTrig(a,cs);expected.conditions=cs;
        ok=ok && s.introduced==(cs.empty()?Vector<uint8_t>{}:Vector<uint8_t>{0});
        if(s.rule==Rule::TrigImpossible){ok=ok && !trigInRange(a,f);expected.conclusion=Conclusion::Empty;expected.periodicStage=7;}
        else {ok=ok&&trigInRange(a,f);expected.periodicStage=1;}
        break;
    }
    case Rule::TrigPrincipal:
        ok=before.periodicStage==1 && s.operand==a.print(f.target) && s.auxiliaries==Vector<std::string>{a.print(trigPrincipal(a,f))};expected.periodicStage=2;break;
    case Rule::TrigFamilies:{
        ok=before.periodicStage==2 && s.operand.empty() && s.auxiliaries.empty();relation=Relation::ExhaustiveSplit;
        const gen alpha=trigPrincipal(a,f),turn=trigHalfTurn(a),period=f.kind==2?turn:2*turn;
        for(unsigned i=0;i<(f.kind==2?1u:2u);++i){PeriodicFamily family;
            family.variable=d.input.variables[0];family.lhs=a.print(f.argument);
            family.offset=a.print(a.normal(i?(f.kind==0?turn-alpha:-alpha):alpha));family.period=a.print(a.normal(period));
            family.binderScope=fingerprint(d.input);family.theoremStep=s.before;family.theoremBranch=uint8_t(i);family.degrees=d.input.degrees;
            expected.families.push_back(std::move(family));
        }expected.periodicStage=3;break;
    }
    case Rule::FamilyShift:
        ok=before.periodicStage==3&&!a.zero(f.constant)&&s.operand==a.print(f.constant)&&s.auxiliaries.empty();relation=Relation::Equivalent;
        for(auto& family:expected.families){family.lhs=a.print(a.normal(f.argument-f.constant));family.offset=a.print(a.normal(a.parse(family.offset)-f.constant));}expected.periodicStage=4;break;
    case Rule::FamilyDivide:
        ok=!a.same(f.coefficient,gen(1))&&(before.periodicStage==4||(before.periodicStage==3&&a.zero(f.constant)))&&s.operand==a.print(f.coefficient)&&s.auxiliaries.empty();relation=Relation::Equivalent;
        for(auto& family:expected.families){family.lhs=d.input.variables[0];family.offset=a.print(a.normal(a.parse(family.offset)/f.coefficient));family.period=a.print(a.normal(a.parse(family.period)/f.coefficient));}expected.periodicStage=5;break;
    case Rule::FamilyNormalize:
        ok=(before.periodicStage==5 || (a.same(f.coefficient,gen(1)) && (before.periodicStage==4 || (before.periodicStage==3&&a.zero(f.constant))))) && s.operand.empty()&&s.auxiliaries.empty();
        for(auto& family:expected.families)family.lhs=d.input.variables[0];
        normalizeFamilies(a,expected.families);expected.periodicStage=6;break;
    case Rule::FamilyFinish:{
        ok=before.periodicStage==6&&!before.families.empty()&&s.operand.empty()&&s.auxiliaries.empty();
        // Universal original-definedness follows from the replayed theorem:
        // a*x+b is its principal/symmetric angle plus whole periods, for ALL
        // integers, not just k=0. atan's open principal range excludes all poles.
        const gen alpha=trigPrincipal(a,f),half=trigHalfTurn(a),turn=f.kind==2?half:2*half;
        for(auto& family:expected.families){
            const gen image=a.normal(f.coefficient*a.parse(family.offset)+f.constant);
            const gen wanted=family.theoremBranch?(f.kind==0?half-alpha:-alpha):alpha;
            const gen periodImage=a.normal(f.coefficient*a.parse(family.period)/turn);
            ok=ok&&family.lhs==d.input.variables[0]&&a.same(image,wanted)&&(a.same(periodImage,gen(1))||a.same(periodImage,gen(-1)));
            family.originalCheck=Verdict::Verified;
        }expected.periodicStage=7;expected.conclusion=Conclusion::Periodic;break;
    }
    default:return a.reject(__LINE__);
    }
    if(s.rule!=Rule::TrigRange&&s.rule!=Rule::TrigImpossible&&!s.introduced.empty())ok=false;
    if(!ok||s.relation!=relation||expected.periodicStage!=after.periodicStage||expected.conclusion!=after.conclusion||
       !sameConditions(expected.conditions,after.conditions)||!sameFamilies(expected.families,after.families)||
       before.fingerprint!=fingerprint(before)||after.fingerprint!=fingerprint(after))return a.reject(__LINE__);
    return Verdict::Verified;
}
