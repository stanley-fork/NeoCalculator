// SPDX-License-Identifier: GPL-3.0-or-later
// Host diagnostic only: compare current Giac entry points in separate calls.
// Product cases still enter through real Calculation keys, not this program.
#include "math/giac/GiacEngine.h"
#include <iomanip>
#include <iostream>
bool setting_complex_enabled = false;
int main(int argc, char** argv) {
    auto& engine = numos::GiacEngine::instance();
    if (!engine.begin()) return 2;
    for (int i=1; i<argc; ++i) {
        const auto raw=engine.evaluateStructured(argv[i]);
        const auto simplified=engine.simplify(argv[i]);
        std::cout << "{\"input\":" << std::quoted(argv[i])
            << ",\"evaluate\":" << std::quoted(raw.base.exactText)
            << ",\"simplify\":" << std::quoted(simplified.exactText)
            << ",\"status\":" << int(simplified.status) << "}\n";
    }
}
