// SPDX-License-Identifier: GPL-3.0-or-later
// Isolated vendor context: never changes the production answer adapter.
#include "math/CalculationEngine.h"
#include "math/AngleModeRuntime.h"
#include "giacPCH.h"
#include <iostream>
#include <lvgl.h>
#undef main
bool setting_complex_enabled=false;
void parsed(const giac::gen& g,unsigned depth=0){if(depth>20)return;std::cout<<'('<<unsigned(g.type);if(g.type==giac::_SYMB){std::cout<<':'<<g._SYMBptr->sommet.ptr()->s;parsed(g._SYMBptr->feuille,depth+1);}else if(g.type==giac::_VECT)for(const auto& v:*g._VECTptr)parsed(v,depth+1);else std::cout<<':'<<g.print();std::cout<<')';}
int main(){std::cout.setf(std::ios::unitbuf);lv_init();using namespace numos;using namespace vpam;auto& engine=GiacEngine::instance();if(!engine.begin())return 1;
 for(auto f:{FuncKind::Sin,FuncKind::Cos,FuncKind::Tan,FuncKind::ArcSin,FuncKind::ArcCos,FuncKind::ArcTan}){auto n=makeFunction(f,makeVariable('x'));std::string s,e;CalculationEngine::serializeForGiac(n.get(),s,e);std::cout<<"AST|"<<dumpTree(n.get())<<"|"<<s<<'\n';}
 std::cout<<"CONSTANTS|pi="<<giac::cst_pi.print()<<"|two="<<giac::cst_two_pi.print()<<"|half="<<giac::cst_pi_over_2.print()<<'\n';
 for(bool deg:{false,true}){setAngleMode(deg?vpam::AngleMode::DEG:vpam::AngleMode::RAD);giac::context ctx;giac::angle_radian(!deg,&ctx);giac::step_infolevel(&ctx)=0;
  for(const char* text:{"sin(x)=0","sin(x)=1/2","cos(x)=1/2","tan(x)=1","sin(2*x)=1/2","asin(1/3)","acos(1/2)","atan(1)"}){
   giac::gen g(text,&ctx);std::cout<<"PARSED|"<<deg<<'|'<<text<<'|';parsed(g);std::cout<<'\n';
   auto v=engine.evaluateStructured(text);std::cout<<"VALUE|"<<v.base.exactText<<"|kind="<<unsigned(v.tree.kind)<<'\n';
   const std::string s(text);auto at=s.find('=');if(at==std::string::npos)continue;
   auto answer=engine.solveStructured({s.substr(0,at),s.substr(at+1)},"x",SolveDomainPolicy::RealOnly);
   std::cout<<"ORDINARY|"<<deg<<'|'<<text<<"|set="<<unsigned(answer.setKind);for(const auto& t:answer.groups)for(const auto& x:t.values)std::cout<<'|'<<x.exactText;std::cout<<'\n';
   for(bool all:{false,true}){const bool saved=giac::all_trig_sol(&ctx);giac::all_trig_sol(all,&ctx);const auto result=giac::_solve(giac::makesequence(g,giac::gen("x",&ctx)),&ctx);giac::all_trig_sol(saved,&ctx);std::cout<<"PUBLIC|"<<deg<<"|all="<<all<<'|'<<result.print(&ctx)<<'|';parsed(result);std::cout<<'\n';}
  }
 }setAngleMode(vpam::AngleMode::RAD);return 0;
}
