#!/usr/bin/env python3
"""Real Calculation bracket keys, numeric oracles, history and visible slots.

--baseline records the detecting failure against the preserved pre-bracket binary.
No AST injection or renderer helper is used to obtain expected values.
"""
import argparse
import importlib.util
import json
import os
import subprocess
from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('input_gate', ROOT/'scripts/test-calculation-input.py')
gate = importlib.util.module_from_spec(spec)
spec.loader.exec_module(gate)

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--bin', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--baseline', action='store_true')
    a = p.parse_args()
    os.chdir(ROOT)
    a.out.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, NUMOS_CALC_INPUT_TRACE='1', PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH', ''))
    records = []

    def run(name, body, *, expected_failure=False, brackets=True):
        script = 'wait 200\nopen_app Calculation\nwait 30\n'+body+'\nlog BRACKETS_DONE\n'
        replay = a.out/(name+'.numos')
        replay.write_text(script, encoding='utf-8')
        frames = sum(int(s.split()[1])+1 if s.startswith('wait ') else 1 for s in script.splitlines())+100
        r = subprocess.run([str(a.bin.resolve()), '--headless', '--deterministic', '--quiet', '--frames', str(frames), '--script', str(replay)], env=env, capture_output=True, timeout=60)
        log = r.stdout+r.stderr
        (a.out/(name+'.log')).write_bytes(log)
        passed = r.returncode==4 if expected_failure else r.returncode==0 and b'BRACKETS_DONE' in log
        if brackets and not expected_failure:
            passed = passed and b'Paren []' in log
        records.append(dict(id=name, passed=bool(passed), exit=r.returncode, detectingFailure=expected_failure))
        print('PASS' if passed else 'FAIL', name, flush=True)

    def shot(name):
        return 'wait 3\nscreenshot '+(a.out/(name+'.ppm')).as_posix()+'\n'

    # Both desktop text and real electrical matrix events lost the grouping before.
    for route, events in [
        ('desktop', 'sdl_text [2+3]*4\n'),
        ('production', 'calc_physical 4 0\ncalc_physical 1 4\n'+gate.keys('2 + 3', True)+'calc_physical 4 0\ncalc_physical 0 4\n'+gate.keys('* 4', True)),
    ]:
        body = events+'assert_calc_input dump\n'+shot(route+'-editable')
        body += gate.keys('ENTER')+'assert_calc_status ok\nassert_calc_input near 20 0\n'
        body += 'assert_calc_input serialized (2+3)*4\n'+shot(route+'-evaluated')
        run(route, body, expected_failure=a.baseline)
    if not a.baseline:
        for f in json.loads((ROOT/'tests/fixtures/math-brackets.json').read_text()):
            body = gate.keys(f['keys'])+'assert_calc_input dump\nassert_calc_input structure\n'+shot(f['id']+'-editable')
            body += 'assert_calc_input '+('incomplete' if f.get('complete') is False else 'complete')+'\n'
            if f.get('evaluate') is not False:
                body += gate.keys('ENTER')+'assert_calc_status '+('parse_error' if f.get('complete') is False else 'ok')+'\n'
                if 'expected' in f:
                    body += f"assert_calc_input near {f['expected']:.17g} 1e-10\n"
                body += 'assert_calc_input dump\n'+shot(f['id']+'-evaluated')
            run(f['id'], body, brackets=f['id'] not in ['bracket-delete','bracket-empty-delete','bracket-unmatched-close'])
        run('history', gate.keys('[ 2 + 3 ] ENTER AC 7 ENTER UP UP')+
            'assert_calc_input dump\nassert_calc_input serialized (2+3)\nassert_calc_input near 5 0\n'+shot('history'))
    for path in a.out.glob('*.ppm'):
        im = Image.open(path).convert('RGB')
        assert im.size==(320, 240)
        im.save(path.with_suffix('.png'))
        im.resize((1280,960), Image.Resampling.NEAREST).save(path.with_name(path.stem+'-4x.png'))
    (a.out/'results.json').write_text(json.dumps(records, indent=2))
    return 0 if all(r['passed'] for r in records) else 1

if __name__=='__main__':
    raise SystemExit(main())
