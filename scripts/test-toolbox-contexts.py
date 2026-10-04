#!/usr/bin/env python3
"""Check every advertised receiver capability through actual product controls."""
import argparse,json,os,re,subprocess,sys
sys.stdout.reconfigure(errors="replace")
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--calculation-evidence',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
root=Path(__file__).resolve().parents[1];rows=json.loads((root/'src/math/toolbox-catalog.json').read_text(encoding='utf8'))
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-wmingw32/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'])
def keys(s):return ''.join('key '+k+'\n' for k in s.split())
def pick(e):return keys('TOOLBOX')+'sdl_text '+e['en']+'\n'+keys('DOWN ENTER')
def fill(e):
 s=''
 for i,value in enumerate(e['fixture']):
  if i:s+=keys('RIGHT')
  for c in value:
   s+=pick(next(r for r in rows if r['id']==182)) if c=='i' else keys({'-':'NEGATE','+':'ADD','^':'POW','.':'DOT'}.get(c,c))
  if '^' in value:s+=keys('RIGHT')
 return s
records=[]
for app,cap in [('Equations',2),('Calculus',4),('Grapher',8)]:
 for e in rows:
  if e.get('keyboardShortcut') or not e['capabilities']&cap:continue
  name=f'{app}-{e["id"]}-{e["variant"]}'
  script='wait 200\nopen_app '+app+'\nwait 30\n'
  if app=='Equations':script+=keys('ENTER ENTER')
  if app=='Grapher':script+=keys('DOWN ENTER')
  script+=pick(e)+fill(e)
  if app=='Equations':
   original=(a.calculation_evidence/f'catalog-{e["id"]}-{e["variant"]}.log').read_text(encoding='utf8',errors='replace')
   serial=re.search(r'serialized=(.+?) diagnostic=',original).group(1)
   script+='assert_equations input '+serial+'\n'
  elif app=='Calculus':script+=keys('ENTER')+'assert_calculus_status ok\n'
  else:
   script+=keys('ENTER')+'wait 10\nassert_graph_compile_status 0 ok\n'
   if re.fullmatch(r'-?\d+(?:\.\d+)?',e['expected']):script+=f'assert_graph_eval_near 0 2 {e["expected"]} 1e-10\n'
  script+='log TOOLBOX_CONTEXT_DONE\n';path=a.out/(name+'.numos');path.write_text(script,encoding='utf8')
  r=subprocess.run([a.bin,'--headless','--quiet','--deterministic','--frames','750','--script',os.path.relpath(path,root)],cwd=root,env=env,capture_output=True,timeout=90)
  (a.out/(name+'.log')).write_bytes(r.stdout+r.stderr);ok=r.returncode==0 and b'TOOLBOX_CONTEXT_DONE' in r.stdout
  records.append(dict(name=name,passed=ok,exit=r.returncode));print('PASS' if ok else 'FAIL',name,flush=True)
  if not ok:print(r.stdout.decode(errors='replace')[-1400:])
(a.out/'results.json').write_text(json.dumps(records,indent=2))
raise SystemExit(0 if all(r['passed'] for r in records) else 1)
