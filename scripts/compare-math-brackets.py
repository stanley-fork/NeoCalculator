#!/usr/bin/env python3
"""Check actual STIX submissions, continuous stems and shared cursor geometry.

Input is the private --brackets output of test-math-spacing.py. Expected glyph
ranges are the documented supplemental font mapping, not production helpers.
"""
import argparse
import importlib.util
import json
from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('spacing', ROOT/'scripts/compare-math-spacing.py')
spacing = importlib.util.module_from_spec(spec)
spec.loader.exec_module(spacing)
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--probe', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
a = p.parse_args()
reports = []
levels = set()
assembly_levels = set()
for f in json.loads((ROOT/'tests/fixtures/math-brackets.json').read_text()):
    records = spacing.events(a.probe/(f['id']+'.log'))
    failures = spacing.validate(records, require_math_minus=True)
    start = next(i for i,e in enumerate(records) if e['event']=='frame')
    end = next((i for i in range(start+1,len(records)) if records[i]['event']=='frame'),len(records))
    frame = records[start:end]
    glyphs = [e for e in frame if e['event']=='delimiterGlyph' and 0xe020<=e['cp']<0xe040]
    expected = f['id'] not in ['bracket-delete','bracket-empty-delete','bracket-unmatched-close']
    if bool(glyphs)!=expected:
        failures.append('missing/unexpected authentic bracket glyph submissions')
    im = Image.open(a.probe/(f['id']+'.ppm')).convert('RGB')
    # 4-bpp STIX antialiasing can leave a cap-edge sample at RGB 238. Ink is
    # any non-white sample; a genuinely missing stem row remains pure white.
    dark = lambda x,y: im.getpixel((x,y))!=(255,255,255)
    groups = {}
    for g in glyphs:
        if not g['stix']:failures.append('bracket submitted with fallback font')
        levels.add(g['em'])
        if (g['cp']-0xe020)%16>=13:assembly_levels.add(g['em'])
        groups.setdefault((g['x'],g['em']),[]).append(g)
    for (x,em),pieces in groups.items():
        # Actual font ink bounds, clipped by the cap bounds for assemblies.
        caps = [g for g in pieces if (g['cp']-0xe020)%16!=14]
        top = min(g['baseline']-g['inkH']-g['offsetY'] for g in caps)
        bottom = max(g['baseline']-g['offsetY'] for g in caps)
        left = min(x+g['offsetX'] for g in pieces)
        right = max(x+g['offsetX']+g['inkW'] for g in pieces)
        if not (0<=left<right<=320 and 0<=top<bottom<=240):
            failures.append('bracket fixture unexpectedly clipped')
            continue
        if not all(any(dark(px,y) for px in range(left,right)) for y in range(top,bottom)):
            failures.append(f'broken bracket stem / assembly seam at {x}, em {em}')
    for n in (e for e in frame if e['event']=='node' and e['type']==3):
        x,y,w,h=n['x'],n['baseline']-n['ascent'],n['width'],n['ascent']+n['descent']
        edges=[(x+i,y) for i in range(w)]+[(x+i,y+h-1) for i in range(w)]
        edges += [(x,y+j) for j in range(h)]+[(x+w-1,y+j) for j in range(h)]
        if not all(dark(px,py) for px,py in edges):failures.append('pending slot border missing')
    reports.append(dict(id=f['id'],groups=len(groups),failures=failures))
assert levels=={8,12,18},levels
assert assembly_levels=={8,12,18},assembly_levels
a.out.parent.mkdir(parents=True,exist_ok=True)
a.out.write_text(json.dumps(dict(cases=reports,emSizes=sorted(levels),assemblyEmSizes=sorted(assembly_levels)),indent=2))
bad=[r for r in reports if r['failures']]
print(json.dumps(dict(cases=len(reports),failures=bad),indent=2))
raise SystemExit(bool(bad))
