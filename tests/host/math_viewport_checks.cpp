// SPDX-License-Identifier: GPL-3.0-or-later
#include "ui/MathRenderer.h"
#include "math/CalculationEngine.h"
#include <cstdlib>
#include <iostream>
#include <new>
#include <string>
using namespace vpam;
bool setting_complex_enabled=false;
static unsigned checks=0,failures=0,allocations=0,drawn=0;
static bool deny=false,observe=false;
static const MathNode* target=nullptr;
static int targetX=0,targetBaseline=0;
void* operator new(std::size_t n){if(deny){++allocations;throw std::bad_alloc();}if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void* p) noexcept{std::free(p);}void operator delete(void* p,std::size_t) noexcept{std::free(p);}
namespace vpam {
void* testNodeAllocate(std::size_t n){if(deny){++allocations;throw std::bad_alloc();}return std::malloc(n);}
void testNodeRelease(void* p){std::free(p);}
void mathSpacingDrawProbe(const MathNode* n,int x,int y,const FontMetrics&){if(observe){++drawn;if(n==target){targetX=x;targetBaseline=y;}}}
void mathSpacingTextProbe(int,int,unsigned,const char*,int){}
void mathSpacingGlyphProbe(int,int,unsigned,uint32_t,const lv_font_glyph_dsc_t&){}
void mathSpacingCursorProbe(const NodeRow*,int,int,int,int,const FontMetrics&){}
}
static void check(bool b,const char* label){++checks;if(!b){if(failures<25)std::cerr<<"FAIL "<<label<<'\n';++failures;}}
static uint16_t pixels[320*240],buffer[320*240];
static void flush(lv_display_t* d,const lv_area_t* a,uint8_t* data){auto* p=reinterpret_cast<uint16_t*>(data);for(int y=a->y1;y<=a->y2;++y)for(int x=a->x1;x<=a->x2;++x)pixels[y*320+x]=*p++;lv_display_flush_ready(d);}
static unsigned count(const MathNode* n){unsigned c=1;for(int i=0;i<n->childCount();++i)if(n->child(i))c+=count(n->child(i));return c;}
int main(){
 lv_init();auto* display=lv_display_create(320,240);lv_display_set_color_format(display,LV_COLOR_FORMAT_RGB565);
 lv_display_set_buffers(display,buffer,nullptr,sizeof(buffer),LV_DISPLAY_RENDER_MODE_PARTIAL);lv_display_set_flush_cb(display,flush);
 MathCanvas canvas;canvas.create(lv_screen_active());canvas.setAutoHeightEnabled(false);canvas.setMathStyle(MathStyle::TEXT);
 lv_obj_set_pos(canvas.obj(),6,25);lv_obj_set_size(canvas.obj(),308,209);
 auto frame=[&]{drawn=0;observe=true;deny=true;try{lv_obj_invalidate(canvas.obj());lv_refr_now(display);}catch(...){check(false,"own C++ allocation in geometry/draw");}deny=false;observe=false;};
 auto caret=[&]{lv_area_t c{};check(canvas.cursorBounds(c),"caret available");if (!(c.x1>=6&&c.x2<=313&&c.y1>=25&&c.y2<=233)&&failures<25) {lv_area_t a{};lv_obj_get_coords(canvas.obj(),&a);std::cerr<<"caret="<<c.x1<<","<<c.y1<<","<<c.x2<<","<<c.y2<<" area="<<a.x1<<","<<a.y1<<","<<a.x2<<","<<a.y2<<"\n";}check(c.x1>=6&&c.x2<=313&&c.y1>=25&&c.y2<=233,"whole caret inside viewport");check(c.y2-c.y1>=5,"caret not collapsed to one pixel");};
 for(int depth:{13,14,15,20,40,65,90}){
  NodeRow root;CursorController cursor;cursor.init(&root);for(int i=0;i<depth;++i)cursor.insertRoot();cursor.insertDigit('2');
  const unsigned expected=count(&root);canvas.setExpression(&root,&cursor);canvas.resetCursorBlink();target=cursor.cursor().row;
  frame();check(drawn==expected,"every accepted/editor node painted, no depth truncation");caret();
  lv_area_t c{};canvas.cursorBounds(c);check(c.x1==targetX+cursor.cursor().row->childXOffset(cursor.cursor().index),"draw and cursor use same placement");
  for(int i=0;i<depth*2+4;++i){cursor.moveLeft();target=cursor.cursor().row;frame();check(drawn==expected,"full traversal while navigating");caret();}
  canvas.setExpression(nullptr,nullptr);
 }
 {
  NodeRow root;CursorController cursor;cursor.init(&root);
  for(int i=0;i<12;++i){cursor.insertDigit('1');cursor.insertFraction();}cursor.insertDigit('2');
  std::string before,error;check(numos::CalculationEngine::serializeForGiac(&root,before,error),"tall fraction evaluable");
  canvas.setExpression(&root,&cursor);canvas.resetCursorBlink();
  for(int i=0;i<100;++i){frame();caret();cursor.moveLeft();}
  std::string after;check(numos::CalculationEngine::serializeForGiac(&root,after,error)&&before==after,"navigation keeps serialization");
  canvas.setExpression(&root,nullptr);target=root.child(0);frame();const int top=targetBaseline;
  check(canvas.scrollVerticalBounded(-30000),"tall readonly content pans");frame();const int bottom=targetBaseline;
  check(bottom<top,"vertical origin moved");check(!canvas.scrollVerticalBounded(-30000),"vertical lower edge bounded");frame();check(targetBaseline==bottom,"vertical lower edge stable");
  check(canvas.scrollVerticalBounded(30000),"return to top");frame();check(targetBaseline==top,"vertical round trip no drift");
  lv_obj_set_height(canvas.obj(),root.layout().height()+4);frame();check(!canvas.hasVerticalOverflow(),"resize recomputes vertical bound");
  lv_obj_set_height(canvas.obj(),209);canvas.setExpression(nullptr,nullptr);
 }
 for(int digits:{1,33,61,97,201}){
  NodeRow root;CursorController cursor;cursor.init(&root);for(int i=0;i<digits;++i)cursor.insertDigit(char('1'+i%9));
  target=root.child(0);canvas.setExpression(&root,nullptr);frame();const int left=targetX;
  // Independent pixel formula: this canvas is 308px wide, with 8px per side.
  const int excess=std::max(0,root.layout().width-292);
  canvas.scrollBy(-30000);frame();check(targetX==left-excess,"scrollBy uses actual content end");
  canvas.scrollBy(-30000);frame();check(targetX==left-excess,"no signed wrap or overscroll");
  check(!canvas.scrollBounded(-1),"bounded pan signals endpoint");canvas.scrollBy(30000);frame();check(targetX==left,"horizontal round trip");
  canvas.setExpression(&root,&cursor);canvas.resetCursorBlink();frame();caret();
  lv_obj_set_width(canvas.obj(),100);frame();lv_area_t c{};canvas.cursorBounds(c);check(c.x2<=105,"resize reveals full caret");
  lv_obj_set_width(canvas.obj(),308);canvas.setExpression(nullptr,nullptr);
 }
 check(allocations==0,"zero own hot-path allocations");
 std::cout<<"CHECKS "<<checks<<" FAILURES "<<failures<<" NODE_BYTES "<<sizeof(MathNode)<<" CANVAS_BYTES "<<sizeof(MathCanvas)<<'\n';
 canvas.destroy();lv_display_delete(display);lv_deinit();return failures?1:0;
}
