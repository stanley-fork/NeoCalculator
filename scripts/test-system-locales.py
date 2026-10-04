#!/usr/bin/env python3
"""Real Settings keyboard selection and restart persistence; no locale injection API."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile
from PIL import Image

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--bin', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
a.out.mkdir(parents=True, exist_ok=True)
env = dict(os.environ, PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;' + os.environ.get('PATH', ''))
records = []
def keys(text):
    return ''.join('key ' + k + '\n' for k in text.split())
def shot(name):
    return 'wait 3\nscreenshot ' + Path(os.path.relpath(a.out / (name + '.ppm'), root)).as_posix() + '\n'
def run(name, body, fs):
    script = 'wait 200\nopen_app Settings\nwait 30\n' + keys('DOWN DOWN DOWN DOWN') + body + 'log SYSTEM_LOCALE_DONE\n'
    replay = a.out / (name + '.numos')
    replay.write_text(script, encoding='utf8')
    frames = sum(int(line.split()[1]) + 1 if line.startswith('wait ') else 1 for line in script.splitlines()) + 100
    result = subprocess.run([str(a.bin.resolve()), '--headless', '--quiet', '--deterministic',
        '--frames', str(frames), '--script', os.path.relpath(replay, root),
        '--fs-sandbox-dir', os.path.relpath(fs, root)], cwd=root, env=env, capture_output=True, timeout=120)
    log = (result.stdout + result.stderr).decode('utf8', errors='replace')
    (a.out / (name + '.log')).write_text(log, encoding='utf8')
    assert result.returncode == 0 and 'SYSTEM_LOCALE_DONE' in log, (name, result.returncode, log[-2500:])
    for image_path in a.out.glob(name + '*.ppm'):
        image = Image.open(image_path)
        assert image.size == (320, 240)
        image.save(image_path.with_suffix('.png'))
    return log

# Persisted IDs are an independent oracle, not read from the implementation.
for index, (tag, expected) in enumerate([('en-US', 0), ('en-GB', 4), ('es-ES', 1), ('es-419', 5)]):
    fs = Path(tempfile.mkdtemp(prefix=tag + '-', dir=a.out))
    # A roundtrip on the default selection also exercises repeated replacement.
    sequence = keys('RIGHT LEFT') if index == 0 else keys(' '.join(['RIGHT'] * index))
    run(tag + '-selected', sequence + shot(tag + '-selected'), fs)
    stored = (fs / 'settings.dat').read_bytes()
    assert len(stored) == 10 and stored[:5] == bytes([0x31, 0x30, 0x54, 0x53, 1]) and stored[9] == expected
    run(tag + '-reloaded', shot(tag + '-reloaded') + keys('HOME') +
        'wait 20\nopen_app Calculation\nwait 20\n' + keys('2 ADD 2 ENTER') + 'assert_calc_exact 4\n', fs)
    before = Image.open(a.out / (tag + '-selected.png')).crop((0, 28, 320, 240)).tobytes()
    after = Image.open(a.out / (tag + '-reloaded.png')).crop((0, 28, 320, 240)).tobytes()
    assert before == after, (tag, 'Settings changed after restart')
    assert (fs / 'settings.dat').read_bytes() == stored, 'read-only launch wrote settings'
    records.append(dict(locale=tag, storageId=expected, persisted=True, identicalSettingsAfterRestart=True))

for name, offset, invalid in [('future-version', 4, 99), ('test-locale', 9, 2), ('unknown-locale', 9, 253)]:
    fs = Path(tempfile.mkdtemp(prefix=name + '-', dir=a.out))
    old = bytearray([0x31, 0x30, 0x54, 0x53, 1, 0, 0, 0, 10, 0])
    old[offset] = invalid
    (fs / 'settings.dat').write_bytes(old)
    run(name, keys('RIGHT') + shot(name), fs)
    assert (fs / 'settings.dat').read_bytes()[9] == 4, 'invalid record did not fall back to English US'
    records.append(dict(case=name, safeFallback=True))

(a.out / 'results.json').write_text(json.dumps(dict(passed=True, records=records), indent=2), encoding='utf8')
print('PASS four system regions, actual keyboard selection, repeated save, restart and invalid record recovery')
