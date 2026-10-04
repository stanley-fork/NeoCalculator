// SPDX-License-Identifier: GPL-3.0-or-later
#include "ui/MathRenderer.h"
#include "ui/MathTypography.h"
#include "math/CalculationEngine.h"
#include "math/giac/EngineContracts.h"
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <new>
#include <vector>

using namespace vpam;
bool setting_complex_enabled = false;
static unsigned checks = 0, failures = 0, allocations = 0;
static bool denyAllocations = false;
void* operator new(std::size_t size) {
    if (denyAllocations) { ++allocations; throw std::bad_alloc(); }
    if (auto* p = std::malloc(size ? size : 1)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
namespace vpam {
void* testNodeAllocate(std::size_t size) {
    if (denyAllocations) { ++allocations; throw std::bad_alloc(); }
    return std::malloc(size);
}
void testNodeRelease(void* p) { std::free(p); }
void mathSpacingDrawProbe(const MathNode*, int, int, const FontMetrics&) {}
void mathSpacingTextProbe(int, int, unsigned, const char*, int) {}
void mathSpacingGlyphProbe(int, int, unsigned, uint32_t, const lv_font_glyph_dsc_t&) {}
void mathSpacingCursorProbe(const NodeRow*, int, int, int, int, const FontMetrics&) {}
}
static void check(bool condition, const char* message) {
    ++checks;
    if (!condition) { if (failures < 30) std::cerr << "FAIL " << message << '\n'; ++failures; }
}

// Independent oracle: literal TeX 3.141592653 math_spacing string. The codes
// are TeX's, not the production table encoding. No production helper is used
// to obtain an expected space/class/position.
constexpr char tex[] = "0234000122*4000133**3**344*0400400*000000234000111*1111112341011";
static int space(unsigned left, unsigned right, unsigned style, int em) {
    const char code = tex[8*left+right];
    if (code == '2') return 3*em/18;
    if (style >= 2) return 0;
    if (code == '1') return 3*em/18;
    if (code == '3') return 4*em/18;
    if (code == '4') return 5*em/18;
    return 0;
}
static std::vector<unsigned> classes(std::vector<unsigned> atoms) {
    unsigned previous = 1; // TeX mlist_to_hlist starts with op_noad.
    for (std::size_t i=0; i<atoms.size(); ++i) {
        const unsigned current=atoms[i];
        if (current==2 && (previous==1 || previous==2 || previous==3 || previous==4 || previous==6))
            atoms[i]=0;
        else if (i && (current==3 || current==5 || current==6) && previous==2) atoms[i-1]=0;
        previous=atoms[i];
    }
    if (!atoms.empty() && atoms.back()==2) atoms.back()=0;
    return atoms;
}
class Atom final : public MathNode {
    MathClass c;
public:
    explicit Atom(unsigned k) : MathNode(NodeType::Number), c(static_cast<MathClass>(k)) {}
    MathClass mathClass() const override { return c; }
    void calculateLayout(const FontMetrics& fm) override {
        setScriptLevel(fm.scriptLevel); _layout.width=7; _layout.ascent=9; _layout.descent=2;
    }
};
static NodePtr row(NodePtr a) { auto p=makeRow(); static_cast<NodeRow*>(p.get())->appendChild(std::move(a)); return p; }
static int glyph(const lv_font_t* f, uint32_t cp, uint32_t next=0) {
    lv_font_glyph_dsc_t g{}; check(lv_font_get_glyph_dsc(f,&g,cp,next),"oracle font glyph exists"); return g.adv_w;
}
int main() {
    lv_init(); MathCanvas canvas;
    const auto fm=canvas.normalMetrics();
    for (int em : {8,12,18}) for (unsigned s=0;s<4;++s)
        for (unsigned a=0;a<8;++a) for (unsigned b=0;b<8;++b)
            check(interAtomSpacingPx(static_cast<MathClass>(a),static_cast<MathClass>(b),static_cast<MathStyle>(s),em)
                  ==space(a,b,s,em),"TeX table/style entry");
    NodeRow unary; unary.appendChild(makeOperator(OpKind::Sub)); unary.appendChild(makeNumber("5"));
    unary.calculateLayout(fm);
    check(unary.layout().width==glyph(ui::mathPrimaryFont(),0x2212)+glyph(ui::mathPrimaryFont(),'5'),"detect mathematical unary minus advance + zero binary space");
    NodeRow relation; relation.appendChild(makeVariable('x')); relation.appendChild(makeOperator(OpKind::Eq));
    relation.appendChild(makeOperator(OpKind::Sub)); relation.appendChild(makeNumber("5")); relation.calculateLayout(fm);
    // A variable's box also contains its STIX italic overhang. Keep the
    // spacing oracle independent: derive that box from the actual font ink.
    lv_font_glyph_dsc_t xInk{};lv_font_get_glyph_dsc(ui::mathPrimaryFont(),&xInk,'x',0);
    const int xBox=std::max<int>(xInk.adv_w,xInk.ofs_x+xInk.box_w)-std::min<int>(0,xInk.ofs_x);
    check(relation.layout().width==xBox+glyph(ui::mathPrimaryFont(),'=')+
          glyph(ui::mathPrimaryFont(),0x2212)+glyph(ui::mathPrimaryFont(),'5')+10,"relation then unary: two thick spaces, no binary gap");
    auto exponent=makeRow(); auto* e=static_cast<NodeRow*>(exponent.get());
    e->appendChild(makeOperator(OpKind::Sub)); e->appendChild(makeNumber("5"));
    e->appendChild(makeOperator(OpKind::Sub)); e->appendChild(makeNumber("6"));
    auto power=makePower(row(makeNumber("5")),std::move(exponent)); auto* p=static_cast<NodePower*>(power.get());
    NodeRow outer; outer.appendChild(std::move(power)); outer.appendChild(makeOperator(OpKind::Sub)); outer.appendChild(makeNumber("6"));
    outer.calculateLayout(fm);
    check(e->layout().width==2*glyph(ui::mathScriptFont(),0x2212)+glyph(ui::mathScriptFont(),'5')+glyph(ui::mathScriptFont(),'6'),"detect exponent conditional spacing and mathematical minus advances");
    check(p->layout().width==p->base()->layout().width+e->layout().width+p->italicCorrectionPx()+1,"exactly one SpaceAfterScript at em18");
    check(outer.layout().width==p->layout().width+glyph(ui::mathPrimaryFont(),0x2212)+9+8,"outer subtraction after power stays binary");
    for (const char* text : {"3.14","66.63","0123456789"}) {
        int expected=0; for (const char* s=text;*s;++s) expected+=glyph(ui::mathPrimaryFont(),*s,s[1]);
        NodeNumber n(text); n.calculateLayout(fm); check(n.layout().width==expected,"real numeric advance");
    }
    for (auto kind : {FuncKind::Sin,FuncKind::Cos,FuncKind::Ln,FuncKind::ArcSin}) {
        NodeFunction f(kind,row(makeVariable('x'))); f.calculateLayout(fm);
        int expected=0; for (const char* s=f.label();*s;) {uint32_t cp,next;auto n=utf8Decode((const uint8_t*)s,cp);utf8Decode((const uint8_t*)s+n,next);expected+=glyph(ui::mathPrimaryFont(),cp,next);s+=n;}
        check(f.labelWidth()==expected,"proportional function label");
    }
#ifdef NUMOS_SPACING_POSITIONS
    // Exhaustive four-atom lists include runs of binaries, relations, close and
    // punctuation. Tests demand exact pixel origins, not just a helper call.
    for (unsigned code=0;code<4096;++code) {
        std::vector<unsigned> raw; NodeRow r;
        for (unsigned i=0;i<4;++i) {raw.push_back((code>>(3*i))&7);r.appendChild(std::make_unique<Atom>(raw.back()));}
        const auto expected=classes(raw);
        for (unsigned s=0;s<4;++s) {
            auto metrics=fm;metrics.style=static_cast<MathStyle>(s);r.calculateLayout(metrics);
            int x=0;
            for (unsigned i=0;i<4;++i) {
                if(i)x+=space(expected[i-1],expected[i],s,18);
                const auto& l=r.child(i)->layout();
                check(unsigned(l.effectiveLeft)==expected[i] && unsigned(l.effectiveRight)==expected[i],"ordered TeX demotion");
                check(r.childXOffset(i)==x,"exact contextual child origin");x+=7;
            }
            check(r.layout().width==x && r.childXOffset(4)==x,"row end cursor equals total advance");
        }
    }
    for (int n : {63,64,65,399,400,2000}) {
        NodeRow r;
        for (int i=0;i<n;++i) r.appendChild(makeNumber("1"));
        r.calculateLayout(fm);
        check(r.layout().width==9*n && r.childXOffset(n)==9*n,"long row has no hidden cap");
        // Force unary minus across each old 64 boundary; no static fallback.
        for (int i=0;i<n;i+=32) r.replaceChild(i,makeOperator(OpKind::Sub));
        r.calculateLayout(fm);
        for (int i=0;i<n;++i)check(r.childXOffset(i)>=0,"long row placement remains representable");
    }
    // Real admissibility boundary: 400 serializer nodes includes this root.
    for (int n : {399,400}) {
        NodeRow r; for(int i=0;i<n;++i) r.appendChild(makeVariable('x'));
        std::string s,err;check(numos::CalculationEngine::serializeForGiac(&r,s,err)==(n==399),"unchanged 400-node semantic budget");
    }
    for (unsigned middle=0;middle<8;++middle) for (unsigned last=0;last<8;++last) {
        NodeRow flat,wrapped;
        flat.appendChild(std::make_unique<Atom>(0));flat.appendChild(std::make_unique<Atom>(middle));flat.appendChild(std::make_unique<Atom>(last));
        wrapped.appendChild(std::make_unique<Atom>(0));auto inner=makeRow();auto* inside=static_cast<NodeRow*>(inner.get());
        inside->appendChild(std::make_unique<Atom>(middle));inside->appendChild(row(std::make_unique<Atom>(last)));wrapped.appendChild(std::move(inner));
        flat.calculateLayout(fm);wrapped.calculateLayout(fm);
        check(flat.layout().width==wrapped.layout().width,"transparent nested rows preserve full-list context");
        check(wrapped.childXOffset(1)==flat.childXOffset(1),"wrapper owns its incoming gap exactly once");
        check(wrapped.childXOffset(1)+inside->childXOffset(1)==flat.childXOffset(2),"nested wrapper placement");
    }
    NodeRow pending;pending.appendChild(makeOperator(OpKind::Eq));pending.appendChild(makeEmpty());pending.appendChild(makeOperator(OpKind::Sub));pending.appendChild(makeNumber("5"));
    pending.calculateLayout(fm);
    check(pending.child(1)->layout().width>0 && pending.child(2)->layout().effectiveLeft==MathClass::ORD,"placeholder retained and transparent to sign context");
    NodeFunction f(FuncKind::Sin,row(makeVariable('x')));
    NodeRow functions;functions.appendChild(makeNumber("2"));functions.appendChild(std::make_unique<NodeFunction>(FuncKind::Sin,row(makeVariable('x'))));
    auto script=fm.superscript();functions.calculateLayout(script);
    check(functions.child(1)->layout().spaceBefore==2,"unconditional thin function space survives script style");
    check(fm.superscript().superscript().superscript().emSize==8,"minimum physical font stays em8 at deeper scripts");
    for(const auto& metrics : {fm,fm.superscript(),fm.superscript().superscript()}) {
        const auto* font=metrics.scriptLevel==0?ui::mathPrimaryFont():metrics.scriptLevel==1?ui::mathScriptFont():ui::mathScriptScriptFont();
        // Independent glyph oracle: explicit Unicode, never the production
        // codepoint resolver. Covers input operators AND signed result runs.
        lv_font_glyph_dsc_t minus{}, hyphen{}, plus{};
        check(lv_font_get_glyph_dsc(font,&minus,0x2212,0),"STIX minus exists at every size");
        lv_font_get_glyph_dsc(font,&hyphen,0x002D,0);
        lv_font_get_glyph_dsc(font,&plus,0x002B,0);
        check(minus.box_w>hyphen.box_w && minus.adv_w==plus.adv_w,"minus ink longer than hyphen and same advance as plus");
        NodeOperator sign(OpKind::Sub);sign.calculateLayout(metrics);
        check(sign.layout().width==minus.adv_w,"operator reserves the real mathematical minus advance");
        check(std::string(sign.symbol())=="-","presentation preserves semantic operator symbol");
        NodeNumber signedNumber("-5");signedNumber.calculateLayout(metrics);
        check(signedNumber.value()=="-5" && signedNumber.layout().width==glyph(font,0x2212,'5')+glyph(font,'5'),"signed result keeps ASCII value and mathematical minus geometry");
        NodeSymbol unicodeMinus("\xE2\x88\x92");unicodeMinus.calculateLayout(metrics);
        check(unicodeMinus.layout().width==sign.layout().width,"explicit Unicode minus and semantic ASCII minus draw identically");
        NodeSymbol run("5-6");run.calculateLayout(metrics);
        check(run.layout().width==glyph(font,'5',0x2212)+glyph(font,0x2212,'6')+glyph(font,'6'),"both current and next codepoints resolved for pair kerning");
        NodeSymbol longRun(std::string(100,'-'));longRun.calculateLayout(metrics);
        check(longRun.layout().width==99*glyph(font,0x2212,0x2212)+glyph(font,0x2212),"minus mapping has no normalization-buffer length discontinuity");
        NodeSymbol proportional("AV.1");proportional.calculateLayout(metrics);
        check(proportional.layout().width==glyph(font,'A','V')+glyph(font,'V','.')+glyph(font,'.','1')+glyph(font,'1'),"proportional run uses existing pair kerning and decimal advance");
        NodeSymbol delta("\xCE\x94");delta.calculateLayout(metrics);
        check(delta.layout().width==glyph(ui::mathGlyphFont(font,0x394),0x394),"Delta keeps the existing one-glyph fallback and its advance");
    }
    // Deny C++ and AST allocations throughout repeated hot-path layout. This
    // includes real LVGL descriptor queries but not the LVGL draw-task queue.
    denyAllocations=true;
    try { for(int i=0;i<100;++i) {outer.calculateLayout(fm);functions.calculateLayout(script);pending.calculateLayout(fm);} }
    catch (...) {denyAllocations=false;check(false,"layout allocated while allocator denied");}
    denyAllocations=false;check(allocations==0,"zero own layout allocations");
#endif
    // Host comparative cost only, not ESP32 latency or total task-stack peaks.
    for(int length : {63,64,65,399,2000}) {
        NodeRow r;
        for(int i=0;i<length;++i)r.appendChild(i%2?makeOperator(OpKind::Sub):makeNumber("5"));
        for(int i=0;i<50;++i)r.calculateLayout(fm);
        const auto start=std::chrono::steady_clock::now();
        for(int i=0;i<1000;++i)r.calculateLayout(fm);
        const auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-start).count();
        std::cout << "BENCH {\"nodes\":" << length << ",\"iterations\":1000,\"nanoseconds\":" << ns << "}\n";
    }
    std::cout << "CHECKS " << checks << " FAILURES " << failures << "\n";
    return failures ? 1 : 0;
}
