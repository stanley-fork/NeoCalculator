// SPDX-License-Identifier: GPL-3.0-or-later
#include "ui/MathRenderer.h"
#include "ui/MathTypography.h"
#include "math/font/StixParentheses.h"
#include "math/CalculationEngine.h"
#include <iostream>
#include <cstdlib>
#include <new>
using namespace vpam;
bool setting_complex_enabled=false;
static unsigned checks=0,failures=0,allocations=0;static bool deny=false;
void* operator new(std::size_t n){if(deny){++allocations;throw std::bad_alloc();}if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void* p) noexcept{std::free(p);}void operator delete(void* p,std::size_t) noexcept{std::free(p);}
namespace vpam {
void mathBracketPieceProbe(int,int,int,uint32_t,const lv_font_t*){}
void* testNodeAllocate(std::size_t n){if(deny){++allocations;throw std::bad_alloc();}return std::malloc(n);}
void testNodeRelease(void* p){std::free(p);}
void mathSpacingDrawProbe(const MathNode*,int,int,const FontMetrics&){}
void mathSpacingTextProbe(int,int,unsigned,const char*,int){}
void mathSpacingGlyphProbe(int,int,unsigned,uint32_t,const lv_font_glyph_dsc_t&){}
void mathSpacingCursorProbe(const NodeRow*,int,int,int,int,const FontMetrics&){}
}
static void check(bool ok,const char* s){++checks;if(!ok){if(failures<20)std::cerr<<"FAIL "<<s<<'\n';++failures;}}
int main(){
 lv_init();MathCanvas canvas;auto fm=canvas.normalMetrics();
 for(auto metrics:{fm,fm.superscript(),fm.superscript().superscript()}){
  const auto* font=ui::mathParenthesisFont(metrics.emSize);
  lv_font_glyph_dsc_t glyphs[32]{};
  for(int i=0;i<32;++i){check(lv_font_get_glyph_dsc(font,&glyphs[i],0xe020+i,0)&&!glyphs[i].is_placeholder,"all bracket variants/parts in actual font");}
  // Explicit codepoints/descriptors form the oracle, not the plan under test.
  for(int target=1;target<=512;++target){
   int variant=13,width=0,height=target;
   for(int i=0;i<13;++i)if(std::max(glyphs[i].box_h,glyphs[i+16].box_h)>=target){variant=i;break;}
   if(variant<13){height=std::max(glyphs[variant].box_h,glyphs[variant+16].box_h);}
   for(int side:{0,16})for(int i=variant;i<(variant==13?16:variant+1);++i){const auto& g=glyphs[side+i];width=std::max(width,std::max<int>(g.adv_w,g.ofs_x+g.box_w));}
   const auto p=stixParenthesisPlan(target,metrics.emSize,true);
   check(p.variant==variant&&p.height==height&&p.width==width,"plan matches real STIX variant/assembly ink and advance");
  }
  NodeRow root;CursorController cursor;cursor.init(&root);
  cursor.insertParen(DelimKind::Bracket);cursor.insertDigit('1');cursor.insertFraction();cursor.insertDigit('2');cursor.closeBracket();
  cursor.insertOperator(OpKind::Add);cursor.insertDigit('1');root.calculateLayout(metrics);
  check(root.child(0)->type()==NodeType::Paren&&static_cast<NodeParen*>(root.child(0))->delimKind()==DelimKind::Bracket,"editor retains square group");
  check(cursor.cursor().row==&root && cursor.cursor().index==3,"close bracket exits nested fraction to containing row");
  std::string serialized,error;check(numos::CalculationEngine::serializeForGiac(&root,serialized,error)&&serialized=="(((1)/(2)))+1","bracket grouping serializes without Giac vector syntax");
  auto copy=cloneNode(&root);check(dumpTree(copy.get()).find("Paren []")!=std::string::npos,"history clone preserves bracket kind");
  deny=true;try{for(int i=0;i<100;++i)root.calculateLayout(metrics);}catch(...){check(false,"layout allocated");}deny=false;
 }
 check(allocations==0,"zero own layout allocations");
 std::cout<<"CHECKS "<<checks<<" FAILURES "<<failures<<'\n';return failures?1:0;
}
