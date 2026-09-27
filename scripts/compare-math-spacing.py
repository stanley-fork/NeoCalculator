#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Independent TeX rules vs observed NumOS layout/draw/cursor, exact integer px.

The reference engine's screenshots are qualitative and have NO pixel tolerance
gate. This gate uses the verified tex.web string, observed real glyph advances,
and every visited cursor boundary. It does not import production helpers.
"""
import argparse, json, re
from pathlib import Path

TEX = '0234000122*4000133**3**344*0400400*000000234000111*1111112341011'

def space(left, right, style, em):
    code=TEX[left*8+right]
    return (3 if code=='2' else {'1':3,'3':4,'4':5}.get(code,0) if style<2 else 0)*em//18

def events(path):
    return [json.loads(l) for l in path.read_text(encoding='utf-8').splitlines() if l.startswith('{')]

def semantic_tree(value):
    # Exclude ONLY dumpTree's five measured layout numbers, not entire nodes,
    # expressions, steps, conditions, or semantic payload sections.
    return re.sub(r'  \[-?\d+x-?\d+ a-?\d+/d-?\d+\]', '', value)

def validate(records, require_math_minus=False):
    failures=[];frames=[];frame=[]
    for e in records:
        if e['event']=='frame':
            if frame:frames.append(frame)
            frame=[]
        frame.append(e)
    if frame:frames.append(frame)
    before=next(e['tree'] for e in records if e['event']=='treeBefore')
    after=next(e['tree'] for e in records if e['event']=='treeAfter')
    if semantic_tree(before)!=semantic_tree(after):failures.append('semantic tree changed during layout/draw/navigation')
    if next(e['result'] for e in records if e['event']=='engineBefore') != next(e['result'] for e in records if e['event']=='engineAfter'):
        failures.append('Giac result changed during layout/draw/navigation')
    def check(ok,message):
        if not ok and message not in failures:failures.append(message)
    for frame in frames:
        if require_math_minus:
            glyphs=[]
            for e in frame:
                if e['event']=='glyph':glyphs.append(e)
                elif e['event']=='text':
                    expected=[0x2212 if c=='-' else ord(c) for c in e['text']]
                    check([g['cp'] for g in glyphs]==expected,'draw submitted a hyphen or wrong glyph instead of mathematical minus')
                    check(sum(g['advance'] for g in glyphs)==e['advance'],'draw pen disagrees with submitted glyph advances')
                    glyphs=[]
        nodes={e['path']:e for e in frame if e['event']=='node'}
        if not nodes:continue
        children=lambda n:[nodes[n['path']+'/'+str(i)] for i in range(n['children'])]
        def flatten(n):
            if n['type']==0:
                for child in children(n):yield from flatten(child)
            else:yield n
        def boundary(n):
            if n['type'] in [5,15]:return boundary(children(n)[0])
            if n['type']==0:return boundary(children(n)[0]) if n['children']==1 else (0,0)
            if n['type']==7:return (7,7)
            if n['type'] in [8,9]:return (1,0)
            return n['leftStatic'],n['rightStatic']
        for root in nodes.values():
            if root['type']!=0:continue
            parent=nodes.get(root['path'].rsplit('/',1)[0])
            if parent and parent['type']==0:continue
            leaves=list(flatten(root));effective=[list(boundary(n)) for n in leaves]
            prev=None
            for i,n in enumerate(leaves):
                if n['type']==3:continue
                if effective[i][0]==2 and (prev is None or effective[prev][1] in [1,2,3,4,6]):effective[i]=[0,0]
                elif effective[i][0] in [3,5,6] and prev is not None and effective[prev][1]==2:effective[prev]=[0,0]
                prev=i
            if prev is not None and effective[prev][1]==2:effective[prev]=[0,0]
            pen=root['x'];previous=None
            for i,n in enumerate(leaves):
                if n['type']!=3:
                    if previous is not None:pen+=space(previous,effective[i][0],root['style'],root['em'])
                    previous=effective[i][1]
                check(n['x']==pen,f"{n['path']}: draw origin violates independent contextual spacing")
                if 'effectiveLeft' in n:
                    check([n['effectiveLeft'],n['effectiveRight']]==effective[i],f"{n['path']}: wrong effective class")
                pen+=n['width']
            check(root['width']==pen-root['x'],f"{root['path']}: reserved row width violates TeX spaces")
        for n in nodes.values():
            if n['type'] in [1,2,10,11,17]:
                text=next((e for e in frame if e['event']=='text' and (e['x'],e['baseline'])==(n['x'],n['baseline'])),None)
                if text:check(n['width']==text['advance'],f"{n['path']}: measured advance differs from drawn text")
            if n['type']==5:
                base,exp=children(n)
                check(n['width']==exp['x']-n['x']+exp['width']+n['scriptReserve'],f"{n['path']}: duplicate/missing script reservation")
        cursor=next((e for e in reversed(frame) if e['event']=='cursor'),None)
        bounds=next((e for e in reversed(frame) if e['event']=='cursorBounds'),None)
        if cursor and bounds:
            row=nodes[cursor['path']];kids=children(row);i=cursor['index']
            expected=kids[i]['x'] if i<len(kids) else row['x']+row['width']
            check(cursor['origin']==row['x'] and cursor['baseline']==row['baseline'],f"{row['path']}: Finder origin/baseline differs from draw")
            check(cursor['origin']+cursor['offset']==expected,f"{row['path']}[{i}]: logical cursor differs from drawing")
            check(bounds['x']==expected,f"{row['path']}[{i}]: scroll/clamp hid logical cursor")
            check(cursor['style']==row['style'] and cursor['em']==row['em'],f"{row['path']}: cursor style/font differs")
    return failures

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--baseline',type=Path,required=True);p.add_argument('--candidate',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    p.add_argument('--require-math-minus',action='store_true',help='Independently require U+2212 for ASCII minus in math text')
    a=p.parse_args();report=[]
    fixtures=json.loads((Path(__file__).resolve().parents[1]/'tests/fixtures/math-spacing.json').read_text())
    for f in fixtures:
        old=events(a.baseline/(f['id']+'.log'));new=events(a.candidate/(f['id']+'.log'))
        baseline=validate(old,a.require_math_minus);candidate=validate(new,a.require_math_minus)
        old_sem=[e for e in old if e['event'] in ['semantic','treeBefore','engineBefore','engineAfter']]
        new_sem=[e for e in new if e['event'] in ['semantic','treeBefore','engineBefore','engineAfter']]
        if old_sem!=new_sem:candidate.append('baseline/candidate semantic input differs')
        report.append(dict(id=f['id'],baselineFailures=baseline,candidateFailures=candidate))
    assert any(r['baselineFailures'] for r in report),'No baseline detector fired'
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
    failures=[(r['id'],r['candidateFailures']) for r in report if r['candidateFailures']]
    print(json.dumps(dict(cases=len(report),baselineDetected=sum(bool(r['baselineFailures']) for r in report),candidateFailures=failures),indent=2))
    raise SystemExit(bool(failures))

if __name__=='__main__':main()
