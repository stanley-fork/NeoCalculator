#!/usr/bin/env python3
"""50 mixed native cycles using the unchanged firmware-sized LVGL pool.

CALCULUS_PROBE uses lv_mem_monitor (payload blocks, not total C++ heap).
This script intentionally does not reset Giac or simulate physical flash.
"""
import argparse
import importlib.util
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('eq', ROOT/'scripts/test-equations-rebuild.py')
eq = importlib.util.module_from_spec(spec)
spec.loader.exec_module(eq)


def main():
    p=argparse.ArgumentParser()
    p.add_argument('--bin', required=True)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--transcendental', action='store_true')
    p.add_argument('--trig', action='store_true')
    p.add_argument('--i18n', action='store_true')
    p.add_argument('--composition', action='store_true')
    args=p.parse_args()
    os.chdir(ROOT)
    args.out.mkdir(parents=True, exist_ok=True)
    starts=[
        eq.single('SHIFT sqrt 2 x - 3 RIGHT = 5').replace('key SHIFT\nkey sqrt\n','equations_physical 4 0\nequations_physical 2 2\n'),
        eq.single('sqrt x + 1 RIGHT = x - 1'),
        eq.single('2 sqrt x + 1 RIGHT + 1 = 7'),
        eq.system(eq.SYSTEMS['system-2x2'][0]),
        eq.system(eq.SYSTEMS['system-3x3'][0]),
    ]
    if args.transcendental:
        starts=[starts[0],starts[1],eq.single('2 x ^ 2 RIGHT + 3 x - 4 = 0'),
                eq.single('2 ^ x + 0 RIGHT = 8'),eq.single('ln x - 1 RIGHT = 2')]
    if args.trig:
        starts=[eq.single('2 x ^ 2 RIGHT + 3 x - 4 = 0'),starts[1],eq.single('ln x - 1 RIGHT = 2'),
                eq.single('sin x RIGHT = 1 / 2 RIGHT'),eq.single('cos x RIGHT = 1 / 2 RIGHT'),
                eq.single('tan 3 x RIGHT = 1'),eq.single('sin 2 x RIGHT = 1 / 2 RIGHT')]
        starts=[('set_angle_mode deg\n' if i==6 else 'set_angle_mode rad\n')+s for i,s in enumerate(starts)]
    if args.composition:
        starts=[eq.single(s) for s in [
            '2 x ^ 2 RIGHT + 3 x - 4 = 0',
            'sqrt x + 1 RIGHT = x - 1',
            'x ^ 4 RIGHT - 5 x ^ 2 RIGHT + 4 = 0',
            'SHIFT ln 2 x RIGHT - 3 SHIFT ln x RIGHT + 2 = 0',
            'ln x - 1 RIGHT ^ 2 RIGHT - 3 ln x - 1 RIGHT + 2 = 0',
            '6 sin x RIGHT ^ 2 RIGHT - 5 sin x RIGHT + 1 = 0',
            'tan 2 x RIGHT ^ 2 RIGHT = 3',
            '6 sin 3 x - 1 RIGHT ^ 2 RIGHT - 5 sin 3 x - 1 RIGHT + 1 = 0']]
        starts=[('set_angle_mode deg\n' if i==7 else 'set_angle_mode rad\n')+s for i,s in enumerate(starts)]
        starts=[s.replace('key SHIFT\nkey ln\n','equations_physical 4 0\nequations_physical 3 3\n') for s in starts]
    env=dict(os.environ,NUMOS_EQUATIONS_BOUNDS='1')
    dll=eq.helper.sdl2_dll_dir(args.bin)
    if dll:env['PATH']=dll+os.pathsep+env.get('PATH','')
    def run(name,script):
        script+='log NONLINEAR_LIFECYCLE_DONE\n'
        file=args.out/(name+'.numos');file.write_text(script,encoding='utf-8')
        frames=sum(int(x.split()[1]) if x.startswith('wait ') else 1 for x in script.splitlines())+200
        cmd=[args.bin,'--headless','--deterministic','--quiet','--frames',str(frames),'--script',str(file)]
        r=subprocess.run(cmd,env=env,capture_output=True,timeout=240)
        (args.out/(name+'.log')).write_bytes(r.stdout+r.stderr)
        assert r.returncode==0 and b'NONLINEAR_LIFECYCLE_DONE' in r.stdout,(name,r.returncode,r.stdout[-2000:])
        return r.stdout.decode(errors='replace')
    counts=[]
    for i,start in enumerate(starts):
        text=run('discover-'+str(i),start+eq.keys('tools')+'assert_equations trace complete\nassert_equations view dump\n')
        view=json.loads(next(x.split('[TUTOR_VIEW] ',1)[1] for x in text.splitlines() if '[TUTOR_VIEW] ' in x))
        counts.append(view['count'])
    script='wait 200\nset_equations_complex_policy real\n'
    for cycle in range(50):
        i=cycle%len(starts)
        script+=starts[i].replace(eq.OPEN,'open_app Equations\nwait 30\n')
        script+='assert_equations trace complete\ncalculus_probe\n'+eq.keys('tools')+'assert_equations trace builds 1\ncalculus_probe\n'
        if args.i18n:script+='assert_equations locale es\nassert_equations trace builds 1\n'
        for page in range(counts[i]):
            script+=f'assert_equations view page {page}\nassert_equations trace formulas\n'+eq.keys('DOWN DOWN VAR RIGHT')
        if args.i18n:script+='assert_equations locale en\nassert_equations locale es\nassert_equations trace builds 1\n'
        script+='assert_equations trace check\ncalculus_probe\n'+eq.keys('BACK tools')+'assert_equations trace builds 1\nassert_equations trace formulas\ncalculus_probe\n'
        # Cancel a changed draft: the committed trace and epoch remain current.
        script+=eq.keys('BACK BACK UP UP UP UP ENTER')+'assert_equations state editing\n'
        script+=eq.keys('AC x = 7 BACK')+'assert_equations state list\nassert_equations trace builds 1\nassert_equations epochs current\ncalculus_probe\n'
        if args.trig:
            # A mode change makes a cached proof unavailable; navigation itself
            # must not build a new proof. The next Solve owns regeneration.
            script+='set_angle_mode '+('rad' if i==6 else 'deg')+'\nassert_equations trace stale\n'
        script+=eq.keys('HOME')+'wait 30\nassert_equations closed\nassert_app Menu\ncalculus_probe\n'
    text=run('mixed50',script)
    rows=[dict(p.split('=',1) for p in line.split('|')[1:]) for line in text.splitlines() if line.startswith('CALCULUS_PROBE|')]
    assert len(rows)==300
    home=rows[5::6]
    assert all(0<int(r['pool_free'])<65536 and 0<int(r['pool_total'])<=65536 for r in rows),'fixed pool must be enabled'
    assert all(r['handles']=='0' for r in rows)
    assert all(len({r[k] for r in home[10:]})==1 for k in ('objects','timers')),'post-HOME object drift'
    if args.trig:
        # lv_mem_monitor sums TLSF payload blocks, excluding block headers.
        # The mixed RAD/DEG sequence can coalesce one additional 8-byte header.
        # Require EXACTLY unchanged live payload and no loss of free payload;
        # do not mistake a larger total/free pair for retained allocation.
        assert len({int(r['pool_total'])-int(r['pool_free']) for r in home})==1,'post-HOME live payload drift'
        free=[int(r['pool_free']) for r in home]
        assert min(free)==free[0] and max(free)-min(free)<=8,'post-HOME free payload loss'
    else:
        assert all(len({r[k] for r in home[10:]})==1 for k in ('pool_total','pool_free')),'post-HOME drift'
    phases={}
    for fixture in range(len(starts)):
        for phase in range(5):
            group=[rows[c*6+phase] for c in range(10,50) if c%len(starts)==fixture]
            assert all(len({r[k] for r in group})==1 for k in ('objects','timers'))
            # TLSF alignment/size-class layout can cycle; require a bounded
            # repeated pattern, not a monotonically losing allocator history.
            free=[int(r['pool_free']) for r in group]
            assert max(free)-min(free)<=256,(fixture,phase,free)
            phases[f'{fixture}-{phase}']={'samples':group,'free_range':[min(free),max(free)]}
    result=dict(pass_=True,cycles=50,observations=rows,phases=phases,post_home=home[-1],minimum_sampled_pool_free=min(int(r['pool_free']) for r in rows),
                scope='lv_mem_monitor payload blocks; sampled, not total dynamic-heap peak; no Giac reset')
    (args.out/'result.json').write_text(json.dumps(result,indent=2))
    print('PASS mixed50; sampled minimum pool free',result['minimum_sampled_pool_free'],'post HOME',home[-1],flush=True)


if __name__=='__main__':main()
