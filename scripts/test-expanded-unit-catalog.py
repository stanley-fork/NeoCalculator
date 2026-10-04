#!/usr/bin/env python3
"""Expanded catalogue through the real native modal/editor keyboard harness.

Exact multiplicative units compute; contextual, measured and affine entries stay deferred.
Screenshots are actual 320x240 native renders, suitable for the delivery gallery.
"""
import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import time
from PIL import Image

sys.stdout.reconfigure(encoding='utf8', errors='replace')
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--bin', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--only')
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
a.out.mkdir(parents=True, exist_ok=True)
env = dict(os.environ, PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;' + os.environ.get('PATH', ''))
results = {}

def keys(*codes): return ''.join('key ' + str(c) + '\n' for c in codes)
def search(query): return keys('TOOLBOX') + 'sdl_text ' + query + '\n' + keys('DOWN')
def selected(item, prefix=0): return f'assert_calc_input toolbox selected {32768 | item} {prefix}\n'
def selected_reference(rid, prefix=0): return f'assert_calc_input toolbox selected {16384 | rid} {prefix}\n'
def unit(uid, prefix=0, count=1): return f'assert_calc_input unit {uid} {prefix} {count}\n'
def pick(query, item, prefix=0): return search(query) + selected(item, prefix) + keys('ENTER') + 'assert_calc_input toolbox closed\n'
def shot(name): return 'wait 2\nscreenshot ' + Path(os.path.relpath(a.out / (name + '.ppm'), root)).as_posix() + '\n'
def ordinary(): return keys('AC','2','ADD','2','ENTER') + 'assert_calc_exact 4\n'
def guarded(pending=False): return keys('ENTER') + ('assert_calc_status units_unavailable\n' if pending else 'assert_calc_status ok\n')
def locale(index):
    return keys('HOME') + 'wait 20\nopen_app Settings\nwait 20\n' + keys(*(['DOWN']*4), *(['RIGHT']*index), 'HOME') + 'wait 20\nopen_app Calculation\nwait 20\n'

def run(name, body, fs=None, verify=None):
    if a.only and a.only not in name: return
    if fs is None: fs = Path(tempfile.mkdtemp(prefix=name + '-', dir=a.out))
    script = 'wait 200\nopen_app Calculation\nwait 30\n' + body + 'log EXPANDED_UNIT_DONE\n'
    replay = a.out / (name + '.numos')
    replay.write_text(script, encoding='utf8')
    frames = sum(int(line.split()[1])+1 if line.startswith('wait ') else 1 for line in script.splitlines()) + 100
    start = time.perf_counter()
    r = subprocess.run([str(a.bin.resolve()), '--headless', '--quiet', '--deterministic', '--frames', str(frames),
        '--script', os.path.relpath(replay, root), '--fs-sandbox-dir', os.path.relpath(fs, root)], cwd=root, env=env,
        capture_output=True, timeout=180)
    log = (r.stdout+r.stderr).decode('utf8', errors='replace')
    (a.out / (name+'.log')).write_text(log, encoding='utf8')
    passed = r.returncode == 0 and 'EXPANDED_UNIT_DONE' in log and (verify(log) if verify else True)
    for line in script.splitlines():
        if not line.startswith('screenshot '): continue
        path = root / line[len('screenshot '):]
        if not path.is_file(): continue
        im = Image.open(path)
        assert im.size == (320,240)
        im.save(path.with_suffix('.png'))
    results[name] = dict(passed=bool(passed), exit=r.returncode, seconds=time.perf_counter()-start)
    (a.out/'results.json').write_text(json.dumps(results, indent=2)+'\n', encoding='utf8')
    print('PASS' if passed else 'FAIL', name, flush=True)
    if not passed: print(log[-2800:])

# Exact capital-C alias is a prefixed variant, never a second nutritional identity.
run('calorie-capital', search('Cal') + selected(1156,10) + shot('calorie-capital') + keys('RIGHT') +
    selected(1156,10) + 'assert_calc_input toolbox dump\n' + keys('ENTER') + unit(1156,10) +
    'assert_calc_input no_result\n' + guarded() + ordinary())
run('calorie-conventions', pick('cal_th',1156) + unit(1156) + keys('AC') + pick('cal_IT',1157) + unit(1157) +
    guarded() + ordinary())
run('watt-hour-prefix', keys('2') + search('Wh') + selected(114) + keys('RIGHT') + selected(114) +
    'assert_calc_input toolbox dump\n' + shot('watt-hour-prefix-centred') + keys('UP','UP','UP') +
    selected(114,10) + shot('watt-hour-prefix-kwh') + keys('ENTER') + unit(14,10) + unit(48) +
    'assert_calc_input no_result\nassert_calc_input structure\n' + guarded() + ordinary(),
    verify=lambda log:'selection=12 top=10 count=25' in log)
run('watt-hour-cancel', keys('2','ADD','3') + search('Wh') + selected(114) + keys('RIGHT','UP','UP','UP','BACK') +
    selected(114) + keys('BACK') + 'assert_calc_input serialized 2+3\n')
run('gallons', search('galUS') + selected(1041) + shot('gallons-us') + keys('ENTER') + unit(1041) +
    keys('AC') + search('galImp') + selected(1042) + shot('gallons-imperial') + keys('ENTER') + unit(1042) +
    keys('AC') + pick('galDry',1051) + unit(1051) + guarded() + ordinary())
run('fuel-coefficient', keys('5') + search('L/100km') + selected(1298) + keys('ENTER') + unit(46) + unit(1,10) +
    'assert_calc_input structure\nassert_calc_input dump\n' + shot('fuel-coefficient') + guarded() +
    shot('fuel-coefficient-unavailable') + keys('UP') +
    unit(46) + unit(1,10) + 'assert_calc_input dump\n' + ordinary(),
    verify=lambda log: 'Fraction' in log and 'Number "100"' in log)
run('count-components', pick('Sa/s',1292) + unit(1292) + unit(3) + 'assert_calc_input structure\n' +
    keys('AC') + pick('FLOPS',1295) + unit(1295) + unit(3) + shot('count-components-flops') +
    keys('AC') + pick('bpm',1294) + unit(1294) + unit(47) + guarded() + ordinary())
run('historical', search('Vara de Burgos') + selected(1302) + shot('historical-vara') + keys('ENTER') +
    unit(1302) + guarded(True) + ordinary())
run('typed-physical-reference', search('Speed of light in vacuum') + selected_reference(1) +
    shot('typed-physical-reference') + keys('ENTER') + 'assert_calc_input dump\nassert_calc_input structure\n' +
    guarded(True) + ordinary(), verify=lambda log:'QuantityReference[1:0]' in log)

for index,tag in enumerate(['en-US','en-GB','es-ES','es-419']):
    q = 'Caloría termoquímica' if index >= 2 else 'Thermochemical calorie'
    # Search by the enduring calorie symbol; screenshot verifies regional modal labels.
    run(tag+'-catalogue', locale(index) + search('cal_th') + selected(1156) + shot(tag+'-catalogue') +
        keys('ENTER') + unit(1156) + keys('AC') + pick('milímetro',1,15) + unit(1,15) +
        guarded() + shot(tag+'-catalogue-unavailable') + ordinary())

fs = Path(tempfile.mkdtemp(prefix='favorites-persistent-', dir=a.out))
run('favorites-write', search('Cal') + selected(1156,10) + keys('FORMAT','ENTER','BACK','BACK') +
    search('Wh') + selected(114) + keys('RIGHT','UP','UP','UP') + selected(114,10) +
    keys('FORMAT','ENTER','BACK','BACK','BACK'), fs)
run('favorites-read', keys('TOOLBOX','UP','RIGHT','ENTER') + selected(1156,10) + shot('favorites-read-cal') +
    keys('RIGHT') + selected(1156,10) + keys('BACK','DOWN') + selected(114,10) + shot('favorites-read-kwh') +
    keys('RIGHT') + selected(114,10) + keys('ENTER') + unit(14,10) + unit(48) +
    'assert_calc_input no_result\n' + guarded() + ordinary(), fs)

for name,q,item,prefix in [('thermochemical','cal_th',1156,0),('it-calorie','cal_IT',1157,0),('dalton','Dalton',1103,0),
    ('hongkong','Hong Kong catty',1307,0),('rai','Thai rai',1310,0),('imperial-pint','ptImp',1046,0),
    ('us-pint','ptUS',1045,0),('metric-hp','CV',1169,0),('darcy','Darcy',1221,0)]:
    run('search-'+name, search(q)+selected(item,prefix)+keys('ENTER')+unit(item,prefix)+guarded(item==1103))

# Context and uncertainty are visible through the same real FORMAT > Help flow.
for spanish in (False,True):
    tag='es' if spanish else 'en'
    pre=locale(2) if spanish else ''
    for name,q,rid in [('ph','pH',114),('mach','Mach number',113),('decibel','dB',104),
            ('normal-gas','Nm³',137),('electron-mass','Electron mass',3),
            ('atomic-time','Atomic unit of time',5),('planck-length','Planck length',7),
            ('planck-temperature','Planck temperature',12)]:
        case=tag+'-reference-help-'+name
        # Exact symbols can legitimately collide: pH = picohenry or acidity,
        # dB = decibyte or decibel. Verify both identities before selecting help.
        ambiguity = ''
        if name in ('ph', 'decibel'):
            uid, prefix = (22, 18) if name == 'ph' else (1287, 13)
            ambiguity = selected(uid, prefix) + shot(tag+'-ambiguity-'+name) + keys('DOWN')
        run(case,pre+search(q)+ambiguity+selected_reference(rid)+keys('FORMAT','DOWN','DOWN','DOWN','ENTER')+
            'assert_calc_input toolbox geometry\n'+shot(case)+keys('BACK')+selected_reference(rid)+
            keys('ENTER')+'assert_calc_input dump\nassert_calc_input structure\n'+guarded(True)+ordinary(),
            verify=lambda log,rid=rid: f'QuantityReference[{rid}:0]' in log)
    case=tag+'-dalton-help'
    run(case,pre+search('Dalton')+selected(1103)+keys('FORMAT','DOWN','DOWN','DOWN','ENTER')+
        'assert_calc_input toolbox geometry\n'+shot(case)+keys('BACK','ENTER')+unit(1103)+guarded(True)+ordinary())

raise SystemExit(0 if results and all(r['passed'] for r in results.values()) else 1)
