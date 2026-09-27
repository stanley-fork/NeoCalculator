// SPDX-License-Identifier: GPL-3.0-or-later
// Host-only observation of the real LVGL renderer. The runner inserts four
// observation calls in a private copy of MathRenderer.cpp, never in firmware.
#include "ui/MathRenderer.h"
#include "ui/MathTypography.h"
#include "ui/MathTextNormalization.h"
#include "math/CalculationEngine.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace vpam;
bool setting_complex_enabled = false;
static bool observing = false;
static std::vector<std::pair<const MathNode*, std::string>> paths;
static std::string pathOf(const MathNode* n) {
    for (const auto& p : paths) if (p.first == n) return p.second;
    return "?";
}
static std::string quoted(const std::string& s) {
    std::string out = "\"";
    for (char c : s) {
        if (c == '\\' || c == '"') out += '\\';
        if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else out += c;
    }
    return out + "\"";
}
static void resultTree(std::ostream& out, const numos::EngineResultNode& n) {
    out << "{\"kind\":" << unsigned(n.kind) << ",\"text\":" << quoted(n.text)
        << ",\"fallback\":" << unsigned(n.fallbackReason) << ",\"rows\":" << unsigned(n.rows)
        << ",\"columns\":" << unsigned(n.columns) << ",\"leftClosed\":" << n.leftClosed
        << ",\"rightClosed\":" << n.rightClosed << ",\"children\":[";
    for (size_t i=0;i<n.children.size();++i) { if(i)out << ','; resultTree(out,n.children[i]); }
    out << "]}";
}
static std::string engineResult(const std::string& input) {
    const auto r=numos::GiacEngine::instance().evaluateStructured(input.c_str());
    std::ostringstream out;
    out << "{\"status\":" << unsigned(r.base.status) << ",\"exact\":" << quoted(r.base.exactText)
        << ",\"approximate\":" << quoted(r.base.approximateText) << ",\"diagnostic\":" << quoted(r.base.diagnostic)
        << ",\"hasTree\":" << r.hasTree << ",\"hasApproximateTree\":" << r.hasApproximateTree
        << ",\"fallback\":" << unsigned(r.fallbackReason) << ",\"tree\":";
    resultTree(out,r.tree); out << ",\"approximateTree\":"; resultTree(out,r.approximateTree);
    return out.str()+"}";
}
static void indexTree(const MathNode* n, const std::string& path) {
    paths.emplace_back(n, path);
    for (int i = 0; i < n->childCount(); ++i)
        indexTree(n->child(i), path + "/" + std::to_string(i));
}
namespace vpam {
#ifdef NUMOS_BRACKETS
void mathBracketPieceProbe(int x,int baseline,int em,uint32_t cp,const lv_font_t* font) {
    if (!observing) return;
    lv_font_glyph_dsc_t glyph{};lv_font_get_glyph_dsc(font,&glyph,cp,0);
    std::cout << "{\"event\":\"delimiterGlyph\",\"cp\":" << cp << ",\"x\":" << x-glyph.adv_w/2
        << ",\"baseline\":" << baseline << ",\"em\":" << em << ",\"inkW\":" << glyph.box_w
        << ",\"inkH\":" << glyph.box_h << ",\"offsetX\":" << glyph.ofs_x << ",\"offsetY\":" << glyph.ofs_y
        << ",\"stix\":" << (font==ui::mathParenthesisFont(em)?"true":"false") << "}\n";
}
#endif
void mathSpacingDrawProbe(const MathNode* node, int x, int y, const FontMetrics& fm) {
    if (!observing) return;
    const auto& l = node->layout();
    int scriptReserve = (node->type() == NodeType::Power || node->type() == NodeType::Subscript ||
                         node->type() == NodeType::LogBase) ? spaceAfterScriptPx(fm) : 0;
    std::cout << "{\"event\":\"node\",\"path\":" << quoted(pathOf(node))
        << ",\"type\":" << int(node->type()) << ",\"static\":" << int(node->mathClass())
        << ",\"leftStatic\":" << int(node->leftMathClass()) << ",\"rightStatic\":" << int(node->rightMathClass())
        << ",\"style\":" << int(fm.style) << ",\"level\":" << int(fm.scriptLevel) << ",\"em\":" << fm.emSize
        << ",\"children\":" << node->childCount()
        << ",\"x\":" << x << ",\"baseline\":" << y << ",\"width\":" << l.width
        << ",\"ascent\":" << l.ascent << ",\"descent\":" << l.descent
        << ",\"inkAscent\":" << l.inkAscent << ",\"inkDescent\":" << l.inkDescent
        << ",\"scriptReserve\":" << scriptReserve;
#ifdef NUMOS_SPACING_POSITIONS
    std::cout << ",\"effectiveLeft\":" << int(l.effectiveLeft)
        << ",\"effectiveRight\":" << int(l.effectiveRight)
        << ",\"spaceBefore\":" << l.spaceBefore << ",\"rowX\":" << l.rowX;
#endif
    std::cout << "}\n";
}
void mathSpacingTextProbe(int x, int y, unsigned level, const char* text, int advance) {
    if (!observing) return;
    std::cout << "{\"event\":\"text\",\"text\":" << quoted(text) << ",\"x\":" << x << ",\"baseline\":" << y
        << ",\"level\":" << level << ",\"advance\":" << advance << "}\n";
}
void mathSpacingGlyphProbe(int pen, int y, unsigned level, uint32_t cp, const lv_font_glyph_dsc_t& g) {
    if (!observing) return;
    // Observe the actual descriptor/codepoint submitted to LVGL, not a second
    // implementation that predicts what drawTextBaseline should have drawn.
    std::cout << "{\"event\":\"glyph\",\"cp\":" << cp << ",\"x\":" << pen << ",\"baseline\":" << y
        << ",\"level\":" << level << ",\"advance\":" << g.adv_w << ",\"inkX\":" << pen + g.ofs_x
        << ",\"inkY\":" << y - g.box_h - g.ofs_y << ",\"inkW\":" << g.box_w
        << ",\"inkH\":" << g.box_h << ",\"found\":true}\n";
}
void mathSpacingCursorProbe(const NodeRow* row, int index, int origin, int baseline, int offset, const FontMetrics& fm) {
    if (!observing) return;
    std::cout << "{\"event\":\"cursor\",\"path\":" << quoted(pathOf(row)) << ",\"index\":" << index
        << ",\"origin\":" << origin << ",\"baseline\":" << baseline << ",\"offset\":" << offset
        << ",\"style\":" << int(fm.style) << ",\"em\":" << fm.emSize
        << ",\"fontAscent\":" << fm.ascent << ",\"fontDescent\":" << fm.descent << "}\n";
}
}
static void key(CursorController& c, const std::string& k) {
#ifdef NUMOS_BRACKETS
    if (k == "[") { c.insertParen(DelimKind::Bracket); return; }
    if (k == "]") { c.closeBracket(); return; }
    if (k == "SQRT") { c.insertRoot(); return; }
#endif
    if (k.size() == 1 && ((k[0] >= '0' && k[0] <= '9') || k[0] == '.')) c.insertDigit(k[0]);
    else if (k == "+") c.insertOperator(OpKind::Add);
    else if (k == "-" || k == "neg") c.insertOperator(OpKind::Sub);
    else if (k == "*") c.insertOperator(OpKind::Mul);
    else if (k == "=") c.insertOperator(OpKind::Eq);
    else if (k == "^") c.insertPower();
    else if (k == "exp") c.insertPowerOfTen();
    else if (k == "/") c.insertFraction();
    else if (k == "(") c.insertParen();
    else if (k == ")" || k == "RIGHT") c.moveRight();
    else if (k == "LEFT") c.moveLeft();
    else if (k == "UP") c.moveUp();
    else if (k == "DOWN") c.moveDown();
    else if (k == "DEL") c.backspace();
    else if (k == "sin") c.insertFunction(FuncKind::Sin);
    else if (k == "ln") c.insertFunction(FuncKind::Ln);
    else if (k == "pi") c.insertConstant(ConstKind::Pi);
    else if (k.size() == 1) c.insertVariable(k[0]);
    else { std::cerr << "Unknown key " << k << '\n'; std::exit(2); }
}
static uint16_t framebuffer[320 * 240], drawbuffer[320 * 240];
static void flush(lv_display_t* d, const lv_area_t* a, uint8_t* data) {
    auto* src = reinterpret_cast<uint16_t*>(data);
    for (int y = a->y1; y <= a->y2; ++y)
        for (int x = a->x1; x <= a->x2; ++x) framebuffer[y * 320 + x] = *src++;
    lv_display_flush_ready(d);
}
static void save(const char* path) {
    std::ofstream out(path, std::ios::binary); out << "P6\n320 240\n255\n";
    for (auto v : framebuffer) {
        unsigned char rgb[] = {static_cast<unsigned char>(((v >> 11) & 31) * 255 / 31),
                              static_cast<unsigned char>(((v >> 5) & 63) * 255 / 63),
                              static_cast<unsigned char>((v & 31) * 255 / 31)};
        out.write(reinterpret_cast<const char*>(rgb), 3);
    }
}
int main(int argc, char** argv) {
    if (argc < 3) return 2;
    lv_init();
    auto* d = lv_display_create(320, 240);
    lv_display_set_color_format(d, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(d, drawbuffer, nullptr, sizeof(drawbuffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(d, flush);
    auto root = std::make_unique<NodeRow>(); CursorController ctrl; ctrl.init(root.get());
    std::istringstream keys(argv[1]); std::string token;
    while (keys >> token) key(ctrl, token);
    paths.reserve(4096); indexTree(root.get(), "r");
    std::string before, error; bool complete = numos::CalculationEngine::serializeForGiac(root.get(), before, error);
    std::cout << "{\"event\":\"semantic\",\"complete\":" << (complete ? "true" : "false")
        << ",\"serialized\":" << quoted(before) << ",\"error\":" << quoted(error) << "}\n";
    // Layout fields are captured separately; the pre-layout tree includes every
    // authored value, operator and child in order in dumpTree's existing format.
    std::cout << "{\"event\":\"treeBefore\",\"tree\":" << quoted(dumpTree(root.get())) << "}\n";
    // Host-only evaluation surrounds (never runs inside) layout/draw/navigation.
    if (!numos::GiacEngine::instance().begin()) return 4;
    const std::string engineBefore=complete?engineResult(before):"null";
    std::cout << "{\"event\":\"engineBefore\",\"result\":" << engineBefore << "}\n";
    MathCanvas canvas; canvas.create(lv_screen_active()); canvas.setAutoHeightEnabled(false);
    canvas.setMathStyle(MathStyle::TEXT); lv_obj_set_pos(canvas.obj(), 0, 0); lv_obj_set_size(canvas.obj(), 320, 240);
    canvas.setExpression(root.get(), &ctrl); canvas.resetCursorBlink();
    auto frame = [&](unsigned index) {
        std::cout << "{\"event\":\"frame\",\"index\":" << index << "}\n";
        observing = true; canvas.resetCursorBlink(); lv_obj_invalidate(lv_screen_active()); lv_refr_now(d); observing = false;
        lv_area_t bounds{}; canvas.cursorBounds(bounds);
        std::cout << "{\"event\":\"cursorBounds\",\"x\":" << bounds.x1 << ",\"y\":" << bounds.y1
            << ",\"bottom\":" << bounds.y2 << "}\n";
    };
    frame(0); save(argv[2]);
    // Matching non-blinking formula views for the qualitative reference panel.
    // Keep the editor attached: incomplete slots must still paint their ink.
    canvas.stopCursorBlink(); lv_refr_now(d);
    save((std::string(argv[2])+"-formula.ppm").c_str());
    // Visit the actual editor's boundaries, including returning through scripts
    // and fractions. No cursor coordinates or prebuilt AST are injected.
    std::vector<std::pair<const NodeRow*,int>> visited;
    for (unsigned i=1; i<=2*paths.size()+4; ++i) {
        const auto pos=std::make_pair(static_cast<const NodeRow*>(ctrl.cursor().row),ctrl.cursor().index);
        if (std::find(visited.begin(),visited.end(),pos)!=visited.end()) break;
        visited.push_back(pos); ctrl.moveLeft(); frame(i);
        std::string current,currentError;
        const bool currentComplete=numos::CalculationEngine::serializeForGiac(root.get(),current,currentError);
        if (before!=current || complete!=currentComplete || error!=currentError) return 3;
    }
    std::cout << "{\"event\":\"treeAfter\",\"tree\":" << quoted(dumpTree(root.get())) << "}\n";
    std::string after, afterError;
    bool afterComplete = numos::CalculationEngine::serializeForGiac(root.get(), after, afterError);
    if (before != after || complete != afterComplete || error != afterError) return 3;
    const std::string engineAfter=complete?engineResult(after):"null";
    std::cout << "{\"event\":\"engineAfter\",\"result\":" << engineAfter << "}\n";
    if(engineBefore!=engineAfter) return 5;
    std::cout << "{\"event\":\"sizes\",\"node\":" << sizeof(MathNode) << ",\"row\":" << sizeof(NodeRow)
        << ",\"layout\":" << sizeof(LayoutResult) << ",\"metrics\":" << sizeof(FontMetrics) << "}\n";
    canvas.destroy(); root.reset(); lv_display_delete(d); lv_deinit();
}
