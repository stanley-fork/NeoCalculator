// SPDX-License-Identifier: GPL-3.0-or-later
#include "ui/MathRenderer.h"
#include "math/CalculationEngine.h"
#include "math/CalculationFormat.h"
#include <cstdlib>
#include <iostream>
#include <new>
using namespace vpam;
bool setting_complex_enabled = false;
static unsigned checks = 0, failures = 0, allocations = 0;
static bool deny = false;
static const MathNode* degreeNode = nullptr;
static int degreeX = 0, degreeBaseline = 0, originX = 0, originBaseline = 0;
void* operator new(std::size_t n) {
    if (deny) { ++allocations; throw std::bad_alloc(); }
    if (auto* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
namespace vpam {
void* testNodeAllocate(std::size_t n) {
    if (deny) { ++allocations; throw std::bad_alloc(); }
    return std::malloc(n);
}
void testNodeRelease(void* p) { std::free(p); }
void mathSpacingDrawProbe(const MathNode* n, int x, int y, const FontMetrics&) {
    if (n == degreeNode) { degreeX = x; degreeBaseline = y; }
    if (degreeNode && n == degreeNode->parent()) { originX = x; originBaseline = y; }
}
void mathSpacingTextProbe(int,int,unsigned,const char*,int) {}
void mathSpacingGlyphProbe(int,int,unsigned,uint32_t,const lv_font_glyph_dsc_t&) {}
void mathSpacingCursorProbe(const NodeRow*,int,int,int,int,const FontMetrics&) {}
}
static void check(bool pass, const char* message) {
    ++checks;
    if (!pass) { if (failures < 30) std::cerr << "FAIL " << message << '\n'; ++failures; }
}
static uint16_t buffer[320*240];
static void flush(lv_display_t* display, const lv_area_t*, uint8_t*) { lv_display_flush_ready(display); }
int main() {
    lv_init();
    auto* display = lv_display_create(320,240);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display, buffer, nullptr, sizeof(buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush);
    MathCanvas canvas; canvas.create(lv_screen_active()); canvas.setAutoHeightEnabled(false);
    lv_obj_set_size(canvas.obj(),320,240);
    for (auto style : {MathStyle::DISPLAY_STYLE,MathStyle::TEXT,MathStyle::SCRIPT,MathStyle::SCRIPTSCRIPT}) {
        canvas.setMathStyle(style);
        for (const char* digits : {"3","16","128","65536","549755813888"}) {
            for (bool fraction : {false,true}) {
                auto degree = makeRow(); auto* degreeRow = static_cast<NodeRow*>(degree.get());
                degreeRow->appendChild(makeNumber(digits));
                NodeRow row;
                row.appendChild(makeRoot(fraction ? makeFraction(makeNumber("1"),makeNumber("2")) : makeNumber("2"),std::move(degree)));
                auto* radical = static_cast<NodeRoot*>(row.child(0));
                CursorController cursor; cursor.init(&row); cursor.moveLeft();
                canvas.setExpression(&row,&cursor);
                degreeNode = degreeRow;
                const auto fm = canvas.normalMetrics();
                deny = true;
                try { row.calculateLayout(fm); lv_obj_invalidate(canvas.obj()); lv_refr_now(display); }
                catch (...) { check(false,"geometry allocated"); }
                deny = false;
                const auto& d = degreeRow->layout(); const auto& r = radical->layout();
                check(degreeX == originX + radical->degreeX(),"degree draw X equals reservation");
                check(degreeBaseline == originBaseline + radical->degreeBaseline(),"degree baseline contract");
                check(radical->degreeX() >= 0 && radical->degreeX()+d.width <= r.width,"degree horizontal containment");
                check(radical->degreeBaseline()-d.ascent >= -r.ascent,"degree top containment");
                check(radical->radicandX()+radical->radicand()->layout().width <= r.width,"radicand containment");
                // Independent vector-envelope bound: degree ends before the
                // rising stroke's foot. Reintroducing the old fixed X fails.
                check(degreeX+d.width <= originX+radical->radicalX()+NodeRoot::RADICAL_HOOK_W,"degree/radical ink clearance");
                canvas.setExpression(nullptr,nullptr); degreeNode = nullptr;
            }
        }
    }
    check(allocations == 0,"zero own allocations in layout and paint");
    for (int exponent = -310; exponent <= 310; ++exponent) {
        const int actual = numos::engineeringExponent(exponent);
        check(actual % 3 == 0 && actual <= exponent && exponent-actual < 3,"ENG floor/multiple-of-three invariant");
    }
    struct Numeric { const char* source; const char* mantissa; int exp; };
    for (const auto& value : {Numeric{"1234","1.234",3},Numeric{"0.00001234","12.34",-6},Numeric{"-0.1","100",-3},Numeric{"0","0",0},Numeric{"1e1000","10",999}}) {
        auto formatted = numos::powerOfTenFormat(value.source,true);
        check(bool(formatted),"numeric ENG accepted");
        const int offset = value.source[0]=='-' ? 1 : 0;
        check(static_cast<NodeNumber*>(formatted->child(offset))->value() == value.mantissa,"independent mantissa oracle");
        auto* power = static_cast<NodePower*>(formatted->child(offset+2));
        auto* exp = power->exponent();
        check(static_cast<NodeNumber*>(exp->child(value.exp<0?1:0))->value() == std::to_string(std::abs(value.exp)),"independent exponent oracle");
    }
    for (const char* invalid : {"", "nan", "inf", "1/3", "2*pi", "1e", "1..2", ".", "--1", "1e9999999999"}) {
        numos::DecimalParts value; check(!numos::decimalParts(invalid,value),"reject nonnumeric without guessing");
    }
    struct Fixed { const char* source; unsigned places; const char* expected; };
    for (const auto& f : {Fixed{"3.14159265359",2,"3.14"},Fixed{"9.995",2,"10.00"},
        Fixed{"-0.005",2,"-0.01"},Fixed{"-0.004",2,"0.00"},Fixed{"1e-1000",9,"0.000000000"},
        Fixed{"1234",0,"1234"},Fixed{"0",3,"0.000"},Fixed{"-2.5",0,"-3"}}) {
        std::string actual;
        check(numos::fixedDecimal(f.source,f.places,actual)&&actual==f.expected,"FIX independent rounding oracle");
    }
    {
        auto half=makeFraction(makeNumber("1"),makeNumber("2"));
        auto asin=makeFunction(FuncKind::ArcSin,makeNumber("0.5"));
        auto sin=makeFunction(FuncKind::Sin,makeNumber("0.5"));
        check(!numos::authoredAngle(half.get()),"plain fraction is not an angle");
        check(numos::authoredAngle(asin.get()),"inverse sine is an angle");
        check(!numos::authoredAngle(sin.get()),"sine output is a ratio");
        auto ratio=makeFraction(std::move(asin),makeFunction(FuncKind::ArcTan,makeNumber("1")));
        check(!numos::authoredAngle(ratio.get()),"angle divided by angle is dimensionless");
    }
    for (int count : {19,20,24,30,39,40,41}) {
        NodeRow row; CursorController cursor;cursor.init(&row);
        for(int i=0;i<count;++i)cursor.insertRoot();cursor.insertDigit('2');
        std::string text,error;
        check(numos::CalculationEngine::serializeForGiac(&row,text,error,true)==(count<40),"semantic nesting budget boundary");
    }
    {
        NodeRow row; CursorController cursor;cursor.init(&row);cursor.insertDigit('5');
        const auto before=dumpTree(&row);deny=true;
        check(!cursor.insertFactorial(),"factorial handles allocation failure");deny=false;
        check(dumpTree(&row)==before,"factorial failure retains operand");
        check(cursor.insertFactorial(),"postfix factorial captured operand");
        std::string source,error;check(numos::CalculationEngine::serializeForGiac(&row,source,error)&&source=="factorial((5))","factorial serializes semantically");
        cursor.backspace(); check(row.childCount()==1&&row.child(0)->type()==NodeType::Number,"DEL unwraps factorial");
    }
    std::cout << checks << " checks, " << failures << " failures\n";
    return failures ? 1 : 0;
}
