#!/usr/bin/env python3
"""Persistent scoped allocator failures through product events on private build."""
import argparse,json,os,re,subprocess,sys
sys.stdout.reconfigure(encoding='utf8',errors='replace')
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('--query',default='Logarithm');a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
root=Path(__file__).resolve().parents[1]
def keys(s):return ''.join('key '+k+'\n' for k in s.split())
prefix='wait 200\nopen_app Calculation\nwait 30\n'+keys('2 POW 3')
search=keys('TOOLBOX')+'sdl_text '+a.query+'\n'+keys('DOWN')
verify='assert_calc_input serialized ((2)^(3))\nassert_calc_input structure\n'
records=[]
def run(scope,point,call,body,stale=False):
 name=f'{scope}-{point}-{call}';path=a.out/(name+'.numos')
 dismiss='' if scope=='open' and point==1 else keys('BACK' if scope in ('open','save','preview') else 'BACK BACK')
 script=prefix+body+verify+'wait 3\nscreenshot '+(a.out/(name+'.ppm')).as_posix()+'\n'+dismiss+'assert_calc_input toolbox closed\n'+keys('ENTER')+'assert_calc_input near 8 0\n'+keys('HOME')+'wait 30\nopen_app Calculation\nwait 30\n'+keys('2 ADD 3 ENTER')+'assert_calc_input near 5 0\nlog TOOLBOX_FAULT_DONE\n'
 path.write_text(script,encoding='utf8')
 env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'],NUMOS_TOOLBOX_SCOPE=scope,NUMOS_TOOLBOX_FAIL_AT=str(point),NUMOS_TOOLBOX_FAIL_CALL=str(call))
 if stale:env['NUMOS_TOOLBOX_STALE']='1'
 fs=a.out/(name+'-data')
 r=subprocess.run([a.bin,'--headless','--deterministic','--quiet','--frames','900','--script',os.path.relpath(path,root),'--fs-sandbox-dir',os.path.relpath(fs,root)],cwd=root,env=env,capture_output=True,timeout=90)
 (a.out/(name+'.log')).write_bytes(r.stdout+r.stderr)
 text=r.stdout.decode(errors='replace');lines=[dict(re.findall(r'(\w+)=([\w-]+)',s)) for s in text.splitlines() if s.startswith('TOOLBOX_ALLOC')]
 ok=r.returncode==0 and 'TOOLBOX_FAULT_DONE' in text and bool(lines) and (stale or any(int(s['failures'])>0 for s in lines))
 records.append(dict(name=name,passed=ok,scopes=lines,exit=r.returncode));print('PASS' if ok else 'FAIL',name,flush=True)
 if not ok:print(text[-2500:],r.stderr.decode(errors='replace')[-1000:])
for point in (1,2,3):run('open',point,1,keys('TOOLBOX'))
for point in (1,2,4):run('preview',point,1,keys('TOOLBOX'))
for point in (1,2,5):run('search',point,1,search)
for point in (1,2,5,8):run('insert',point,1,search+keys('ENTER'))
for point in (1,2,3):run('save',point,1,search+keys('FORMAT ENTER BACK BACK'))
run('insert',0,1,search+keys('ENTER')+'assert_calc_input toolbox open\n',True)
(a.out/'results.json').write_text(json.dumps(records,indent=2))
raise SystemExit(0 if all(r['passed'] for r in records) else 1)
