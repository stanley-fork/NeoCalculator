// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <lvgl.h>
LV_FONT_DECLARE(montserrat_es_10);
LV_FONT_DECLARE(montserrat_es_12);
LV_FONT_DECLARE(montserrat_es_14);
namespace ui {
// WHY: retain the accepted ASCII geometry and kerning. Only missing Spanish
// glyphs use the small fallback subset. No layout/render allocation or STIX change.
inline const lv_font_t* tutorFont10() {
    static const lv_font_t font=[] { auto f=lv_font_montserrat_10; f.fallback=&montserrat_es_10; return f; }();
    return &font;
}
inline const lv_font_t* tutorFont12() {
    static const lv_font_t font=[] { auto f=lv_font_montserrat_12; f.fallback=&montserrat_es_12; return f; }();
    return &font;
}
inline const lv_font_t* tutorFont14() {
    static const lv_font_t font=[] { auto f=lv_font_montserrat_14; f.fallback=&montserrat_es_14; return f; }();
    return &font;
}
}
