// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/CalculationEngine.h"
#include "giacPCH.h"
#include <iostream>
using namespace numos;using namespace vpam;
bool setting_complex_enabled=false;
void structured(const EngineResultNode& n){std::cout<<"("<<unsigned(n.kind)<<":"<<n.text;for(const auto& c:n.children)structured(c);std::cout<<")";}
void parsed(const giac::gen& g,giac::context* c,unsigned depth=0){if(depth>20)return;std::cout<<"("<<unsigned(g.type);if(g.type==giac::_SYMB){std::cout<<":"<<g._SYMBptr->sommet.ptr()->s;parsed(g._SYMBptr->feuille,c,depth+1);}else if(g.type==giac::_VECT){for(const auto& v:*g._VECTptr)parsed(v,c,depth+1);}else std::cout<<":"<<g.print(c);std::cout<<")";}
int main(){auto& e=GiacEngine::instance();if(!e.begin())return 1;
 for(unsigned i=0;i<5;++i){NodePtr n;if(i==0)n=makePower(makeConstant(ConstKind::E),makeVariable('x'));if(i==1)n=makePower(makeNumber("2"),makeVariable('x'));if(i==2)n=makeFunction(FuncKind::Ln,makeVariable('x'));if(i==3)n=makeFunction(FuncKind::Log,makeVariable('x'));if(i==4)n=makeLogBase(makeNumber("2"),makeVariable('x'));std::string text,error;if(!CalculationEngine::serializeForGiac(n.get(),text,error))return 2;std::cout<<"AUTHORED|"<<dumpTree(n.get())<<"|canonical="<<text<<"\n";}
 giac::context context; // Isolated read-only representation probe, never production UI.
 for(const char* text:{"exp(x)","e^x","(exp(1))^x","2^x","ln(x)","log(x)","log10(x)","logb(x,2)","ln(-1)","exp(ln(x))"}){giac::gen g(text,&context);std::cout<<"PARSED|"<<text<<"|";parsed(g,&context);std::cout<<"\n";auto v=e.evaluateStructured(text);std::cout<<"STRUCTURED|"<<v.base.exactText<<"|";structured(v.tree);std::cout<<"\n";auto answer=e.solveStructured({text,"2"},"x",SolveDomainPolicy::RealOnly);tutor::Snapshot s;s.authored={{text,"2"}};s.variables={"x"};s.inputEpoch=1;auto d=e.explainEquations(s,answer);std::cout<<"TUTOR|status="<<unsigned(d.status)<<"|reason="<<d.diagnostic<<"|ordinary="<<unsigned(answer.setKind);for(const auto& g:answer.groups)for(const auto& a:g.values)std::cout<<"|"<<a.exactText;std::cout<<"\n";}}
