// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <algorithm>
#include <cstdint>

namespace vpam {
// Pixel viewport policy only: it never changes typographic dimensions.
inline int16_t viewportOffset(int32_t requested, int32_t excess) {
    return static_cast<int16_t>(std::max(-std::max<int32_t>(0, excess),
                                        std::min<int32_t>(0, requested)));
}

inline int16_t followViewport(int16_t offset, int32_t start, int32_t end,
                              int32_t visibleStart, int32_t visibleEnd,
                              int32_t excess) {
    int32_t requested = offset;
    if (start < visibleStart) requested += visibleStart - start;
    else if (end > visibleEnd) requested -= end - visibleEnd;
    return viewportOffset(requested, excess);
}
} // namespace vpam
