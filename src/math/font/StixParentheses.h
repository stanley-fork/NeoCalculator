// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <algorithm>
#include "stix_parenthesis_ink.h"

namespace vpam {

// The supplemental font contains 13 authentic size variants and three assembly
// pieces per side, first for parentheses, then brackets. Selection uses raster metrics, so layout
// and drawing cannot disagree at a rounded OpenType design-size threshold.
inline const StixParenInk* stixParenthesisInk(int16_t em, bool bracket = false) {
    return (em >= 15 ? kStixParens18 : em >= 10 ? kStixParens12 : kStixParens8)
        + (bracket ? 32 : 0);
}

struct StixParenthesisPlan {
    int16_t height;
    int16_t width;
    uint8_t variant; // 0..12: font variant; 13: top/extender/bottom assembly
};

inline StixParenthesisPlan stixParenthesisPlan(int16_t targetHeight, int16_t em, bool bracket = false) {
    const auto* ink = stixParenthesisInk(em, bracket);
    for (uint8_t i = 0; i < 13; ++i) {
        const int16_t height = std::max(ink[i].height, ink[i + 16].height);
        if (height >= targetHeight) {
            const int16_t width = std::max({ink[i].advance, ink[i + 16].advance,
                static_cast<int16_t>(ink[i].xOffset + ink[i].width),
                static_cast<int16_t>(ink[i + 16].xOffset + ink[i + 16].width)});
            return {height, width, i};
        }
    }
    const int16_t width = std::max({ink[13].advance, ink[29].advance,
        static_cast<int16_t>(ink[13].xOffset + ink[13].width),
        static_cast<int16_t>(ink[29].xOffset + ink[29].width)});
    return {targetHeight, width, 13};
}

inline int16_t stixBracketOverlapPx(int16_t em) {
    // WHY: MATH's minimum overlap, rounded up once, prevents seams without
    // consuming a cap. The generated connector limit is checked at build time.
    static_assert(kStixBracketMinOverlapDu > 0 &&
                  kStixBracketMinOverlapDu <= kStixBracketMaxOverlapDu, "STIX connectors");
    return std::max<int16_t>(1, (kStixBracketMinOverlapDu * em + 999) / 1000);
}

} // namespace vpam
