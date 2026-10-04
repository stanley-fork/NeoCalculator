// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>

namespace numos::i18n {
// WHY: persisted IDs are stable. The former English/Spanish settings retain
// bytes 0/1; the proof renderer's French/Pseudo test locales retain 2/3.
enum class Locale : uint8_t {
    EnglishUS = 0, English = EnglishUS,
    SpanishSpain = 1, Spanish = SpanishSpain,
    French = 2, Pseudo = 3,
    EnglishUK = 4, SpanishLatinAmerica = 5
};

// System presentation preference, never part of a mathematical snapshot.
inline Locale productLocale = Locale::EnglishUS;
inline constexpr Locale productLocales[] = {
    Locale::EnglishUS, Locale::EnglishUK,
    Locale::SpanishSpain, Locale::SpanishLatinAmerica
};
inline constexpr uint8_t productLocaleCount = 4;

constexpr bool isSpanish(Locale locale) {
    return locale == Locale::SpanishSpain || locale == Locale::SpanishLatinAmerica;
}
constexpr bool isEnglishUK(Locale locale) { return locale == Locale::EnglishUK; }
constexpr Locale baseLocale(Locale locale) {
    return isSpanish(locale) ? Locale::Spanish :
           locale == Locale::EnglishUK ? Locale::English : locale;
}
constexpr bool isProductLocale(Locale locale) {
    return locale == Locale::EnglishUS || locale == Locale::EnglishUK || isSpanish(locale);
}
constexpr Locale storedLocale(uint8_t value) {
    const auto locale = static_cast<Locale>(value);
    return isProductLocale(locale) ? locale : Locale::EnglishUS;
}
constexpr uint8_t localeStorageValue(Locale locale) {
    return static_cast<uint8_t>(isProductLocale(locale) ? locale : Locale::EnglishUS);
}
inline uint8_t localeStorageValue() { return localeStorageValue(productLocale); }
constexpr uint8_t productLocaleIndex(Locale locale) {
    return locale == Locale::EnglishUK ? 1 :
           locale == Locale::SpanishSpain ? 2 :
           locale == Locale::SpanishLatinAmerica ? 3 : 0;
}
constexpr Locale adjacentLocale(Locale locale, bool forward) {
    return productLocales[(productLocaleIndex(locale) + (forward ? 1 : 3)) % productLocaleCount];
}
constexpr const char* localeTag(Locale locale) {
    switch (locale) {
        case Locale::EnglishUK: return "en-GB";
        case Locale::SpanishSpain: return "es-ES";
        case Locale::SpanishLatinAmerica: return "es-419";
        case Locale::French: return "fr";
        case Locale::Pseudo: return "en-XA";
        default: return "en-US";
    }
}
constexpr const char* localeDisplayName(Locale locale) {
    switch (locale) {
        case Locale::EnglishUK: return "English (UK)";
        case Locale::SpanishSpain: return "Español (España)";
        case Locale::SpanishLatinAmerica: return "Español (Latinoamérica)";
        default: return "English (US)";
    }
}
} // namespace numos::i18n
