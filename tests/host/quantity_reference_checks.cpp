// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/MathAST.h"
#include "math/CursorController.h"
#include "math/CalculationEngine.h"
#include "math/VariableManager.h"
#include "ui/MathTypography.h"
#include "ui/MathTextMetrics.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace vpam;
using namespace numos;
namespace {
unsigned checks = 0;
void check(bool ok, const char* message) {
    ++checks; if (!ok) { std::fprintf(stderr, "FAIL %s\n", message); std::exit(1); }
}
bool parentLinks(const MathNode* node, const MathNode* parent = nullptr) {
    if (!node || node->parent() != parent) return false;
    for (int i = 0; i < node->childCount(); ++i) if (!parentLinks(node->child(i), node)) return false;
    return true;
}
void glyphs(const MathNode* node, const lv_font_t* font) {
    if (node->type() == NodeType::Symbol) {
        const auto* p = reinterpret_cast<const uint8_t*>(static_cast<const NodeSymbol*>(node)->name().c_str());
        while (*p) {
            uint32_t cp = 0; p += utf8Decode(p, cp);
            lv_font_glyph_dsc_t glyph{}; const lv_font_t* selected = nullptr;
            if (!ui::mathTextGlyph(font, cp, 0, glyph, selected) || glyph.is_placeholder) {
                std::fprintf(stderr, "Reference glyph U+%04X at line height %d\n", unsigned(cp), font->line_height);
                check(false, "missing STIX notation glyph");
            }
            check(glyph.adv_w > 0, "reference notation has zero advance");
        }
    }
    for (int i = 0; i < node->childCount(); ++i) glyphs(node->child(i), font);
}
}
int main() {
    lv_init();
    check(!units::kReferences.empty(), "real reference data required");
    check(GiacEngine::instance().begin(), "engine initialization");
    auto& engine = CalculationEngine::instance();
    engine.noteAnsRotated("42", false);
    unsigned accepted = 0;
    for (const auto& ref : units::kReferences) for (unsigned prefix = 0; prefix < 25; ++prefix) {
        const units::ReferenceAtom atom{ref.id, units::PrefixId(prefix)};
        auto node = makeQuantityReference(atom);
        check(bool(node) == units::valid(atom), "reference constructor policy");
        if (!node) continue;
        ++accepted;
        const auto* value = static_cast<const NodeQuantityReference*>(node.get());
        check(node->type() == NodeType::QuantityReference && value->atom() == atom,
              "reference identity is not a unit/free variable");
        check(parentLinks(node.get()), "notation ownership");
        check(node->childCount() == 1 && node->child(0) == value->notation() && !node->child(1), "private notation traversal");
        for (const auto* font : {ui::mathPrimaryFont(), ui::mathScriptFont(), ui::mathScriptScriptFont()}) glyphs(node.get(), font);
        if (ref.id == units::ReferenceId(1))
            check(static_cast<const NodeSymbol*>(value->notation())->name() == u8"𝑐", "physical constant c must be italic");
        if (ref.id == units::ReferenceId(3)) {
            const auto* notation = static_cast<const NodeSubscript*>(value->notation());
            check(notation->type() == NodeType::Subscript &&
                  static_cast<const NodeSymbol*>(notation->base())->name() == u8"𝑚" &&
                  static_cast<const NodeSymbol*>(notation->subscript())->name() == "e", "electron mass italic base/upright descriptive subscript");
        }
        if (ref.id == units::ReferenceId(137) || ref.id == units::ReferenceId(138)) {
            check(value->notation()->type() == NodeType::Power &&
                  static_cast<const NodeSymbol*>(value->notation()->child(1))->name() == "3",
                  "normal/standard gas volume lost cubic notation");
        }
        auto clone = cloneNode(node.get());
        check(clone && static_cast<NodeQuantityReference*>(clone.get())->atom() == atom && parentLinks(clone.get()), "typed reference clone");
        uint8_t bytes[5]{};
        units::ReferenceAtom decoded{};
        check(units::encode(atom, bytes) && units::decodeReference(bytes, sizeof(bytes), decoded) && decoded == atom,
              "closed reference codec roundtrip");
        units::Atom unitDestination{units::UnitId(1), units::PrefixId(0)};
        check(!units::decode(bytes, sizeof(bytes), unitDestination), "reference bytes admitted as unit");
        std::string serialized, error;
        check(!CalculationEngine::serializeForGiac(node.get(), serialized, error) && error.find("quantity references") != std::string::npos,
              "reference accidentally serialized as a CAS variable");
        check(engine.evaluate(node.get()).status == MathEngineStatus::UnitsUnavailable, "reference evaluation guard");
        auto fm = defaultFontMetrics();
        for (unsigned level = 0; level < 3; ++level) {
            node->calculateLayout(fm);
            const auto& outer = node->layout();
            const auto& inner = value->notation()->layout();
            check(outer.width == inner.width && outer.ascent == inner.ascent && outer.descent == inner.descent && outer.width > 0,
                  "reference geometry differs from common notation layout");
            check(node->scriptLevel() == fm.scriptLevel, "reference script level");
            fm = fm.superscript();
        }
        auto root = makeRow();
        auto* row = static_cast<NodeRow*>(root.get());
        CursorController cursor; cursor.init(row); cursor.insertDigit('2');
        check(cursor.insertPrepared(std::move(node), nullptr), "typed reference insertion");
        check(row->childCount() == 3 && row->child(2)->type() == NodeType::QuantityReference,
              "reference concatenated to a number");
        cursor.moveLeft(); check(cursor.cursor().row == row && cursor.cursor().index == 2, "cursor entered reference notation");
        cursor.moveRight(); check(cursor.cursor().row == row && cursor.cursor().index == 3, "cursor failed to pass atomic reference");
        cursor.insertPower(); cursor.insertDigit('2');
        check(row->child(2)->type() == NodeType::Power && scanUnits(root.get()) == UnitScan::Present,
              "editing reference power loses identity");
        check(engine.evaluate(root.get()).status == MathEngineStatus::UnitsUnavailable, "derived expression erased reference before guard");
        auto ratio = makeFraction(makeQuantityReference(atom), makeQuantityReference(atom));
        check(engine.evaluate(ratio.get()).status == MathEngineStatus::UnitsUnavailable, "reference cancellation reached CAS");
        auto zero = makeRow(); auto* z = static_cast<NodeRow*>(zero.get());
        z->appendChild(makeNumber("0")); z->appendChild(makeOperator(OpKind::Mul)); z->appendChild(makeQuantityReference(atom));
        check(engine.evaluate(z).status == MathEngineStatus::UnitsUnavailable, "zero product erased reference");
        auto removable = makeRow(); auto* r = static_cast<NodeRow*>(removable.get()); CursorController del; del.init(r);
        check(del.insertPrepared(makeQuantityReference(atom), nullptr), "reference delete setup");
        del.backspace(); check(scanUnits(r) == UnitScan::None, "reference not deleted atomically");
    }
    const units::ReferenceAtom invalid{units::ReferenceId(65535), units::PrefixId(0)};
    check(!makeQuantityReference(invalid), "unknown reference became first record");
    const units::ReferenceAtom first{units::kReferences[0].id, units::PrefixId(0)};
    units::ReferenceAtom destination = first;
    for (const auto& bad : {std::array<uint8_t,5>{{0x52,2,1,0,0}},
                           std::array<uint8_t,5>{{0x52,1,255,255,0}},
                           std::array<uint8_t,5>{{0x55,1,1,0,0}},
                           std::array<uint8_t,5>{{0x52,1,1,0,25}}})
        check(!units::decodeReference(bad.data(), bad.size(), destination) && destination == first,
              "corrupt reference payload changed destination");
    check(engine.evaluate(makeVariable(VAR_ANS).get()).exactText == "42", "reference guard changed Ans");
    check(engine.evaluate(makeVariable('c').get()).ok(), "ordinary c became a physical constant");
    auto ordinary = makeRow(); auto* r = static_cast<NodeRow*>(ordinary.get());
    r->appendChild(makeNumber("2")); r->appendChild(makeOperator(OpKind::Add)); r->appendChild(makeNumber("2"));
    check(engine.evaluate(r).exactText == "4", "ordinary calculation did not recover");
    std::printf("Quantity reference checks=%u variants=%u NodeQuantityReference=%zu bytes\n", checks, accepted, sizeof(NodeQuantityReference));
}
