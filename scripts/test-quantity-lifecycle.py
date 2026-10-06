#!/usr/bin/env python3
"""100 measured fixed-pool mixed cycles, each with a full history; no Giac reset."""
from pathlib import Path
exec(Path(__file__).with_name('test-calculation-quantities.py').read_text(encoding='utf8').split('\nsummation=')[0].replace('timeout=180','timeout=600'))
body=''
for i in range(104):
 body+=keys('AC 2 ADD 3 ENTER')*60
 body+=keys('AC')+amount('2','metre',1)+keys('ENTER')+check('2','2','1:0:1')
 body+=target('mm',1,15)+check('2000','2','1:15:1')+output()+all_units()+search('km')+selected(1,10)+'assert_calc_input toolbox dump\n'+keys('RIGHT DOWN BACK BACK BACK')+check('2000','2','1:15:1')
 # Confirm contextual destinations too; neither operation replaces canonical Ans.
 body+=output()+keys('DOWN ENTER')+check('200','2','1:14:1')
 body+=output()+keys('DOWN ENTER')+check('2000','2','1:15:1')
 body+=keys('AC TOOLBOX')+search('mm')+selected(1,15)+keys('FORMAT ENTER BACK BACK')+keys('TOOLBOX UP RIGHT ENTER')+selected(1,15)+keys('ENTER ENTER')+check('1/1000','1/1000','1:0:1')
 body+=keys('UP')+check('1/1000','1/1000','1:0:1')
 body+=keys('AC')+amount('2','metre',1)+keys('ADD')+amount('3','Second',3)+keys('ENTER')+'assert_calc_status quantity_error\n'
 body+=keys('AC ans ENTER')+check('1/1000','1/1000','1:0:1')
 body+=keys('AC TOOLBOX UP RIGHT ENTER FORMAT ENTER BACK BACK HOME')+'wait 20\nassert_app Menu\ncalculus_probe\nopen_app Calculation\nwait 20\n'
run('mixed-cycles',body)
import re
log=(a.out/'mixed-cycles.log').read_text(encoding='utf8')
homes=[dict(re.findall(r'(\w+)=(\d+)',line)) for line in log.splitlines() if line.startswith('CALCULUS_PROBE|app=Menu|')]
modal=[dict(re.findall(r'(\w+)=(\d+)',line)) for line in log.splitlines() if line.startswith('[TOOLBOX-MEM]')]
assert len(homes)==104 and len(modal)==104,(len(homes),len(modal))
for h in homes[4:]:
 for f in ('objects','timers','pool_total','pool_free','handles'):assert h[f]==homes[4][f],(f,h,homes[4])
assert 0<int(homes[4]['pool_total'])<=65536
(a.out/'resources.json').write_text(json.dumps(dict(warmups=4,cycles=100,historyCapacity=50,evaluationsToFillBeforeEach=60,homes=homes,modal=modal,minimumSampledFree=min(int(m['free']) for m in modal),minimumSampledLargestBlock=min(int(m['largest']) for m in modal)),indent=2))
assert all(r['passed'] for r in records.values())
print('PASS 100 mixed cycles, full history, fixed 64KiB pool')
