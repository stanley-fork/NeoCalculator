// SPDX-License-Identifier: GPL-3.0-or-later
// Persistent C++ allocation fault: remains armed through return and teardown.
#include "math/giac/GiacEngine.h"
#include <cstdlib>
#include <new>
#include <cstdio>
namespace { bool armed=false;size_t attempts=0,failAt=SIZE_MAX,failures=0; }
void* operator new(size_t n) {
    if(armed && ++attempts>=failAt){++failures;throw std::bad_alloc();}
    if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();
}
void* operator new[](size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,size_t)noexcept{std::free(p);}
void operator delete[](void* p,size_t)noexcept{std::free(p);}
bool setting_complex_enabled=false;
int main(){using namespace numos;using namespace numos::tutor;
    auto& engine=GiacEngine::instance();if(!engine.begin())return 1;
    unsigned tested=0,bad=0;
    for(const auto& eq:std::initializer_list<Equation>{{"2*abs(x-1)+3","11"},{"abs(x^2-5)","4"},{"sqrt(x+1)","x-1"},
            {"2*exp(x)+1","7"},{"4^x","2"},{"ln(x-1)","2"},{"logb(x,2)","3"},{"2^(x+0)","8"},{"sin(3*x-1)","1/3"},{"cos(-2*x)","1"},{"tan(3*x)","1"}}) {
        Snapshot input;input.authored={eq};input.variables={"x"};input.inputEpoch=1;
        auto answer=engine.solveStructured({eq.lhs,eq.rhs},"x",SolveDomainPolicy::RealOnly);
        size_t total=SIZE_MAX;
        for(unsigned warm=0;warm<3;++warm) {
            attempts=0;failAt=SIZE_MAX;armed=true;
            {auto d=engine.explainEquations(input,answer);armed=false;if(d.status!=Status::Complete)return 2;}
            // Giac's first parser intern can add a cold allocation. Fault sites
            // refer to the warmed workload actually repeated below.
            if(attempts<total)total=attempts;
        }
        for(unsigned sample=0;sample<80;++sample) {
            failAt=sample<16?sample+1:1+(total-1)*(sample-16)/63;
            attempts=failures=0;const auto live=traceAllocations.live;bool escaped=false;Status status=Status::Unsupported;
            try {armed=true;{auto d=engine.explainEquations(input,answer);status=d.status;}armed=false;}
            catch(...){armed=false;escaped=true;}
            const auto restored=traceAllocations.live==live;
            auto healthy=engine.explainEquations(input,answer);
            const bool pass=!escaped && failures>0 && status!=Status::Complete && restored && healthy.status==Status::Complete;
            std::printf("FAULT|lhs=%s|at=%zu|of=%zu|failures=%zu|status=%u|escaped=%u|vectors_restored=%u|recovered=%u\n",eq.lhs.c_str(),failAt,total,failures,unsigned(status),unsigned(escaped),unsigned(restored),unsigned(healthy.status==Status::Complete));
            ++tested;bad+=!pass;
        }
    }
    std::printf("RESULT|tested=%u|failed=%u\n",tested,bad);return bad?1:0;
}
