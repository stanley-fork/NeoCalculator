// SPDX-License-Identifier: GPL-3.0-or-later
// Ordinary Giac output adaptation. No tutor state, planner, or theorem calls.
struct PeriodicDeclined {};
#ifdef NATIVE_SIM
// Host-only corruption/failure at the owned-answer publication boundary.
static unsigned g_periodicFault=0;
#endif
struct PeriodicWork {
    giac::context* ctx;
    unsigned calls=0,nodes=0;
    explicit PeriodicWork(giac::context* c):ctx(c){}
    void tick(){if(++calls>256)throw PeriodicDeclined{};}
    giac::gen normal(const giac::gen& g){tick();return giac::ratnormal(g,ctx);}
    bool equal(const giac::gen& a,const giac::gen& b){return giac::is_zero(normal(a-b));}
    bool rational(const giac::gen& g){return g.type==giac::_INT_||g.type==giac::_ZINT||(g.type==giac::_FRAC&&rational(g._FRACptr->num)&&rational(g._FRACptr->den));}
};
struct PeriodicScope {
    giac::context* ctx;bool all,rad,complex;size_t quoted;
    explicit PeriodicScope(giac::context* c):ctx(c),all(giac::all_trig_sol(c)),rad(giac::angle_radian(c)),complex(giac::complex_mode(c)),quoted(c->quoted_global_vars?c->quoted_global_vars->size():0){}
    ~PeriodicScope(){giac::all_trig_sol(all,ctx);giac::angle_radian(rad,ctx);giac::complex_mode(complex,ctx);if(ctx->quoted_global_vars)ctx->quoted_global_vars->resize(quoted);}
    void quote(const giac::gen& g){if(!ctx->quoted_global_vars)throw PeriodicDeclined{};ctx->quoted_global_vars->push_back(g);}
};
bool periodicAtom(const giac::gen& g){return g.type==giac::_SYMB&&(g._SYMBptr->sommet==giac::at_sin||g._SYMBptr->sommet==giac::at_cos||g._SYMBptr->sommet==giac::at_tan);}
bool periodicSyntax(const giac::gen& g,unsigned depth=0){
    if(depth>PeriodicLimits::depth)return true;
    if(periodicAtom(g))return true;
    if(g.type==giac::_SYMB)return periodicSyntax(g._SYMBptr->feuille,depth+1);
    if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)if(periodicSyntax(c,depth+1))return true;
    return false;
}
// The authored walk is BEFORE eval/substitution: even 0*x/x and A*x/x with
// A=0 retain their forbidden variable denominator. Reuses the 02C discipline.
bool periodicSource(PeriodicWork& w,const giac::gen& g,const giac::gen& variable,
                    bool allowVariable,std::vector<SolveBinding>& bindings,unsigned depth=0){
    if(depth>PeriodicLimits::depth||++w.nodes>PeriodicLimits::nodes)throw PeriodicDeclined{};
    if(w.rational(g)||g==giac::gen(giac::_IDNT_pi()))return true;
    if(g.type==giac::_IDNT){
        if(g==variable)return allowVariable;
        const auto name=g.print(w.ctx);
        if(name.size()!=1||name[0]<'A'||name[0]>'F')return false;
        w.tick();const auto value=g.eval(1,w.ctx);if(!w.rational(value))return false;
        for(const auto& b:bindings)if(b.name==name)return b.value==value.print(w.ctx);
        bindings.push_back({name,value.print(w.ctx)});return true;
    }
    if(g.type==giac::_VECT){for(const auto& c:*g._VECTptr)if(!periodicSource(w,c,variable,allowVariable,bindings,depth+1))return false;return true;}
    if(g.type!=giac::_SYMB)return false;
    const auto op=g._SYMBptr->sommet;const auto& f=g._SYMBptr->feuille;
    if(op==giac::at_inv){if(!periodicSource(w,f,variable,false,bindings,depth+1))return false;w.tick();const auto v=f.eval(1,w.ctx);return !giac::is_undef(v)&&!giac::is_inf(v)&&!w.equal(v,0);}
    if(op==giac::at_division){
        if(f.type!=giac::_VECT||f._VECTptr->size()!=2)return false;
        if(!periodicSource(w,f._VECTptr->back(),variable,false,bindings,depth+1)||!periodicSource(w,f._VECTptr->front(),variable,allowVariable,bindings,depth+1))return false;
        w.tick();const auto den=f._VECTptr->back().eval(1,w.ctx);return !giac::is_undef(den)&&!giac::is_inf(den)&&!w.equal(den,0);
    }
    if(op==giac::at_pow){
        if(!allowVariable||f.type!=giac::_VECT||f._VECTptr->size()!=2)return false;
        const auto& exponent=f._VECTptr->back();if(exponent.type!=giac::_INT_||exponent.val<0||exponent.val>2)return false;
        return periodicSource(w,f._VECTptr->front(),variable,true,bindings,depth+1);
    }
    return (op==giac::at_plus||op==giac::at_prod||op==giac::at_neg||op==giac::at_binary_minus)&&periodicSource(w,f,variable,allowVariable,bindings,depth+1);
}
uint64_t periodicHash(uint64_t h,const std::string& s){for(unsigned char c:s){h^=c;h*=1099511628211ull;}h^=255;return h*1099511628211ull;}
size_t periodicTreeBytes(const EngineResultNode& n,unsigned depth=0){
    if(depth>PeriodicLimits::depth)throw PeriodicDeclined{};
    size_t bytes=n.text.capacity()+1+n.children.capacity()*sizeof(EngineResultNode);
    for(const auto& c:n.children)bytes+=periodicTreeBytes(c,depth+1);return bytes;
}
void periodicTree(const giac::gen& g,giac::context* ctx,EngineResultNode& node){
    int budget=PeriodicLimits::nodes;genToNode(g,ctx,node,0,budget);
    if(firstFallbackReason(node)!=EngineFallbackReason::None)throw PeriodicDeclined{};
}
// Match exactly an observed producer identifier, not its spelling. No user
// identifier, name pattern, or sample root can create integer semantics here.
void periodicOutputSymbols(const giac::gen& g,const giac::vecteur& parameters,unsigned depth=0,unsigned* remaining=nullptr){
    unsigned local=PeriodicLimits::nodes;if(!remaining)remaining=&local;
    if(depth>PeriodicLimits::depth||!*remaining)throw PeriodicDeclined{};--*remaining;
    if(g.type==giac::_IDNT){if(g==giac::gen(giac::_IDNT_pi()))return;for(const auto& p:parameters)if(g==p)return;throw PeriodicDeclined{};}
    if(g.type==giac::_SYMB){
        const auto op=g._SYMBptr->sommet;
        if(op!=giac::at_plus&&op!=giac::at_prod&&op!=giac::at_neg&&op!=giac::at_binary_minus&&
           op!=giac::at_division&&op!=giac::at_inv&&op!=giac::at_pow&&op!=giac::at_sqrt&&
           op!=giac::at_asin&&op!=giac::at_acos&&op!=giac::at_atan)throw PeriodicDeclined{};
        periodicOutputSymbols(g._SYMBptr->feuille,parameters,depth+1,remaining);
    }
    else if(g.type==giac::_VECT)for(const auto& c:*g._VECTptr)periodicOutputSymbols(c,parameters,depth+1,remaining);
    else if(g.type==giac::_FRAC){periodicOutputSymbols(g._FRACptr->num,parameters,depth+1,remaining);periodicOutputSymbols(g._FRACptr->den,parameters,depth+1,remaining);}
    else if(g.type!=giac::_INT_&&g.type!=giac::_ZINT)throw PeriodicDeclined{};
}
bool periodicTreeEqual(const EngineResultNode& a,const EngineResultNode& b){
    if(a.kind!=b.kind||a.text!=b.text||a.fallbackReason!=b.fallbackReason||
       a.rows!=b.rows||a.columns!=b.columns||a.leftClosed!=b.leftClosed||
       a.rightClosed!=b.rightClosed||a.children.size()!=b.children.size())return false;
    for(size_t i=0;i<a.children.size();++i)if(!periodicTreeEqual(a.children[i],b.children[i]))return false;
    return true;
}
// Replay the conversion against EVERY unmodified solver branch before it can
// acquire complete scope. The permitted loss is only an exactly duplicate coset.
bool periodicConversionReplay(PeriodicWork& w,const giac::vecteur& raw,const giac::gen& parameter,
        const giac::gen& variable,uint64_t scope,bool degrees,const StructuredSolveResult& answer,
        const PeriodicRestriction* restriction){
    if(answer.origin!=SolveOrigin::GiacAllTrig||answer.coverage!=SolveCoverage::PeriodicComplete||
       answer.families.empty()||answer.families.size()>PeriodicLimits::families||
       answer.binderScope!=scope||answer.restrictions.size()!=(restriction?1u:0u))return false;
    if(restriction&&(answer.restrictions[0].side!=restriction->side||
       answer.restrictions[0].exactText!=restriction->exactText||
       !periodicTreeEqual(answer.restrictions[0].expression,restriction->expression)))return false;
    unsigned covered=0;
    for(const auto& branch:raw){
        giac::gen p,o;w.tick();if(!giac::is_linear_wrt(branch,parameter,p,o,w.ctx))return false;
        p=w.normal(p);o=w.normal(o);if(giac::is_strictly_positive(-p,w.ctx))p=-p;
        bool found=false;
        for(size_t i=0;i<answer.families.size();++i){const auto& f=answer.families[i];
            if(f.variable!=variable.print(w.ctx)||f.lhs!=f.variable||f.binderScope!=scope||
               f.binderId!=1||f.domain!=IntegerDomain::AllIntegers||f.degrees!=degrees)return false;
            const auto fp=giac::gen(f.period,w.ctx).eval(1,w.ctx),fo=giac::gen(f.offset,w.ctx).eval(1,w.ctx);
            if(!w.equal(p,fp))continue;
            const auto shift=w.normal((o-fo)/p);
            if(shift.type!=giac::_INT_&&shift.type!=giac::_ZINT)continue;
            EngineResultNode op,pp;periodicTree(fo,w.ctx,op);periodicTree(fp,w.ctx,pp);
            if(!periodicTreeEqual(op,f.offsetValue)||!periodicTreeEqual(pp,f.periodValue))return false;
            found=true;covered|=1u<<i;
        }
        if(!found)return false;
    }
    return covered==((1u<<answer.families.size())-1u);
}
bool tryPeriodicSolve(giac::context* ctx,const giac::gen& authoredLeft,const giac::gen& authoredRight,
                      const giac::gen& variable,bool degrees,uint32_t generation,StructuredSolveResult& output){
    try {
        PeriodicScope scope(ctx);scope.quote(variable);PeriodicWork w(ctx);
        StructuredSolveResult result;result.engineGeneration=generation;result.degrees=degrees;
        auto node=authoredLeft,target=authoredRight;unsigned side=0;
        if(!periodicAtom(node)){std::swap(node,target);side=1;}
        if(!periodicAtom(node)||!periodicSource(w,target,variable,false,result.bindings)||
           !periodicSource(w,node._SYMBptr->feuille,variable,true,result.bindings))return false;
        // Current solve-variable assumptions are not an unrestricted real domain.
        if(ctx->tabptr&&ctx->tabptr->find(variable.print(ctx).c_str())!=ctx->tabptr->end())return false;
        w.tick();const auto argument=node._SYMBptr->feuille.eval(1,ctx);
        w.tick();target=target.eval(1,ctx);target=w.normal(target);
        giac::gen slope,intercept;w.tick();
        if(!giac::is_linear_wrt(argument,variable,slope,intercept,ctx)||!w.rational(slope)||w.equal(slope,0)||!w.rational(target))return false;
        std::vector<SolveBinding> unused;if(!periodicSource(w,intercept,variable,false,unused))return false;
        const auto kind=node._SYMBptr->sommet;
        giac::gen left=authoredLeft,right=authoredRight;
        if(degrees){int budget=kSolveWalkBudget;left=explicitDegreeTrig(left,0,budget);budget=kSolveWalkBudget;right=explicitDegreeTrig(right,0,budget);}
        // All exact inverse scalars are RAD, also for DEG-authored equations.
        giac::angle_radian(true,ctx);giac::complex_mode(false,ctx);giac::all_trig_sol(true,ctx);
        giac::vecteur parameters;bool overflow=false;w.tick();
        const auto raw=giac::numos_periodic_solve(giac::makesequence(giac::symb_equal(left,right),variable),parameters,overflow,ctx);
        if(overflow||raw.type!=giac::_VECT||raw._VECTptr->size()>PeriodicLimits::families)throw PeriodicDeclined{};
        result.rawExactText=raw.print(ctx);result.origin=SolveOrigin::GiacAllTrig;result.coverage=SolveCoverage::PeriodicComplete;
        result.binderScope=periodicHash(periodicHash(1469598103934665603ull,authoredLeft.print(ctx)),authoredRight.print(ctx));
        result.binderScope=periodicHash(result.binderScope,variable.print(ctx)+std::to_string(generation)+(degrees?"deg":"rad"));
        for(const auto& b:result.bindings)result.binderScope=periodicHash(result.binderScope,b.name+"="+b.value);
        const bool impossible=kind!=giac::at_tan&&(giac::is_strictly_positive(target-1,ctx)||giac::is_strictly_positive(-1-target,ctx));
        if(raw._VECTptr->empty()){
            if(!impossible)throw PeriodicDeclined{};
            result.status=MathEngineStatus::Ok;result.setKind=SolutionSetKind::NoSolution;result.periodicCalls=w.calls;output=std::move(result);return true;
        }
        if(impossible||parameters.size()!=1)throw PeriodicDeclined{};
        for(const auto& p:parameters)scope.quote(p);
        for(const auto& rawBranch:*raw._VECTptr){
            periodicOutputSymbols(rawBranch,parameters);
            giac::gen period,offset;w.tick();
            if(!giac::is_linear_wrt(rawBranch,parameters.front(),period,offset,ctx))throw PeriodicDeclined{};
            period=w.normal(period);offset=w.normal(offset);
            periodicOutputSymbols(period,{});periodicOutputSymbols(offset,{});
            if(!w.equal(rawBranch,offset+period*parameters.front())||w.equal(period,0))throw PeriodicDeclined{};
            if(giac::is_strictly_positive(-period,ctx))period=-period;
            if(!giac::is_strictly_positive(period,ctx))throw PeriodicDeclined{};
            StructuredPeriodicFamily f;f.variable=variable.print(ctx);f.lhs=f.variable;f.offset=offset.print(ctx);f.period=period.print(ctx);f.degrees=degrees;f.binderScope=result.binderScope;
            periodicTree(offset,ctx,f.offsetValue);periodicTree(period,ctx,f.periodValue);
            // Exact same-period integer shift; never compare printed formulas.
            bool duplicate=false;for(const auto& old:result.families)if(w.equal(period,giac::gen(old.period,ctx))){const auto shift=w.normal((offset-giac::gen(old.offset,ctx))/period);duplicate|=shift.type==giac::_INT_||shift.type==giac::_ZINT;}
            if(!duplicate)result.families.push_back(std::move(f));
        }
        PeriodicRestriction condition;
        if(kind==giac::at_tan){
            condition.side=uint8_t(side);
            const auto expression=giac::symbolic(giac::at_cos,argument);condition.exactText=expression.print(ctx);periodicTree(expression,ctx,condition.expression);result.restrictions.push_back(condition);
        }
#ifdef NATIVE_SIM
        switch(g_periodicFault){
        case 1:result.families[0].offset="99";break;
        case 2:result.families[0].period="99";break;
        case 3:result.families.pop_back();break;
        case 4:result.families.push_back(result.families[0]);result.families.back().offset="99";break;
        case 5:result.restrictions.clear();break;
        case 6:result.families[0].domain=static_cast<IntegerDomain>(9);break;
        case 7:result.families[0].period="-("+result.families[0].period+")";break;
        case 8:result.families[0].offset="(-1)^k";break;
        case 9:result.families[0].periodValue.kind=EngineNodeKind::Unsupported;break;
        case 10:throw std::bad_alloc();
        case 11:w.calls=256;break;
        case 12:throw std::runtime_error("periodic publication injection");
        case 13:result.families[0].binderScope=0;break;
        default:break;
        }
#endif
        if(!periodicConversionReplay(w,*raw._VECTptr,parameters.front(),variable,result.binderScope,
                                    degrees,result,kind==giac::at_tan?&condition:nullptr))throw PeriodicDeclined{};
        size_t bytes=sizeof(result)+result.rawExactText.capacity()+result.families.capacity()*sizeof(StructuredPeriodicFamily)+result.restrictions.capacity()*sizeof(PeriodicRestriction)+result.bindings.capacity()*sizeof(SolveBinding);
        for(const auto& f:result.families)bytes+=f.variable.capacity()+f.lhs.capacity()+f.offset.capacity()+f.period.capacity()+4+periodicTreeBytes(f.offsetValue)+periodicTreeBytes(f.periodValue);
        for(const auto& c:result.restrictions)bytes+=c.exactText.capacity()+1+periodicTreeBytes(c.expression);
        for(const auto& b:result.bindings)bytes+=b.name.capacity()+b.value.capacity()+2;
        if(bytes>PeriodicLimits::payload)throw PeriodicDeclined{};
        result.periodicPayload=unsigned(bytes);result.periodicCalls=w.calls;result.status=MathEngineStatus::Ok;result.setKind=SolutionSetKind::Periodic;
        output=std::move(result);return true;
    }catch(const PeriodicDeclined&){return false;}
}

