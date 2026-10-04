// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/ToolboxCatalog.h"
#include "math/ToolboxStore.h"
#include "math/CalculationEngine.h"
#include "math/CursorController.h"
#include "math/VariableManager.h"
#include "math/units/ReferenceRegistry.h"
#include "i18n/Locale.h"
#include "hal/FileSystem.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <new>

using namespace vpam;
using namespace numos;
namespace {
unsigned checks = 0;
size_t observedAllocations = 0;
bool observeAllocations = false;
void check(bool value, const char* message) {
    ++checks;
    if (!value) { std::printf("FAIL %s\n", message); std::exit(1); }
}
toolbox::Identity unitIdentity(uint16_t id, uint16_t prefix = 0) {
    return {uint16_t(0x8000 | id), prefix};
}
toolbox::Identity referenceIdentity(uint16_t id, uint16_t prefix = 0) {
    return {uint16_t(0x4000 | id), prefix};
}
const toolbox::Entry& entry(toolbox::Identity id) {
    const auto* e = toolbox::find(id);
    check(e != nullptr, "requested stable entry exists");
    return *e;
}
units::Atom unitAtom(uint16_t id, uint8_t prefix = 0) {
    return {units::UnitId(id), units::PrefixId(prefix)};
}
units::ReferenceAtom referenceAtom(uint16_t id, uint8_t prefix = 0) {
    return {units::ReferenceId(id), units::PrefixId(prefix)};
}
bool sameTree(const MathNode* a, const MathNode* b, unsigned depth = 0) {
    if (!a || !b || depth > 80 || a->type() != b->type() || a->childCount() != b->childCount()) return false;
    if (a->type() == NodeType::Unit && !(static_cast<const NodeUnit*>(a)->atom() == static_cast<const NodeUnit*>(b)->atom())) return false;
    if (a->type() == NodeType::QuantityReference && !(static_cast<const NodeQuantityReference*>(a)->atom() == static_cast<const NodeQuantityReference*>(b)->atom())) return false;
    if (a->type() == NodeType::Number && static_cast<const NodeNumber*>(a)->value() != static_cast<const NodeNumber*>(b)->value()) return false;
    if (a->type() == NodeType::Operator && static_cast<const NodeOperator*>(a)->op() != static_cast<const NodeOperator*>(b)->op()) return false;
    for (int i = 0; i < a->childCount(); ++i) if (!sameTree(a->child(i), b->child(i), depth + 1)) return false;
    return true;
}
bool hasUnit(const MathNode* n, units::Atom atom, unsigned depth = 0) {
    if (!n || depth > 80) return false;
    if (n->type() == NodeType::Unit && static_cast<const NodeUnit*>(n)->atom() == atom) return true;
    for (int i = 0; i < n->childCount(); ++i) if (hasUnit(n->child(i), atom, depth + 1)) return true;
    return false;
}
bool hasNumber(const MathNode* n, const char* value, unsigned depth = 0) {
    if (!n || depth > 80) return false;
    if (n->type() == NodeType::Number && static_cast<const NodeNumber*>(n)->value() == value) return true;
    for (int i = 0; i < n->childCount(); ++i) if (hasNumber(n->child(i), value, depth + 1)) return true;
    return false;
}
void query(const char* text, toolbox::Identity wanted, toolbox::Identity forbidden = {}) {
    check(toolbox::searchRank(entry(wanted), text) == 0, "exact query reaches correct variant");
    if (forbidden.id) check(toolbox::searchRank(entry(forbidden), text) == 99, "exact symbol excludes wrong variant");
}
uint32_t crc(const uint8_t* data, size_t count) {
    uint32_t value = 0xffffffffu;
    for (size_t i = 0; i < count; ++i) {
        value ^= data[i];
        for (unsigned bit = 0; bit < 8; ++bit) value = (value >> 1) ^ (0xedb88320u & uint32_t(-int(value & 1)));
    }
    return ~value;
}
}

