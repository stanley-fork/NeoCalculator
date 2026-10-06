#!/usr/bin/env python3
"""Persistent private allocator faults in contextual-menu construction and commit."""
from pathlib import Path
import re
exec(Path(__file__).with_name('test-calculation-quantities.py').read_text(encoding='utf8').split('\nsummation=')[0])
quantity=amount('2','metre',1)+keys('ENTER')+check('2','2','1:0:1')
recovery=keys('AC 2 ADD 2 ENTER')+check('4')
env.update(NUMOS_TOOLBOX_SCOPE='quick',NUMOS_TOOLBOX_FAIL_AT='0',NUMOS_TOOLBOX_FAIL_CALL='2')
run('measure-quick',quantity+output()+'assert_calc_input unit_menu geometry\n'+keys('BACK')+recovery)
log=(a.out/'measure-quick.log').read_text(encoding='utf8')
sample=next(dict(re.findall(r'(\w+)=([\w-]+)',line)) for line in log.splitlines() if line.startswith('TOOLBOX_ALLOC scope=quick'))
for point in range(1,int(sample['attempts'])+1):
 env['NUMOS_TOOLBOX_FAIL_AT']=str(point)
 run('quick-'+str(point),quantity+output()+'assert_calc_input unit_menu closed\n'+check('2','2','1:0:1')+recovery)
for key in ('NUMOS_TOOLBOX_SCOPE','NUMOS_TOOLBOX_FAIL_AT','NUMOS_TOOLBOX_FAIL_CALL'):env.pop(key,None)
for row in range(1,6):
 env['NUMOS_UNIT_QUICK_FAIL_ROW']=str(row)
 run('row-'+str(row),quantity+output()+'assert_calc_input unit_menu closed\n'+check('2','2','1:0:1')+recovery)
env.pop('NUMOS_UNIT_QUICK_FAIL_ROW')
env['NUMOS_UNIT_QUICK_STALE']='1'
run('stale-quick',quantity+output()+keys('DOWN ENTER')+'assert_calc_input unit_menu closed\n'+check('2','2','1:0:1')+recovery)
env.pop('NUMOS_UNIT_QUICK_STALE')

for point in (1,2): # session, then admission/rank buffer: exactly two allocations
 env.update(NUMOS_TOOLBOX_SCOPE='selector',NUMOS_TOOLBOX_FAIL_AT=str(point),NUMOS_TOOLBOX_FAIL_CALL='1')
 run('index-session-'+str(point),quantity+output()+all_units()+'assert_calc_input toolbox closed\n'+check('2','2','1:0:1')+recovery)
 faultLog=(a.out/('index-session-'+str(point)+'.log')).read_text(encoding='utf8')
 assert re.search(r'TOOLBOX_ALLOC scope=selector .*failures=[1-9]',faultLog)
env.update(NUMOS_TOOLBOX_SCOPE='publish',NUMOS_TOOLBOX_FAIL_AT='1',NUMOS_TOOLBOX_FAIL_CALL='1')
run('failed-confirmation',quantity+output()+keys('DOWN ENTER')+'assert_calc_input unit_menu geometry\n'+check('2','2','1:0:1')+keys('BACK')+recovery)
(a.out/'quick-allocation-sample.json').write_text(json.dumps(sample,indent=2))
raise SystemExit(0 if all(r['passed'] for r in records.values()) else 1)
