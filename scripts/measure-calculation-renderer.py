#!/usr/bin/env python3
"""Private host measurement of the real Calculation event path.

Builds instrumented copies against matching native objects. No probe object is
linked into a product, no expression is injected, and no cache is introduced.
Times include instrumentation overhead and are not ESP32 latency claims.
"""
import argparse, ctypes, json, os, re, shlex, subprocess
from pathlib import Path

p=argparse.ArgumentParser(description=__doc__)
for flag in ('source','build','compile-commands','out'):
    p.add_argument('--'+flag,type=Path,required=True)
p.add_argument('--run',action='store_true',help='Measure paired actions and bounded history replacement')
a=p.parse_args();source=a.source.resolve();build=a.build.resolve();out=a.out.resolve()
out.mkdir(parents=True,exist_ok=True)
db=json.loads(a.compile_commands.read_text())
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH',''))
records=[]
def run(name,cmd):
    r=subprocess.run(list(map(str,cmd)),cwd=source,env=env,capture_output=True)
    (out/(name+'.log')).write_bytes(r.stdout+r.stderr)
    records.append(dict(name=name,command=list(map(str,cmd)),exit=r.returncode))
    (out/'commands.json').write_text(json.dumps(records,indent=2))
    r.check_returncode()

header=r'''#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <lvgl.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <set>
namespace calc_profile {
inline bool active=false;
inline unsigned layoutDepth=0,drawDepth=0;
inline unsigned long long liveNodes=0,liveBytes=0;
inline struct Counters {
 unsigned long long layouts=0,nodes=0,layoutGlyphs=0,drawGlyphs=0,otherGlyphs=0;
 unsigned long long draws=0,paintNodes=0,letters=0,lines=0,rects=0,viewport=0;
 unsigned long long layoutNs=0,drawNs=0,drawLayoutNs=0,nodeAllocations=0;
} counters;
inline unsigned long long now() {
 return std::chrono::duration_cast<std::chrono::nanoseconds>(
  std::chrono::steady_clock::now().time_since_epoch()).count();
}
struct LayoutScope {
 unsigned long long start;
 LayoutScope():start(layoutDepth++==0?now():0) {if(active){++counters.nodes;if(start)++counters.layouts;}}
 ~LayoutScope(){--layoutDepth;if(active&&start)counters.layoutNs+=now()-start;}
};
struct DrawScope {
 unsigned long long start,layoutStart;int16_t &x,&y;int16_t oldX,oldY;
 DrawScope(int16_t& a,int16_t& b):start(now()),layoutStart(counters.layoutNs),x(a),y(b),oldX(a),oldY(b){++drawDepth;if(active)++counters.draws;}
 ~DrawScope(){--drawDepth;if(active){counters.drawNs+=now()-start;counters.drawLayoutNs+=counters.layoutNs-layoutStart;if(x!=oldX||y!=oldY)++counters.viewport;}}
};
inline void begin(){counters={};active=true;}
inline void report(const char* label){
 active=false;const auto& c=counters;
 std::printf("[COST] %s layouts=%llu methods=%llu layout_glyphs=%llu draw_glyphs=%llu other_glyphs=%llu draws=%llu paint_nodes=%llu letters=%llu lines=%llu rects=%llu viewport_changes=%llu layout_ns=%llu draw_exclusive_ns=%llu node_allocations=%llu\n",label,c.layouts,c.nodes,c.layoutGlyphs,c.drawGlyphs,c.otherGlyphs,c.draws,c.paintNodes,c.letters,c.lines,c.rects,c.viewport,c.layoutNs,c.drawNs-c.drawLayoutNs,c.nodeAllocations);
}
unsigned long long privateBytes();
}
'''
(out/'profile.h').write_text(header)
runtime=r'''#include "profile.h"
#include <new>
#include <windows.h>
#include <psapi.h>
unsigned long long calc_profile::privateBytes(){PROCESS_MEMORY_COUNTERS_EX p{};p.cb=sizeof(p);return GetProcessMemoryInfo(GetCurrentProcess(),reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&p),sizeof(p))?p.PrivateUsage:0;}
namespace vpam {
struct alignas(std::max_align_t) NodeHeader {std::size_t bytes;};
void* testNodeAllocate(std::size_t bytes){
 auto* p=static_cast<NodeHeader*>(std::malloc(sizeof(NodeHeader)+bytes));
 if(!p)throw std::bad_alloc();p->bytes=bytes;
 ++calc_profile::liveNodes;calc_profile::liveBytes+=bytes;
 if(calc_profile::active)++calc_profile::counters.nodeAllocations;
 return p+1;
}
void testNodeRelease(void* ptr){if(!ptr)return;auto* p=static_cast<NodeHeader*>(ptr)-1;--calc_profile::liveNodes;calc_profile::liveBytes-=p->bytes;std::free(p);}
}
extern "C" bool __real_lv_font_get_glyph_dsc(const lv_font_t*,lv_font_glyph_dsc_t*,uint32_t,uint32_t);
extern "C" bool __wrap_lv_font_get_glyph_dsc(const lv_font_t* f,lv_font_glyph_dsc_t* d,uint32_t c,uint32_t n){
 if(calc_profile::active){auto& k=calc_profile::counters;if(calc_profile::layoutDepth)++k.layoutGlyphs;else if(calc_profile::drawDepth)++k.drawGlyphs;else ++k.otherGlyphs;}
 return __real_lv_font_get_glyph_dsc(f,d,c,n);
}
'''
(out/'profile-runtime.cpp').write_text(runtime)
app=(source/'src/apps/CalculationApp.cpp').read_text(encoding='utf-8')
anchor='bool CalculationApp::debugInput(const std::string& expected) const {'
assert app.count(anchor)==1
observation=r'''
    if(expected.compare(0,14,"profile-begin ")==0){calc_profile::begin();return true;}
    if(expected.compare(0,15,"profile-report ")==0){calc_profile::report(expected.c_str()+15);return true;}
    if(expected.compare(0,10,"ownership ")==0){
        calc_profile::active=false;
        std::set<const vpam::MathNode*> seen;bool unique=true;unsigned historyNodes=0,exactTrees=0,approxTrees=0;
        auto visit=[&](auto&& self,const vpam::MathNode* n,const vpam::MathNode* parent)->unsigned {
            if(!n)return 0;if(!seen.insert(n).second||n->parent()!=parent){unique=false;return 0;}
            unsigned count=1;for(int i=0;i<n->childCount();++i)count+=self(self,n->child(i),n);return count;
        };
        for(const auto& entry:_history){
            historyNodes+=visit(visit,entry.exprAST.get(),nullptr);
            historyNodes+=visit(visit,entry.resultAST.get(),nullptr);
            historyNodes+=visit(visit,entry.approximateAST.get(),nullptr);
            exactTrees+=entry.resultAST?1:0;approxTrees+=entry.approximateAST?1:0;
        }
        visit(visit,_rootNode.get(),nullptr);visit(visit,_resultNode.get(),nullptr);
        visit(visit,_structuredResult.get(),nullptr);visit(visit,_structuredApproxResult.get(),nullptr);
        lv_mem_monitor_t memory{};lv_mem_monitor(&memory);
        const auto giac=numos::GiacEngine::instance().runtimeDiagnostics();
        std::printf("[OWNERSHIP] %s entries=%zu capacity=%zu history_nodes=%u exact_trees=%u approx_trees=%u unique=%d live_nodes=%llu node_bytes=%llu private_bytes=%llu lvgl_free=%zu lvgl_largest=%zu handles=%u contexts=%u first=%s last=%s\n",expected.c_str()+10,_history.size(),_history.capacity(),historyNodes,exactTrees,approxTrees,unique,calc_profile::liveNodes,calc_profile::liveBytes,calc_profile::privateBytes(),memory.free_size,memory.free_biggest_size,giac.liveRetainedHandles,giac.activeContexts,_history.empty()?"":_history.front().exactText.c_str(),_history.empty()?"":_history.back().exactText.c_str());
        return unique&&_history.size()<=MAX_HISTORY;
    }
'''
app=app.replace(anchor,anchor+observation)
ast=(source/'src/math/MathAST.cpp').read_text(encoding='utf-8')
ast,count=re.subn(r'(void \w+::calculateLayout\(const FontMetrics& fm\) \{)',r'\1\n    calc_profile::LayoutScope measured;',ast)
assert count>=24,count
renderer=(source/'src/ui/MathRenderer.cpp').read_text(encoding='utf-8')
renderer=renderer.replace('void MathCanvas::onDraw(lv_event_t* e) {','void MathCanvas::onDraw(lv_event_t* e) {\n    calc_profile::DrawScope measured(_scrollX,_scrollY);')
renderer=renderer.replace('    _scrollX = next;','    if(calc_profile::active && moved)++calc_profile::counters.viewport;\n    _scrollX = next;')
renderer=renderer.replace('    _scrollY = next;','    if(calc_profile::active && moved)++calc_profile::counters.viewport;\n    _scrollY = next;')
anchor='    if (!node) return;\n    if (fm.scriptLevel >= 2) {'
assert renderer.count(anchor)==1
renderer=renderer.replace(anchor,'    if(calc_profile::active && layer)++calc_profile::counters.paintNodes;\n'+anchor)
for function,counter in [('lv_draw_letter','letters'),('lv_draw_line','lines'),('lv_draw_rect','rects')]:
    renderer=re.sub(r'(?m)^(\s*)'+function+r'\(',r'\1if(calc_profile::active)++calc_profile::counters.'+counter+r';\n\1'+function+'(',renderer)
