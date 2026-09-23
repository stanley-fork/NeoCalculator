#!/usr/bin/env python3
"""Complete 320x240 product walkthroughs using the existing canonical key harness."""
from pathlib import Path
import importlib.util,sys
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('teaching',ROOT/'scripts/test-tutor-teaching-ui.py')
t=importlib.util.module_from_spec(spec);spec.loader.exec_module(t)
t.CASES={
 'composition-square':['x ^ 4 RIGHT - 5 x ^ 2 RIGHT + 4 = 0'],
 'composition-nested-root':['x ^ 4 RIGHT - 2 x ^ 2 RIGHT - 1 = 0'],
 'composition-affine-square':['( 2 x - 1 ) ^ 4 RIGHT - 5 ( 2 x - 1 ) ^ 2 RIGHT + 4 = 0'],
 'exp-composition':['SHIFT ln 2 x RIGHT - 3 SHIFT ln x RIGHT + 2 = 0'],
 'exp-composition-empty':['SHIFT ln 2 x RIGHT + SHIFT ln x RIGHT = 0'],
 'composition-log':['ln x - 1 RIGHT ^ 2 RIGHT - 3 ln x - 1 RIGHT + 2 = 0'],
 'composition-sine':['6 sin x RIGHT ^ 2 RIGHT - 5 sin x RIGHT + 1 = 0'],
 'composition-sine-endpoint':['2 sin x RIGHT ^ 2 RIGHT - 3 sin x RIGHT + 1 = 0'],
 'composition-sine-deg':['6 sin 3 x - 1 RIGHT ^ 2 RIGHT - 5 sin 3 x - 1 RIGHT + 1 = 0'],
 'composition-cosine':['cos x RIGHT ^ 2 RIGHT = 3 / 4 RIGHT'],
 'composition-tangent':['tan 2 x RIGHT ^ 2 RIGHT = 3'],
 'composition-outer':['2 sin x RIGHT + 1 = 0'],
}
def presentation(name,mode,root,views):
    finalFamilies=set();seenAuxiliary=False;seenReturn=False
    for view in views:
        child=view.get('child',0);d=root['composition']['children'][child-1] if child else root
        if child:seenAuxiliary|=child==1;seenReturn|=child>1
        for f in view['formulas']:
            assert f.get('child',0)==child
            assert f['step']<len(d['steps']) and d['steps'][f['step']]['verdict']==1
            assert f['ast'] is not None
            if f['kind']=='equation':assert f['row']<len(d['states'][f['state']]['branches'][f['branch']]['equations'])
            if f['kind']=='periodic_family':
                family=d['states'][f['state']]['families'][f['branch']]
                assert family['originalCheck']==1 or d['states'][f['state']]['conclusion']==0
                if not child and view['kind']=='final':finalFamilies.add(f['branch'])
        assert '[invalid' not in view['prose'] and 'Amount used' not in view['prose']
        if any(d['steps'][f['step']]['rule'] in ['equation.add','equation.divide'] for f in view['formulas']):
            assert any(f['kind']=='balanced_operation' for f in view['formulas'])
    if root.get('composition'):
        assert seenAuxiliary or not root['composition']['children'][0]['steps']
        assert seenReturn or not any(p['child'] for p in root['composition']['preimages'])
    if root['states'][-1]['conclusion']==5:assert finalFamilies==set(range(len(root['states'][-1]['families'])))
    if name=='composition-outer':assert 'composition' not in root and len(views)<=10
t.check_presentation=presentation
if __name__=='__main__':
    if '--cases' not in sys.argv:sys.argv+=['--cases',*t.CASES.keys()]
    t.main()
