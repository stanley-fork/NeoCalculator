# SPDX-License-Identifier: GPL-3.0-or-later
# Reproducible host-only allocation overlay, reused from the accepted 02A probe.
# Instruments requested C++/MathAST payload; excludes Giac malloc and LVGL pool.
from pathlib import Path
import json,shlex,subprocess,shutil,argparse
parser=argparse.ArgumentParser()
parser.add_argument('--source',default='C:/.codex-cache/numos-teaching-ux-baseline')
parser.add_argument('--build',default='C:/.piobuild/numOS/tutor-teaching-baseline-build/emulator_pc')
parser.add_argument('--label',default='baseline')
parser.add_argument('--database')
parser.add_argument('--results',action='store_true',help='Instrument periodic Results preparation instead of Steps')
parser.add_argument('--out',required=True)
parser.add_argument('--scratch',required=True)
options=parser.parse_args()
repo=Path(__file__).resolve().parents[1];out=Path(options.out).resolve();out.mkdir(parents=True,exist_ok=True)
baseline=Path(options.source).resolve()
build=Path(options.build).resolve()
scratch=Path(options.scratch).resolve();scratch.mkdir(parents=True,exist_ok=True)
(scratch/'ui-allocation-probe.h').write_text('#pragma once\n#include "math/MathAST.h"\nnamespace uiprobe {\nvoid* allocateNode(std::size_t size);\nvoid releaseNode(void* ptr) noexcept;\nvoid begin();\nvoid finish(unsigned nodes);\ninline unsigned nodes(const vpam::MathNode* node,unsigned depth=0) {\n    if(!node)return 0;\n    if(depth>32)return 1;\n    unsigned result=1;\n    for(int i=0;i<node->childCount();++i)result+=nodes(node->child(i),depth+1);\n    return result;\n}\ntemplate<class F>struct Guard { F fn;~Guard(){fn();} };\ntemplate<class F>Guard<F> guard(F fn){return {fn};}\n}\n',encoding='utf-8')
(scratch/'ui-allocation-probe.cpp').write_text('#include <new>\n#include <cstdlib>\n#include <cstddef>\n#include <cstdio>\n#include <chrono>\nnamespace {\nstruct alignas(std::max_align_t) Header {size_t bytes;};\nsize_t live=0,peak=0,allocations=0,before=0,beforeAllocations=0;\nsize_t nodeLive=0,nodePeak=0,nodeBefore=0,nodeAllocations=0,combinedPeak=0;\nsize_t attempts=0,failAt=0,failPage=0,page=0,failures=0;\nbool active=false,persistent=false;\nstd::chrono::steady_clock::time_point started;\nvoid fault(){\n if(active){++attempts;if(failAt && page==failPage && (attempts==failAt || (persistent&&attempts>failAt))){++failures;throw std::bad_alloc();}}\n}\nvoid* allocate(size_t n){\n fault();\n auto* h=static_cast<Header*>(std::malloc(sizeof(Header)+(n?n:1)));if(!h)throw std::bad_alloc();h->bytes=n;live+=n;++allocations;if(live>peak)peak=live;if(live+nodeLive>combinedPeak)combinedPeak=live+nodeLive;return h+1;\n}\nvoid release(void* p)noexcept{if(!p)return;auto* h=static_cast<Header*>(p)-1;live-=h->bytes;std::free(h);}\n}\nvoid* operator new(size_t n){return allocate(n);}void* operator new[](size_t n){return allocate(n);}\nvoid operator delete(void* p)noexcept{release(p);}void operator delete[](void* p)noexcept{release(p);}\nvoid operator delete(void* p,size_t)noexcept{release(p);}void operator delete[](void* p,size_t)noexcept{release(p);}\nnamespace uiprobe {\nvoid* allocateNode(size_t n){fault();auto* h=static_cast<Header*>(std::malloc(sizeof(Header)+n));if(!h)throw std::bad_alloc();h->bytes=n;nodeLive+=n;++nodeAllocations;if(nodeLive>nodePeak)nodePeak=nodeLive;if(live+nodeLive>combinedPeak)combinedPeak=live+nodeLive;return h+1;}\nvoid releaseNode(void* p)noexcept{if(!p)return;auto* h=static_cast<Header*>(p)-1;nodeLive-=h->bytes;std::free(h);}\nvoid begin(){\n if(!page){const auto* at=std::getenv("NUMOS_UI_FAIL_AT");failAt=at?std::strtoul(at,nullptr,10):0;const auto* target=std::getenv("NUMOS_UI_FAIL_PAGE");failPage=target?std::strtoul(target,nullptr,10):1;const auto* mode=std::getenv("NUMOS_UI_FAIL_PERSISTENT");persistent=mode&&*mode==\'1\';}\n ++page;attempts=0;failures=0;before=live;peak=live;beforeAllocations=allocations;nodeBefore=nodeLive;nodePeak=nodeLive;nodeAllocations=0;combinedPeak=live+nodeLive;started=std::chrono::steady_clock::now();active=true;\n}\nvoid finish(unsigned nodes){active=false;const auto micros=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-started).count();std::printf("UI_ALLOCATION|micros=%lld|cpp_peak_extra=%zu|cpp_live_delta=%lld|cpp_allocations=%zu|nodes=%u|page=%zu|attempts=%zu|failures=%zu|node_peak_extra=%zu|node_live_delta=%lld|node_live_before=%zu|node_live_after=%zu|node_allocations=%zu|combined_peak_extra=%zu\\n",static_cast<long long>(micros),peak-before,static_cast<long long>(live)-static_cast<long long>(before),allocations-beforeAllocations,nodes,page,attempts,failures,nodePeak-nodeBefore,static_cast<long long>(nodeLive)-static_cast<long long>(nodeBefore),nodeBefore,nodeLive,nodeAllocations,combinedPeak-before-nodeBefore);}\n}\n',encoding='utf-8')
source=(baseline/'src/apps/EquationsApp.cpp').read_text(encoding='utf-8')
source='#include "ui-allocation-probe.h"\n'+source
needle='void EquationsApp::drawStep() {'
injected='\n    uiprobe::begin(); auto memoryReviewGuard=uiprobe::guard([&]{unsigned total=0;for(const auto& root:_viewNodes)total+=uiprobe::nodes(root.get());uiprobe::finish(total);});'
if options.results:
 needle='if(_giacResult.setKind==numos::SolutionSetKind::Periodic) {'
 assert source.count(needle)==1
 source=source.replace(needle,needle+injected,1)
