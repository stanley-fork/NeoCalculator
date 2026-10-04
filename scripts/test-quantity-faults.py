#!/usr/bin/env python3
"""Persistent allocation faults in NumOS quantity ownership, excluding Giac internals."""
from pathlib import Path
# Share only event constructors/runner, not prepared expressions or expected values.
exec(Path(__file__).with_name('test-calculation-quantities.py').read_text(encoding='utf8').split('\nsummation=')[0])
long='123456789123456789'
q=amount(long,'metre',1)+keys('ENTER')
ordinary=keys('AC 2 ADD 2 ENTER')+check('4')
for scope in ('analysis','convert','compose','ans','memory','selector','publish','format','history'):
 for point in ((1,) if scope in ('ans','selector') else (1,2) if scope=='analysis' else (1,2,4)):
  env['NUMOS_TOOLBOX_SCOPE']=scope;env['NUMOS_TOOLBOX_FAIL_AT']=str(point);env['NUMOS_TOOLBOX_FAIL_CALL']='2' if scope=='history' else '1'
  if scope in ('analysis','convert','compose'):
   body=q+'assert_calc_status out_of_memory\nassert_calc_input unit 1 0 1\nassert_calc_input structure\n'+keys('ENTER')+check(long,long,'1:0:1')
  elif scope=='ans':
   if point>1:continue # short mirror and shared ownership need exactly one long-text copy
   body=q+'assert_calc_input no_result\nassert_calc_input unit 1 0 1\n'+keys('ENTER')+check(long,long,'1:0:1')
  elif scope=='memory':
   body=q+keys('STO 1 AC ALPHA 1 ENTER')+check('0') # failed STO keeps original empty A
  elif scope=='selector':
   body=q+output()+keys('DOWN ENTER')+'assert_calc_input toolbox closed\n'+check(long,long,'1:0:1')
  elif scope=='publish':
   body=q+output()+keys('DOWN ENTER')+search('cm')+selected(1,14)+keys('ENTER')+'assert_calc_input toolbox open\n'+check(long,long,'1:0:1')+keys('BACK BACK')
  elif scope=='format':
   body=q+keys('FORMAT')+check(long,long,'1:0:1')
  else:
   # First UP selects the newest scalar; second UP attempts the older quantity.
   body=q+keys('AC 7 ENTER UP UP')+check('7')+'assert_calc_input structure\n'
  name=scope+'-'+str(point);run(name,body+ordinary)
  log=(a.out/(name+'.log')).read_text(encoding='utf8')
  import re
  samples=[dict(re.findall(r'(\w+)=([\w-]+)',line)) for line in log.splitlines() if line.startswith('TOOLBOX_ALLOC')]
  records[name]['samples']=samples
  records[name]['passed'] &= any(int(s['failures'])>0 for s in samples)
(a.out/'results.json').write_text(json.dumps(records,indent=2))
raise SystemExit(0 if all(r['passed'] for r in records.values()) else 1)
