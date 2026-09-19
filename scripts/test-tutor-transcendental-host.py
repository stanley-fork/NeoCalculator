#!/usr/bin/env python3
"""Link tutor tests to a matching pinned native build, then execute focused gates.

Build emulator_pc first. Each source/build pair must describe the same snapshot;
the object hashes and commands are recorded, and no cache sources are modified.
"""
from pathlib import Path
import argparse, hashlib, json, os, subprocess

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--source',type=Path,required=True);p.add_argument('--build',type=Path,required=True)
p.add_argument('--trig',action='store_true');p.add_argument('--out',type=Path,required=True);p.add_argument('--compiler',default='g++')
a=p.parse_args();source=a.source.resolve();build=a.build.resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
objects=[p for p in (build/'src').rglob('*.o') if 'math' in p.parts or p.name=='FileSystem.o']
libraries=[*build.rglob('liblvgl.a'),*build.rglob('libgiac.a'),*build.rglob('libtommath.a')]
assert objects and len(libraries)==3,'matching emulator_pc build required'
flags=['-std=gnu++17','-O1','-ffunction-sections','-fdata-sections','-DNATIVE_SIM','-DLV_CONF_INCLUDE_SIMPLE','-DLV_USE_STDLIB_MALLOC=LV_STDLIB_CLIB','-I.','-Isrc','-I.pio/libdeps/emulator_pc/lvgl']
env=dict(os.environ);env['PATH']='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+env.get('PATH','')
records=[]
def run(name,command):
    r=subprocess.run(command,cwd=source,env=env,capture_output=True)
    (out/(name+'.log')).write_bytes(r.stdout+r.stderr);records.append(dict(name=name,command=command,exit=r.returncode))
    (out/'commands.json').write_text(json.dumps(records,indent=2));r.check_returncode()
units=['tutor_engine_main','tutor_transcendental_checks','tutor_nonlinear_checks','tutor_nonlinear_allocation','tutor_conclusion_presentation','tutor_transcendental_timing','tutor_teaching_math','tutor_transcendental_probe']
if a.trig:units+=['tutor_trig_checks','tutor_trig_probe']
for name in units:
    unit_flags=flags.copy()
    if name=='tutor_teaching_math':
        unit_flags.remove('-DNATIVE_SIM');unit_flags.append('-DNUMOS_GIAC_HOST_HARNESS=1')
    if name in ('tutor_transcendental_probe','tutor_trig_probe'):
        unit_flags+=['-IC:/SDL2/x86_64-w64-mingw32/include','-Ilib/giac','-Ilib/giac/src','-Ilib/libtommath','-D_USE_MATH_DEFINES',
                     *['-D'+d for d in 'HAVE_CONFIG_H IN_GIAC GIAC_KHICAS NO_GUI GIAC_GENERIC EMBEDDED USE_GMP_REPLACEMENTS UMAP DOUBLEVAL'.split()]]
    obj=out/(name+'.o');run(name+'-compile',[a.compiler,*unit_flags,'-c','tests/host/'+name+'.cpp','-o',str(obj)])
    rsp=out/(name+'.rsp');rsp.write_text('\n'.join('"'+p.as_posix()+'"' for p in [obj,*objects,*libraries]))
    binary=out/(name+'.exe');run(name+'-link',[a.compiler,'@'+str(rsp),'-Wl,--gc-sections','-o',str(binary)])
    if name not in ('tutor_engine_main','tutor_teaching_math'):run(name,[str(binary)]);print(name,'PASS',flush=True)
import importlib.util,shutil
spec=importlib.util.spec_from_file_location('teaching_review',source/'scripts/tutor-teaching-math-review.py')
review=importlib.util.module_from_spec(spec);spec.loader.exec_module(review)
review.OUT=out/'historical-mutations';(review.OUT/'candidate-host').mkdir(parents=True,exist_ok=True)
shutil.copy2(out/'tutor_teaching_math.exe',review.OUT/'candidate-host/mutations.exe');review.mutations()
for script,folder,extra in [('test-tutor-engine.py','historical',[]),('test-tutor-nonlinear.py','02a-seeded',[]),('test-tutor-nonlinear.py','02a-challenge',['--challenge','tests/fixtures/tutor-nonlinear-challenge.json']),('test-tutor-transcendental.py','02b-seeded',[]),('test-tutor-transcendental.py','02b-challenge',['--challenge','tests/fixtures/tutor-transcendental-challenge.json'])]:
    import sys
    run(folder,[sys.executable,'-X','utf8','scripts/'+script,'--bin',str(out/'tutor_engine_main.exe'),'--out',str(out/folder),*extra]);print(folder,'PASS',flush=True)
if a.trig:
    for folder,extra in [('02c-seeded',[]),('02c-challenge',['--challenge','tests/fixtures/tutor-trig-challenge.json'])]:
        run(folder,[sys.executable,'-X','utf8','scripts/test-tutor-trig.py','--bin',str(out/'tutor_engine_main.exe'),'--out',str(out/folder),*extra]);print(folder,'PASS',flush=True)
(out/'objects.json').write_text(json.dumps([dict(path=str(p),sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in [*objects,*libraries]],indent=2))
