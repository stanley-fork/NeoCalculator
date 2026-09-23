// SPDX-License-Identifier: GPL-3.0-or-later
// Exact, bounded admission over Giac nodes. No text recognizer or second parser.
enum class TransKind { None, Exp, Power, Ln, Log10, LogBase };
bool transDisplayNeeded(const gen& g,unsigned depth) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(transAtom(g)||trigAtom(g)||(g.type==giac::_SYMB&&(g._SYMBptr->sommet==giac::at_asin||g._SYMBptr->sommet==giac::at_acos||g._SYMBptr->sommet==giac::at_atan)))return true;
    if(g.type==giac::_SYMB)return transDisplayNeeded(g._SYMBptr->feuille,depth+1);
    if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)if(transDisplayNeeded(c,depth+1))return true;
    return false;
}
gen transDisplay(Algebra& a,const gen& g,unsigned depth) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(g.type==giac::_VECT){giac::vecteur children;children.reserve(g._VECTptr->size());for(const auto& c:*g._VECTptr)children.push_back(transDisplay(a,c,depth+1));return gen(children,g.subtype);}
    if(g.type!=giac::_SYMB)return g;
    auto op=g._SYMBptr->sommet;const gen leaf=transDisplay(a,g._SYMBptr->feuille,depth+1);
    // Normalize only syntax required by the structured converter. In particular,
    // never evaluate logb into a quotient or expand e^(2x-1) into a product.
    if(op==giac::at_binary_minus && leaf.type==giac::_VECT && leaf._VECTptr->size()==2)
        return giac::symbolic(giac::at_plus,giac::makevecteur(leaf._VECTptr->front(),giac::symbolic(giac::at_neg,leaf._VECTptr->back())));
    if(op==giac::at_division && leaf.type==giac::_VECT && leaf._VECTptr->size()==2)
        return giac::symbolic(giac::at_prod,giac::makevecteur(leaf._VECTptr->front(),giac::symbolic(giac::at_inv,leaf._VECTptr->back())));
    return giac::symbolic(op,leaf);
}
struct TransFunction { TransKind kind=TransKind::None; gen node,inner,base; Path path; bool logarithm()const{return kind==TransKind::Ln||kind==TransKind::Log10||kind==TransKind::LogBase;} bool natural()const{return kind==TransKind::Exp||kind==TransKind::Ln;} };
bool solveVariable(const gen& g,const Snapshot& s,unsigned depth=0) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(g.type==giac::_IDNT)return std::find(s.variables.begin(),s.variables.end(),g.print())!=s.variables.end();
    if(g.type==giac::_SYMB)return solveVariable(g._SYMBptr->feuille,s,depth+1);
    if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)if(solveVariable(c,s,depth+1))return true;
    if(g.type==giac::_FRAC)return solveVariable(g._FRACptr->num,s,depth+1)||solveVariable(g._FRACptr->den,s,depth+1);
    return false;
}
TransFunction transFunction(const gen& g,Path path={}) {
    TransFunction f;f.node=g;f.path=std::move(path);
    if(g.type!=giac::_SYMB)return f;
    const auto op=g._SYMBptr->sommet;const auto& leaf=g._SYMBptr->feuille;
    f.inner=leaf;f.base=giac::symbolic(giac::at_exp,gen(1));
    if(op==giac::at_exp)f.kind=TransKind::Exp;
    else if(op==giac::at_ln)f.kind=TransKind::Ln;
    else if(op==giac::at_log10){f.kind=TransKind::Log10;f.base=gen(10);}
    else if(op==giac::at_logb && leaf.type==giac::_VECT && leaf._VECTptr->size()==2){f.kind=TransKind::LogBase;f.inner=leaf._VECTptr->front();f.base=leaf._VECTptr->back();}
    else if(op==giac::at_pow && leaf.type==giac::_VECT && leaf._VECTptr->size()==2){f.kind=TransKind::Power;f.base=leaf._VECTptr->front();f.inner=leaf._VECTptr->back();}
    // Giac prints the exact power a^(1/2) as sqrt(a). Only the theorem's
    // counterpart reader uses this alias; admission still counts authored roots
    // as 02A roots, never as exponentials with a variable base.
    else if(op==giac::at_sqrt){f.kind=TransKind::Power;f.base=leaf;f.inner=gen(1)/gen(2);}
    return f;
}
void transFind(const gen& g,Vector<TransFunction>& found,Path path={},unsigned depth=0) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(transAtom(g)){if(found.size()>=2)throw UnsupportedInput{__LINE__,"multiple exponential/logarithmic occurrences"};found.push_back(transFunction(g,path));return;}
    if(g.type==giac::_SYMB)transFind(g._SYMBptr->feuille,found,path,depth+1);
    else if(g.type==giac::_VECT)for(size_t i=0;i<g._VECTptr->size();++i){auto p=path;p.children.push_back(uint8_t(i));transFind((*g._VECTptr)[i],found,p,depth+1);}
}
bool hasTrans(const Equation& e,Algebra& a,bool openOnly=false) {
    Vector<TransFunction> found;transFind(a.parse(e.lhs),found);transFind(a.parse(e.rhs),found);
    for(const auto& f:found)if(!openOnly||solveVariable(f.inner,a.input))return true;
    return false;
}
bool exactAlgebraicTarget(Algebra& a,const gen& g){
    if(a.rational(g))return true;
    if(transExponent(g))return false;
    composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};return composition::sign(w,g)!=2;
}
bool exactPositive(Algebra& a,const gen& g){
    const gen value=a.normal(g);
    if(a.rational(value)){a.tick();return giac::is_strictly_positive(value,a.ctx);}
    if(!transExponent(value)){
        composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};
        const int sign=composition::sign(w,value);
        if(sign!=2)return sign==1;
    }
    const auto f=transFunction(value);
    if(f.kind==TransKind::Exp || f.kind==TransKind::Power){
        const auto inner=a.normal(f.inner);
        composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};
        if(a.rational(inner)||(!transExponent(inner)&&composition::sign(w,inner)!=2))
            return f.kind==TransKind::Exp||exactPositive(a,f.base);
    }
    if(f.kind==TransKind::Ln && a.rational(a.normal(f.inner)))return exactPositive(a,f.inner-gen(1));
    if(value.type==giac::_SYMB && value._SYMBptr->sommet==giac::at_inv)return exactPositive(a,value._SYMBptr->feuille);
    throw UnsupportedInput{__LINE__,"exact positivity theorem unavailable"};
}
gen transPreservingNormal(Algebra& a,const gen& g) {
    Vector<TransFunction> found;transFind(g,found);
    for(const auto& f:found)if(solveVariable(f.inner,a.input)){
        OpaqueScope scope(a.ctx);
        return replaceTree(a.normal(replaceTree(g,f.node,scope.value)),scope.value,f.node);
    }
    return a.normal(g);
}
void validTransBase(Algebra& a,const TransFunction& f) {
    if(f.kind==TransKind::None)throw UnsupportedInput{__LINE__,"expected exponential or logarithm node"};
    if(f.natural())return;
    const gen base=a.normal(a.bound(f.base));
    if(!a.rational(base)||!exactPositive(a,base)||a.same(base,gen(1)))throw UnsupportedInput{__LINE__,"base must be exact rational, positive and different from one"};
}
void affineSource(Algebra& a,const gen& g,unsigned depth=0) {
    if(depth>Limits::depth)throw LimitReached{__LINE__};
    if(g.type==giac::_FRAC){
        if(solveVariable(g._FRACptr->den,a.input))throw UnsupportedInput{__LINE__,"variable denominator in affine argument"};
        affineSource(a,g._FRACptr->num,depth+1);return;
    }
    if(g.type==giac::_VECT){for(const auto& c:*g._VECTptr)affineSource(a,c,depth+1);return;}
    if(g.type!=giac::_SYMB)return;
    const auto op=g._SYMBptr->sommet;const auto& f=g._SYMBptr->feuille;
    if(op==giac::at_inv && solveVariable(f,a.input))throw UnsupportedInput{__LINE__,"variable denominator in affine argument"};
    if(op==giac::at_division && f.type==giac::_VECT && f._VECTptr->size()==2 && solveVariable(f._VECTptr->back(),a.input))throw UnsupportedInput{__LINE__,"variable denominator in affine argument"};
    if(op==giac::at_pow && f.type==giac::_VECT && f._VECTptr->size()==2){
        const auto& exponent=f._VECTptr->back();
        if(exponent.type!=giac::_INT_ || exponent.val<0)throw UnsupportedInput{__LINE__,"non-polynomial affine argument"};
    }else if(op!=giac::at_plus && op!=giac::at_neg && op!=giac::at_binary_minus && op!=giac::at_prod && op!=giac::at_inv && op!=giac::at_division)
        throw UnsupportedInput{__LINE__,"nested function in affine argument"};
    affineSource(a,f,depth+1);
}
void affineArgument(Algebra& a,const gen& g) {
    affineSource(a,g);
    auto c=a.coefficients(g,a.parse(a.input.variables.front()));
    if(!a.zero(c[2]))throw UnsupportedInput{__LINE__,"02B requires an affine argument or exponent"};
}
struct TransForm {TransFunction function;gen coefficient,remainder,other;bool reversed=false;};
TransForm transForm(Algebra& a,const Equation& e) {
    TransForm result;Vector<TransFunction> left,right;transFind(a.parse(e.lhs),left);Path path;path.side=1;transFind(a.parse(e.rhs),right,path);
    if(left.size()>1||right.size()>1||left.empty()&&right.empty())throw UnsupportedInput{__LINE__,"one function per side"};
    result.reversed=left.empty();result.function=result.reversed?right[0]:left[0];
    const gen side=a.parse(result.reversed?e.rhs:e.lhs);result.other=a.parse(result.reversed?e.lhs:e.rhs);
    OpaqueScope opaque(a.ctx);const gen masked=replaceTree(side,result.function.node,opaque.value);
    affineSource(a,masked);
    a.tick();result.coefficient=a.normal(giac::derive(masked,opaque.value,a.ctx));result.remainder=a.normal(a.sub(masked,opaque.value,gen(0)));
    if(!a.rational(result.coefficient)||a.zero(result.coefficient)||!a.same(masked,result.coefficient*opaque.value+result.remainder))throw UnsupportedInput{__LINE__,"rational coefficient required around one function"};
    affineArgument(a,result.remainder);return result;
}
Condition transCondition(Algebra& a,const gen& value,Path path,ConditionKind kind,ConditionRole role,uint16_t state=0) {
    Condition c=realCondition(a,value,path,role,state);c.kind=kind;return c;
}
Rule conditionRule(const Condition& c) {
    if(c.role==ConditionRole::LogArgument)return Rule::PositiveDomain;
    if(c.role==ConditionRole::ExponentialBase||c.role==ConditionRole::LogarithmBase)return Rule::BaseDomain;
    return c.role==ConditionRole::Radicand?Rule::RadicandDomain:Rule::Domain;
}
bool captureTranscendental(Algebra& a,Vector<Condition>& conditions) {
    bool found=false;for(const auto& e:a.input.authored)found=hasTrans(e,a)||found;
    a.transcendentalInput=found;
    if(!found)return false;
    if(a.input.complex||a.input.authored.size()!=1||a.input.variables.size()!=1)throw UnsupportedInput{__LINE__,"real single-equation exponential/logarithm method only"};
    const auto& e=a.input.authored[0];Vector<TransFunction> functions;
    for(unsigned side=0;side<2;++side){Vector<TransFunction> part;Path path;path.side=uint8_t(side);transFind(a.parse(side?e.rhs:e.lhs),part,path);if(part.size()>1)throw UnsupportedInput{__LINE__,"sum of independent functions"};for(auto f:part)functions.push_back(std::move(f));}
    for(const auto& f:functions) {
        Vector<TransFunction> nested;transFind(f.inner,nested);if(!nested.empty())throw UnsupportedInput{__LINE__,"nested exp/log method not admitted"};
        validTransBase(a,f);affineArgument(a,a.bound(f.inner));
        if(!f.natural()){
            Path base=f.path;
            // log10's base is implicit in the function, not an argument child.
            if(f.kind!=TransKind::Log10)base.children.push_back(f.kind==TransKind::LogBase?1:0);
            const auto role=f.logarithm()?ConditionRole::LogarithmBase:ConditionRole::ExponentialBase;
            conditions.push_back(transCondition(a,a.bound(f.base),base,ConditionKind::Positive,role));
            conditions.push_back(transCondition(a,a.bound(f.base),base,ConditionKind::NotOne,role));
        }
        if(f.logarithm()){
            const auto value=a.bound(f.inner);
            if(!solveVariable(value,a.input)&&!exactPositive(a,value))throw UnsupportedInput{__LINE__,"logarithm argument is not positive in the real domain"};
            Path arg=f.path;if(f.kind==TransKind::LogBase)arg.children.push_back(0);
            conditions.push_back(transCondition(a,value,arg,ConditionKind::Positive,ConditionRole::LogArgument));
        }
    }
    if(conditions.size()>Limits::conditions)throw LimitReached{__LINE__};
    Equation bound{a.print(a.bound(a.parse(e.lhs))),a.print(a.bound(a.parse(e.rhs)))};
    auto form=transForm(a,bound);validTransBase(a,form.function);
    const auto other=transFunction(form.other);
    if(other.kind!=TransKind::None && (other.logarithm()==form.function.logarithm()) && a.same(other.base,form.function.base)){
        validTransBase(a,other);affineArgument(a,other.inner);
        if(!a.same(form.coefficient,gen(1))||!a.zero(form.remainder))throw UnsupportedInput{__LINE__,"same-function equality must be isolated"};
    }else {
        const gen target=a.normal((form.other-form.remainder)/form.coefficient);
        if(solveVariable(target,a.input))throw UnsupportedInput{__LINE__,"inverse target must be an exact real constant"};
        if(!a.rational(target)){
            composition::Work w{a.ctx,a.calls,Limits::symbolicCalls};
            if(transExponent(target)||composition::sign(w,target)==2)throw UnsupportedInput{__LINE__,"exact algebraic target undecided"};
        }
    }
    return true;
}
bool transDomainSafe(Algebra& a,const gen& g,const State& state,unsigned depth,unsigned& nodes) {
    const auto f=transFunction(g);if(f.kind==TransKind::None)return false;
    if(!domainSafe(a,f.inner,state,depth+1,nodes))return false;
    if(!f.natural()){
        if(!domainSafe(a,f.base,state,depth+1,nodes))return false;
        if(!a.presentationOnly){try{validTransBase(a,f);}catch(const UnsupportedInput&){return false;}}
    }
    if(a.presentationOnly)return true;
    if(a.input.complex)return false;
    if(!f.logarithm())return true;
    for(const auto& c:state.conditions)if(c.kind==ConditionKind::Positive&&a.same(a.parse(c.nonzero),f.inner))return true;
    return !solveVariable(f.inner,a.input)&&exactPositive(a,f.inner);
}
// Deliberately bounded rational-exponent recognition. Replay checks a^p=c^q,
// plus positive a,c, so the unique positive qth root justifies a^(p/q)=c.
bool exactPower(Algebra& a,const gen& base,const gen& target,gen& exponent) {
    if(!a.rational(base)||!a.rational(target)||!exactPositive(a,target))return false;
    for(int q=1;q<=8;++q)for(int p=-16;p<=16;++p){a.tick();if(a.same(giac::pow(base,p,a.ctx),giac::pow(target,q,a.ctx))){exponent=a.normal(gen(p)/gen(q));return true;}}
    return false;
}