units={'CalculationApp.cpp':app,'MathAST.cpp':ast,'MathRenderer.cpp':renderer}

def arguments(command):
    if os.name!='nt':return shlex.split(command)
    parse=ctypes.windll.shell32.CommandLineToArgvW;parse.argtypes=[ctypes.c_wchar_p,ctypes.POINTER(ctypes.c_int)];parse.restype=ctypes.POINTER(ctypes.c_wchar_p)
    count=ctypes.c_int();ptr=parse(command,ctypes.byref(count));args=[ptr[i] for i in range(count.value)];ctypes.windll.kernel32.LocalFree(ptr);return args

objects={}
for name,text in units.items():
    entry=next(e for e in db if e['file'].endswith(name));cmd=arguments(entry['command'])
    private=out/name;private.write_text('#include "profile.h"\n'+text,encoding='utf-8')
    obj=out/(name+'.o');cmd[cmd.index('-o')+1]=str(obj);cmd[-1]=str(private)
    cmd[1:1]=['-I'+str(Path(entry['file']).parent),'-DNUMOS_MATH_AST_TEST_ALLOCATOR']
    run(name,cmd);objects[Path(name).with_suffix('.o').name]=obj
entry=next(e for e in db if e['file'].endswith('CalculationApp.cpp'));cmd=arguments(entry['command'])
runtime_obj=out/'profile-runtime.o';cmd[cmd.index('-o')+1]=str(runtime_obj);cmd[-1]=str(out/'profile-runtime.cpp');run('runtime',cmd)
linked=[objects.get(f.name,f) for f in (build/'src').rglob('*.o')]
libraries=[*build.rglob('libgiac.a'),*build.rglob('libtommath.a'),*build.rglob('liblvgl.a')];assert len(libraries)==3
rsp=out/'profile.rsp';rsp.write_text('\n'.join('"'+f.as_posix()+'"' for f in [runtime_obj,*linked,*libraries]))
run('link',['g++','@'+str(rsp),'-LC:/SDL2/x86_64-w64-mingw32/lib','-lmingw32','-lSDL2main','-lSDL2','-lpsapi','-Wl,--wrap=lv_font_get_glyph_dsc','-Wl,--gc-sections','-o',out/'profile.exe'])
print('Private cost/ownership observer:',out/'profile.exe')
if not a.run:raise SystemExit(0)

