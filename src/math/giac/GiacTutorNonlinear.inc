// SPDX-License-Identifier: GPL-3.0-or-later
// Private Giac boundary helpers. Traverse the existing parsed tree BEFORE eval.
struct Special {
    unsigned count = 0;
    bool radical = false;
    gen node, inner;
    Path path;
};
struct OpaqueScope {
    giac::context* ctx;
    gen value{giac::identificateur(" tutor opaque ")};
    size_t size;
    explicit OpaqueScope(giac::context* c):ctx(c),size(c->quoted_global_vars?c->quoted_global_vars->size():0) {
        if(ctx->quoted_global_vars)ctx->quoted_global_vars->push_back(value);
    }
    ~OpaqueScope(){if(ctx->quoted_global_vars)ctx->quoted_global_vars->resize(size);}
};
void findSpecial(const gen& g, Special& out, Path path={}, unsigned depth=0) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(g.type==giac::_SYMB) {
        auto op=g._SYMBptr->sommet;
        if(op==giac::at_abs || op==giac::at_sqrt) {
            if(++out.count>1)throw UnsupportedInput{__LINE__,"nested or multiple special functions"};
            out.radical=op==giac::at_sqrt;out.node=g;out.inner=g._SYMBptr->feuille;out.path=path;
        }
        findSpecial(g._SYMBptr->feuille,out,path,depth+1);
    } else if(g.type==giac::_VECT) {
        for(size_t i=0;i<g._VECTptr->size();++i) {
            auto child=path;child.children.push_back(uint8_t(i));
            findSpecial((*g._VECTptr)[i],out,std::move(child),depth+1);
        }
    } else if(g.type==giac::_FRAC) {
        findSpecial(g._FRACptr->num,out,path,depth+1);
        findSpecial(g._FRACptr->den,out,path,depth+1);
    }
}
Special findSpecialEquation(Algebra& a,const Equation& eq) {
    Special out;Path p;findSpecial(a.parse(eq.lhs),out,p);p.side=1;findSpecial(a.parse(eq.rhs),out,p);return out;
}
// Structural replacement is not algebraic substitution: it must not evaluate
// sqrt(x^2) to abs(x) or collapse authored nested functions before admission.
gen replaceTree(const gen& g,const gen& from,const gen& to,unsigned depth=0) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(g==from)return to;
    if(g.type==giac::_SYMB)return giac::symbolic(g._SYMBptr->sommet,replaceTree(g._SYMBptr->feuille,from,to,depth+1));
    if(g.type==giac::_VECT) {
        giac::vecteur children;children.reserve(g._VECTptr->size());
        for(const auto& c:*g._VECTptr)children.push_back(replaceTree(c,from,to,depth+1));
        return gen(children,g.subtype);
    }
    return g; // parsed fractional arithmetic has symbolic division/inverse nodes
}
gen transPreservingNormal(Algebra&,const gen&);
gen preservingNormal(Algebra& a,const gen& g) {
    if(a.transcendentalInput)return transPreservingNormal(a,g);
    Special special;findSpecial(g,special);
    if(!special.count)return a.normal(g);
    OpaqueScope scope(a.ctx);const gen& opaque=scope.value;
    return replaceTree(a.normal(replaceTree(g,special.node,opaque)),opaque,special.node);
}
bool hasIdentifier(const gen& g,unsigned depth=0) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(g.type==giac::_IDNT)return true;
    if(g.type==giac::_SYMB)return hasIdentifier(g._SYMBptr->feuille,depth+1);
    if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)if(hasIdentifier(c,depth+1))return true;
    return false;
}
bool hasOpenSpecial(const gen& g,unsigned depth=0) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(g.type==giac::_SYMB) {
        auto op=g._SYMBptr->sommet;
        if((op==giac::at_sqrt || op==giac::at_abs) && hasIdentifier(g._SYMBptr->feuille))return true;
        return hasOpenSpecial(g._SYMBptr->feuille,depth+1);
    }
    if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)if(hasOpenSpecial(c,depth+1))return true;
    return false;
}
bool transDisplayNeeded(const gen&,unsigned=0);
gen transDisplay(Algebra&,const gen&,unsigned=0);
gen displayEval(Algebra& a,const gen& g) {
    // WHY: preserve variable-function provenance without adding a mask, context
    // quote or polynomial capture to the optimized closed quadratic-root path.
    if(transDisplayNeeded(g))return transDisplay(a,g);
    if(!hasOpenSpecial(g))return g.eval(1,a.ctx);
    Special special;findSpecial(g,special);
    if(!special.count)return g.eval(1,a.ctx);
    OpaqueScope scope(a.ctx);const gen& opaque=scope.value;
    return replaceTree(replaceTree(g,special.node,opaque).eval(1,a.ctx),opaque,special.node);
}
struct IsolatedForm {
    Special special;
    bool reversed=false;
    gen coefficient,remainder,other;
};
IsolatedForm isolatedForm(Algebra& a,const Equation& eq) {
    IsolatedForm f;f.special=findSpecialEquation(a,eq);
    if(!f.special.count || a.input.complex || a.input.variables.size()!=1)
        throw UnsupportedInput{__LINE__,"real single-function method only"};
    f.reversed=f.special.path.side==1;
    gen side=a.parse(f.reversed?eq.rhs:eq.lhs);
    f.other=a.parse(f.reversed?eq.lhs:eq.rhs);
    OpaqueScope scope(a.ctx);const gen& opaque=scope.value;
    const gen masked=replaceTree(side,f.special.node,opaque);
    a.tick();f.coefficient=a.normal(giac::derive(masked,opaque,a.ctx));
    f.remainder=a.normal(a.sub(masked,opaque,gen(0)));
    if(!a.rational(f.coefficient) || a.zero(f.coefficient) ||
       !a.same(masked,f.coefficient*opaque+f.remainder))
        throw UnsupportedInput{__LINE__,"special term must have a nonzero rational coefficient"};
    const gen x=a.parse(a.input.variables[0]);
    a.coefficients(f.special.inner,x);a.coefficients(f.remainder,x);a.coefficients(f.other,x);
    return f;
}
void validateSpecialSource(Algebra& a) {
    for(const auto& e:a.input.authored) {
        auto special=findSpecialEquation(a,e);
        if(!special.count)continue;
        if(a.input.authored.size()!=1 || a.input.complex)throw UnsupportedInput{__LINE__,"real single equation only"};
        Equation bound{a.print(a.bound(a.parse(e.lhs))),a.print(a.bound(a.parse(e.rhs)))};
        auto f=isolatedForm(a,bound);
        // Verify the child problem is polynomial before an irreversible rule.
        const gen rhs=a.normal((f.other-f.remainder)/f.coefficient);
        const gen x=a.parse(a.input.variables[0]);
        a.coefficients(f.special.inner-(f.special.radical?rhs*rhs:rhs),x);
        if(!f.special.radical)a.coefficients(f.special.inner+rhs,x);
    }
}
Condition realCondition(Algebra& a,const gen& value,const Path& path,ConditionRole role,uint16_t state) {
    Condition c;c.nonzero=a.print(value);c.source=path;c.kind=ConditionKind::Nonnegative;
    c.role=role;c.sourceState=state;c.verification=Verdict::Verified;
    auto tree=std::make_shared<EngineResultNode>();int budget=Limits::sourceNodes;
    genToNode(value.eval(1,a.ctx),a.ctx,*tree,0,budget);
    if(tree->kind==EngineNodeKind::Unsupported)throw UnsupportedInput{__LINE__};
    c.expression=std::move(tree);return c;
}
bool exactPositive(Algebra&,const gen&);
bool conditionTrue(Algebra& a,const Condition& c,const gen& candidate) {
    const gen value=a.normal(a.sub(a.parse(c.nonzero),a.parse(a.input.variables[0]),candidate));
    if(c.kind==ConditionKind::Nonzero)return !a.zero(value);
    if(c.kind==ConditionKind::NotOne)return !a.same(value,gen(1));
    if(a.zero(value))return c.kind==ConditionKind::Nonnegative;
    // New logarithm/base conditions use only bounded exact sign theorems.
    if(c.role>=ConditionRole::LogArgument)return exactPositive(a,value);
    a.tick();if(giac::is_strictly_positive(value,a.ctx))return true;
    if(a.negative(value))return false;
    throw UnsupportedInput{__LINE__,"exact sign could not be established"};
}
std::string candidateFailure(Algebra& a,const State& state,const gen& candidate) {
    for(const auto& c:state.conditions)if(!conditionTrue(a,c,candidate))
        return c.role==ConditionRole::IsolatedRange?"sign":c.role==ConditionRole::Radicand?"radicand":c.role==ConditionRole::LogArgument?"log_domain":"domain";
    const gen x=a.parse(a.input.variables[0]);
    for(const auto& e:a.input.authored) {
        const gen l=a.normal(a.sub(a.bound(a.parse(e.lhs)),x,candidate));
        const gen r=a.normal(a.sub(a.bound(a.parse(e.rhs)),x,candidate));
        if(!a.same(l,r)) {
            const gen difference=a.normal(l-r);a.tick();
            if(a.negative(difference) || giac::is_strictly_positive(difference,a.ctx))return "original";
            throw UnsupportedInput{__LINE__,"original equality undecided by exact checks"};
        }
    }
    return {};
}
