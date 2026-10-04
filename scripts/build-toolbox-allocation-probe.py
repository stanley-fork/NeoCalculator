#!/usr/bin/env python3
"""Private native overlay: persistent C++/AST faults; no production fault API.

LVGL's C allocator and Giac are excluded. Fixed LVGL pool is tested separately.
NUMOS_TOOLBOX_SCOPE=open|preview|search|insert|save, NUMOS_TOOLBOX_FAIL_AT=N,
NUMOS_TOOLBOX_FAIL_CALL=N select the scope/invocation. Failure stays active
through the rest of that scope, including recovery, not just one allocation.
"""
from pathlib import Path
import argparse,json,shlex,subprocess
p=argparse.ArgumentParser();p.add_argument('--database',required=True);p.add_argument('--build',required=True);p.add_argument('--out',required=True);p.add_argument('--quantities',action='store_true');a=p.parse_args()
root=Path(__file__).resolve().parents[1];out=Path(a.out).resolve();out.mkdir(parents=True,exist_ok=True)
header='''#pragma once
#include <cstddef>
namespace tbprobe {
struct Scope { bool own=false;const char* name;explicit Scope(const char*);~Scope(); };
void* node(std::size_t);void releaseNode(void*);
struct Suspend {bool was;Suspend();~Suspend();};
}
'''
source='''#include "probe.h"
#include <new>
#include <cstdlib>
#include <cstdio>
#include <cstring>
namespace {
struct alignas(std::max_align_t) Header {size_t size;};
size_t live=0,peak=0,before=0,attempts=0,failures=0,call=0,target=1,point=0;
bool active=false;
void* allocate(size_t n) {
 if(active && ++attempts>=point && point){++failures;throw std::bad_alloc();}
 auto* p=static_cast<Header*>(std::malloc(sizeof(Header)+(n?n:1)));
 if(!p)throw std::bad_alloc();p->size=n;live+=n;if(live>peak)peak=live;return p+1;
}
void release(void* p){if(p){auto* h=static_cast<Header*>(p)-1;live-=h->size;std::free(h);}}
}
void* operator new(size_t n){return allocate(n);}void* operator new[](size_t n){return allocate(n);}
void operator delete(void* p)noexcept{release(p);}void operator delete[](void* p)noexcept{release(p);}
void operator delete(void* p,size_t)noexcept{release(p);}void operator delete[](void* p,size_t)noexcept{release(p);}
namespace tbprobe {
Suspend::Suspend():was(active){active=false;}Suspend::~Suspend(){active=was;}
void* node(size_t n){return allocate(n);}void releaseNode(void* p){release(p);}
Scope::Scope(const char* value):name(value) {
 const char* selected=std::getenv("NUMOS_TOOLBOX_SCOPE");
 if(active || !selected || std::strcmp(selected,name))return;
 ++call;const char* at=std::getenv("NUMOS_TOOLBOX_FAIL_AT");point=at?std::strtoul(at,nullptr,10):0;
 const char* nth=std::getenv("NUMOS_TOOLBOX_FAIL_CALL");target=nth?std::strtoul(nth,nullptr,10):1;
 if(call!=target)return;
 own=active=true;attempts=failures=0;before=peak=live;
}
Scope::~Scope(){if(!own)return;active=false;std::printf("TOOLBOX_ALLOC scope=%s call=%zu attempts=%zu failures=%zu peak=%zu delta=%lld\\n",name,call,attempts,failures,peak-before,(long long)live-(long long)before);}
}
'''
(out/'probe.h').write_text(header);(out/'probe.cpp').write_text(source)
ui=(root/'src/ui/Toolbox.cpp').read_text(encoding='utf8')
injections={
 'bool open(lv_obj_t* parent,Receiver receiver) {':'open',
 'bool text(const char* utf8) {':'search',
 'void activate(bool right=false) {':'insert',
 '        if(item.entry) {':'preview',
}
for needle,scope in injections.items():
 assert ui.count(needle)==1,(needle,ui.count(needle))
 ui=ui.replace(needle,needle+'\n tbprobe::Scope faultScope("'+scope+'");')
ui=ui.replace('s->rows[i]=lv_obj_create(s->panel);',
    's->rows[i]=(std::getenv("NUMOS_TOOLBOX_FAIL_ROW") && std::strtoul(std::getenv("NUMOS_TOOLBOX_FAIL_ROW"),nullptr,10)==i+1)?nullptr:lv_obj_create(s->panel);')
