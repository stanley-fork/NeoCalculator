#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
# Persistent/one-shot page allocation failures; production transactional view.
from pathlib import Path
import argparse,importlib.util,os,re,subprocess,json,time
root=Path(__file__).resolve().parents[1];os.chdir(root);out=root/'out/tutor-engine-02b/ui-faults';out.mkdir(parents=True,exist_ok=True);ap=argparse.ArgumentParser();ap.add_argument('--bin',required=True);ap.add_argument('--limit',type=int,default=32);ap.add_argument('--quick',action='store_true');ap.add_argument('--tag',default='candidate-faults');ap.add_argument('--case',choices=['exponential','logarithm','logbase','exp-isolate','log-isolate','trig-sine','trig-negative','trig-tangent','trig-degree','trig-impossible'],required=True);args=ap.parse_args();target=out/args.tag;target.mkdir(exist_ok=True)
spec=importlib.util.spec_from_file_location('eq',root/'scripts/test-equations-rebuild.py');eq=importlib.util.module_from_spec(spec);spec.loader.exec_module(eq)
env=dict(os.environ,NUMOS_EQUATIONS_BOUNDS='1');dll=eq.helper.sdl2_dll_dir(args.bin)
if dll:env['PATH']=dll+os.pathsep+env.get('PATH','')
cases={'exponential':eq.single('2 ^ x RIGHT = 8'),'logarithm':eq.single('ln x - 1 RIGHT = 2'),'logbase':eq.single('logbase 2 RIGHT x RIGHT = 3')}
cases.update({'exp-isolate':eq.single('2 SHIFT ln x RIGHT + 1 = 7'),'log-isolate':eq.single('3 ln x RIGHT - 6 = 0')})
cases.update({'trig-sine':eq.single('sin 3 x - 1 RIGHT = 1 / 3 RIGHT'),'trig-negative':eq.single('cos 0 - 2 x RIGHT = 1'),'trig-tangent':eq.single('tan 3 x RIGHT = 1'),'trig-degree':'set_angle_mode deg\n'+eq.single('sin 2 x RIGHT = 1 / 3 RIGHT')})
cases['trig-impossible']=eq.single('sin x RIGHT = 2')
cases={args.case:cases[args.case]}

def run(name,script,extra=None):
 p=target/(name+'.numos');p.write_text(script+'log RESOURCE_FAULT_DONE\n');e=env.copy();e.update(extra or {})
 r=subprocess.run([args.bin,'--headless','--deterministic','--quiet','--frames','1200','--script',str(p.relative_to(root))],capture_output=True,env=e,timeout=20)
 log=r.stdout.decode(errors='replace');(target/(name+'.log')).write_bytes(r.stdout+r.stderr)
 if r.returncode or 'RESOURCE_FAULT_DONE' not in log:raise AssertionError((name,r.returncode,log[-1600:]))
 rows=[{k:int(v) for k,v in [f.split('=',1) for f in x.split('|')[1:]]} for x in log.splitlines() if x.startswith('UI_ALLOCATION|')]
 views=[json.loads(x.split('[TUTOR_VIEW] ',1)[1]) for x in log.splitlines() if '[TUTOR_VIEW] ' in x]
 return rows,views
results=[];started=time.monotonic()
for name,start in cases.items():
 opening=start+eq.keys('tools');rows,views=run(name+'-pages',opening+'assert_equations view dump\n'+(eq.keys('RIGHT')+'assert_equations view dump\n')*12)
 unique={v['page']:v for v in views}
 selected=[p for p,v in unique.items() if v['kind'] in ('coefficients','discriminant','quadratic_formula')]
 selected=list(unique)
 if not selected:selected=[next((p for p,v in unique.items() if any(f['kind'] in ('operand','row_operation','conditions') for f in v['formulas'])),0)]
 for page in selected:
  prefix=opening+eq.keys(' '.join(['RIGHT']*page))
  tail='assert_equations view dump\nassert_equations trace complete\nassert_equations trace check\n'+eq.keys('BACK')+'assert_equations state result\nassert_equations trace builds 1\n'+eq.keys('tools')+'assert_equations trace formulas\nassert_equations view dump\nassert_equations trace complete\nassert_equations trace check\nassert_equations trace builds 1\n'
  healthy,expected=run(f'{name}-{page}-healthy',prefix+tail)
  attempt=healthy[page]['attempts']
  limit=min(args.limit,attempt)
  points=sorted(set(list(range(1,min(9,attempt+1)))+list(range(max(1,attempt-7),attempt+1))+[1+(attempt-1)*i//max(1,limit-1) for i in range(limit)]))
  if args.quick:points=sorted(set([1,max(1,attempt//2),attempt]))
  for persistent in [False,True]:
   for at in points:
    tag=f'{name}-{page}-'+('persistent' if persistent else 'once')+f'-{at:04}'
    actual,observed=run(tag,prefix+tail,{'NUMOS_UI_FAIL_AT':str(at),'NUMOS_UI_FAIL_PAGE':str(page+1),'NUMOS_UI_FAIL_PERSISTENT':'1' if persistent else '0'})
    fault=actual[page];assert fault['failures']>=1,(tag,'fault not reached',fault)
    first,last=observed
    assert last['formulas']==expected[-1]['formulas'],(tag,'reopen differs')
    if first['formulas']:assert first['formulas']==expected[0]['formulas'],(tag,'incomplete math published')
    else:assert 'could not be prepared' in first['prose'].lower(),(tag,'no unavailable explanation',first['prose'])
    results.append({'case':name,'page':page,'kind':unique[page]['kind'],'at':at,'persistent':persistent,'failures':fault['failures'],'recovered':True,'fallback':not first['formulas']})
    (target/'results.json').write_text(json.dumps({'pass':True,'tested':len(results),'elapsed_seconds':time.monotonic()-started,'results':results},indent=2))
  print(name,page,unique[page]['kind'],attempt,len(points)*2,flush=True)
print('FAULT PASS',len(results),time.monotonic()-started,flush=True)
