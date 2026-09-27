// SPDX-License-Identifier: GPL-3.0-or-later
// Editor/serializer and persistent allocation-fault checks, separate from app keys.
#include "math/CursorController.h"
#include "math/CalculationEngine.h"
#include <cstdio>
#include <cstdlib>
#include <new>
namespace { bool armed=false; size_t attempts=0, failAt=SIZE_MAX, failures=0, live=0; }
void* operator new(size_t n) {
    if (armed && ++attempts>=failAt) { ++failures; throw std::bad_alloc(); }
    if (void* p=std::malloc(n?n:1)) { ++live; return p; }
    throw std::bad_alloc();
}
void* operator new[](size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { if(p) { --live; std::free(p); } }
void operator delete[](void* p) noexcept { ::operator delete(p); }
void operator delete(void* p,size_t) noexcept { ::operator delete(p); }
void operator delete[](void* p,size_t) noexcept { ::operator delete(p); }
namespace vpam {
void* testNodeAllocate(size_t n) { return ::operator new(n); }
void testNodeRelease(void* p) { ::operator delete(p); }
}
using namespace vpam;
static void check(bool ok,const char* label) { if(!ok) { std::printf("FAIL %s\n",label); std::exit(1); } }
static bool parents(const MathNode* n,const MathNode* p=nullptr) {
    if(!n || n->parent()!=p)return false;
    for(int i=0;i<n->childCount();++i)if(!parents(n->child(i),n))return false;
    return true;
}
static bool serial(const MathNode* n,const char* expected) {
    std::string text,error;
    const bool ok=numos::CalculationEngine::serializeForGiac(n,text,error);
    return expected ? ok && text==expected : !ok;
}
int main() {
    for(auto sign:{OpKind::Sub,OpKind::Add}) {
        auto row=makeRow();CursorController c;c.init(static_cast<NodeRow*>(row.get()));
        c.insertDigit('1');c.insertDigit('0');c.insertPower();c.insertOperator(sign);
        check(serial(row.get(),nullptr),"sign-only exponent remains incomplete");
        c.insertDigit('3');check(parents(row.get()),"negative exponent parents");
        check(serial(row.get(),sign==OpKind::Sub?"((10)^((-1)*3))":"((10)^(3))"),"signed placeholder filled");
        auto copy=cloneNode(row.get());check(parents(copy.get()),"clone ownership");
    }
    // Inserting a template after a mantissa never captures the mantissa as its base.
    for(bool mantissa:{false,true}) {
        auto row=makeRow();auto* root=static_cast<NodeRow*>(row.get());CursorController c;c.init(root);
        if(mantissa)c.insertDigit('2');
        check(c.insertPowerOfTen(),"scientific insertion");
        check(serial(root,nullptr),"one pending exponent");
        auto* power=static_cast<NodePower*>(root->child(root->childCount()-1));
        check(power->base()->childCount()==1 && power->base()->child(0)->type()==NodeType::Number &&
              static_cast<NodeNumber*>(power->base()->child(0))->value()=="10","base10 identity");
        check(power->exponent()==c.cursor().row && c.cursor().index==0,"active exponent slot");
        c.insertOperator(OpKind::Sub);c.insertDigit('3');
        check(serial(root,mantissa?"2*((10)^((-1)*3))":"((10)^((-1)*3))"),"scientific serialization");
        check(parents(root),"scientific parents");
    }
    unsigned tested=0;
    for(bool mantissa:{false,true}) for(size_t point=1;point<=16;++point) {
        const auto before=live;
        {
            auto row=makeRow();CursorController c;c.init(static_cast<NodeRow*>(row.get()));
            if(mantissa)c.insertDigit('2');
            const auto cursor=c.cursor();const auto nodes=row->childCount();
            attempts=failures=0;failAt=point;armed=true;
            c.insertPower();
            if(failures) check(c.cursor().row==cursor.row && c.cursor().index==cursor.index && row->childCount()==nodes,"power fault preserved base/cursor");
            armed=false;
            if(failures) { ++tested;c.insertPower(); }
            c.insertOperator(OpKind::Sub);c.insertDigit('2');
            check(serial(row.get(),mantissa?"((2)^((-1)*2))":nullptr),"power after failure");
            check(parents(row.get()),"generic power ownership");
        }
        check(live==before,"generic power allocation restoration");
    }
    {
        auto row=makeRow();CursorController c;c.init(static_cast<NodeRow*>(row.get()));
        c.insertDigit('2');c.insertPower();c.insertPower();c.insertDigit('2');
        c.backspace();c.backspace();c.insertOperator(OpKind::Sub);c.insertDigit('2');
        check(serial(row.get(),"((2)^((-1)*2))"),"nested incomplete template removed");
        check(parents(row.get()),"delete/re-entry parents");
    }
    {
        auto row=makeRow();CursorController c;c.init(static_cast<NodeRow*>(row.get()));
        c.insertParen();c.insertDigit('2');c.insertOperator(OpKind::Sub);c.insertDigit('3');c.moveRight();
        const auto cursor=c.cursor();
        attempts=failures=0;failAt=1;armed=true;c.backspace();
        check(failures && c.cursor().row==cursor.row && c.cursor().index==cursor.index,"unwrap capacity failure preserves cursor");
        armed=false;check(serial(row.get(),"(2-3)"),"unwrap failure preserves expression");
        c.backspace();check(serial(row.get(),"2-3"),"unwrap preserves term order");
        check(parents(row.get()),"unwrap parents");++tested;
    }
    {
        auto row=makeRow();CursorController c;c.init(static_cast<NodeRow*>(row.get()));
        c.insertDigit('2');c.insertOperator(OpKind::Div);c.insertOperator(OpKind::Sub);c.insertDigit('2');
        check(serial(row.get(),"2/((-1)*2)"),"negative divisor stays grouped");
    }
    for(bool mantissa:{false,true}) for(size_t point=1;point<=16;++point) {
        const auto before=live;
        {
            auto row=makeRow();CursorController c;c.init(static_cast<NodeRow*>(row.get()));
            if(mantissa)c.insertDigit('2');
            const auto cursor=c.cursor();const auto nodes=row->childCount();
            attempts=failures=0;failAt=point;armed=true;
            const bool inserted=c.insertPowerOfTen();
            // Fault stays active through failure unwinding. No logging allocates here.
            check(inserted || (c.cursor().row==cursor.row && c.cursor().index==cursor.index && row->childCount()==nodes),"transactional failure");
            armed=false;
            if(failures) {
                ++tested;check(!inserted,"allocation failure result");
                check(serial(row.get(),mantissa?"2":nullptr),"failure preserved input");
                check(c.insertPowerOfTen(),"healthy recovery without engine reset");
            }
            check(parents(row.get()),"post-fault ownership");
        }
        check(live==before,"failed and healthy insertion released every C++ allocation");
    }
    std::printf("PASS editor/serializer; %u persistent allocation sites; sizeof(Cursor)=%zu NodePower=%zu NodeRow=%zu\n",tested,sizeof(Cursor),sizeof(NodePower),sizeof(NodeRow));
}
