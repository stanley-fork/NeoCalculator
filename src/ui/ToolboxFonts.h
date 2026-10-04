// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "TutorFonts.h"
#include "MathTypography.h"
#include "../math/InputSymbols.h"

namespace ui::toolbox {
inline const lv_font_t* queryFont() {
    // WHY: a search can mix Spanish text and Greek aliases. Keep the existing
    // UI face, with a static STIX fallback and the same alternate-form mapping
    // as MathCanvas. The copied STIX descriptor keeps bitmap lookup consistent.
    static const lv_font_t greek=[] {
        auto f=*ui::mathScriptFont();
        f.get_glyph_dsc=[](const lv_font_t* font,lv_font_glyph_dsc_t* glyph,uint32_t cp,uint32_t next) {
            return ui::mathScriptFont()->get_glyph_dsc(font,glyph,
                numos::inputsymbol::glyphCodepoint(cp),numos::inputsymbol::glyphCodepoint(next));
        };
        f.fallback=ui::mathGlyphFont(ui::mathScriptFont(),0x394);
        return f;
    }();
    static const lv_font_t spanish=[] {auto f=montserrat_es_12;f.fallback=&greek;return f;}();
    static const lv_font_t query=[] {auto f=lv_font_montserrat_12;f.fallback=&spanish;return f;}();
    return &query;
}
}
