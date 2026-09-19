// SPDX-License-Identifier: GPL-3.0-or-later
// Host diagnostics only. Logging and extra independent replay are outside timers.
#include "math/giac/GiacEngine.h"
#include <chrono>
#include <cstdio>
bool setting_complex_enabled=false;
int main(){using namespace numos;using namespace numos::tutor;
    using Clock=std::chrono::steady_clock;
    auto& engine=GiacEngine::instance();if(!engine.begin())return 1;
    for(const auto& equation:std::initializer_list<Equation>{{"x","1"},{"2*x^2+3*x-4","0"},
            {"abs(2*x-3)","5"},{"sqrt(x+1)","x-1"},{"exp(2*x-1)","exp(3)"},
            {"2^x","8"},{"ln(x-1)","2"},{"logb(x,2)","3"},
            {"sin(x)","1/2"},{"sin(2*x)","1/2"},{"cos(x)","1/2"},
            {"tan(3*x)","1"},{"sin(3*x-1)","1/3"}}){
        Snapshot input;input.authored={equation};input.variables={"x"};input.inputEpoch=1;
        for(unsigned sample=0;sample<36;++sample){
            const auto start=Clock::now();
            auto answer=engine.solveStructured({equation.lhs,equation.rhs},"x",SolveDomainPolicy::RealOnly);
            const auto solved=Clock::now();auto trace=engine.explainEquations(input,answer);const auto built=Clock::now();
            if(trace.status!=Status::Complete||engine.verifyDerivation(trace,trace.input)!=Verdict::Verified)return 2;
            std::printf("TIMING|lhs=%s|rhs=%s|sample=%u|phase=%s|ordinary_us=%lld|generation_us=%lld|calls=%u|retained=%zu|vectors_peak=%zu\n",
                equation.lhs.c_str(),equation.rhs.c_str(),sample,sample==0?"first":sample<6?"warmup":"steady",
                (long long)std::chrono::duration_cast<std::chrono::microseconds>(solved-start).count(),
                (long long)std::chrono::duration_cast<std::chrono::microseconds>(built-solved).count(),
                trace.metrics.symbolicCalls,retainedBytes(trace),size_t(trace.metrics.peakVectorHeapBytes));
        }
    }return 0;
}
