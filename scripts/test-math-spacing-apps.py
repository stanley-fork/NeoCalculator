#!/usr/bin/env python3
"""The same two key-built fraction controls in three other shared consumers."""
import argparse,importlib.util,json,os,subprocess,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('gate',ROOT/'scripts/test-calculation-input.py');gate=importlib.util.module_from_spec(spec);spec.loader.exec_module(gate)
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--bin',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
a.out.mkdir(parents=True,exist_ok=True);env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH',''))
fixtures=json.loads((ROOT/'tests/fixtures/math-spacing.json').read_text());records=[]
for f in fixtures:
 if not f['id'].startswith('user-'):continue
 for app,enter,prefix in [('Equations','key ENTER\nkey ENTER\n','x = '),('Calculus','',''),('Grapher','key DOWN\nkey ENTER\n','y = ')]:
  name=f['id']+'-'+app;shot=lambda label:f'screenshot {(a.out/(name+"-"+label+".ppm")).as_posix()}\n'
  script=f'wait 200\nopen_app {app}\nwait 60\n'+enter+gate.keys(prefix+f['keys'])+'wait 5\n'+shot('editable')+f'assert_app {app}\n'
  if app=='Calculus':script+='assert_calculus_layout\nkey ENTER\nwait 30\nassert_calculus_status ok\nassert_calculus_result_exact 0\n'+shot('evaluated')
  if app=='Grapher':
   # GraphModel's public sample is float32. Compare the independently rounded
   # expected number exactly; this is not a geometric tolerance.
   sample=struct.unpack('f',struct.pack('f',f['expected']))[0]
   script+=f'key ENTER\nwait 30\nassert_graph_compile_status 0 ok\nassert_graph_eval_near 0 2 {sample:.17g} 0\n'+shot('committed')
  script+='log SPACING_APPS_DONE\n';path=a.out/(name+'.numos');path.write_text(script)
  cmd=[str(a.bin.resolve()),'--headless','--deterministic','--quiet','--frames','900','--script',str(path)]
  r=subprocess.run(cmd,env=env,capture_output=True,timeout=120);(a.out/(name+'.log')).write_bytes(r.stdout+r.stderr)
  records.append(dict(app=app,id=f['id'],command=cmd,exit=r.returncode));assert r.returncode==0 and b'SPACING_APPS_DONE' in r.stdout,(name,r.returncode)
(a.out/'results.json').write_text(json.dumps(records,indent=2)+'\n');print('PASS',len(records),'shared-app fraction controls')
