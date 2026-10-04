// SPDX-License-Identifier: GPL-3.0-or-later
#include "i18n/Locale.h"
#include "math/tutor/Locale.h"
#include "apps/CompactSettingsRecord.h"
#include "demo/DemoSettingsRecord.h"
#include <cstdio>
#include <cstring>

int main() {
    using namespace numos::i18n;
    unsigned checks = 0, failed = 0;
    auto check = [&](bool pass, const char* name) {
        ++checks;
        if (!pass) { ++failed; std::fprintf(stderr, "%s\n", name); }
    };
    numos::tutor::productLocale = Locale::EnglishUK;
    check(productLocale == Locale::EnglishUK, "duplicated system preference");
    productLocale = Locale::EnglishUS;
    check(static_cast<unsigned>(Locale::English) == 0 && static_cast<unsigned>(Locale::Spanish) == 1 &&
          static_cast<unsigned>(Locale::French) == 2 && static_cast<unsigned>(Locale::Pseudo) == 3,
          "legacy IDs changed");
    const uint8_t expectedIds[] = {0, 4, 1, 5};
    const char* expectedTags[] = {"en-US", "en-GB", "es-ES", "es-419"};
    for (unsigned i = 0; i < 4; ++i) {
        const auto locale = productLocales[i];
        check(localeStorageValue(locale) == expectedIds[i], "locale encoded as menu index");
        check(std::strcmp(localeTag(locale), expectedTags[i]) == 0, "locale tag");
        check(productLocaleIndex(locale) == i && storedLocale(expectedIds[i]) == locale, "region identity");
        check(adjacentLocale(locale, true) == productLocales[(i+1)%4] &&
              adjacentLocale(locale, false) == productLocales[(i+3)%4], "bidirectional cycle");
        check(baseLocale(locale) == (i >= 2 ? Locale::Spanish : Locale::English), "regional message fallback");
        const numos::settings::CompactSettings original{true, false, true, 12, locale};
        auto web = numos::settings::encodeCompactSettings(original);
        numos::settings::CompactSettings decoded;
        check(web.size() == 10 && web[9] == expectedIds[i], "ST01 storage contract");
        check(numos::settings::decodeCompactSettings(web.data(), web.size(), decoded) &&
              decoded.locale == locale && decoded.angleDeg && !decoded.complexEnabled &&
              decoded.educationEnabled && decoded.precision == 12, "web/native roundtrip");
        for (uint8_t version : {uint8_t(2), uint8_t(3)}) {
            auto hardware = numos::demo::encodeSettingsRecord(true, false, true, 12, 128, expectedIds[i]);
            hardware[4] = version;
            const auto checksum = numos::demo::settingsRecordChecksum(hardware.data(), 12);
            std::memcpy(hardware.data()+12, &checksum, 4);
            numos::demo::DecodedSettings hw;
            check(numos::demo::decodeSettingsRecord(hardware.data(), hardware.size(), hw) &&
                  hw.tutorLanguage == expectedIds[i] && hw.angleDeg && hw.precision == 12, "WROOM region roundtrip");
        }
        for (unsigned offset : {0U, 4U, 5U, 6U, 7U, 8U}) {
            auto bad = web; bad[offset] = 255;
            decoded.locale = Locale::SpanishLatinAmerica;
            decoded.precision = 8;
            check(!numos::settings::decodeCompactSettings(bad.data(), bad.size(), decoded) &&
                  decoded.locale == Locale::SpanishLatinAmerica && decoded.precision == 8,
                  "invalid compact record changed state");
        }
        check(!numos::settings::decodeCompactSettings(nullptr, web.size(), decoded) &&
              !numos::settings::decodeCompactSettings(web.data(), web.size()-1, decoded) &&
              !numos::settings::decodeCompactSettings(web.data(), web.size()+1, decoded), "record size accepted");
    }
    for (unsigned value = 0; value <= 255; ++value) {
        // Oracle deliberately independent of production whitelist/index table.
        const uint8_t expected = value == 1 || value == 4 || value == 5 ? value : 0;
        check(localeStorageValue(storedLocale(value)) == expected, "invalid persisted locale became product/test locale");
        auto bytes = numos::settings::encodeCompactSettings({});
        bytes[9] = static_cast<uint8_t>(value);
        numos::settings::CompactSettings decoded;
        check(numos::settings::decodeCompactSettings(bytes.data(), bytes.size(), decoded) &&
              localeStorageValue(decoded.locale) == expected, "compact unknown locale policy");
    }
    check(baseLocale(Locale::French) == Locale::French && baseLocale(Locale::Pseudo) == Locale::Pseudo,
          "internal rendering test locales changed");
    std::printf("System locale checks=%u failed=%u\n", checks, failed);
    return failed ? 1 : 0;
}
