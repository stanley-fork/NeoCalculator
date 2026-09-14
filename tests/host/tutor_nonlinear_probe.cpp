// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/CalculationEngine.h"
#include <iostream>
#include <chrono>
using namespace numos;
using namespace vpam;
#if defined(NUMOS_GIAC_HOST_HARNESS)
bool setting_complex_enabled = true;
#endif
void tree(const EngineResultNode& n) {
    std::cout << '(' << unsigned(n.kind) << ':' << n.text;
    for (const auto& c:n.children) tree(c);
    std::cout << ')';
}
int main(int argc,char**) {
    auto& engine=GiacEngine::instance(); if(!engine.begin())return 1;
    if(argc>1) {
        using Clock=std::chrono::steady_clock;
        for(const auto& e:{SolveEquation{"abs(2*x-3)","5"},{"sqrt(x+1)","x-1"},{"2*sqrt(x+1)+1","7"}}) {
            // One process, no context reset. Sample 0 is first use, 1..5 warm
            // the context; 6..35 are fresh solves/proofs, not cached pages.
            for(unsigned i=0;i<36;++i) {
                const auto begin=Clock::now();auto answer=engine.solveStructured(e,"x",SolveDomainPolicy::RealOnly);
                const auto solved=Clock::now();
                tutor::Snapshot s;s.authored={{e.lhs,e.rhs}};s.variables={"x"};s.inputEpoch=i+1;
                auto d=engine.explainEquations(s,answer);const auto explained=Clock::now();
                if(d.status!=tutor::Status::Complete || engine.verifyDerivation(d,d.input)!=tutor::Verdict::Verified)return 3;
                std::cout<<"TIME|"<<e.lhs<<'|'<<i<<'|'
                  <<std::chrono::duration<double,std::micro>(solved-begin).count()<<'|'
                  <<std::chrono::duration<double,std::micro>(explained-solved).count()<<'\n';
            }
        }
        return 0;
    }
    for(bool radical:{false,true}) {
        auto inner=makeRow(); auto* r=static_cast<NodeRow*>(inner.get());
        r->appendChild(makeVariable('x'));r->appendChild(makeOperator(OpKind::Add));r->appendChild(makeNumber("1"));
        NodePtr authored=radical?makeRoot(std::move(inner)):makeParen(std::move(inner),DelimKind::Bar);
        std::string canonical,error;
        if(!CalculationEngine::serializeForGiac(authored.get(),canonical,error))return 2;
        std::cout<<"AUTHORED|"<<dumpTree(authored.get())<<"|canonical="<<canonical<<'\n';
    }
    for(const auto& e: {SolveEquation{"abs(2*x-3)","5"}, {"sqrt(x+1)","x-1"},
                       {"sqrt((x-1)^2)","3"}, {"abs(abs(x))","3"}, {"sqrt(x^2)","x"}}) {
        auto value=engine.evaluateStructured(e.lhs.c_str());
        std::cout<<"STRUCTURE|"<<e.lhs<<"|exact="<<value.base.exactText<<'|';tree(value.tree);std::cout<<'\n';
        auto answer=engine.solveStructured(e,"x",SolveDomainPolicy::RealOnly);
        tutor::Snapshot snapshot;snapshot.authored={{e.lhs,e.rhs}};snapshot.variables={"x"};snapshot.inputEpoch=1;
        auto d=engine.explainEquations(snapshot,answer);
        std::cout<<"TUTOR|"<<e.lhs<<'='<<e.rhs<<"|answer="<<unsigned(answer.setKind)<<"|status="<<unsigned(d.status)<<"|reason="<<d.diagnostic<<'\n';
    }
}
