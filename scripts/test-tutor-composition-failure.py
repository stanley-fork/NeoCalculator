#!/usr/bin/env python3
"""Persistent page faults at definition, auxiliary, return and final union pages.

Use the existing build-tutor-ui-allocation-probe.py Steps overlay. Its fault
stays armed through failed publication and recovery, then the next page scope
allows a healthy reopen without resetting Giac. No production fault hooks.
"""
from pathlib import Path
import argparse,importlib.util,json,os,subprocess
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('eq',ROOT/'scripts/test-equations-rebuild.py')
eq=importlib.util.module_from_spec(spec);spec.loader.exec_module(eq)
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,NUMOS_EQUATIONS_BOUNDS='1');dll=eq.helper.sdl2_dll_dir(a.bin)
if dll:env['PATH']=dll+os.pathsep+env.get('PATH','')
def run(name,script,extra=None):
    script+='log COMPOSITION_FAULT_DONE\n';file=a.out/(name+'.numos');file.write_text(script)
    frames=sum(int(x.split()[1]) if x.startswith('wait ') else 1 for x in script.splitlines())+200
    r=subprocess.run([a.bin,'--headless','--deterministic','--quiet','--frames',str(frames),'--script',str(file)],cwd=ROOT,env=dict(env,**(extra or {})),capture_output=True,timeout=90)
    (a.out/(name+'.log')).write_bytes(r.stdout+r.stderr)
    assert r.returncode==0 and b'COMPOSITION_FAULT_DONE' in r.stdout,(name,r.returncode,r.stdout[-1200:])
    lines=r.stdout.decode(errors='replace').splitlines()
    views=[json.loads(x.split('[TUTOR_VIEW] ',1)[1]) for x in lines if '[TUTOR_VIEW] ' in x]
    samples=[{k:int(v) for k,v in (f.split('=',1) for f in x.split('|')[1:])} for x in lines if x.startswith('UI_ALLOCATION|')]
    return views,samples
cases={
    'square':'x ^ 4 RIGHT - 5 x ^ 2 RIGHT + 4 = 0',
    'log':'ln x - 1 RIGHT ^ 2 RIGHT - 3 ln x - 1 RIGHT + 2 = 0',
    'sine':'6 sin x RIGHT ^ 2 RIGHT - 5 sin x RIGHT + 1 = 0',
    'tangent':'tan 2 x RIGHT ^ 2 RIGHT = 3'}
records=[]
for name,keys in cases.items():
    start=eq.single(keys)+eq.keys('tools')
    views,_=run(name+'-discover',start+'assert_equations view dump\n'+(eq.keys('RIGHT')+'assert_equations view dump\n')*30)
    unique={v['page']:v for v in views}
    selected={0,1,max(unique)}
    for child in {v.get('child',0) for v in unique.values()}:
        selected.add(next(i for i,v in unique.items() if v.get('child',0)==child))
    selected.update(i for i,v in unique.items() if v['kind']=='final' and not v.get('child',0))
    for page in sorted(selected):
        prefix=start+eq.keys('RIGHT '*page)
        tail='assert_equations view dump\nassert_equations trace complete\nassert_equations trace check\n'+eq.keys('BACK')+'assert_equations state result\n'+eq.keys('tools')+'assert_equations trace formulas\nassert_equations view dump\nassert_equations trace builds 1\n'+eq.keys('HOME')+'wait 30\nassert_equations closed\n'
        expected,healthy=run(f'{name}-{page}-healthy',prefix+tail);attempts=healthy[page]['attempts']
        for persistent in [False,True]:
            for at in sorted({1,max(1,attempts//2),attempts}):
                tag=f'{name}-{page}-{int(persistent)}-{at}'
                actual,samples=run(tag,prefix+tail,dict(NUMOS_UI_FAIL_AT=str(at),NUMOS_UI_FAIL_PAGE=str(page+1),NUMOS_UI_FAIL_PERSISTENT=str(int(persistent))))
                assert samples[page]['failures']>0
                assert actual[-1]['formulas']==expected[-1]['formulas'],'healthy reopening changed math'
                if actual[0]['formulas']:assert actual[0]['formulas']==expected[0]['formulas'],'stale/partial formula published'
                else:assert actual[0]['prose'],'failure must be explicit'
                records.append(dict(case=name,page=page,child=unique[page].get('child',0),at=at,persistent=persistent,sample=samples[page]))
                (a.out/'results.json').write_text(json.dumps(dict(pass_=True,tests=len(records),records=records),indent=2))
        print(name,page,'PASS',flush=True)
