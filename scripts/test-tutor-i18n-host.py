#!/usr/bin/env python3
"""Build and run focused bilingual host checks against a matching native snapshot."""
from pathlib import Path
import argparse,json,subprocess,os,hashlib
p=argparse.ArgumentParser();p.add_argument('--source',type=Path,required=True);p.add_argument('--build',type=Path,required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('--compiler',default='g++');a=p.parse_args()
source=a.source.resolve();build=a.build.resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
objects=[p for p in (build/'src').rglob('*.o') if 'math' in p.parts or 'fonts' in p.parts or p.name=='FileSystem.o'];libs=[*build.rglob('liblvgl.a'),*build.rglob('libgiac.a'),*build.rglob('libtommath.a')];assert len(libs)==3
flags=['-std=gnu++17','-O1','-ffunction-sections','-fdata-sections','-DNATIVE_SIM','-DLV_CONF_INCLUDE_SIMPLE','-DLV_USE_STDLIB_MALLOC=LV_STDLIB_CLIB','-I.','-Isrc','-I.pio/libdeps/emulator_pc/lvgl']
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH']);records=[]
def run(name,cmd):
 r=subprocess.run(cmd,cwd=source,env=env,capture_output=True);(out/(name+'.log')).write_bytes(r.stdout+r.stderr);records.append(dict(name=name,command=cmd,exit=r.returncode));(out/'commands.json').write_text(json.dumps(records,indent=2));r.check_returncode()
for name in ['tutor_i18n_main','tutor_i18n_checks']:
 obj=out/(name+'.o');binary=out/(name+'.exe');run(name+'-compile',[a.compiler,*flags,'-c','tests/host/'+name+'.cpp','-o',str(obj)])
 rsp=out/(name+'.rsp');rsp.write_text('\n'.join('"'+p.as_posix()+'"' for p in [obj,*objects,*libs]));run(name+'-link',[a.compiler,'@'+str(rsp),'-Wl,--gc-sections','-o',str(binary)])
 if name.endswith('checks'):run(name,[str(binary)]);print((out/(name+'.log')).read_text())
(out/'objects.json').write_text(json.dumps([dict(path=str(p),sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in [*objects,*libs]],indent=2))
