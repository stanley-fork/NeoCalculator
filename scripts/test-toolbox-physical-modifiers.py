#!/usr/bin/env python3
"""Regression for PCB ALPHA being consumed before Toolbox could open Search.

Uses the real physical-key adapter, generated contacts and production modal.
No text or finished AST injection. Outputs and filesystem are isolated.
"""
import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import sys
sys.stdout.reconfigure(errors="replace")

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--bin', type=Path, required=True)
parser.add_argument('--out', type=Path, required=True)
args = parser.parse_args()
args.out.mkdir(parents=True, exist_ok=True)
mapping = (root/'src/input/generated/ProductionKeypadMap.generated.h').read_text(encoding='utf8')
contacts = mapping.split('kProductionKeypadMap = {{', 1)[1].split('}};', 1)[0]
coordinates = [match.groups() for line in contacts.splitlines()
               if (match := re.match(r'\s*\{(\d+), (\d+),', line))]
planes = [line for line in mapping.split('kTextPlaneDefinitions = {{', 1)[1].split('}};', 1)[0].splitlines()
          if '{{{' in line]
assert len(coordinates) == len(planes) == 50

script = 'wait 200\nopen_app Calculation\nwait 30\nkey 2\nkey ADD\nkey 3\nkey TOOLBOX\n'
for letter in 'metre':
    index = next(i for i, line in enumerate(planes) if 'SemanticId::code_'+letter+',' in line)
    script += 'calc_physical 3 0\nassert_calc_input toolbox query\n'
    script += 'calc_physical %s %s\n' % coordinates[index]
script += ('key DOWN\nassert_calc_input toolbox selected 32769 0\n'
           'assert_calc_input serialized 2+3\nkey RIGHT\n'
           'assert_calc_input toolbox selected 32769 0\nkey BACK\nkey BACK\n'
           'assert_calc_input serialized 2+3\nkey BACK\nkey AC\n'
           'calc_physical 4 0\ncalc_physical 4 0\nassert_modifier shift\n'
           'key HOME\nlog TOOLBOX_PHYSICAL_MODIFIERS_PASS\n')
path = args.out/'physical-modifiers.numos'
path.write_text(script, encoding='utf8')
fs = args.out/'fs'
fs.mkdir(exist_ok=True)
env = dict(os.environ, PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH', ''))
command = [str(args.bin.resolve()), '--headless', '--quiet', '--deterministic', '--frames', '500',
           '--script', os.path.relpath(path, root), '--fs-sandbox-dir', os.path.relpath(fs, root)]
run = subprocess.run(command, cwd=root, env=env, capture_output=True, timeout=90)
log = run.stdout + run.stderr
(args.out/'run.log').write_bytes(log)
passed = run.returncode == 0 and b'TOOLBOX_PHYSICAL_MODIFIERS_PASS' in log
(args.out/'result.json').write_text(json.dumps(dict(passed=passed, exit=run.returncode,
    command=command, assertions=log.count(b': PASS -')), indent=2))
if not passed:
    print(log.decode('utf8', errors='replace')[-2500:])
raise SystemExit(0 if passed else 1)
