#!/usr/bin/env python3
"""Persistent allocation faults in localized Steps, locale reflow and Results.

Use the private binaries produced by build-tutor-ui-allocation-probe.py.
The fault remains active through recovery inside the requested publication;
the subsequent publication is healthy without resetting Giac.
"""
from pathlib import Path
import argparse, importlib.util, json, os, subprocess
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('eq',ROOT/'scripts/test-equations-rebuild.py')
eq=importlib.util.module_from_spec(spec);spec.loader.exec_module(eq)
p=argparse.ArgumentParser();p.add_argument('--steps-bin',required=True);p.add_argument('--results-bin',required=True);p.add_argument('--out',type=Path,required=True)
a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'])
spanish='wait 200\nopen_app Settings\nwait 30\n'+eq.keys('DOWN DOWN DOWN DOWN ENTER HOME')+'wait 30\n'
def run(name,binary,script,extra=None):
    script+='log I18N_FAULT_DONE\n';path=a.out/(name+'.numos');path.write_text(script,encoding='utf-8')
    result=subprocess.run([binary,'--headless','--deterministic','--quiet','--frames','1800','--script',os.path.relpath(path,ROOT)],cwd=ROOT,env=dict(env,**(extra or {})),capture_output=True,timeout=90)
    (a.out/(name+'.log')).write_bytes(result.stdout+result.stderr)
    assert result.returncode==0 and b'I18N_FAULT_DONE' in result.stdout,(name,result.returncode,result.stdout[-1200:])
    lines=result.stdout.decode().splitlines()
    samples=[{k:int(v) for k,v in (part.split('=',1) for part in line.split('|')[1:])} for line in lines if line.startswith('UI_ALLOCATION|')]
    marker='[PERIODIC_RESULT] ' if binary==a.results_bin else '[TUTOR_VIEW] '
    views=[json.loads(line.split(marker,1)[1]) for line in lines if marker in line]
    return samples,views
records=[];healthy=[]
for name,expression,mode in [('log-page','ln x - 1 RIGHT = 2','page'),('radical-page','sqrt x + 1 RIGHT = x - 1','page'),('locale-reflow','sin 3 x - 1 RIGHT = 1 / 3 RIGHT','locale'),('periodic-results','sin 3 x - 1 RIGHT = 1 / 3 RIGHT','results')]:
    result_mode=mode=='results';binary=a.results_bin if result_mode else a.steps_bin
    start=('' if mode=='locale' else spanish)+eq.single(expression)
    if not result_mode:start+=eq.keys('tools')
    target=2 if mode=='locale' else 1
    if mode=='locale':start+='assert_equations locale es\n'
    dump='assert_equations periodic dump\n' if result_mode else 'assert_equations view dump\n'
    tail=dump+'assert_equations trace builds 1\n'+(eq.keys('tools BACK') if result_mode else eq.keys('BACK tools'))+dump+'assert_equations trace builds 1\nassert_equations trace check\n'+eq.keys('HOME')+'wait 30\nassert_equations closed\n'
    samples,expected=run(name+'-healthy',binary,start+tail);healthy.append(dict(case=name,samples=samples))
    count=samples[target-1]['attempts']
    # Every early allocation plus evenly spread late allocations, including the last.
    points=sorted(set([*range(1,min(count,8)+1),count,*[1+(count-1)*i//15 for i in range(16)]]))
    for persistent in [False,True]:
        for at in points:
            tag=f'{name}-{int(persistent)}-{at}'
            screenshot=''
            if at==1 and persistent:
                screenshot='wait 4\nscreenshot '+os.path.relpath(a.out/(tag+'.ppm'),ROOT).replace('\\','/')+'\n'
            rows,views=run(tag,binary,start+screenshot+tail,dict(NUMOS_UI_FAIL_AT=str(at),NUMOS_UI_FAIL_PAGE=str(target),NUMOS_UI_FAIL_PERSISTENT=str(int(persistent))))
            assert rows[target-1]['failures']>0,(tag,'fault not reached')
            assert views[-1]['formulas']==expected[-1]['formulas'],(tag,'recovery changed math')
            if views[0]['formulas']:assert views[0]['formulas']==expected[0]['formulas'],(tag,'partial formula publication')
            elif not result_mode:
                assert 'preparar' in views[0]['prose'] and 'could not' not in views[0]['prose'],(tag,'untranslated failure',views[0]['prose'])
                assert views[0]['title']=='Pasos',(tag,'previous-language title survived failed reflow')
            records.append(dict(case=name,at=at,persistent=persistent,sample=rows[target-1],recovered=True))
    print(name,len(points)*2,'PASS',flush=True)
for path in a.out.glob('*.ppm'):Image.open(path).save(path.with_suffix('.png'))
(a.out/'results.json').write_text(json.dumps(dict(pass_=True,tests=len(records),healthy=healthy,records=records),indent=2))