def keys(text):
    physical={'SHIFT':(4,0),'ALPHA':(3,0),'FORMAT':(4,5),'EXP':(2,9)}
    return ''.join('calc_physical %d %d\n'%physical[k] if k in physical else 'key '+k+'\n' for k in text.split())

def replay(name,body):
    script='wait 200\nopen_app Calculation\nwait 30\n'+body+'log MEASUREMENT_DONE\n'
    path=out/(name+'.numos');path.write_text(script,encoding='utf-8')
    frames=sum(int(line.split()[1])+1 if line.startswith('wait ') else 1 for line in script.splitlines())+100
    run(name,[out/'profile.exe','--headless','--deterministic','--quiet','--frames',frames,'--script',path])
    log=(out/(name+'.log')).read_text(encoding='utf-8',errors='replace')
    assert 'MEASUREMENT_DONE' in log
    return log

workloads={
 'short':'2 + 2',
 'scientific-fraction':'6 6 . 6 3 / 2 EXP neg 3',
 'nested-exponent':'5 ^ neg 5 ^ neg 6',
 'long-within-contract':' + '.join(['2']*150),
}
body=''
for name,entered in workloads.items():
    for repetition in range(3):
        body+=keys('AC '+entered)+'wait 5\n'
        for action,events in [('blink','wait 80\n'),('navigate',keys('LEFT RIGHT')+'wait 3\n'),('edit',keys('DEL 2')+'wait 3\n')]:
            label=f'{name}-{action}-{repetition}'
            body+=f'assert_calc_input profile-begin {label}\n'+events+f'assert_calc_input profile-report {label}\nassert_calc_input structure\n'
