// SPDX-License-Identifier: GPL-3.0-or-later
// Read-only production boundary plus isolated pinned-vendor representation probe.
#include "math/CalculationEngine.h"
#include "math/AngleModeRuntime.h"
#include "giacPCH.h"
#include "numos_periodic.h"
#include <iostream>
#include <lvgl.h>
#undef main
bool setting_complex_enabled=false;
static void tree(const giac::gen& g,unsigned depth=0) {
    if(depth>32){std::cout<<"DEPTH";return;}
    std::cout<<"{type:"<<unsigned(g.type)<<",subtype:"<<int(g.subtype);
    if(g.type==giac::_SYMB){std::cout<<",op:"<<g._SYMBptr->sommet.ptr()->s<<",leaf:";tree(g._SYMBptr->feuille,depth+1);}
    else if(g.type==giac::_VECT){std::cout<<",children:[";for(const auto& v:*g._VECTptr){tree(v,depth+1);std::cout<<',';}std::cout<<']';}
    else std::cout<<",value:"<<g.print();std::cout<<'}';
}
struct Flags {
    giac::context* c;bool all,radians,complex;
    explicit Flags(giac::context* p):c(p),all(giac::all_trig_sol(p)),radians(giac::angle_radian(p)),complex(giac::complex_mode(p)){}
    ~Flags(){giac::all_trig_sol(all,c);giac::angle_radian(radians,c);giac::complex_mode(complex,c);}
};
int main(){using namespace vpam;using namespace numos;std::cout.setf(std::ios::unitbuf);lv_init();auto& engine=GiacEngine::instance();if(!engine.begin())return 1;
    struct Fixture{FuncKind f;const char* fn;const char* a;const char* b;const char* target;};
    const Fixture fixtures[]={{FuncKind::Sin,"sin","1","0","0"},{FuncKind::Sin,"sin","1","0","1/2"},{FuncKind::Sin,"sin","1","0","1"},{FuncKind::Sin,"sin","1","0","-1"},{FuncKind::Sin,"sin","1","0","2"},{FuncKind::Cos,"cos","1","0","0"},{FuncKind::Cos,"cos","1","0","1/2"},{FuncKind::Cos,"cos","1","0","1"},{FuncKind::Cos,"cos","1","0","-1"},{FuncKind::Tan,"tan","1","0","1"},{FuncKind::Sin,"sin","2","0","1/2"},{FuncKind::Tan,"tan","3","0","1"},{FuncKind::Sin,"sin","3","-1","1/3"},{FuncKind::Sin,"sin","-2","1","1/2"}};
    giac::context ctx;giac::step_infolevel(&ctx)=0;
    for(bool deg:{false,true})for(const auto& f:fixtures){
        setAngleMode(deg?vpam::AngleMode::DEG:vpam::AngleMode::RAD);
        auto arg=makeRow();auto* row=static_cast<NodeRow*>(arg.get());if(f.a[0]=='-'){row->appendChild(makeNumber("0"));row->appendChild(makeOperator(OpKind::Sub));}row->appendChild(makeNumber(f.a[0]=='-'?f.a+1:f.a));row->appendChild(makeOperator(OpKind::Mul));row->appendChild(makeVariable('x'));row->appendChild(makeOperator(f.b[0]=='-'?OpKind::Sub:OpKind::Add));row->appendChild(makeNumber(f.b[0]=='-'?f.b+1:f.b));
        auto ast=makeFunction(f.f,std::move(arg));std::string lhs,error;if(!CalculationEngine::serializeForGiac(ast.get(),lhs,error)){std::cerr<<error;return 2;}
        std::cout<<"AUTHORED|"<<dumpTree(ast.get())<<"|canonical="<<lhs<<'='<<f.target<<"|degrees="<<deg<<'\n';
        const auto ordinary=engine.solveStructured({lhs,f.target},"x",SolveDomainPolicy::RealOnly);
        std::cout<<"ORDINARY|status="<<unsigned(ordinary.status)<<"|kind="<<unsigned(ordinary.setKind)<<"|raw="<<ordinary.rawExactText<<'\n';
        auto before=*ctx.tabptr;const bool oldAll=giac::all_trig_sol(&ctx),oldRad=giac::angle_radian(&ctx);
        {Flags restore(&ctx);giac::angle_radian(!deg,&ctx);giac::complex_mode(false,&ctx);giac::all_trig_sol(true,&ctx);
            const giac::gen input(lhs+"="+f.target,&ctx);std::cout<<"INPUT|";tree(input);std::cout<<'\n';
            const auto output=giac::_solve(giac::makesequence(input,giac::gen("x",&ctx)),&ctx);
            std::cout<<"ALL|"<<output.print(&ctx)<<'|';tree(output);std::cout<<'\n';
            for(const auto& id:giac::lidnt(output)){std::cout<<"IDENT|"<<id.print(&ctx)<<"|evaluated="<<id.eval(1,&ctx).print(&ctx)<<'\n';}
        }
        std::cout<<"RESTORED|flags="<<(oldAll==giac::all_trig_sol(&ctx)&&oldRad==giac::angle_radian(&ctx))<<"|symbols="<<(before==*ctx.tabptr)<<'\n';
    }
    // Isolated collision probe: reset only this executable's solver counter.
    giac::_reset_solve_counter(0,&ctx);giac::gen("n_0:=27",&ctx).eval(1,&ctx);
    {Flags restore(&ctx);giac::all_trig_sol(true,&ctx);giac::angle_radian(true,&ctx);
        auto g=giac::_solve(giac::makesequence(giac::gen("sin(x)=1/2",&ctx),giac::gen("x",&ctx)),&ctx);
        std::cout<<"COLLISION|"<<g.print(&ctx)<<"|user="<<giac::gen("n_0",&ctx).eval(1,&ctx).print(&ctx)<<'|';tree(g);std::cout<<'\n';
    }
    giac::_reset_solve_counter(0,&ctx);
    const auto symbols=*ctx.tabptr;const auto quotes=ctx.quoted_global_vars->size();
    giac::gen previous;
    for(unsigned i=0;i<3;++i){Flags restore(&ctx);giac::all_trig_sol(true,&ctx);giac::angle_radian(true,&ctx);
        giac::vecteur parameters;bool overflow=false;
        const auto result=giac::numos_periodic_solve(giac::makesequence(giac::gen("sin(x)=1/2",&ctx),giac::gen("x",&ctx)),parameters,overflow,&ctx);
        if(overflow||parameters.size()!=1||parameters[0].type!=giac::_IDNT||
           (i&&result!=previous)||*ctx.tabptr!=symbols||ctx.quoted_global_vars->size()!=quotes)return 3;
        previous=result;
        std::cout<<"PRODUCER|iteration="<<i<<"|integer="<<parameters[0].print(&ctx)<<"|user="<<parameters[0].eval(1,&ctx).print(&ctx)<<"|raw="<<result.print(&ctx)<<'\n';
    }
    setAngleMode(vpam::AngleMode::RAD);return 0;
}