void* operator new(size_t n) {
    if (observeAllocations) ++observedAllocations;
    if (void* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void* operator new[](size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { ::operator delete(p); }
void operator delete(void* p, size_t) noexcept { ::operator delete(p); }
void operator delete[](void* p, size_t) noexcept { ::operator delete(p); }

int main() {
    const auto initialLocale = i18n::productLocale;
    const auto& metre = entry(unitIdentity(1));
    const auto& litre = entry(unitIdentity(46));
    struct LocaleCase { i18n::Locale locale; const char* metre; const char* litre; };
    const LocaleCase localeCases[] = {
        {i18n::Locale::EnglishUS, "Meter", "Liter"},
        {i18n::Locale::EnglishUK, "Metre", "Litre"},
        {i18n::Locale::SpanishSpain, "Metro", "Litro"},
        {i18n::Locale::SpanishLatinAmerica, "Metro", "Litro"},
    };
    for (const auto& c : localeCases) {
        i18n::productLocale = c.locale;
        check(!std::strcmp(toolbox::displayName(metre), c.metre), "regional metre display");
        check(!std::strcmp(toolbox::displayName(litre), c.litre), "regional litre display");
        check(metre.identity == unitIdentity(1) && litre.identity == unitIdentity(46), "locale never changes identity");
    }
    struct GasLocaleCase { uint16_t id; const char* names[4]; };
    const GasLocaleCase gasLocales[] = {
        {137, {"Normal cubic meter of gas", "Normal cubic metre of gas",
               "Metro cúbico normal de gas", "Metro cúbico normal de gas"}},
        {138, {"Standard cubic meter of gas", "Standard cubic metre of gas",
               "Metro cúbico estándar de gas", "Metro cúbico estándar de gas"}},
    };
    for (const auto& gas : gasLocales) {
        const auto identity = referenceIdentity(gas.id);
        const auto& reference = entry(identity);
        for (size_t locale = 0; locale < 4; ++locale) {
            i18n::productLocale = localeCases[locale].locale;
            check(!std::strcmp(toolbox::displayName(reference), gas.names[locale]), "gas reference uses regional meter/metre display");
            check(reference.identity == identity, "gas reference locale preserves stable identity");
            // WHY: every regional spelling remains discoverable regardless of
            // the current display preference; names never replace typed IDs.
            for (const char* alias : gas.names) query(alias, identity);
        }
    }
    query("cal", unitIdentity(1156), unitIdentity(1156, 10));
    query("Cal", unitIdentity(1156, 10), unitIdentity(1156));
    query("kcal", unitIdentity(1156, 10), unitIdentity(1156));
    query("Wh", unitIdentity(114), unitIdentity(114, 10));
    query("kWh", unitIdentity(114, 10), unitIdentity(114));
    query("MHz", unitIdentity(10, 9), unitIdentity(10, 15));
    query("mHz", unitIdentity(10, 15), unitIdentity(10, 9));
    query("mA", unitIdentity(4, 15), unitIdentity(4, 9));
    query("MA", unitIdentity(4, 9), unitIdentity(4, 15));
    check(!toolbox::find({0xc001, 0}), "conflicting unit/reference namespace rejected");
    check(!toolbox::find({0x4001, 25}) && !toolbox::find({0x7fff, 0}), "invalid reference prefix and ID rejected");

    const auto& provider = toolbox::catalogProvider();
    const auto mass = provider.at(provider.context, 208, 0);
    check(mass.entry && mass.entry->identity == unitIdentity(2, 10), "kg access is kilo gram, not a second unit");
    auto kg = toolbox::prepare(*mass.entry);
    check(kg.node && hasUnit(kg.node.get(), unitAtom(2, 10)), "kg constructor preserves gram anchor");
    check(provider.initialFrom(provider.context, 0x8002, unitIdentity(2, 10)) == 9, "kg prefix selector opens at kilo");
    check(provider.initialFrom(provider.context, 0x8001, unitIdentity(1, 15)) == 15, "mm favorite opens at milli");
    check(provider.initialFrom(provider.context, 0x400d, referenceIdentity(13, 15)) == 15, "milli gravity favorite opens at exact reference variant");

    auto fuel = toolbox::prepare(entry(unitIdentity(1298)));
    check(fuel.node && fuel.node->type() == NodeType::Fraction, "L/100km is an explicit fraction");
    check(hasUnit(fuel.node->child(0), unitAtom(46)), "L/100km numerator has typed litre");
    check(hasNumber(fuel.node->child(1), "100") && hasUnit(fuel.node->child(1), unitAtom(1, 10)), "L/100km denominator contains actual coefficient and kilometre");
    check(!hasNumber(fuel.node->child(0), "100"), "coefficient has correct direction");

    size_t unitVariants = 0, referenceVariants = 0;
    for (size_t i = 0; i < toolbox::entryCount(); ++i) {
        const auto& e = *toolbox::entryAt(i);
        if (e.recipe != toolbox::Recipe::Unit && e.recipe != toolbox::Recipe::QuantityReference) continue;
        if (e.recipe == toolbox::Recipe::Unit) ++unitVariants; else ++referenceVariants;
        check(toolbox::available(e, toolbox::Calculation), "typed catalogue available in Calculation");
        check(!toolbox::available(e, toolbox::Equations | toolbox::Grapher | toolbox::Calculus), "unsupported receivers filter all typed quantities");
        auto built = toolbox::prepare(e);
        check(bool(built.node), "every catalogue variant prepares");
        check(scanUnits(built.node.get()) == UnitScan::Present, "constructor contains typed quantity");
        auto cloned = cloneNode(built.node.get());
        check(sameTree(built.node.get(), cloned.get()), "clone preserves structure and stable IDs");
        std::string text, error;
        check(!CalculationEngine::serializeForGiac(built.node.get(), text, error), "quantity cannot be serialized as CAS variable");
        if (e.recipe == toolbox::Recipe::QuantityReference) {
            check(built.node->type() == NodeType::QuantityReference, "physical/context reference remains one typed atom");
            const auto atom = static_cast<NodeQuantityReference*>(built.node.get())->atom();
            check(atom == referenceAtom(uint16_t(e.argument), uint8_t(e.identity.variant)), "reference constructor retains exact prefix");
            uint8_t record[5]{};
            auto restored = referenceAtom(1);
            check(units::encode(atom, record) && units::decodeReference(record, sizeof(record), restored) && restored == atom, "reference machine codec roundtrip");
            auto unit = unitAtom(1, 15); const auto previous = unit;
            check(!units::decode(record, sizeof(record), unit) && unit == previous, "reference bytes cannot become unit bytes");
        }
        for (const auto style : {MathStyle::DISPLAY_STYLE, MathStyle::TEXT, MathStyle::SCRIPT, MathStyle::SCRIPTSCRIPT}) {
            auto fm = defaultFontMetrics();
            if (style == MathStyle::SCRIPT || style == MathStyle::SCRIPTSCRIPT) fm = fm.superscript();
            if (style == MathStyle::SCRIPTSCRIPT) fm = fm.superscript();
            fm.style = style;
            observedAllocations = 0; observeAllocations = true;
            built.node->calculateLayout(fm);
            observeAllocations = false;
            check(observedAllocations == 0, "synthetic-metric layout performs no C++ allocation");
            check(built.node->layout().width > 0 && built.node->layout().ascent + built.node->layout().descent > 0, "every style has nonempty quantity geometry");
            check(sameTree(built.node.get(), cloned.get()), "layout style does not mutate identity");
        }
    }
    check(referenceVariants == 154, "all 58 reference definitions and applicable prefixes covered");
    check(unitVariants > 2000, "expanded unit constructors exercised");
    {
        auto destination = referenceAtom(13, 15); const auto original = destination;
        const uint8_t corrupt[][5] = {{0x52, 2, 1, 0, 0}, {0x52, 1, 255, 255, 0}, {0x52, 1, 1, 0, 10}, {0x55, 1, 1, 0, 0}};
        for (const auto& bytes : corrupt) check(!units::decodeReference(bytes, sizeof(bytes), destination) && destination == original, "bad reference codec preserves destination");
        check(!makeQuantityReference(referenceAtom(65535)) && !makeQuantityReference(referenceAtom(1, 10)), "bad typed reference constructor rejects data");
    }

    // Host filesystem is private to this test; never touches user favorites.
    const auto path = std::filesystem::path("out/unit-catalog-02/expanded-host-store");
    std::filesystem::create_directories(path);
    std::filesystem::remove(path / "toolbox-favorites-a.dat");
    std::filesystem::remove(path / "toolbox-favorites-b.dat");
    LittleFS.setRoot(path.generic_string().c_str()); LittleFS.begin(false);
    const std::array<toolbox::Identity, 6> favorites = {{unitIdentity(1, 15), unitIdentity(114, 10), unitIdentity(1156, 10),
                                                       referenceIdentity(3), referenceIdentity(13, 15), referenceIdentity(118, 15)}};
    toolbox::Store store; store.load();
    for (const auto id : favorites) check(store.toggle(id) == toolbox::FavoriteResult::Added, "favorite accepts exact unit/reference variant");
    check(store.save(), "favorites journal writes");
    toolbox::Store restored; restored.load();
    check(restored.favoriteCount() == favorites.size(), "favorites reload count");
    for (size_t i = 0; i < favorites.size(); ++i) check(restored.favorite(i) == favorites[i], "favorites reload mathematical identity");
    check(restored.move(favorites[4], -1) && restored.save(), "explicit favorite reordering persists");
    i18n::productLocale = i18n::Locale::EnglishUK;
    toolbox::Store afterLocale; afterLocale.load();
    check(afterLocale.favorite(3) == favorites[4] && afterLocale.favorite(4) == favorites[3], "locale and ordering retain reference IDs");
    check(afterLocale.recentCount() == 0, "loading favorites does not invent recent insertions");
    afterLocale.inserted(favorites[1]); afterLocale.inserted(favorites[4]); afterLocale.inserted(favorites[1]);
    check(afterLocale.recentCount() == 2 && afterLocale.recent(0) == favorites[1] && afterLocale.recent(1) == favorites[4], "recents retain exact variants");
    {
        // A valid CRC does not authorize a conflicting namespace or unknown ID.
        uint8_t record[28] = {'N','T','B','X',1,0,3,0,1,0,0,0,1,0xc0,0,0,13,0x40,15,0,255,0x7f,0,0};
        const auto sum = crc(record, 24);
        for (unsigned b = 0; b < 4; ++b) record[24+b] = uint8_t(sum >> (8*b));
        std::array<toolbox::Identity, toolbox::Store::kFavorites> ids{}; size_t count = 0; uint32_t generation = 0;
        check(toolbox::Store::decode(record, sizeof(record), ids, count, generation) == toolbox::StoreStatus::Ready
              && count == 1 && ids[0] == referenceIdentity(13, 15), "journal rejects corrupt IDs without first-row fallback");
    }

    check(GiacEngine::instance().begin(), "Giac starts for ordinary calculation regression");
    auto& engine = CalculationEngine::instance();
    auto previous = engine.evaluate(makeNumber("42").get()); engine.noteAnsRotated(previous.exactText, false);
    for (const auto id : {referenceIdentity(1), referenceIdentity(14), referenceIdentity(113), referenceIdentity(13, 15)}) {
        auto value = toolbox::prepare(entry(id));
        check(engine.evaluate(value.node.get()).status == MathEngineStatus::UnitsUnavailable, "all reference kinds use deliberate interim guard");
    }
    check(engine.evaluate(makeVariable(VAR_ANS).get()).exactText == "42", "guard leaves Ans intact");
    auto ordinary = makeRow(); auto* row = static_cast<NodeRow*>(ordinary.get());
    row->appendChild(makeNumber("2")); row->appendChild(makeOperator(OpKind::Add)); row->appendChild(makeNumber("2"));
    check(engine.evaluate(row).exactText == "4", "ordinary arithmetic resumes after references");
    i18n::productLocale = initialLocale;
    std::printf("PASS %u checks; %zu unit variants; %zu reference variants; NodeUnit=%zu; NodeQuantityReference=%zu host bytes\n",
                checks, unitVariants, referenceVariants, sizeof(NodeUnit), sizeof(NodeQuantityReference));
}