elif needle in source:source=source.replace(needle,needle+injected,1)
else:
 inc=(baseline/'src/apps/TutorStepsView.inc').read_text(encoding='utf-8')
 needle='void EquationsApp::drawStep(bool preserveScroll) {'
 assert needle in inc
 inc=inc.replace(needle,needle+injected,1)
 (scratch/'TutorStepsView.inc').write_text(inc,encoding='utf-8')
 (out/('TutorStepsView-'+options.label+'-probe.inc')).write_text(inc,encoding='utf-8')
stem='EquationsApp-'+options.label+'-probe'
(scratch/(stem+'.cpp')).write_text(source,encoding='utf-8')
(out/(stem+'.cpp')).write_text(source,encoding='utf-8')
record=next(x for x in json.loads((Path(options.database) if options.database else baseline/'compile_commands.json').read_text()) if x['file'].endswith('EquationsApp.cpp'))
args=shlex.split(record['command'].replace('\\','/'))
args+=['-I'+str(scratch),'-Isrc/apps','-Isrc/math']
math_source=(baseline/'src/math/MathAST.cpp').read_text(encoding='utf-8')
math_source='#include "ui-allocation-probe.h"\n'+math_source.replace('p = std::malloc(size);','p = uiprobe::allocateNode(size);',1).replace('std::free(ptr);','uiprobe::releaseNode(ptr);',1)
(scratch/'MathAST-node-probe.cpp').write_text(math_source,encoding='utf-8')
(out/('MathAST-'+options.label+'-node-probe.cpp')).write_text(math_source,encoding='utf-8')
for name in [stem,'ui-allocation-probe','MathAST-node-probe']:
 command=args.copy();command[command.index('-o')+1]=str(scratch/(name+'.o'))
 command[command.index(record['file'].replace('\\','/'))]=str(scratch/(name+'.cpp'))
 result=subprocess.run(command,cwd=baseline,capture_output=True)
 (out/(name+'-compile.log')).write_bytes(result.stdout+result.stderr)
 assert result.returncode==0,result.stderr.decode(errors='replace')[-1500:]
objects=[p for p in (build/'src').rglob('*.o') if p.name not in ('EquationsApp.o','MathAST.o')]
objects += [scratch/(stem+'.o'),scratch/'ui-allocation-probe.o',scratch/'MathAST-node-probe.o']
libs=list(build.rglob('*.a'))
rsp='\n'.join('"'+str(p).replace('\\','/')+'"' for p in objects)+'\n-Wl,--start-group\n'+'\n'.join('"'+str(p).replace('\\','/')+'"' for p in libs)+'\n-Wl,--end-group\n-LC:/SDL2/x86_64-w64-mingw32/lib\n-lmingw32\n-lSDL2main\n-lSDL2\n'
binary='native-'+options.label+'-profile'
(scratch/(binary+'.rsp')).write_text(rsp)
(out/(binary+'.rsp')).write_text(rsp)
result=subprocess.run(['C:/mingw64/bin/g++.exe','@'+str(scratch/(binary+'.rsp')),'-Wl,--gc-sections','-static-libstdc++','-static-libgcc','-o',str(scratch/(binary+'.exe'))],cwd=baseline,capture_output=True)
(out/(binary+'-link.log')).write_bytes(result.stdout+result.stderr)
assert result.returncode==0,result.stderr.decode(errors='replace')[-1500:]
shutil.copy2(scratch/(binary+'.exe'),out/(binary+'.exe'))
print('Isolated '+options.label+' UI allocation profiler linked.')
