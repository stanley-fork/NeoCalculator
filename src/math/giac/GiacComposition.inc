// SPDX-License-Identifier: GPL-3.0-or-later
// Private, neutral admission certificate. It never solves an equation and has
// no dependency on tutor states, roots or families. Both consumers replay the
// same bounded structural substitution against the authored tree.
namespace composition {
using giac::gen;
enum class Kind : uint8_t { Square, Exp, Power, Ln, Log10, LogBase, Sin, Cos, Tan };
struct Declined { unsigned line=0; };
struct BudgetExceeded {};
struct Work {
    giac::context* ctx;
    unsigned& calls;
    unsigned limit, nodes=0;
    void tick(){if(calls>=limit)throw BudgetExceeded{};++calls;}
    gen normal(const gen& g){tick();return giac::ratnormal(g.eval(1,ctx),ctx);}
    bool same(const gen& a,const gen& b){return a==b||giac::is_zero(normal(a-b));}
    bool rational(const gen& g)const{return g.type==giac::_INT_||g.type==giac::_ZINT||
        (g.type==giac::_FRAC&&rational(g._FRACptr->num)&&rational(g._FRACptr->den));}
};
struct Form {
    Kind kind=Kind::Square;
    gen atom,inner,base=gen(0),slope,intercept;
    std::array<gen,3> coefficients;
    uint8_t side=0;
    bool periodic()const{return kind>=Kind::Sin;}
    bool logarithm()const{return kind>=Kind::Ln&&kind<=Kind::LogBase;}
};
// A deliberately small exact sign certificate for rational quadratic surds.
// Giac's general comparison may use evalf internally; it is NOT our proof here.
// Return 2 when the bounded certificate does not decide the sign.
int sign(Work& w,const gen& source,unsigned depth=0){
    if(depth>8)return 2;
    const gen g=w.normal(source);
    if(giac::is_zero(g))return 0;
    if(w.rational(g))return giac::is_strictly_positive(g,w.ctx)?1:-1;
    if(g.type!=giac::_SYMB)return 2;
    const auto op=g._SYMBptr->sommet;const auto& f=g._SYMBptr->feuille;
    if(op==giac::at_neg){const int s=sign(w,f,depth+1);return s==2?2:-s;}
    if(op==giac::at_inv)return sign(w,f,depth+1);
    if(op==giac::at_sqrt){const int s=sign(w,f,depth+1);return s==0?0:s==1?1:2;}
    if(op==giac::at_prod&&f.type==giac::_VECT){int result=1;for(const auto& v:*f._VECTptr){int s=sign(w,v,depth+1);if(s==2)return 2;result*=s;}return result;}
    if(op==giac::at_pow&&f.type==giac::_VECT&&f._VECTptr->size()==2){
        const int base=sign(w,f._VECTptr->front(),depth+1);const auto exponent=w.normal(f._VECTptr->back());
        if(exponent.type==giac::_INT_){if(!exponent.val)return 1;if(base==2||(!base&&exponent.val<0))return 2;return exponent.val%2?base:base?1:0;}
        if(w.same(exponent,gen(1)/2))return base==0?0:base==1?1:2;
    }
    // Extract r+s*sqrt(q), using exact arithmetic and the square comparison.
    gen radical,radicand;unsigned roots=0;
    auto find=[&](const gen& v,auto&& self,unsigned d)->void{
        if(d>20){roots=3;return;}
        if(v.type==giac::_SYMB){const auto fn=v._SYMBptr->sommet;const auto& leaf=v._SYMBptr->feuille;gen q;bool root=false;
            if(fn==giac::at_sqrt){q=leaf;root=true;}
            else if(fn==giac::at_pow&&leaf.type==giac::_VECT&&leaf._VECTptr->size()==2&&w.same(leaf._VECTptr->back(),gen(1)/2)){q=leaf._VECTptr->front();root=true;}
            if(root){if(!roots){radical=v;radicand=w.normal(q);roots=1;}else if(v!=radical)roots=3;return;}self(leaf,self,d+1);
        }else if(v.type==giac::_VECT)for(const auto& c:*v._VECTptr)self(c,self,d+1);
    };find(g,find,0);
    if(roots!=1||!w.rational(radicand)||sign(w,radicand,depth+1)!=1)return 2;
    w.tick();const gen r=w.normal(giac::subst(g,radical,gen(0),false,w.ctx));
    w.tick();const gen s=w.normal(giac::subst(g,radical,gen(1),false,w.ctx)-r);
    if(!w.rational(r)||!w.rational(s)||!w.same(g,r+s*radical))return 2;
    const int rs=sign(w,r,depth+1),ss=sign(w,s,depth+1);
    if(!rs)return ss;if(!ss||rs==ss)return rs;
    const int difference=sign(w,r*r-s*s*radicand,depth+1);
    return difference==2?2:difference==0?0:difference>0?rs:ss;
}
bool containsVariable(const gen& g,const gen& x,unsigned depth=0){
    if(depth>20)throw Declined{__LINE__};
    if(g==x)return true;
    if(g.type==giac::_SYMB)return containsVariable(g._SYMBptr->feuille,x,depth+1);
    if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)if(containsVariable(c,x,depth+1))return true;
    if(g.type==giac::_FRAC)return containsVariable(g._FRACptr->num,x,depth+1)||containsVariable(g._FRACptr->den,x,depth+1);
    return false;
}
bool nestedRoot(const gen& g,unsigned inside=0,unsigned depth=0){
    if(depth>20)return true;
    if(g.type==giac::_SYMB){const auto op=g._SYMBptr->sommet;const auto& f=g._SYMBptr->feuille;
        bool root=op==giac::at_sqrt;
        if(op==giac::at_pow&&f.type==giac::_VECT&&f._VECTptr->size()==2){const auto& e=f._VECTptr->back();root=e.type==giac::_FRAC&&e._FRACptr->num==gen(1)&&e._FRACptr->den==gen(2);}
        if(root&&inside)return true;return nestedRoot(f,inside+unsigned(root),depth+1);
    }
    if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)if(nestedRoot(c,inside,depth+1))return true;
    return false;
}
// Normalize arguments without entering simplify's radical-denesting routine.
// This proves, for example, sqrt(1+sqrt(2)) == sqrt(sqrt(2)+1) exactly.
gen normalizeRoots(Work& w,const gen& g,unsigned depth=0){
    if(depth>20)throw Declined{__LINE__};
    if(g.type==giac::_VECT){giac::vecteur values;values.reserve(g._VECTptr->size());
        for(const auto& v:*g._VECTptr)values.push_back(normalizeRoots(w,v,depth+1));return gen(values,g.subtype);}
    if(g.type==giac::_SYMB)return w.normal(giac::symbolic(g._SYMBptr->sommet,normalizeRoots(w,g._SYMBptr->feuille,depth+1)));
    return g;
}
bool closedAlgebraic(Work& w,const gen& g,unsigned depth=0){
    if(depth>20||++w.nodes>160)return false;
    if(w.rational(g))return true;
    if(g.type!=giac::_SYMB)return false;
    const auto op=g._SYMBptr->sommet;const auto& f=g._SYMBptr->feuille;
    if(op!=giac::at_plus&&op!=giac::at_prod&&op!=giac::at_neg&&op!=giac::at_binary_minus&&
       op!=giac::at_division&&op!=giac::at_inv&&op!=giac::at_pow&&op!=giac::at_sqrt)return false;
    if(f.type==giac::_VECT){for(const auto& v:*f._VECTptr)if(!closedAlgebraic(w,v,depth+1))return false;}
    else if(!closedAlgebraic(w,f,depth+1))return false;
    if(op==giac::at_inv){const int s=sign(w,f);return s==1||s==-1;}
    if(op==giac::at_division){if(f.type!=giac::_VECT||f._VECTptr->size()!=2)return false;const int s=sign(w,f._VECTptr->back());return s==1||s==-1;}
    if(op==giac::at_sqrt){const int s=sign(w,f);return s==0||s==1;}
    if(op==giac::at_pow){if(f.type!=giac::_VECT||f._VECTptr->size()!=2)return false;const auto& e=f._VECTptr->back();return e.type==giac::_INT_&&e.val>=0&&e.val<=4;}
    return true;
}
unsigned polynomialDegree(const gen& g,const gen& t,unsigned depth=0){
    if(depth>20)throw Declined{__LINE__};
    if(g==t)return 1;
    if(g.type==giac::_INT_||g.type==giac::_ZINT||g.type==giac::_FRAC)return 0;
    if(g.type!=giac::_SYMB)throw Declined{__LINE__};
    const auto op=g._SYMBptr->sommet;const auto& f=g._SYMBptr->feuille;
    if(op==giac::at_pow){
        if(f.type!=giac::_VECT||f._VECTptr->size()!=2||f._VECTptr->back().type!=giac::_INT_)throw Declined{__LINE__};
        const auto n=f._VECTptr->back().val;if(n<0||n>2)throw Declined{__LINE__};
        const auto degree=polynomialDegree(f._VECTptr->front(),t,depth+1)*unsigned(n);if(degree>2)throw Declined{__LINE__};return degree;
    }
    if(op==giac::at_neg)return polynomialDegree(f,t,depth+1);
    if(op==giac::at_inv){if(polynomialDegree(f,t,depth+1))throw Declined{__LINE__};return 0;}
    if(f.type!=giac::_VECT)throw Declined{__LINE__};unsigned degree=0,index=0;
    for(const auto& c:*f._VECTptr){const auto next=polynomialDegree(c,t,depth+1);
        if(op==giac::at_plus||op==giac::at_binary_minus)degree=std::max(degree,next);
        else if(op==giac::at_prod)degree+=next;
        else if(op==giac::at_division){if(index&&next)throw Declined{__LINE__};if(!index)degree=next;}
        else throw Declined{__LINE__};
        if(degree>2)throw Declined{__LINE__};++index;
    }return degree;
}
bool function(const gen& g){
    if(g.type!=giac::_SYMB)return false;const auto op=g._SYMBptr->sommet;
    return op==giac::at_exp||op==giac::at_ln||op==giac::at_log10||op==giac::at_logb||
           op==giac::at_sin||op==giac::at_cos||op==giac::at_tan;
}
// Before binding/evaluation: denominators must contain no solve variable, even
// when their numerator or a stored multiplier will later evaluate to zero.
void source(Work& w,const gen& g,const gen& x,unsigned depth=0){
    if(depth>20||++w.nodes>160)throw Declined{__LINE__};
    if(w.rational(g)||g.type==giac::_IDNT)return;
    if(g.type==giac::_VECT){for(const auto& c:*g._VECTptr)source(w,c,x,depth+1);return;}
    if(g.type!=giac::_SYMB)throw Declined{__LINE__};
    const auto op=g._SYMBptr->sommet;const auto& f=g._SYMBptr->feuille;
    if(op==giac::at_inv){if(containsVariable(f,x))throw Declined{__LINE__};}
    else if(op==giac::at_division){if(f.type!=giac::_VECT||f._VECTptr->size()!=2||containsVariable(f._VECTptr->back(),x))throw Declined{__LINE__};}
    else if(op==giac::at_pow){
        if(f.type!=giac::_VECT||f._VECTptr->size()!=2)throw Declined{__LINE__};
        const auto& exponent=f._VECTptr->back();
        if(!containsVariable(exponent,x)&&(exponent.type!=giac::_INT_||exponent.val<0||exponent.val>4))throw Declined{__LINE__};
    }else if(op!=giac::at_plus&&op!=giac::at_prod&&op!=giac::at_neg&&op!=giac::at_binary_minus&&
             op!=giac::at_sqrt&&!function(g))throw Declined{__LINE__};
    source(w,f,x,depth+1);
}
bool affineTree(Work& w,const gen& g,const gen& x,unsigned depth=0){
    if(depth>20)throw Declined{__LINE__};
    if(w.rational(g)||g==x||g==gen(giac::_IDNT_pi()))return true;
    if(g.type==giac::_VECT){for(const auto& c:*g._VECTptr)if(!affineTree(w,c,x,depth+1))return false;return true;}
    if(g.type!=giac::_SYMB)return false;
    const auto op=g._SYMBptr->sommet;const auto& f=g._SYMBptr->feuille;
    if(op==giac::at_inv)return !containsVariable(f,x)&&affineTree(w,f,x,depth+1)&&!w.same(f,0);
    if(op==giac::at_division){if(f.type!=giac::_VECT||f._VECTptr->size()!=2||containsVariable(f._VECTptr->back(),x)||w.same(f._VECTptr->back(),0))return false;}
    else if(op!=giac::at_plus&&op!=giac::at_prod&&op!=giac::at_neg&&op!=giac::at_binary_minus)return false;
    return affineTree(w,f,x,depth+1);
}
bool readAtom(Work& w,const gen& g,const gen& x,Form& f){
    if(g.type!=giac::_SYMB)return false;
    const auto op=g._SYMBptr->sommet;const auto& leaf=g._SYMBptr->feuille;
    f.atom=g;
    if(op==giac::at_pow){
        if(leaf.type!=giac::_VECT||leaf._VECTptr->size()!=2)return false;
        const auto& b=leaf._VECTptr->front();const auto& e=leaf._VECTptr->back();
        if(e.type==giac::_INT_&&(e.val==2||e.val==4)){
            f.kind=Kind::Square;f.inner=b;f.atom=giac::symbolic(giac::at_pow,giac::makesequence(b,2));
        }else if(containsVariable(e,x)){
            if(containsVariable(b,x)||!affineTree(w,b,x))return false;
            f.kind=Kind::Power;f.inner=e;f.base=w.normal(b);
        }
        else return false;
    }else if(op==giac::at_logb){
        if(leaf.type!=giac::_VECT||leaf._VECTptr->size()!=2)return false;
        const auto& base=leaf._VECTptr->back();
        if(containsVariable(base,x)||!affineTree(w,base,x))return false;
        f.kind=Kind::LogBase;f.inner=leaf._VECTptr->front();f.base=w.normal(base);
    }else{
        f.inner=leaf;
        if(op==giac::at_exp)f.kind=Kind::Exp;
        else if(op==giac::at_ln)f.kind=Kind::Ln;
        else if(op==giac::at_log10){f.kind=Kind::Log10;f.base=10;}
        else if(op==giac::at_sin)f.kind=Kind::Sin;
        else if(op==giac::at_cos)f.kind=Kind::Cos;
        else if(op==giac::at_tan)f.kind=Kind::Tan;
        else return false;
    }
    if(!affineTree(w,f.inner,x))return false;
    const gen affine=w.normal(f.inner);
    w.tick();if(!giac::is_linear_wrt(affine,x,f.slope,f.intercept,w.ctx))return false;
    f.slope=w.normal(f.slope);f.intercept=w.normal(f.intercept);
    if(!w.rational(f.slope)||w.same(f.slope,0)||(!f.periodic()&&!w.rational(f.intercept)))return false;
    if(f.kind==Kind::Power||f.kind==Kind::Log10||f.kind==Kind::LogBase){
        w.tick();if(!w.rational(f.base)||!giac::is_strictly_positive(f.base,w.ctx)||w.same(f.base,1))return false;
    }
    // A canonical affine argument survives print/parse round trips without
    // changing its algebraic identity or the original authored provenance.
    f.inner=affine;
    if(f.kind==Kind::Square)f.atom=giac::symbolic(giac::at_pow,giac::makesequence(f.inner,2));
    else if(f.kind==Kind::Power)f.atom=giac::symbolic(giac::at_pow,giac::makesequence(f.base,f.inner));
    else if(f.kind==Kind::LogBase)f.atom=giac::symbolic(giac::at_logb,giac::makesequence(f.inner,f.base));
    else f.atom=giac::symbolic(op,f.inner);
    return true;
}
void atoms(Work& w,const gen& g,const gen& x,std::vector<Form>& out,uint8_t side,unsigned depth=0){
    if(depth>20)throw Declined{__LINE__};
    Form f;if(readAtom(w,g,x,f)){
        for(const auto& old:out)if(old.atom==f.atom)return;
        if(out.size()>=8)throw Declined{__LINE__};f.side=side;out.push_back(std::move(f));return;
    }
    if(function(g))return; // no nested special-function admission
    if(g.type==giac::_SYMB)atoms(w,g._SYMBptr->feuille,x,out,side,depth+1);
    else if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)atoms(w,c,x,out,side,depth+1);
}
// Semantic identities admitted here: (ax+b)^4=((ax+b)^2)^2,
// exp(2u)=exp(u)^2, and B^(2u)=(B^u)^2 for a checked positive base.
// No trigonometric identity is inferred from an argument multiplier.
gen replace(Work& w,const gen& g,const gen& x,const Form& f,const gen& t,unsigned depth=0){
    if(depth>20)throw Declined{__LINE__};
    Form other;
    if(readAtom(w,g,x,other)&&other.kind==f.kind){
        if(f.kind==Kind::Square&&w.same(other.inner,f.inner)){
            const auto& e=g._SYMBptr->feuille._VECTptr->back();
            return e.val==4?t*t:t;
        }
        if((f.kind==Kind::Exp||f.kind==Kind::Power)){
            const bool sameBase=f.kind==Kind::Exp||w.same(other.base,f.base);
            if(sameBase&&w.same(other.inner,f.inner))return t;
            if(sameBase&&w.same(other.inner,2*f.inner))return t*t;
            if(f.kind==Kind::Power&&w.same(other.base,f.base*f.base)&&w.same(other.inner,f.inner))return t*t;
        }else if(w.same(other.inner,f.inner)&&
                 (f.kind!=Kind::LogBase||w.same(other.base,f.base)))return t;
    }
    if(w.rational(g))return g;
    if(g.type==giac::_VECT){giac::vecteur v;v.reserve(g._VECTptr->size());for(const auto& c:*g._VECTptr)v.push_back(replace(w,c,x,f,t,depth+1));return gen(v,g.subtype);}
    if(g.type!=giac::_SYMB)throw Declined{__LINE__};
    const auto op=g._SYMBptr->sommet;const auto& leaf=g._SYMBptr->feuille;
    if(op==giac::at_inv){const auto den=replace(w,leaf,x,f,t,depth+1);if(containsVariable(den,t)||!w.rational(w.normal(den))||w.same(den,0))throw Declined{__LINE__};return giac::symbolic(op,den);}
    if(op==giac::at_pow){
        if(leaf.type!=giac::_VECT||leaf._VECTptr->size()!=2||leaf._VECTptr->back().type!=giac::_INT_||leaf._VECTptr->back().val<0||leaf._VECTptr->back().val>2)throw Declined{__LINE__};
    }else if(op!=giac::at_plus&&op!=giac::at_prod&&op!=giac::at_neg&&op!=giac::at_binary_minus&&op!=giac::at_division)throw Declined{__LINE__};
    const auto result=replace(w,leaf,x,f,t,depth+1);
    if(op==giac::at_division){const auto& den=result._VECTptr->back();if(containsVariable(den,t)||!w.rational(w.normal(den))||w.same(den,0))throw Declined{__LINE__};}
    return giac::symbolic(op,result);
}
Form admit(Work& w,const gen& left,const gen& right,const gen& x,const gen& t){
    w.nodes=0;source(w,left,x);source(w,right,x);
    std::vector<Form> choices;atoms(w,left,x,choices,0);atoms(w,right,x,choices,1);
    for(auto& f:choices)try{
        const auto l=replace(w,left,x,f,t),r=replace(w,right,x,f,t);
        (void)polynomialDegree(l,t);(void)polynomialDegree(r,t);
        const auto p=w.normal(l-r);
        gen rest=p;
        for(unsigned i=0;i<3;++i){w.tick();f.coefficients[i]=w.normal(giac::subst(rest,t,gen(0),false,w.ctx));if(i==2)f.coefficients[i]=w.normal(f.coefficients[i]/2);if(!w.rational(f.coefficients[i]))throw Declined{__LINE__};w.tick();rest=giac::derive(rest,t,w.ctx);}
        if(!w.same(p,f.coefficients[0]+f.coefficients[1]*t+f.coefficients[2]*t*t))continue;
        return f;
    }catch(const Declined&){}
    throw Declined{__LINE__};
}
} // namespace composition
