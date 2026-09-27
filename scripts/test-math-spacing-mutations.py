#!/usr/bin/env python3
"""Four private negative controls; never mutate working-tree/product sources."""
import argparse, importlib.util, json, os, re, shutil, subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--source',type=Path,required=True);p.add_argument('--baseline-source',type=Path,required=True)
p.add_argument('--probe',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
spec=importlib.util.spec_from_file_location('oracle',ROOT/'scripts/compare-math-spacing.py')
oracle=importlib.util.module_from_spec(spec);spec.loader.exec_module(oracle)
commands=json.loads((a.probe/'commands.json').read_text());records=[]
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH',''))
def command(name,cmd):
 r=subprocess.run(cmd,cwd=a.source,env=env,capture_output=True)
 (a.out/(name+'.log')).write_bytes(r.stdout+r.stderr)
 records.append(dict(name=name,command=cmd,exit=r.returncode));return r
def compile_from(name,old,unit,obj,includes):
 cmd=next(r['command'] for r in commands if r['name']==old).copy()
 cmd[cmd.index('-c')+1]=str(unit.resolve());cmd[cmd.index('-o')+1]=str(obj.resolve())
 cmd[1:1]=includes
 command(name,cmd).check_returncode()
for name in ['old-table','static-class','text-in-scripts']:
 folder=a.out/name;private=folder/'src';shutil.copytree(a.source/'src',private,dirs_exist_ok=True)
 table=private/'math/MathTypography.h';row=private/'math/MathRowLayout.h'
 if name=='old-table':
  old=(a.baseline_source/'src/math/MathTypography.h').read_text(encoding='utf-8')
  old=re.search(r'constexpr int8_t kSpacingTable\[8\]\[8\] = \{.*?\n\};',old,re.S).group()
  text=table.read_text(encoding='utf-8');text=re.sub(r'constexpr int8_t kSpacingTable\[8\]\[8\] = \{.*?\n\};',lambda _:old,text,flags=re.S);table.write_text(text,encoding='utf-8')
 elif name=='static-class':
  text=row.read_text();text=text.replace('node._layout.effectiveLeft = node._layout.effectiveRight = MathClass::ORD;','(void)node; // MUTATION: leave binary class unchanged')
  row.write_text(text)
 else:
  text=row.read_text().replace('cl.effectiveLeft, fm.style, fm.emSize','cl.effectiveLeft, MathStyle::TEXT, fm.emSize')
  row.write_text(text)
 includes=['-I'+str(private.resolve()),'-I'+str((private/'ui').resolve())]
 ast=folder/'ast.o';checks=folder/'checks.o'
 compile_from(name+'-ast','allocator-compile',private/'math/MathAST.cpp',ast,includes)
 compile_from(name+'-checks','checks-compile',ROOT/'tests/host/math_spacing_checks.cpp',checks,includes)
 rsp=(a.probe/'checks.rsp').read_text().replace((a.probe/'MathAST-allocator.o').as_posix(),ast.resolve().as_posix()).replace((a.probe/'checks.o').as_posix(),checks.resolve().as_posix())
 link=folder/'link.rsp';link.write_text(rsp);exe=folder/'checks.exe'
 command(name+'-link',['g++','@'+str(link.resolve()),'-Wl,--gc-sections','-o',str(exe.resolve())]).check_returncode()
 result=command(name,[str(exe.resolve())]);assert result.returncode==1,(name,'mutation survived')
 print('KILLED',name,flush=True)
name='cursor-static';folder=a.out/name;folder.mkdir(exist_ok=True)
renderer=(a.probe/'MathRenderer-probe.cpp').read_text(encoding='utf-8')
old=(a.baseline_source/'src/ui/MathRenderer.cpp').read_text(encoding='utf-8')
pattern=r'int16_t MathCanvas::childXOffset\(.*?\n\}'
body=re.search(pattern,old,re.S).group()
renderer=re.sub(pattern,lambda _:body,renderer,count=1,flags=re.S)
unit=folder/'renderer.cpp';unit.write_text(renderer,encoding='utf-8');obj=folder/'renderer.o'
compile_from(name+'-compile','renderer-compile',unit,obj,[])
link=folder/'link.rsp';link.write_text((a.probe/'probe.rsp').read_text().replace((a.probe/'renderer.o').as_posix(),obj.resolve().as_posix()))
exe=folder/'probe.exe';command(name+'-link',['g++','@'+str(link.resolve()),'-Wl,--gc-sections','-o',str(exe.resolve())]).check_returncode()
command(name,[str(exe.resolve()),'neg 5',str((folder/'frame.ppm').resolve())]).check_returncode()
errors=oracle.validate(oracle.events(a.out/(name+'.log')))
assert any('cursor differs' in e for e in errors),errors
records.append(dict(name='cursor-static-detector',failures=errors));print('KILLED cursor-static',flush=True)
(a.out/'results.json').write_text(json.dumps(records,indent=2)+'\n')
