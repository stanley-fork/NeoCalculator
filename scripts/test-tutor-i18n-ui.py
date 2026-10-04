#!/usr/bin/env python3
"""Real Settings selection, retained-page language switching and unavailable UX."""
from pathlib import Path
import argparse,importlib.util,json,os,subprocess
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('t',ROOT/'scripts/test-tutor-teaching-ui.py');t=importlib.util.module_from_spec(spec);spec.loader.exec_module(t);eq=t.eq
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'])
def run(name,script):
 script+='log I18N_PRODUCT_DONE\n';path=a.out/(name+'.numos');path.write_text(script,encoding='utf-8');frames=sum(int(x.split()[1]) if x.startswith('wait ') else 1 for x in script.splitlines())+200
 r=subprocess.run([a.bin,'--headless','--deterministic','--quiet','--frames',str(frames),'--script',os.path.relpath(path,ROOT)],cwd=ROOT,env=env,capture_output=True,timeout=180);(a.out/(name+'.log')).write_bytes(r.stdout+r.stderr);assert r.returncode==0 and b'I18N_PRODUCT_DONE' in r.stdout,(name,r.returncode,r.stdout[-900:])
 return [json.loads(x.split('[TUTOR_VIEW] ',1)[1]) for x in r.stdout.decode().splitlines() if '[TUTOR_VIEW] ' in x]
def shot(name):return 'wait 4\nscreenshot '+os.path.relpath(a.out/(name+'.ppm'),ROOT).replace('\\','/')+'\n'
settings='wait 200\nopen_app Settings\nwait 30\n'+eq.keys('DOWN DOWN DOWN DOWN ENTER ENTER')+shot('settings-es')+eq.keys('HOME')+'wait 30\n'
product=run('settings-product',settings+eq.single('3 x + 5 = 2 0')+eq.keys('tools')+'assert_equations view dump\n'+shot('product-es'))
assert product[0]['title'].startswith('Paso '),'Spanish must be selected through the real Settings row'
records=[]
for name in ['linear','quadratic','rational','system','dependent','radical-extraneous','log-domain','trig-sine','trig-unfamiliar-deg']:
 expr=t.CASES[name];start=eq.single(expr[0]) if len(expr)==1 else eq.system(expr)
 if name.endswith('-deg'):start='set_angle_mode deg\n'+start
 start+=eq.keys('tools')
 count=run(name+'-discover',start+'assert_equations view dump\n')[0]['count'];script=start
 for page in range(count):
  script+='assert_equations view dump\nassert_equations locale es\nassert_equations view dump\nassert_equations trace builds 1\nassert_equations locale en\nassert_equations view dump\nassert_equations trace builds 1\n'+eq.keys('RIGHT')
 views=run(name+'-roundtrip',script)
 for i in range(count):
  before,spanish,after=views[3*i:3*i+3]
  for v in [spanish,after]:
   assert v['conversions']==0,(name,i,'locale performed mathematical conversion')
   assert v['page']==before['page'] and v['step']==before['step'] and v['lastStep']==before['lastStep']
   for x,y in zip(before['formulas'],v['formulas']):
    assert x['ast']==y['ast'],(name,i,'formula changed')
    for key in ['kind','state','step','branch','row']:assert x[key]==y[key]
   assert len(v['formulas'])==len(before['formulas'])
  assert before['title']==after['title'] and before['prose']==after['prose'] and before['heading']==after['heading']
 records.append(dict(case=name,pages=count,noMathConversions=True))
run('unsupported',eq.single('x ^ 3 RIGHT = 7')+eq.keys('tools')+shot('unsupported-en')+'assert_equations locale es\n'+shot('unsupported-es')+eq.keys('BACK HOME')+'wait 30\nassert_equations closed\n')
wide=eq.single(t.CASES['radical-wide'][0])+eq.keys('tools RIGHT RIGHT RIGHT RIGHT RIGHT')
run('pan-roundtrip',wide+shot('pan-start')+eq.keys('VAR')+shot('pan-before')+'assert_equations locale es\n'+shot('pan-es')+'assert_equations locale en\n'+shot('pan-after')+'assert_equations trace builds 1\n')
for path in a.out.glob('*.ppm'):
 im=Image.open(path);assert im.size==(320,240);im.save(path.with_suffix('.png'))
pixels=lambda name:Image.open(a.out/(name+'.png')).crop((0,24,320,220)).tobytes()
assert pixels('pan-start')!=pixels('pan-before'),'wide fixture did not pan'
assert pixels('pan-before')==pixels('pan-after'),'locale roundtrip lost horizontal pan or content geometry'
(a.out/'results.json').write_text(json.dumps(dict(pass_=True,settings=True,records=records,unsupported=True,panRoundtrip=True),indent=2));print('Product selector, retained EN/ES/EN, zero conversions, pan preservation and unsupported UX PASS')
