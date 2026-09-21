// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
#include <string>

namespace numos {
// A bound integer is not a parser identifier or a stored scalar. Its display
// spelling is chosen by the view; alpha-renaming does not change the set.
enum class IntegerDomain : uint8_t { AllIntegers };
struct AffinePeriodicFamily {
    std::string variable, lhs, offset, period;
    uint64_t binderScope = 0;
    uint16_t binderId = 1;
    IntegerDomain domain = IntegerDomain::AllIntegers;
    bool degrees = false;
};
enum class SetComparison : uint8_t { Unknown, Equivalent, Different };
struct PeriodicLimits {
    static constexpr unsigned families = 2;
    static constexpr unsigned residues = 64;
    static constexpr unsigned nodes = 512;
    static constexpr unsigned depth = 32;
    static constexpr unsigned payload = 16384;
};
}
