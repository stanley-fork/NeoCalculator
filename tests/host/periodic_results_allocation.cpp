// SPDX-License-Identifier: GPL-3.0-or-later
// Requested C++ payload only; excludes C/GMP, allocator headers and fragmentation.
#include "math/giac/GiacEngine.h"
#include <cstdlib>
#include <new>
#include <cstdio>
#include <cstddef>
namespace {
struct alignas(std::max_align_t) Header{size_t n;};
bool armed=false;size_t attempts=0,failAt=SIZE_MAX,failures=0,live=0,peak=0;
}
void* operator new(size_t n){
    if(armed&&++attempts>=failAt){++failures;throw std::bad_alloc();}
    auto* p=static_cast<Header*>(std::malloc(sizeof(Header)+(n?n:1)));if(!p)throw std::bad_alloc();
    p->n=n;live+=n;if(live>peak)peak=live;return p+1;
}
void* operator new[](size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{if(p){auto* h=static_cast<Header*>(p)-1;live-=h->n;std::free(h);}}
void operator delete[](void* p)noexcept{::operator delete(p);}
void operator delete(void* p,size_t)noexcept{::operator delete(p);}
void operator delete[](void* p,size_t)noexcept{::operator delete(p);}
bool setting_complex_enabled=false;
int main(){using namespace numos;auto& e=GiacEngine::instance();if(!e.begin())return 2;
    unsigned bad=0,total=0;
    for(const SolveEquation input: {SolveEquation{"sin(3*x-1)","1/3"},SolveEquation{"tan(3*x)","1"},SolveEquation{"sin(x)","2"}}){
        size_t count=SIZE_MAX;
        for(unsigned i=0;i<3;++i){attempts=0;failAt=SIZE_MAX;armed=true;{auto a=e.solveStructured(input,"x",SolveDomainPolicy::RealOnly);armed=false;if(a.coverage!=SolveCoverage::PeriodicComplete)return 3;}if(attempts<count)count=attempts;}
        const auto state=e.debugPeriodicContextState();
        for(unsigned i=0;i<64;++i){failAt=i<16?i+1:1+(count-1)*(i-16)/47;attempts=failures=0;bool escaped=false;MathEngineStatus status=MathEngineStatus::Ok;
            try{armed=true;{auto a=e.solveStructured(input,"x",SolveDomainPolicy::RealOnly);status=a.status;}armed=false;}catch(...){armed=false;escaped=true;}
            const bool restored=state==e.debugPeriodicContextState();
            auto healthy=e.solveStructured(input,"x",SolveDomainPolicy::RealOnly);
            const bool pass=!escaped&&failures&&status!=MathEngineStatus::Ok&&restored&&healthy.coverage==SolveCoverage::PeriodicComplete;
            std::printf("FAULT|lhs=%s|at=%zu|of=%zu|failures=%zu|escaped=%u|status=%u|restored=%u|recovered=%u\n",input.lhs.c_str(),failAt,count,failures,unsigned(escaped),unsigned(status),unsigned(restored),unsigned(healthy.coverage==SolveCoverage::PeriodicComplete));
            ++total;bad+=!pass;
        }
        const auto before=live;peak=live;
        for(unsigned i=0;i<30;++i){auto a=e.solveStructured(input,"x",SolveDomainPolicy::RealOnly);if(a.coverage!=SolveCoverage::PeriodicComplete)return 4;}
        std::printf("PAYLOAD|lhs=%s|peak_extra=%zu|post_solve_delta=%lld\n",input.lhs.c_str(),peak-before,static_cast<long long>(live)-static_cast<long long>(before));
        if(live>before)++bad;
    }
    std::printf("RESULT|cases=%u|failed=%u\n",total,bad);return bad?1:0;
}