ui=ui.replace('    auto& cc=*s->receiver.cursor;','    if(std::getenv("NUMOS_TOOLBOX_STALE"))s->epoch^=1;\n    auto& cc=*s->receiver.cursor;')
(out/'Toolbox.cpp').write_text('#include "probe.h"\n#include <cstdlib>\n'+ui,encoding='utf8')
store=(root/'src/math/ToolboxStore.cpp').read_text(encoding='utf8').replace('bool Store::save() noexcept {','bool Store::save() noexcept {\n tbprobe::Scope faultScope("save");')
(out/'ToolboxStore.cpp').write_text('#include "probe.h"\n'+store,encoding='utf8')
math=(root/'src/math/MathAST.cpp').read_text(encoding='utf8')
assert 'p = std::malloc(size);' in math
math=math.replace('p = std::malloc(size);','p = tbprobe::node(size);',1).replace('std::free(ptr);','tbprobe::releaseNode(ptr);',1)
(out/'MathAST.cpp').write_text('#include "probe.h"\n'+math,encoding='utf8')
overlayNames=['Toolbox','ToolboxStore','MathAST','probe']
if a.quantities:
    scopes={
      'Quantity':{'Analysis evaluate(const MathNode* root,const void* owner,Resolve resolve) {':'analysis',
       'Error convert(const Value& value,const Descriptor& descriptor,Display& destination) {':'convert',
       'NodePtr compose(NodePtr coefficient,const Descriptor& descriptor) {':'compose'},
      'CalculationEngine':{'bool CalculationEngine::commitResultAns(const CalculationEvaluation& result,const vpam::ExactVal* mirror) {':'ans',
       'bool CalculationEngine::storeAns(char varName) {':'memory'},
      'CalculationApp':{'void CalculationApp::openOutputSelector(int component) {':'selector',
       'bool CalculationApp::publishOutput(const numos::quantity::Descriptor& descriptor) {':'publish',
       'bool CalculationApp::publishQuantityFormat(numos::CalculationFormat mode,int shift,unsigned places) {':'format',
       'void CalculationApp::loadHistoryEntry(int index) {':'history'}
    }
    for name,anchors in scopes.items():
        directory='apps' if name=='CalculationApp' else 'math'
        text=(root/'src'/directory/(name+'.cpp')).read_text(encoding='utf8')
        for anchor,scope in anchors.items():
            assert text.count(anchor)==1,anchor
            text=text.replace(anchor,anchor+'\n tbprobe::Scope quantityFault("'+scope+'");')
        if name=='Quantity':
            # The injected failures target NumOS ownership/AST, not Giac's
            # independent allocator/error machinery. Measure those separately.
            text=text.replace('    result=GiacEngine::instance().evaluateStructured', '    {tbprobe::Suspend pause;result=GiacEngine::instance().evaluateStructured')
            text=text.replace('GiacEngine::EvaluationAngle::Current);','GiacEngine::EvaluationAngle::Current);}')
        (out/(name+'.cpp')).write_text('#include "probe.h"\n'+text,encoding='utf8')
        overlayNames.append(name)

db=json.loads(Path(a.database).read_text())
record=next(r for r in db if r['file'].endswith('Toolbox.cpp') and 'NATIVE_SIM' in r['command'])
args=shlex.split(record['command'].replace('\\','/'))+['-Isrc/ui','-Isrc/math','-Isrc/apps','-I'+str(out)]
for name in overlayNames:
 command=args.copy();command[command.index('-o')+1]=str(out/(name+'.o'));command[command.index(record['file'].replace('\\','/'))]=str(out/(name+'.cpp'))
 result=subprocess.run(command,cwd=root,capture_output=True);(out/(name+'-compile.log')).write_bytes(result.stdout+result.stderr)
 assert result.returncode==0,result.stderr.decode(errors='replace')[-2500:]
build=Path(a.build)
objects=[f for f in (build/'src').rglob('*.o') if f.stem not in overlayNames]+[out/(n+'.o') for n in overlayNames]
libs=list(build.rglob('*.a'))
quoted=lambda paths:'\n'.join('"'+str(f).replace('\\','/')+'"' for f in paths)
rsp=quoted(objects)+'\n-Wl,--start-group\n'+quoted(libs)+'\n-Wl,--end-group\n-LC:/SDL2/x86_64-w64-mingw32/lib\n-lmingw32\n-lSDL2main\n-lSDL2\n'
(out/'link.rsp').write_text(rsp)
result=subprocess.run(['C:/mingw64/bin/g++.exe','@'+str(out/'link.rsp'),'-Wl,--gc-sections','-static-libstdc++','-static-libgcc','-o',str(out/'toolbox-probe.exe')],cwd=root,capture_output=True)
(out/'link.log').write_bytes(result.stdout+result.stderr);assert result.returncode==0,result.stderr.decode(errors='replace')[-2500:]
print('PASS isolated Toolbox allocator overlay linked')
