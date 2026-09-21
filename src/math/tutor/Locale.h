// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
namespace numos::tutor {
enum class Locale : uint8_t { English, Spanish, French, Pseudo };
// Presentation preference only: never part of a mathematical snapshot.
inline Locale productLocale = Locale::English;
inline Locale storedLocale(uint8_t value) { return value==1?Locale::Spanish:Locale::English; }
inline uint8_t localeStorageValue() { return productLocale==Locale::Spanish?1:0; }
}
