// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <lvgl.h>
#include "MathTypography.h"
#include "MathTextNormalization.h"
#include "../math/MathTypography.h"

namespace ui {

// WHY: Math input/serialization stores ASCII '-', but its presentation uses the
// STIX mathematical minus already present at every script size. Resolve after
// UTF-8 decoding: no string expansion/buffer limit, AST edit, or new font.
constexpr uint32_t mathTextCodepoint(uint32_t cp) {
    return cp == 0x002D ? 0x2212 : cp;
}

// One glyph resolution contract for measurement and drawing. The descriptor's
// advance already includes LVGL's integer rounding and within-run pair kerning.
// Ink offsets/overhangs are deliberately not folded into this advance.
inline bool mathTextGlyph(const lv_font_t* font, uint32_t cp, uint32_t next,
                          lv_font_glyph_dsc_t& glyph, const lv_font_t*& selected) {
    cp = mathTextCodepoint(cp);
    next = mathTextCodepoint(next);
    selected = mathGlyphFont(font, cp);
    return lv_font_get_glyph_dsc(selected, &glyph, cp, next);
}
inline int16_t missingMathGlyphAdvance(const lv_font_t* font) {
    return std::max<int16_t>(1, font->line_height / 3);
}

inline int16_t measureMathTextAdvance(const void* binding, const char* text) {
    const auto* font = static_cast<const lv_font_t*>(binding);
    char normalized[numos::mathsym::kNormalizedTextBufferBytes];
    const auto result = numos::mathsym::normalizeMathTextNoAlloc(text, normalized, sizeof(normalized));
    const auto* p = reinterpret_cast<const uint8_t*>(result.text ? result.text : text);
    int32_t width = 0;
    while (p && *p) {
        uint32_t cp = 0, next = 0;
        const auto step = vpam::utf8Decode(p, cp);
        vpam::utf8Decode(p + step, next);
        lv_font_glyph_dsc_t glyph{};
        const lv_font_t* selected;
        width += mathTextGlyph(font, cp, next, glyph, selected)
            ? glyph.adv_w : missingMathGlyphAdvance(font);
        p += step;
    }
    return static_cast<int16_t>(std::min<int32_t>(32767, width));
}

} // namespace ui
