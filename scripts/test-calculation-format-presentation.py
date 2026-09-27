#!/usr/bin/env python3
"""Real Calculation events: blank input, pending slots, and EN/ES menu glyphs.

Requires the private build-calculation-observer.py executable. Expectations
inspect its actual label fonts and the rendered framebuffer, not source text.
"""
import argparse
import json
import os
import re
import subprocess
from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--bin', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--expect-failure', action='store_true')
a = p.parse_args()
os.chdir(ROOT)
a.out.mkdir(parents=True, exist_ok=True)
env = dict(os.environ, NUMOS_CALC_INPUT_TRACE='1',
           PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;' + os.environ.get('PATH', ''))
physical = {'SHIFT': (4, 0), 'ALPHA': (3, 0), 'FORMAT': (4, 5)}

def keys(sequence):
    return ''.join('calc_physical %d %d\n' % physical[k] if k in physical
                   else 'key ' + k + '\n' for k in sequence.split())

results = []
for locale in ('en', 'es'):
    folder = a.out / locale
    folder.mkdir(exist_ok=True)
    script = 'wait 200\n'
    if locale == 'es':
        script += 'open_app Settings\nwait 30\n' + keys('DOWN DOWN DOWN DOWN ENTER HOME') + 'wait 30\n'
    script += 'open_app Calculation\nwait 30\n'
    stages = [
        ('empty', '', False), ('power', '2 ^', True),
        ('power-error', 'ENTER', True), ('base', 'AC ^', True),
        ('fraction', 'AC /', True), ('fraction-error', 'ENTER', True),
        ('nested', 'AC 2 ^ /', True), ('delete-to-empty', 'AC 2 DEL', False),
        ('history', 'AC 2 ^ ENTER AC 5 ENTER UP UP', True),
        ('menu', 'AC 7 / 3 ENTER SHIFT ALPHA FORMAT', None),
        ('menu-eng', 'DOWN DOWN DOWN DOWN DOWN', None),
        ('menu-mixed', 'DOWN', None),
    ]
    for name, sequence, pending in stages:
        script += keys(sequence) + f'wait 3\nlog PRESENTATION_{name}\nassert_calc_input dump\nscreenshot {(folder / (name + ".ppm")).as_posix()}\n'
    script += 'log PRESENTATION_DONE\n'
    replay = folder / 'repro.numos'
    replay.write_text(script)
    frames = sum(int(line.split()[1]) + 1 if line.startswith('wait ') else 1
                 for line in script.splitlines()) + 100
    run = subprocess.run([str(a.bin.resolve()), '--headless', '--deterministic', '--quiet',
                          '--frames', str(frames), '--script', str(replay)],
                         env=env, capture_output=True, timeout=60)
    log = (run.stdout + run.stderr).decode('utf-8', errors='replace')
    (folder / 'run.log').write_text(log, encoding='utf-8')
    errors = []
    if run.returncode or 'PRESENTATION_DONE' not in log:
        errors.append('replay did not finish')
    glyphs = re.findall(r'\[FORMAT-GLYPH\] cp=(\d+) present=(\d+)', log)
    if not glyphs or any(present != '1' for _, present in glyphs):
        errors.append('actual menu font has missing glyphs')
    if locale == 'es':
        for label in ('Científica (SCI)', 'Ingeniería (ENG)', 'Fracción mixta', 'Decimal periódico'):
            if '[FORMAT-LABEL] ' + label not in log:
                errors.append('Spanish label not exercised: ' + label)
        if not {237, 243} <= {int(cp) for cp, _ in glyphs}:
            errors.append('accented glyphs not exercised')
    for name, _, pending in stages:
        image = Image.open(folder / (name + '.ppm')).convert('RGB')
        assert image.size == (320, 240)
        image.save(folder / (name + '.png'))
        image.resize((1280, 960), Image.Resampling.NEAREST).save(folder / (name + '-4x.png'))
        gray = sum(count for count, color in image.crop((0, 25, 320, 234)).getcolors(320 * 240)
                   if color == (132, 130, 132))
        if pending is False and gray:
            errors.append(name + ': empty input still has a box')
        if pending is True and gray < 12:
            errors.append(name + ': missing pending-slot ink')
    results.append(dict(locale=locale, passed=not errors, errors=errors, glyphs=len(glyphs)))
    print(locale, errors or 'PASS', flush=True)
(a.out / 'results.json').write_text(json.dumps(results, indent=2))
failed = any(not record['passed'] for record in results)
raise SystemExit(0 if failed == a.expect_failure else 1)
