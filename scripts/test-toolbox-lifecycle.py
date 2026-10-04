#!/usr/bin/env python3
"""Measure the fixed-pool modal at comparable retained-history/HOME boundaries."""
from pathlib import Path
import argparse,json,os,re,subprocess
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
root=Path(__file__).resolve().parents[1]
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'])
def keys(s):return ''.join('key '+k+'\n' for k in s.split())
records=[]
def run(name,body):
 script='wait 200\nopen_app Calculation\nwait 30\n'+body+'log TOOLBOX_LIFECYCLE_DONE\n'
 path=a.out/(name+'.numos');path.write_text(script,encoding='utf-8')
 frames=sum(int(l.split()[1])+1 if l.startswith('wait ') else 1 for l in script.splitlines())+100
 r=subprocess.run([a.bin,'--headless','--quiet','--deterministic','--frames',str(frames),'--script',os.path.relpath(path,root)],env=env,cwd=root,capture_output=True,timeout=150)
 (a.out/(name+'.log')).write_bytes(r.stdout+r.stderr);assert r.returncode==0,(name,r.returncode)
 log=r.stdout.decode(errors='replace');samples=[]
 for line in log.splitlines():
  if line.startswith(('[TOOLBOX]','[TOOLBOX-TIME]','[TOOLBOX-MEM]','CALCULUS_PROBE|')):samples.append(dict(line=line,**dict(re.findall(r'(\w+)=(\d+)',line))))
 records.append(dict(name=name,samples=samples))
 return samples
body=keys('AC 2 + 3 ENTER')*60
body+=keys('AC '+ '2 + '*149+'2')
for i in range(50):
 body+=keys('TOOLBOX')+'assert_calc_input toolbox open\nassert_calc_input toolbox dump\n'+'sdl_text root\n'+keys('DOWN FORMAT DOWN DOWN DOWN ENTER BACK BACK BACK')+'assert_calc_input structure\n'
body+=keys('ENTER')+'assert_calc_input near 300 0\n'
run('retained-history-long-input',body)
body=''
for i in range(54):
 body+=keys('AC 7 / 3 ENTER TOOLBOX')+'sdl_text log\n'+keys('DOWN FORMAT ENTER FORMAT DOWN DOWN DOWN ENTER BACK')
 body+='assert_calc_input toolbox dump\n'+(keys('ENTER 2 RIGHT 8 ENTER') if i%2 else keys('BACK BACK'))
 body+=keys('HOME')+'wait 30\nassert_app Menu\ncalculus_probe\nopen_app Calculation\nwait 30\n'
samples=run('home-50',body)
homes=[s for s in samples if s['line'].startswith('CALCULUS_PROBE|app=Menu|')]
assert len(homes)==54
# Retain all four startup samples, then compare fifty complete cycles after
# each mixed path has run twice. Do not hide startup costs or use a tolerance:
# the raw cold series has an 8-byte step on its second insertion boundary.
# TLSF reports block capacity, including alignment slack, not requested bytes.
for i,h in enumerate(homes[4:]):
 reference=homes[4+i%2]
 for field in ('objects','timers','pool_total','pool_free','handles'):assert h[field]==reference[field],(field,h,reference)
assert 0<int(homes[0]['pool_total'])<=65536
(a.out/'results.json').write_text(json.dumps(records,indent=2))
print('PASS 50 retained-history cycles and 50 HOME cycles after 4 recorded warmups; fixed 64 KiB pool')
