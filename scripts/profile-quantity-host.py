#!/usr/bin/env python3
"""Scoped native new/AST samples; excludes C malloc and is not a heap maximum."""
from pathlib import Path
import re
exec(Path(__file__).with_name('test-calculation-quantities.py').read_text(encoding='utf8').split('\nsummation=')[0])
long='123456789123456789'
body=amount(long,'metre',1)+keys('ENTER')+check(long,long,'1:0:1')
body+=target('cm',1,14)+check(str(int(long)*100),long,'1:14:1')
body+=keys('FORMAT STO 1 AC 7 ENTER UP UP')+check(long,long,'1:0:1')
samples={}
for scope in ('analysis','convert','compose','ans','memory','selector','publish','format','history'):
 env['NUMOS_TOOLBOX_SCOPE']=scope;env['NUMOS_TOOLBOX_FAIL_AT']='0';env['NUMOS_TOOLBOX_FAIL_CALL']='2' if scope=='history' else '1'
 run(scope,body)
 log=(a.out/(scope+'.log')).read_text(encoding='utf8')
 entries=[dict(re.findall(r'(\w+)=([\w-]+)',line)) for line in log.splitlines() if line.startswith('TOOLBOX_ALLOC')]
 assert len(entries)==1 and entries[0]['failures']=='0' and records[scope]['passed'],(scope,entries)
 samples[scope]=entries[0]
(a.out/'allocations.json').write_text(json.dumps(dict(scope='native global new/new[] plus AST; C malloc/LVGL/Giac C heap not covered; Giac C++ wrappers can contribute',samples=samples),indent=2))
print('PASS nine scoped native allocation samples',flush=True)
