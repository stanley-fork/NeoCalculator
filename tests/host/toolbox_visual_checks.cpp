// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/ToolboxCatalog.h"
#include "ui/MathRenderer.h"
#include "ui/TutorFonts.h"
#include "ui/MathTextMetrics.h"
#include "ui/ToolboxFonts.h"
#include <new>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
namespace { bool deny=false;unsigned denied=0; }
void* operator new(size_t n){if(deny){++denied;throw std::bad_alloc();}if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete(void* p,size_t) noexcept {std::free(p);}
namespace {
unsigned checks=0;void check(bool ok,const char* what){++checks;if(!ok){std::printf("FAIL %s\n",what);std::exit(1);}}
uint16_t pixels[320*240],buffer[320*240];
void flush(lv_display_t* display,const lv_area_t* area,uint8_t* data) {
    auto* source=reinterpret_cast<uint16_t*>(data);
    for(int y=area->y1;y<=area->y2;++y)for(int x=area->x1;x<=area->x2;++x)pixels[y*320+x]=*source++;
    lv_display_flush_ready(display);
}
}
int main() {
    lv_init();
    auto* display=lv_display_create(320,240);
    lv_display_set_color_format(display,LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display,buffer,nullptr,sizeof(buffer),LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display,flush);
    vpam::MathCanvas canvas;canvas.create(lv_screen_active());canvas.stopCursorBlink();
    int width=0,height=0;unsigned missingTextGlyphs=0;
    for(size_t i=0;i<numos::toolbox::entryCount();++i) {
        const auto& entry=*numos::toolbox::entryAt(i);
        for(const char* text:{entry.en,entry.es,entry.helpEn,entry.helpEs}) {
            uint32_t at=0;
            while(text[at]) {
                uint32_t code=uint8_t(text[at++]);
                unsigned continuation=code<128?0:code<224?1:code<240?2:3;
                if(continuation)code&=(1u<<(6-continuation))-1;
                while(continuation--)code=(code<<6)|(uint8_t(text[at++])&63);
                for(const auto* font:{ui::tutorFont10(),ui::tutorFont12(),ui::tutorFont14()}) {
                    lv_font_glyph_dsc_t glyph{};
                    const bool found=lv_font_get_glyph_dsc(font,&glyph,code,0) && !glyph.is_placeholder;
                    if(!found)std::printf("entry=%u:%u cp=%u font=%u text=%s\n",entry.identity.id,entry.identity.variant,unsigned(code),unsigned(font->line_height),text);
                    if(!found)++missingTextGlyphs;
                }
            }
        }
        auto prepared=numos::toolbox::preview(entry);
        prepared->calculateLayout(canvas.normalMetrics());
        const auto& box=prepared->layout();
        if(!(box.width>0 && box.width+16<=134))std::printf("wide preview %u:%u width=%d\n",entry.identity.id,entry.identity.variant,int(box.width));
        check(box.width>0 && box.width+16<=134,"preview includes MathCanvas padding");
        check(box.height()>0 && box.height()+8<=139,"preview vertical fit");
        width=std::max(width,int(box.width));height=std::max(height,int(box.height()));
    }
    check(!missingTextGlyphs,"all localized glyphs exist");
    // The old generic delimiter path placed the bars above the argument.
    // Inspect real RGB565 ink, independent of the renderer's coordinate math.
    for(auto style:{vpam::MathStyle::DISPLAY_STYLE,vpam::MathStyle::TEXT,vpam::MathStyle::SCRIPT}) {
        canvas.setMathStyle(style);canvas.setAutoHeightEnabled(false);
        auto root=vpam::makeRow();auto* row=static_cast<vpam::NodeRow*>(root.get());
        row->appendChild(numos::toolbox::preview(*numos::toolbox::find({103,0})));
        canvas.setExpression(row,nullptr);
        const int h=row->layout().height()+8;
        lv_obj_set_pos(canvas.obj(),20,20);lv_obj_set_size(canvas.obj(),134,h);
        lv_obj_invalidate(lv_screen_active());lv_refr_now(display);
        int minY=240,maxY=-1;
        for(int y=20;y<20+h;++y)for(int x=20;x<154;++x)if(pixels[y*320+x]<0x4208){minY=std::min(minY,y);maxY=std::max(maxY,y);}
        check(minY>=23 && maxY<20+h-2 && maxY>minY,"absolute value ink stays inside its measured box");
        canvas.setExpression(nullptr,nullptr);
    }
    // Resolve every authored Greek form at all real font sizes, then check
    // raster ink independently of the layout formulas (including descenders).
    for(auto style:{vpam::MathStyle::DISPLAY_STYLE,vpam::MathStyle::TEXT,vpam::MathStyle::SCRIPT,vpam::MathStyle::SCRIPTSCRIPT}) {
        canvas.setMathStyle(style);
        for(size_t i=0;i<numos::toolbox::entryCount();++i) {
            const auto& entry=*numos::toolbox::entryAt(i);
            if(entry.recipe!=numos::toolbox::Recipe::Symbol && entry.recipe!=numos::toolbox::Recipe::Variable && entry.recipe!=numos::toolbox::Recipe::Constant && entry.recipe!=numos::toolbox::Recipe::Infinity && entry.recipe!=numos::toolbox::Recipe::Unit)continue;
            if(entry.recipe==numos::toolbox::Recipe::Symbol) {
                lv_font_glyph_dsc_t queryGlyph{};
                check(lv_font_get_glyph_dsc(ui::toolbox::queryFont(),&queryGlyph,entry.argument,0) && !queryGlyph.is_placeholder,"Greek alias is readable in the search input");
                auto fm=canvas.normalMetrics();
                for(unsigned level=0;level<3;++level) {
                    lv_font_glyph_dsc_t glyph{};const lv_font_t* selected;
                    check(ui::mathTextGlyph(static_cast<const lv_font_t*>(fm.textFont),entry.argument,0,glyph,selected) && !glyph.is_placeholder && glyph.box_w>0,"Greek STIX glyph exists at each script size");
                    fm=fm.superscript();
                }
            }
            auto root=vpam::makeRow();auto* row=static_cast<vpam::NodeRow*>(root.get());
            row->appendChild(numos::toolbox::prepare(entry).node);
            if(entry.recipe==numos::toolbox::Recipe::Unit) {
                std::array<const vpam::MathNode*,32> nodes{};size_t count=1;nodes[0]=row;
                while(count) {
                    const auto* n=nodes[--count];
                    if(n->type()==vpam::NodeType::Unit) {
                        char text[32]{};numos::units::symbol(static_cast<const vpam::NodeUnit*>(n)->atom(),text,sizeof(text));
                        for(const unsigned char* p=reinterpret_cast<const unsigned char*>(text);*p;) {
                            uint32_t cp=*p++;unsigned tail=cp<128?0:cp<224?1:cp<240?2:3;
                            if(tail)cp&=(1u<<(6-tail))-1;
                            while(tail--)cp=(cp<<6)|(*p++&63);
                            auto fm=canvas.normalMetrics();
                            for(unsigned level=0;level<3;++level) {
                                lv_font_glyph_dsc_t glyph{};const lv_font_t* font;
                                const bool present=ui::mathTextGlyph(static_cast<const lv_font_t*>(fm.textFont),cp,0,glyph,font) && !glyph.is_placeholder && (glyph.box_w || (cp==0x20 && glyph.adv_w));
                                if(!present)std::printf("unit=%u:%u cp=%u level=%u\n",entry.identity.id,entry.identity.variant,unsigned(cp),level);
                                check(present,"real unit glyph at all script sizes");fm=fm.superscript();
                            }
                        }
                    }
                    for(int i=0;i<n->childCount();++i){check(count<nodes.size(),"bounded unit glyph walk");nodes[count++]=n->child(i);}
                }
            }
            deny=true;row->calculateLayout(canvas.normalMetrics());deny=false;
            check(denied==0,"real-font atom layout allocates no heap");
            canvas.setExpression(row,nullptr);
            lv_obj_set_pos(canvas.obj(),20,20);lv_obj_set_size(canvas.obj(),134,100);
            lv_obj_invalidate(lv_screen_active());lv_refr_now(display);
            int minY=240,maxY=-1,minX=320,maxX=-1;
            for(int y=20;y<120;++y)for(int x=20;x<154;++x) {
                const uint16_t pixel=pixels[y*320+x];
                if(pixel<0x4208){minY=std::min(minY,y);maxY=std::max(maxY,y);minX=std::min(minX,x);maxX=std::max(maxX,x);}
                check(!((pixel&31)>12 && ((pixel>>11)&31)<4),"mathematical ink has no blue constants");
            }
            check(maxX>=minX && maxX-minX+1<=row->layout().width,"atom horizontal ink fits measured bounds");
            check(maxY>=minY && maxY-minY+1<=row->layout().height(),"atom descenders fit measured bounds");
            canvas.setExpression(nullptr,nullptr);
        }
    }
    canvas.destroy();lv_display_delete(display);
    std::printf("PASS %u glyph/preview checks; max preview %d x %d; real STIX metrics\n",checks,width,height);
}
