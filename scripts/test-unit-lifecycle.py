#!/usr/bin/env python3
"""100 comparable fixed-64-KiB product cycles, with full history and HOME."""
from pathlib import Path
import argparse,json,os,re,subprocess
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
root=Path(__file__).resolve().parents[1]
def keys(s):return ''.join('key '+k+'\n' for k in s.split())
body='wait 200\nopen_app Calculation\nwait 30\n'
for cycle in range(104):
    # Every cycle starts with a full ordinary history, not only the first one.
    body+=keys('AC 2 ADD 3 ENTER')*60
    body+=keys('AC 2 POW 3 TOOLBOX')+'sdl_text Metre\n'+keys('DOWN RIGHT')
    body+='assert_calc_input toolbox selected 32769 0\nassert_calc_input toolbox dump\n'
    body+=keys('DOWN DOWN DOWN')+'assert_calc_input toolbox selected 32769 15\n'
    body+=keys('FORMAT ENTER BACK BACK BACK')+'assert_calc_input serialized ((2)^(3))\n'
    body+=keys('TOOLBOX UP RIGHT ENTER')+'assert_calc_input toolbox selected 32769 15\n'
    body+=keys('RIGHT')+'assert_calc_input toolbox selected 32769 15\n'
    body+=keys('BACK ENTER')+'assert_calc_input unit 1 15 1\n'+keys('ENTER')+'assert_calc_status units_unavailable\n'
    body+=keys('UP')+'assert_calc_input unit 1 15 1\nassert_calc_input structure\n'
    body+=keys('AC 2 ADD 2 ENTER')+'assert_calc_exact 4\n'+keys('TOOLBOX UP RIGHT ENTER FORMAT ENTER BACK BACK HOME')+'wait 20\nassert_app Menu\ncalculus_probe\nopen_app Calculation\nwait 20\n'
body+='log UNIT_LIFECYCLE_DONE\n'
script=a.out/'cycles.numos';script.write_text(body,encoding='utf8')
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'])
frames=sum(int(l.split()[1])+1 if l.startswith('wait ') else 1 for l in body.splitlines())+80
r=subprocess.run([a.bin,'--headless','--quiet','--deterministic','--frames',str(frames),'--script',os.path.relpath(script,root)],cwd=root,env=env,capture_output=True,timeout=300)
(a.out/'cycles.log').write_bytes(r.stdout+r.stderr);assert r.returncode==0,r.returncode
text=r.stdout.decode('utf8',errors='replace');assert 'UNIT_LIFECYCLE_DONE' in text
homes=[dict(re.findall(r'(\w+)=(\d+)',l)) for l in text.splitlines() if l.startswith('CALCULUS_PROBE|app=Menu|')]
mem=[dict(re.findall(r'(\w+)=(\d+)',l)) for l in text.splitlines() if l.startswith('[TOOLBOX-MEM]')]
assert len(homes)==104 and len(mem)==104
for h in homes[4:]:
    for field in ('objects','timers','pool_total','pool_free','handles'):assert h[field]==homes[4][field],(field,h,homes[4])
assert 0<int(homes[4]['pool_total'])<=65536 # TLSF reports usable bytes, excluding bookkeeping
result={'passed':True,'configuredPoolBytes':65536,'warmups':4,'comparableCycles':100,'ordinaryEvaluationsBeforeEachCycle':60,'minimumSampledFree':min(int(m['free']) for m in mem),'minimumSampledLargestBlock':min(int(m['largest']) for m in mem),'homes':homes,'modalSamples':mem}
(a.out/'results.json').write_text(json.dumps(result,indent=2));print('PASS 100 full-history, prefix, favorite, insert/cancel, HOME cycles; fixed 64 KiB')
