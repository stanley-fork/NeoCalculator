#pragma once
#include <cstdint>
#include <cstring>

namespace numos {
// WHY: this allowlist belongs to the mathematical input boundary, not the UI.
// Results may contain other NodeCalls, but authored input never executes them.
enum class InputCall : uint8_t {
    Sinh, Cosh, Tanh, Asinh, Acosh, Atanh, Real, Imag, Conj, Arg,
    Floor, Ceil, Round, Gcd, Lcm, Remainder, Binomial, Diff, Count
};
struct InputCallSpec { const char* name; uint8_t arity; };
inline constexpr InputCallSpec kInputCalls[] = {
    {"sinh",1}, {"cosh",1}, {"tanh",1}, {"asinh",1}, {"acosh",1}, {"atanh",1},
    {"re",1}, {"im",1}, {"conj",1}, {"arg",1}, {"floor",1}, {"ceil",1},
    {"round",1}, {"gcd",2}, {"lcm",2}, {"irem",2}, {"binomial",2}, {"diff",2}
};
inline const InputCallSpec* inputCall(const char* name, unsigned arity) {
    for (const auto& spec : kInputCalls)
        if (spec.arity == arity && std::strcmp(spec.name, name) == 0) return &spec;
    return nullptr;
}
static_assert(sizeof(kInputCalls)/sizeof(kInputCalls[0]) == unsigned(InputCall::Count));
}