for name,entered in [('history-exact-approx','0 . 1'),('wide-result','2 ^ 2 0 0'),('periodic-result','1 / 9 7')]:
    for repetition in range(3):
        body+=keys('AC '+entered+' ENTER')+'wait 5\n'
        if name=='periodic-result':
            body+=keys('SHIFT ALPHA FORMAT DOWN DOWN ENTER')+'wait 3\n'
        for action,events in [('unchanged','wait 80\n'),('pan',keys('RIGHT '*8+'LEFT '*8)+'wait 3\n'),('format',keys('FORMAT FORMAT')+'wait 3\n'),('menu',keys('SHIFT ALPHA FORMAT BACK')+'wait 3\n'),('history',keys('UP DOWN')+'wait 3\n')]:
            label=f'{name}-{action}-{repetition}'
            body+=f'assert_calc_input profile-begin {label}\n'+events+f'assert_calc_input profile-report {label}\n'
log=replay('cost',body)
cost=[]
for line in log.splitlines():
    if line.startswith('[COST] '):
        fields=line.split();cost.append(dict(id=fields[1],**{k:int(v) for k,v in (f.split('=') for f in fields[2:])}))
assert len(cost)==81,len(cost)
assert all(r['node_allocations']==0 for r in cost if '-blink-' in r['id'] or '-navigate-' in r['id'] or '-unchanged-' in r['id'] or '-pan-' in r['id'])
(out/'cost.json').write_text(json.dumps({'scope':'Instrumented Windows native, method-entry counts and host nanoseconds; no ESP latency inference','records':cost},indent=2))

# Same active app: its 50-entry history stays owned throughout replacement.
# HOME is deliberately not a zero-memory assertion: firmware retains this app.
body=''
patterns=[('0 . 1','FORMAT FORMAT'),('7 / 3','SHIFT ALPHA FORMAT '+'DOWN '*6+'ENTER FORMAT'),('[ SQRT neg 1 ] + 1','SHIFT ALPHA FORMAT DOWN DOWN ENTER FORMAT'),('2 pi','FORMAT FORMAT'),('1 2 3 4','SHIFT EXP FORMAT')]
for cycle in range(1,161):
    expression,formatting=patterns[(cycle-1)%len(patterns)]
    body+=keys('AC '+expression+' ENTER '+formatting)+'wait 3\n'
    if cycle in (1,10,49,50,60,100,110,150,160):body+=f'assert_calc_input ownership cycle-{cycle}\n'
log=replay('history',body)
ownership=[]
for line in log.splitlines():
    if line.startswith('[OWNERSHIP] '):
        fields=line.split();item=dict(id=fields[1],**dict(f.split('=',1) for f in fields[2:]));ownership.append(item)
        assert item['unique']=='1' and int(item['entries'])<=50
settled=[r for r in ownership if int(r['id'].split('-')[1])>=60]
for field in ('entries','capacity','history_nodes','exact_trees','approx_trees','live_nodes','node_bytes','handles','contexts','first','last'):
    assert len({r[field] for r in settled})==1,(field,settled)
assert all(r['entries']=='50' and r['exact_trees']=='50' and r['approx_trees']=='50' for r in settled)
(out/'ownership.json').write_text(json.dumps({'scope':'Active-app ownership and Windows process private bytes; node bytes exclude vector/string storage and Giac internal malloc; LVGL is CLIB in this build','records':ownership},indent=2))
print('PASS 81 measured actions; 160 evaluations with retained, replaced 50-entry history')
