// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include "../i18n/Locale.h"

namespace numos::settings {
// Existing ST01 web record, shared by the native emulator. Explicit bytes
// avoid host endianness or C++ structure padding becoming a storage contract.
inline constexpr std::size_t kCompactSettingsSize = 10;
struct CompactSettings {
    bool angleDeg = false;
    bool complexEnabled = false;
    bool educationEnabled = false;
    uint8_t precision = 10;
    i18n::Locale locale = i18n::Locale::EnglishUS;
};
constexpr bool compactPrecisionValid(uint8_t value) {
    return value == 6 || value == 8 || value == 10 || value == 12;
}
inline std::array<uint8_t, kCompactSettingsSize> encodeCompactSettings(const CompactSettings& state) {
    return {{0x31, 0x30, 0x54, 0x53, 1,
        static_cast<uint8_t>(state.angleDeg), static_cast<uint8_t>(state.complexEnabled),
        static_cast<uint8_t>(state.educationEnabled),
        compactPrecisionValid(state.precision) ? state.precision : uint8_t(10),
        i18n::localeStorageValue(state.locale)}};
}
inline bool decodeCompactSettings(const uint8_t* bytes, std::size_t size, CompactSettings& state) {
    if (!bytes || size != kCompactSettingsSize || bytes[0] != 0x31 || bytes[1] != 0x30 ||
        bytes[2] != 0x54 || bytes[3] != 0x53 || bytes[4] != 1 ||
        bytes[5] > 1 || bytes[6] > 1 || bytes[7] > 1 || !compactPrecisionValid(bytes[8])) return false;
    // WHY: invalid/partial records leave all live settings untouched. An unknown
    // language ID has the established English fallback, never an internal test locale.
    state = {bytes[5] != 0, bytes[6] != 0, bytes[7] != 0, bytes[8], i18n::storedLocale(bytes[9])};
    return true;
}
} // namespace numos::settings
