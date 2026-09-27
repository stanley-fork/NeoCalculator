#!/usr/bin/env python3
"""FORMAT dialogs under the fixed LVGL pool, including EN/ES screenshots."""
import argparse, json, os, re, subprocess
from pathlib import Path
from PIL import Image

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--bin',type=Path,required=True)
p.add_argument('--out',type=Path,required=True)
a=p.parse_args();root=Path(__file__).resolve().parents[1];os.chdir(root)
a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH',''))
def keys(s):
 physical={'SHIFT':(4,0),'ALPHA':(3,0),'FORMAT':(4,5)}
 return ''.join('calc_physical %d %d\n'%physical[k] if k in physical else 'key '+k+'\n' for k in s.split())
menu='SHIFT ALPHA FORMAT'
records=[]
def run(name,body):
 script='wait 200\n'+body+'log FORMAT_LIFECYCLE_DONE\n'
 path=a.out/(name+'.numos');path.write_text(script)
 frames=sum(int(l.split()[1])+1 if l.startswith('wait ') else 1 for l in script.splitlines())+100
 result=subprocess.run([str(a.bin.resolve()),'--headless','--deterministic','--quiet','--frames',str(frames),'--script',str(path)],env=env,capture_output=True,timeout=120)
 log=(result.stdout+result.stderr).decode('utf-8',errors='replace');(a.out/(name+'.log')).write_text(log,encoding='utf-8')
 assert result.returncode==0 and 'FORMAT_LIFECYCLE_DONE' in log,name
 return log
body='open_app Calculation\nwait 30\n'
for i in range(50):
 body+=keys('AC 7 / 3 ENTER '+menu+' '+'DOWN '*6+'ENTER FORMAT '+menu+' '+'DOWN '*8+'ENTER ENTER FORMAT '+menu+' BACK')
 body+='calculus_probe\n'+keys('HOME')+'wait 30\nassert_app Menu\ncalculus_probe\nopen_app Calculation\nwait 30\n'
log=run('pool-50-dialogs',body)
homes=[dict(re.findall(r'(\w+)=(\w+)',line)) for line in log.splitlines() if line.startswith('CALCULUS_PROBE|app=Menu|')]
assert len(homes)==50
# LVGL's monitor excludes allocator/block metadata; it reports usable space,
# not LV_MEM_SIZE. The build uses the unchanged 64 KiB arena from lv_conf.h.
assert 0<int(homes[0]['pool_total'])<=65536,'must use the fixed pool build'
for entry in homes:
 assert entry['handles']=='0'
 for key in ('objects','timers','pool_total','pool_free'):assert entry[key]==homes[0][key],(key,entry,homes[0])
records.append(dict(id='pool-50-dialogs',passed=True,homes=homes))
for locale in ('en','es'):
 body=('open_app Settings\nwait 30\n'+keys('DOWN DOWN DOWN DOWN ENTER HOME')+'wait 30\n' if locale=='es' else '')+'open_app Calculation\nwait 30\n'
 for name,input,select in [('fraction','7 / 3',''),('complex','[ SQRT neg 1 ] + 1',''),('precision','9 . 9 9 5','DOWN '*8+'ENTER')]:
  body+=keys('AC '+input+' ENTER '+menu+' '+select)+'wait 3\nscreenshot '+(a.out/(locale+'-'+name+'.ppm')).as_posix()+'\n'+keys('BACK')
 run(locale,body)
 for path in a.out.glob(locale+'-*.ppm'):
  image=Image.open(path).convert('RGB');assert image.size==(320,240)
  image.save(path.with_suffix('.png'));image.resize((1280,960),Image.Resampling.NEAREST).save(path.with_name(path.stem+'-4x.png'))
 records.append(dict(id=locale,passed=True))
(a.out/'results.json').write_text(json.dumps(records,indent=2))
print('PASS 50 dialog cycles, 64 KiB pool, EN/ES menus')
