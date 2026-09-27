// SPDX-License-Identifier: GPL-3.0-or-later
#include "ui/MathRenderer.h"
#include "math/CalculationEngine.h"
#include <array>
#include <cstdlib>
#include <iostream>
#include <new>

using namespace vpam;
bool setting_complex_enabled = false;
static bool deny = false;
static unsigned allocations = 0, checks = 0, failures = 0;
void* operator new(std::size_t n) {
    if (deny) { ++allocations; throw std::bad_alloc(); }
    if (auto* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
static std::array<uint16_t,320*240> frame{}, buffer{};
static void flush(lv_display_t* display, const lv_area_t* area, uint8_t* pixels) {
    const auto* source = reinterpret_cast<const uint16_t*>(pixels);
    for (int y=area->y1; y<=area->y2; ++y)
        for (int x=area->x1; x<=area->x2; ++x) frame[y*320+x] = *source++;
    lv_display_flush_ready(display);
}
static NodePtr emptySlot() {
    auto slot = makeRow();
    static_cast<NodeRow*>(slot.get())->appendChild(makeEmpty());
    return slot;
}
namespace vpam {
void* testNodeAllocate(std::size_t bytes) {
    if (deny) { ++allocations; return nullptr; }
    return std::malloc(bytes);
}
void testNodeRelease(void* ptr) { std::free(ptr); }
// Observe the measured origin, then check actual framebuffer edges after draw.
static std::array<lv_area_t,8> pending{};
static unsigned pendingCount = 0;
void mathSpacingDrawProbe(const MathNode* n,int x,int y,const FontMetrics&) {
    if (n->type()==NodeType::Empty && pendingCount<pending.size()) {
        const auto& box=n->layout();
        pending[pendingCount++]={x,y-box.ascent,x+box.width-1,y+box.descent-1};
    }
}
void mathSpacingTextProbe(int,int,unsigned,const char*,int) {}
void mathSpacingGlyphProbe(int,int,unsigned,uint32_t,const lv_font_glyph_dsc_t&) {}
void mathSpacingCursorProbe(const NodeRow*,int,int,int,int,const FontMetrics&) {}
}
static void check(bool ok,const char* message) {
    ++checks;if(!ok){++failures;std::cerr<<"FAIL "<<message<<'\n';}
}
int main() {
    lv_init();auto* display=lv_display_create(320,240);
    lv_display_set_color_format(display,LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display,buffer.data(),nullptr,sizeof(buffer),LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display,flush);
    MathCanvas canvas;canvas.create(lv_screen_active());canvas.setAutoHeightEnabled(false);
    lv_obj_set_size(canvas.obj(),320,240);
    for(auto style:{MathStyle::TEXT,MathStyle::SCRIPT,MathStyle::SCRIPTSCRIPT}) {
        canvas.setMathStyle(style);
        for(bool showRoot:{true,false})
        for(int shape=0;shape<4;++shape) for(bool editable:{false,true}) {
            canvas.setEmptyRootPlaceholderVisible(showRoot);
            auto tree=makeRow();auto* row=static_cast<NodeRow*>(tree.get());
            if(shape==0) row->appendChild(makeEmpty());
            if(shape==1) row->appendChild(makePower(emptySlot(),emptySlot()));
            if(shape==2) row->appendChild(makeFraction(emptySlot(),emptySlot()));
            if(shape==3) row->appendChild(makeRoot(emptySlot()));
            CursorController cursor;cursor.init(row);
            std::string serialized,error;
            check(!numos::CalculationEngine::serializeForGiac(row,serialized,error),"pending tree stays incomplete");
            canvas.setExpression(row,editable?&cursor:nullptr);canvas.stopCursorBlink();
            pendingCount=0;frame.fill(0xffff);
            deny=true;
            try { row->calculateLayout(canvas.normalMetrics());lv_obj_invalidate(canvas.obj());lv_refr_now(display); }
            catch(...) {check(false,"render allocated");}
            deny=false;
            check(pendingCount==unsigned(shape==1||shape==2?2:1),"all pending slots visited");
            for(unsigned i=0;i<pendingCount;++i) {
                const auto& box=pending[i];
                bool complete=true, blank=true;
                // 0x808080 converts to RGB565 0x8410. All four actual edges,
                // including the initial root and read-only/history canvas.
                auto gray=[&](int x,int y){return x>=0&&x<320&&y>=0&&y<240&&frame[y*320+x]==0x8410;};
                for(int x=box.x1;x<=box.x2;++x)complete &= gray(x,box.y1)&&gray(x,box.y2);
                for(int y=box.y1;y<=box.y2;++y)complete &= gray(box.x1,y)&&gray(box.x2,y);
                for(int y=box.y1;y<=box.y2;++y)for(int x=box.x1;x<=box.x2;++x)
                    blank &= frame[y*320+x]==0xffff;
                check(shape==0&&!showRoot ? blank : complete,
                      "only opted-out empty root is blank; every pending slot has a border");
            }
            check(!numos::CalculationEngine::serializeForGiac(row,serialized,error),"drawing cannot complete a slot");
            canvas.setExpression(nullptr,nullptr);
        }
    }
    check(allocations==0,"zero own heap in placeholder layout/draw");
    std::cout<<checks<<" checks, "<<failures<<" failures\n";return failures?1:0;
}
