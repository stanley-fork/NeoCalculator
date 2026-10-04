// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
#include <string_view>

namespace numos::inputsymbol {
constexpr bool latin(char c) {
    return (c>='a' && c<='z') || (c>='A' && c<='Z');
}
constexpr bool greek(uint32_t cp) {
    return (cp>=0x391 && cp<=0x3A9 && cp!=0x3A2) ||
           (cp>=0x3B1 && cp<=0x3C9) || cp==0x3D1 || cp==0x3D5 ||
           cp==0x3D6 || cp==0x3F5;
}
// Alternate Greek forms are available in STIX's mathematical italic block.
// Layout, glyph painting and OpenType italic correction share this mapping.
constexpr uint32_t glyphCodepoint(uint32_t cp) {
    switch(cp) {
        case 0x03D1:return 0x1D717;
        case 0x03D5:return 0x1D719;
        case 0x03D6:return 0x1D71B;
        case 0x03F5:return 0x1D716;
        default:return cp;
    }
}
// The authored alphabet is a closed set, not arbitrary CAS source text.
inline bool greek(std::string_view text) {
    if(text.size()!=2)return false;
    const auto a=uint8_t(text[0]),b=uint8_t(text[1]);
    return a>=0xC2 && a<=0xDF && (b&0xC0)==0x80 && greek(((a&31)<<6)|(b&63));
}
inline void utf8(uint32_t cp,char (&out)[5]) {
    out[0]=char(0xC0|(cp>>6));out[1]=char(0x80|(cp&63));out[2]=0;
}
struct Spelling {std::string_view display,cas;};
// WHY: Giac reserves e, i, I and pi. Mathematical Unicode identifiers keep
// letters distinct from constants through evaluation, Ans, storage and recall.
// These are valid single-letter identifiers, never private names in the UI.
inline constexpr Spelling kReserved[] = {
    {"e",u8"𝑒"},{"i",u8"𝑖"},{"I",u8"𝐼"},{u8"π",u8"𝜋"}
};
inline std::string_view toCas(std::string_view text) {
    for(const auto& s:kReserved)if(s.display==text)return s.cas;
    return text;
}
inline std::string_view fromCas(std::string_view text) {
    for(const auto& s:kReserved)if(s.cas==text)return s.display;
    return text;
}
}
