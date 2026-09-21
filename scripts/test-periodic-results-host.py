#!/usr/bin/env python3
"""Compile periodic adaptation/comparison checks against a matching native build."""
from pathlib import Path
import argparse, hashlib, json, os, subprocess

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--source',type=Path,required=True)
p.add_argument('--build',type=Path,required=True)
p.add_argument('--out',type=Path,required=True)
p.add_argument('--compiler',default='g++')
a=p.parse_args();source=a.source.resolve();build=a.build.resolve();out=a.out.resolve()
out.mkdir(parents=True,exist_ok=True)
objects=[p for p in (build/'src').rglob('*.o') if 'math' in p.parts or p.name=='FileSystem.o']
libraries=[*build.rglob('liblvgl.a'),*build.rglob('libgiac.a'),*build.rglob('libtommath.a')]
assert objects and len(libraries)==3,'build emulator_pc from this source first'
flags=['-std=gnu++17','-O1','-ffunction-sections','-fdata-sections','-DNATIVE_SIM','-DLV_CONF_INCLUDE_SIMPLE','-DLV_USE_STDLIB_MALLOC=LV_STDLIB_CLIB','-I.','-Isrc','-I.pio/libdeps/emulator_pc/lvgl']
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH',''))
records=[]
def run(name,command):
    r=subprocess.run(command,cwd=source,env=env,capture_output=True)
    (out/(name+'.log')).write_bytes(r.stdout+r.stderr)
    records.append(dict(name=name,command=command,cwd=str(source),exit=r.returncode))
    (out/'commands.json').write_text(json.dumps(records,indent=2))
    r.check_returncode()
for name in ['periodic_results_checks','periodic_results_allocation','giac_periodic_probe','tutor_engine_main','periodic_result_main']:
    obj=out/(name+'.o');binary=out/(name+'.exe')
    extra=[]
    if name=='giac_periodic_probe':
        extra=['-IC:/SDL2/x86_64-w64-mingw32/include','-Ilib/giac','-Ilib/giac/src','-Ilib/libtommath','-D_USE_MATH_DEFINES',
               *['-D'+d for d in 'HAVE_CONFIG_H IN_GIAC GIAC_KHICAS NO_GUI GIAC_GENERIC EMBEDDED USE_GMP_REPLACEMENTS UMAP DOUBLEVAL'.split()]]
    run(name+'-compile',[a.compiler,*flags,*extra,'-c','tests/host/'+name+'.cpp','-o',str(obj)])
    rsp=out/(name+'.rsp');rsp.write_text('\n'.join('"'+p.as_posix()+'"' for p in [obj,*objects,*libraries]))
    run(name+'-link',[a.compiler,'@'+str(rsp),'-Wl,--gc-sections','-o',str(binary)])
    if not name.endswith('_main'):run(name,[str(binary)]);print(name,'PASS',flush=True)
(out/'objects.json').write_text(json.dumps([dict(path=str(p),sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in [*objects,*libraries]],indent=2))
