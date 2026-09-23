#!/usr/bin/env python3
"""Isolated host timing overlay. Never included in ordinary firmware.

Scopes are inclusive: auxiliary/return/reconciliation are nested within tutor.
Print only after each timer stops. Reuse the same Giac context across cases.
"""
from pathlib import Path
import argparse, hashlib, json, os, shlex, subprocess
p=argparse.ArgumentParser();p.add_argument('--source',type=Path,required=True);p.add_argument('--build',type=Path,required=True);p.add_argument('--scratch',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
source=a.source.resolve();build=a.build.resolve();scratch=a.scratch.resolve();out=a.out.resolve()
scratch.mkdir(parents=True,exist_ok=True);out.mkdir(parents=True,exist_ok=True)
header='''#pragma once
#include <chrono>
#include <cstdio>
namespace compositionprobe {
struct Span { const char* name; std::chrono::steady_clock::time_point start=std::chrono::steady_clock::now();
 explicit Span(const char* n):name(n){} ~Span(){auto us=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-start).count();std::fprintf(stderr,"PHASE|%s|%.3f\\n",name,us);} };
}
'''
(scratch/'CompositionProbe.h').write_text(header)
def replace(text,old,new):
    assert text.count(old)==1,(old,text.count(old));return text.replace(old,new)
engine=(source/'src/math/giac/GiacEngine.cpp').read_text()
engine=replace(engine,'const SolveEquation& equation, const std::string& variable,\n    SolveDomainPolicy policy) {','const SolveEquation& equation, const std::string& variable,\n    SolveDomainPolicy policy) {\ncompositionprobe::Span probe("ordinary");')
(scratch/'GiacEngine.cpp').write_text('#include "CompositionProbe.h"\n'+engine)
tutor=(source/'src/math/giac/GiacTutor.inc').read_text()
tutor=replace(tutor,'Verdict reconcile(Algebra &a, const Derivation &d, const StructuredSolveResult &answer) {','Verdict reconcile(Algebra &a, const Derivation &d, const StructuredSolveResult &answer) {\ncompositionprobe::Span probe("reconciliation");')
tutor=replace(tutor,'const auto start = std::chrono::steady_clock::now();','compositionprobe::Span probe("tutor");\n    const auto start = std::chrono::steady_clock::now();')
(scratch/'GiacTutor.inc').write_text(tutor)
compose=(source/'src/math/giac/GiacTutorComposition.inc').read_text()
compose=replace(compose,'void completeChild(Algebra& a,Derivation& child,std::string& diagnostic){','void completeChild(Algebra& a,Derivation& child,std::string& diagnostic){\ncompositionprobe::Span probe(child.input.childRole==1?"auxiliary":"return");')
(scratch/'GiacTutorComposition.inc').write_text(compose)
main='''#include "math/giac/GiacEngine.h"
#include <cstdio>
#include <iostream>
bool setting_complex_enabled=false;
int main(){using namespace numos;auto& e=GiacEngine::instance();if(!e.begin())return 2;
 const char* inputs[]={"x^4-5*x^2+4","exp(2*x)-3*exp(x)+2","ln(x-1)^2-3*ln(x-1)+2","6*sin(x)^2-5*sin(x)+1","tan(2*x)^2-3","2*x^2+3*x-4"};
 for(auto lhs:inputs)for(unsigned i=0;i<6;++i){std::fprintf(stderr,"CASE|%s|%u\\n",lhs,i);auto answer=e.solveStructured({lhs,"0"},"x",SolveDomainPolicy::RealOnly);
 tutor::Snapshot s;s.inputEpoch=1;s.engineGeneration=e.generation();s.authored={{lhs,"0"}};s.variables={"x"};auto trace=e.explainEquations(s,answer);
 if(trace.status!=tutor::Status::Complete||trace.validity!=tutor::Verdict::Verified)return 3;
 std::cout<<"TRACE|"<<lhs<<"|"<<i<<"|"<<tutor::replayJson(trace)<<'\\n';
 }return 0;}
'''
# Use the public replay schema; never inspect raw gens.
(scratch/'main.cpp').write_text(main)
database=json.loads((source/'compile_commands.json').read_text())
record=next(x for x in database if x['file'].endswith('GiacEngine.cpp') and 'NATIVE_SIM' in x['command'])
command=shlex.split(record['command'].replace('\\','/'));compiler=command[0]
commands=[]
def run(name,args):
    r=subprocess.run(args,cwd=source,capture_output=True,env=dict(os.environ,PATH='C:/mingw64/bin;'+os.environ['PATH']))
    (out/(name+'.log')).write_bytes(r.stdout+r.stderr);commands.append(dict(name=name,command=args,exit=r.returncode));(out/'commands.json').write_text(json.dumps(commands,indent=2));r.check_returncode();return r
for file in ['GiacEngine.cpp','main.cpp']:
    args=command.copy();args[args.index('-o')+1]=str(scratch/(file+'.o'));args[args.index(record['file'].replace('\\','/'))]=str(scratch/file)
    args+=['-I'+str(scratch),'-I'+str(source/'src/math/giac')];run(file,args)
objects=[x for x in (build/'src').rglob('*.o') if ('math' in x.parts or 'fonts' in x.parts or x.name in ['FileSystem.o','MathTypography.o']) and x.name!='GiacEngine.o']
objects+=[scratch/'GiacEngine.cpp.o',scratch/'main.cpp.o',*build.rglob('liblvgl.a'),*build.rglob('libgiac.a'),*build.rglob('libtommath.a')]
rsp=scratch/'link.rsp';rsp.write_text('\n'.join('"'+x.as_posix()+'"' for x in objects));binary=scratch/'profile.exe'
run('link',[compiler,'@'+str(rsp),'-Wl,--gc-sections','-o',str(binary)])
run('measurement',[str(binary)])
(out/'identity.json').write_text(json.dumps({str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [binary,*scratch.glob('*.inc'),*scratch.glob('*.cpp'),scratch/'CompositionProbe.h']},indent=2))
