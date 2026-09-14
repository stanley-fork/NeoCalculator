// SPDX-License-Identifier: GPL-3.0-or-later
// Requested C++ allocation payload only: excludes C/GMP allocations, allocator
// headers and physical heap fragmentation. Real-board sampling is complementary.
#include "math/giac/GiacEngine.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>
namespace {
struct alignas(std::max_align_t) Header { std::size_t size; };
std::size_t liveBytes=0,liveBlocks=0;
}
void* operator new(std::size_t n) {
    auto* h=static_cast<Header*>(std::malloc(sizeof(Header)+(n?n:1)));
    if(!h)throw std::bad_alloc();
    h->size=n;liveBytes+=n;++liveBlocks;return h+1;
}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept {
    if(!p)return;auto* h=static_cast<Header*>(p)-1;
    liveBytes-=h->size;--liveBlocks;std::free(h);
}
void operator delete[](void* p) noexcept {::operator delete(p);}
void operator delete(void* p,std::size_t) noexcept {::operator delete(p);}
void operator delete[](void* p,std::size_t) noexcept {::operator delete(p);}
bool setting_complex_enabled=false;
int main() {
    using namespace numos;
    auto& engine=GiacEngine::instance();if(!engine.begin())return 1;
    struct Case {const char* lhs;const char* rhs;const char* first;const char* second;};
    const Case cases[]={
        {"sqrt(x)","3","9",nullptr},
        {"sqrt(x+1)","x-1","3",nullptr},
        {"sqrt(2*x+3)","x","3",nullptr},
        {"2*sqrt(x+1)+1","7","8",nullptr},
        {"sqrt((x-1)^2)","3","-2","4"},
        {"sqrt(x)","-3",nullptr,nullptr},
        {"3*x+5","20","5",nullptr},
        {"abs(2*x-3)","5","-1","4"}
    };
    unsigned checks=0,failures=0;
    // The 02A radical theorem and authored-square-root adapter are real-domain
    // features. Exercise that contract without imposing a new complex solver.
    for(auto policy:{SolveDomainPolicy::RealOnly}) {
        for(const auto& c:cases) {
            const SolveEquation equation{c.lhs,c.rhs};
            auto run=[&] {
                const auto r=engine.solveStructured(equation,"x",policy);
                const unsigned count=(c.first?1u:0u)+(c.second?1u:0u);
                if(r.status!=MathEngineStatus::Ok || r.groups.size()!=count)return false;
                bool first=!c.first,second=!c.second;
                for(const auto& group:r.groups) {
                    if(group.values.size()!=1 || group.values[0].variable!="x")return false;
                    const auto& v=group.values[0].exactText;
                    if(c.first && v==c.first)first=true;
                    else if(c.second && v==c.second)second=true;
                    else return false;
                }
                return first&&second;
            };
            // Warm normal library intern/capacity initialization. Never reset
            // Giac or its auxiliary-name counter to make the test pass.
            for(unsigned i=0;i<3;++i)if(!run()) {
                const auto r=engine.solveStructured(equation,"x",policy);
                std::printf("WARMUP_FAIL|%s=%s|policy=%u|status=%u|groups=%zu\n",c.lhs,c.rhs,unsigned(policy),unsigned(r.status),r.groups.size());
                for(const auto& group:r.groups)for(const auto& v:group.values)
                    std::printf("VALUE|%s=%s\n",v.variable.c_str(),v.exactText.c_str());
                return 2;
            }
            const auto bytes=liveBytes,blocks=liveBlocks;
            for(unsigned i=0;i<20;++i) {
                const bool valid=run(),stable=liveBytes==bytes&&liveBlocks==blocks;
                ++checks;failures+=!valid||!stable;
                if(!valid||!stable)std::printf("FAIL|%s=%s|policy=%u|iteration=%u|bytes=%lld|blocks=%lld\n",c.lhs,c.rhs,unsigned(policy),i,(long long)liveBytes-(long long)bytes,(long long)liveBlocks-(long long)blocks);
            }
        }
    }
    std::printf("RADICAL_LIFETIME|checks=%u|failures=%u|scope=requested_C++_payload\n",checks,failures);
    return failures?1:0;
}
