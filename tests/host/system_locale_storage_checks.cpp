// SPDX-License-Identifier: GPL-3.0-or-later
#include "apps/CompactSettingsRecord.h"
#include "hal/FileSystem.h"
#include <cstdio>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

int main(int argc, char** argv) {
    if (argc != 2) return 2; // Caller supplies an isolated test filesystem root.
    using namespace numos;
    LittleFSClass::setRoot(argv[1]);
    if (!LittleFS.begin()) return 2;
    unsigned checks = 0, failed = 0;
    auto check = [&](bool pass, const char* name) {
        ++checks; if (!pass) { ++failed; std::fprintf(stderr, "%s\n", name); }
    };
    auto writeCandidate = [&](i18n::Locale locale) {
        const auto bytes = settings::encodeCompactSettings({false, false, false, 10, locale});
        auto file = LittleFS.open("/settings.tmp", "w");
        return file && file.write(bytes.data(), bytes.size()) == bytes.size();
    };
    auto readLocale = [&]() {
        auto file = LittleFS.open("/settings.dat", "r");
        std::array<uint8_t, settings::kCompactSettingsSize> bytes{};
        settings::CompactSettings decoded;
        if (!file || file.read(bytes.data(), bytes.size()) != bytes.size() ||
            !settings::decodeCompactSettings(bytes.data(), bytes.size(), decoded)) return i18n::Locale::Pseudo;
        return decoded.locale;
    };
    for (unsigned round = 0; round < 3; ++round) {
        for (auto locale : i18n::productLocales) {
            check(writeCandidate(locale), "candidate write");
            check(LittleFS.rename("/settings.tmp", "/settings.dat"), "replacement rename");
            check(readLocale() == locale, "repeated regional persistence");
        }
    }
    const auto prior = readLocale();
    check(!LittleFS.rename("/missing.tmp", "/settings.dat") && readLocale() == prior,
          "missing candidate destroyed prior settings");
#ifdef _WIN32
    // A locked destination injects the actual Windows replacement failure;
    // neither the user record nor the completed candidate may be deleted.
    const std::string path = std::string(argv[1]) + "/settings.dat";
    const HANDLE lock = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    check(lock != INVALID_HANDLE_VALUE, "create replacement failure");
    if (lock != INVALID_HANDLE_VALUE) {
        check(writeCandidate(i18n::Locale::EnglishUK), "failure candidate write");
        check(!LittleFS.rename("/settings.tmp", "/settings.dat"), "locked replacement unexpectedly succeeded");
        check(readLocale() == prior && LittleFS.exists("/settings.tmp"), "failed replacement lost old/candidate record");
        CloseHandle(lock);
        check(LittleFS.rename("/settings.tmp", "/settings.dat") && readLocale() == i18n::Locale::EnglishUK,
              "unlock recovery");
    }
#endif
    std::printf("System locale storage checks=%u failed=%u\n", checks, failed);
    return failed ? 1 : 0;
}
