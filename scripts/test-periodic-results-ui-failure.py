#!/usr/bin/env python3
"""Persistent failure through periodic Results preparation and fallback teardown."""
from pathlib import Path
import argparse,importlib.util,json,os,subprocess
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('eq',ROOT/'scripts/test-equations-rebuild.py');eq=importlib.util.module_from_spec(spec);spec.loader.exec_module(eq)
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,NUMOS_EQUATIONS_BOUNDS='1');dll=eq.helper.sdl2_dll_dir(a.bin)
if dll:env['PATH']=dll+os.pathsep+env.get('PATH','')
records=[]
def run(name,script,extra=None):
    script+='log PERIODIC_FAILURE_DONE\n';file=a.out/(name+'.numos');file.write_text(script)
    r=subprocess.run([a.bin,'--headless','--deterministic','--quiet','--frames','1600','--script',os.path.relpath(file,ROOT)],cwd=ROOT,env=dict(env,**(extra or {})),capture_output=True,timeout=60)
    (a.out/(name+'.log')).write_bytes(r.stdout+r.stderr)
    assert r.returncode==0 and b'PERIODIC_FAILURE_DONE' in r.stdout,(name,r.returncode,r.stdout[-1500:])
    lines=r.stdout.decode(errors='replace').splitlines()
    views=[json.loads(x.split('[PERIODIC_RESULT] ',1)[1]) for x in lines if '[PERIODIC_RESULT] ' in x]
    samples=[{k:int(v) for k,v in (f.split('=',1) for f in x.split('|')[1:])} for x in lines if x.startswith('UI_ALLOCATION|')]
    return views,samples
for name,expression in [('sine','sin 3 x - 1 RIGHT = 1 / 3 RIGHT'),('tangent','tan 3 x RIGHT = 1'),
                        ('four-families','6 sin x RIGHT ^ 2 RIGHT - 5 sin x RIGHT + 1 = 0')]:
    start=eq.single(expression)
    tail='assert_equations periodic equivalent\nassert_equations periodic dump\n'+eq.keys('tools BACK')+'assert_equations periodic dump\nassert_equations trace builds 1\n'+eq.keys('HOME')+'wait 30\nassert_equations closed\n'
    expected,samples=run(name+'-healthy',start+tail);count=samples[0]['attempts']
    points=sorted(set([1,count,*[1+(count-1)*i//15 for i in range(16)]]))
    for persistent in [False,True]:
        for at in points:
            tag=f'{name}-{int(persistent)}-{at}'
            views,samples=run(tag,start+tail,dict(NUMOS_UI_FAIL_AT=str(at),NUMOS_UI_FAIL_PAGE='1',NUMOS_UI_FAIL_PERSISTENT=str(int(persistent))))
            assert samples[0]['failures']>0
            assert not views[0]['formulas'] or views[0]['formulas']==expected[0]['formulas'],'partial answer'
            assert views[-1]['families']==expected[-1]['families'] and views[-1]['formulas']==expected[-1]['formulas'],'recovery changed answer'
            records.append(dict(case=name,at=at,persistent=persistent,sample=samples[0]))
            (a.out/'results.json').write_text(json.dumps(dict(pass_=True,tests=len(records),records=records),indent=2))
    print(name,len(points)*2,'PASS',flush=True)
