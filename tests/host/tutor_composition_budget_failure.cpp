// SPDX-License-Identifier: GPL-3.0-or-later
// A persistent allocation fault at the budget diagnostic must stay recoverable.
#include "math/giac/GiacEngine.h"
#include <cstdio>
#include <cstdlib>
#include <new>
#include <string>

namespace {
bool armed = false;
size_t attempts = 0, failAt = SIZE_MAX, failures = 0;
}
void *operator new(size_t bytes) {
    if (armed && ++attempts >= failAt) { ++failures; throw std::bad_alloc(); }
    if (void *result = std::malloc(bytes ? bytes : 1)) return result;
    throw std::bad_alloc();
}
void *operator new[](size_t bytes) { return ::operator new(bytes); }
void operator delete(void *ptr) noexcept { std::free(ptr); }
void operator delete[](void *ptr) noexcept { std::free(ptr); }
void operator delete(void *ptr, size_t) noexcept { std::free(ptr); }
void operator delete[](void *ptr, size_t) noexcept { std::free(ptr); }
bool setting_complex_enabled = false;

int main() {
    using namespace numos;
    using namespace numos::tutor;
    auto &engine = GiacEngine::instance();
    if (!engine.begin() || !engine.assign("A", "1/2").ok()) return 2;
    Snapshot input;
    input.variables = {"x"};
    input.inputEpoch = 91;
    const std::string atom = "logb(A*x+1,2)";
    input.authored = {{atom + "^2+" + atom + "^2+" + atom + "^2+" + atom + "^2-12*" + atom + "+8", "0"}};
    const auto ordinary = engine.solveStructured({input.authored[0].lhs, "0"}, "x", SolveDomainPolicy::RealOnly);
    if (!ordinary.ok()) return 3;
    size_t steadyAttempts = 0;
    for (int warm = 0; warm < 3; ++warm) {
        attempts = 0; failAt = SIZE_MAX; armed = true;
        auto trace = engine.explainEquations(input, ordinary);
        armed = false;
        if (trace.status != Status::Partial || trace.metrics.symbolicCalls != 4097) return 4;
        if (warm == 2) steadyAttempts = attempts;
    }
    if (steadyAttempts < 6) return 5;
    bool diagnosticFaultObserved = false;
    for (size_t target = steadyAttempts - 5; target <= steadyAttempts; ++target) {
        const size_t retainedBefore = traceAllocations.live;
        attempts = failures = 0; failAt = target;
        bool escaped = false;
        Status status = Status::Unsupported;
        bool emptyDiagnostic = false;
        try {
            armed = true;
            auto trace = engine.explainEquations(input, ordinary);
            armed = false;
            status = trace.status;
            emptyDiagnostic = trace.diagnostic.empty();
        } catch (const std::bad_alloc &) { armed = false; escaped = true; }
        if (escaped || status != Status::Partial || failures == 0 || traceAllocations.live != retainedBefore) {
            std::printf("FAIL target=%zu escaped=%u status=%u failures=%zu restored=%u\n",
                        target, unsigned(escaped), unsigned(status), failures,
                        unsigned(traceAllocations.live == retainedBefore));
            return 6;
        }
        if (target == steadyAttempts) diagnosticFaultObserved = emptyDiagnostic;
    }
    if (!diagnosticFaultObserved) return 7;
    // WHY: the ordinary answer survives the failed tutor, and Giac stays live.
    if (!engine.assign("A", "2").ok()) return 8;
    Snapshot healthy;
    healthy.variables = {"x"};
    healthy.inputEpoch = 92;
    healthy.authored = {{"x^4-5*x^2+4", "0"}};
    auto answer = engine.solveStructured({healthy.authored[0].lhs, "0"}, "x", SolveDomainPolicy::RealOnly);
    auto trace = engine.explainEquations(healthy, answer);
    if (!answer.ok() || trace.status != Status::Complete ||
        engine.verifyDerivation(trace, trace.input) != Verdict::Verified) return 9;
    std::printf("composition budget diagnostic fault recovered at %zu allocations; next proof verified\n", steadyAttempts);
    return 0;
}
