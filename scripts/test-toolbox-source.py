#!/usr/bin/env python3
"""Mutations must fail validation before any generated output is written."""
from pathlib import Path
import copy,json,subprocess,tempfile
root=Path(__file__).resolve().parents[1]
rows=json.loads((root/'src/math/toolbox-catalog.json').read_text(encoding='utf8'))
outputs=[root/'src/math/ToolboxEntries.inc',root/'docs/TOOLBOX_01_CATALOG.md']
before=[p.read_bytes() for p in outputs]
with tempfile.TemporaryDirectory(prefix='toolbox-source-') as temporary:
 for name in ['duplicate','missing-es','encoding','keyboard-flag','preview-args','invalid-greek','invalid-latin','invalid-case-alias','invalid-infinity']:
  changed=copy.deepcopy(rows)
  if name=='duplicate':changed[1]['id']=changed[0]['id']
  elif name=='missing-es':del changed[0]['es']
  elif name=='keyboard-flag':changed[0]['keyboardShortcut']='false'
  elif name=='preview-args':changed[0]['previewArgs']='x12345'
  elif name=='invalid-greek':next(e for e in changed if e['recipe']=='Symbol')['argument']=0x3A2
  elif name=='invalid-infinity':next(e for e in changed if e['recipe']=='Infinity')['argument']='UnsignedInfinity'
  elif name=='invalid-latin':next(e for e in changed if e['recipe']=='Variable')['argument']=ord('!')
  elif name=='invalid-case-alias':changed[0]['caseSensitiveAliases']='true'
  else:changed[0]['es']='Fracci?n'
  path=Path(temporary)/(name+'.json');path.write_text(json.dumps(changed),encoding='utf8')
  r=subprocess.run(['python',str(root/'scripts/generate-toolbox-catalog.py'),'--source',str(path),'--check'],capture_output=True,cwd=root)
  assert r.returncode!=0,name
  assert [p.read_bytes() for p in outputs]==before,'failed generation changed outputs'
  print('PASS negative detector',name)