// Exact common-period quotient proof. These are residue classes of a proved
// period, not finite root samples. Unknown pair comparisons never prove mismatch.
SetComparison comparePeriodic(giac::context* ctx,const std::vector<AffinePeriodicFamily>& left,
                              const std::vector<AffinePeriodicFamily>& right){
    try {
        if(left.empty()||right.empty()||left.size()>2||right.size()>2)return SetComparison::Unknown;
        size_t inputBytes=0;
        for(const auto* set:{&left,&right})for(const auto& f:*set){
            const size_t bytes=f.offset.size()+f.period.size()+f.variable.size()+f.lhs.size();
            if(bytes>PeriodicLimits::payload-inputBytes)return SetComparison::Unknown;inputBytes+=bytes;
        }
        PeriodicScope scope(ctx);giac::angle_radian(true,ctx);PeriodicWork w(ctx);
        auto parse=[&](const std::string& text){w.tick();auto g=giac::gen(text,ctx);periodicOutputSymbols(g,{});return w.normal(g.eval(1,ctx));};
        auto positivePeriod=[&](const std::string& text){
            auto p=parse(text);
            if(giac::is_strictly_positive(-p,ctx))p=-p;
            if(!giac::is_strictly_positive(p,ctx))throw PeriodicDeclined{};
            return p;
        };
        const auto base=positivePeriod(left.front().period);
        std::array<unsigned,4> numerator{},denominator{};unsigned count=0,multiple=1;
        auto gcd=[](unsigned a,unsigned b){while(b){auto t=a%b;a=b;b=t;}return a;};
        for(const auto* set:{&left,&right})for(const auto& f:*set){
            if(f.variable!=left.front().variable||f.lhs!=f.variable||f.degrees!=left.front().degrees||f.domain!=IntegerDomain::AllIntegers)return SetComparison::Unknown;
            const auto ratio=w.normal(positivePeriod(f.period)/base);
            giac::gen n=ratio,d=1;if(ratio.type==giac::_FRAC){n=ratio._FRACptr->num;d=ratio._FRACptr->den;}
            if(n.type!=giac::_INT_||d.type!=giac::_INT_||n.val<=0||d.val<=0||n.val>64||d.val>64)return SetComparison::Unknown;
            numerator[count]=unsigned(n.val);denominator[count++]=unsigned(d.val);
            const auto factor=unsigned(n.val)/gcd(multiple,unsigned(n.val));if(multiple>64/factor)return SetComparison::Unknown;multiple*=factor;
        }
        unsigned expansion=0;for(unsigned i=0;i<count;++i){const auto n=(multiple/numerator[i])*denominator[i];if(n>PeriodicLimits::residues-expansion)return SetComparison::Unknown;expansion+=n;}
        const auto common=w.normal(base*int(multiple));std::vector<giac::gen> a,b;a.reserve(expansion);b.reserve(expansion);unsigned i=0;
        for(const auto* set:{&left,&right}){auto& dest=set==&left?a:b;for(const auto& f:*set){const auto offset=parse(f.offset),period=positivePeriod(f.period);const auto n=(multiple/numerator[i])*denominator[i];++i;for(unsigned j=0;j<n;++j)dest.push_back(offset+int(j)*period);}}
        bool unknown=false;
        for(unsigned direction=0;direction<2;++direction){const auto& from=direction?b:a;const auto& to=direction?a:b;for(const auto& x:from){bool found=false,uncertain=false;for(const auto& y:to){const auto q=w.normal((x-y)/common);if(q.type==giac::_INT_||q.type==giac::_ZINT){found=true;break;}if(!w.rational(q))uncertain=true;}
            if(!found&&!uncertain)return SetComparison::Different;if(!found)unknown=true;
        }}
        return unknown?SetComparison::Unknown:SetComparison::Equivalent;
    }catch(const PeriodicDeclined&){return SetComparison::Unknown;}
}
