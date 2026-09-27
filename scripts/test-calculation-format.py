#!/usr/bin/env python3
"""FORMAT/ENG detector through real keys; private read-only observer required."""
import argparse, json, os, re, subprocess
from pathlib import Path
from PIL import Image, ImageChops

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--bin', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
a = p.parse_args(); os.chdir(ROOT); a.out.mkdir(parents=True, exist_ok=True)
env = dict(os.environ, NUMOS_CALC_INPUT_TRACE='1', PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH',''))
physical = {'FORMAT':(4,5),'SHIFT':(4,0),'ALPHA':(3,0),'EXP':(2,9)}
def keys(sequence):
    return ''.join('calc_physical %d %d\n'%physical[k] if k in physical else 'key '+k+'\n' for k in sequence.split())
menu = 'SHIFT ALPHA FORMAT'
cases = {
 'decimal': [('exact','0 . 1 ENTER',0),('decimal','FORMAT',1),('back','FORMAT',0)],
 'pi': [('exact','2 pi ENTER',0),('decimal','FORMAT',1),('back','FORMAT',0)],
 'surd': [('exact','SQRT 2 ENTER',0),('decimal','FORMAT',1),('back','FORMAT',0)],
 'surd-sum': [('exact','[ SQRT 2 ] + [ SQRT 3 ] ENTER',0),('decimal','FORMAT',1),('back','FORMAT',0)],
 'complex': [('exact','[ SQRT neg 1 ] + 1 ENTER',0),('decimal','FORMAT',1),('back','FORMAT',0),('menu',menu,0),('polar','DOWN DOWN ENTER',8),('polar-back','FORMAT',0)],
 'mixed': [('exact','7 / 3 ENTER',0),('menu',menu,0),('mixed','DOWN '*6+'ENTER',6),('back','FORMAT',0)],
 'periodic': [('exact','1 / 9 7 ENTER',0),('menu',menu,0),('periodic','DOWN DOWN ENTER',2),('back','FORMAT',0)],
 'extended': [('exact','1 / 7 ENTER',0),('menu',menu,0),('extended','DOWN DOWN DOWN ENTER',3),('back','FORMAT',0)],
 'engineering': [('exact','1 2 3 4 ENTER',0),('eng','SHIFT EXP',5),('eng-shift','SHIFT EXP',5),('eng-left','LEFT',5),('back','FORMAT',0)],
 'engineering-small': [('exact','0 . 0 0 0 0 1 2 3 4 ENTER',0),('eng','SHIFT EXP',5),('back','FORMAT',0)],
 'engineering-negative': [('exact','neg 1 2 3 4 ENTER',0),('eng','SHIFT EXP',5),('back','FORMAT',0)],
 'engineering-zero': [('exact','0 ENTER',0),('eng','SHIFT EXP',5),('back','FORMAT',0)],
 'prime': [('exact','3 6 0 ENTER',0),('menu',menu,0),('prime','DOWN '*4+'ENTER',7),('back','FORMAT',0)],
 'symbolic-menu': [('exact','x + 1 ENTER',0),('menu',menu,0),('close','BACK',0)],
 'complex-exponential': [('exact','[ SQRT neg 1 ] + 1 ENTER',0),('menu',menu,0),('exponential','DOWN DOWN DOWN ENTER',9),('back','FORMAT',0)],
 'complex-phase-deg': [('exact','[ SQRT neg 1 ] + 1 ENTER',0),('polar',menu+' DOWN DOWN ENTER',8),('menu',menu,8),('degrees','DOWN DOWN DOWN ENTER',8),('back','FORMAT',0)],
 'fix-round': [('exact','9 . 9 9 5 ENTER',0),('menu',menu,0),('digits','DOWN '*8+'ENTER',0),('fixed','ENTER',10),('back','FORMAT',0)],
 'angle-units': [('exact','SHIFT sin 0 . 5 ENTER',0),('menu',menu,0),('degrees','DOWN '*6+'ENTER',12),('gradians',menu+' DOWN ENTER',13),('radians',menu+' UP UP ENTER',11),('back','FORMAT',0)],
 'empty-menu': [('input','SHIFT ALPHA FORMAT',0),('evaluate','2 ENTER',0)],
 'plain-half': [('exact','1 / 2 ENTER',0),('menu',menu,0),('close','BACK',0)],
 'sine-ratio': [('exact','sin 0 ENTER',0),('menu',menu,0),('close','BACK',0)],
 'angle-from-deg': [('exact','SHIFT sin 0 . 5 ENTER',0),('radians',menu+' DOWN '*5+'ENTER',11),('back','FORMAT',0)],
 'complex-from-deg': [('exact','[ SQRT neg 1 ] + 1 ENTER',0),('polar',menu+' DOWN DOWN ENTER',8),('back','FORMAT',0),('exponential',menu+' DOWN DOWN DOWN ENTER',9)],
}
records=[]
for name, stages in cases.items():
 d=a.out/name;d.mkdir(exist_ok=True)
 script='wait 200\nopen_app Calculation\nwait 30\n'
 if name.endswith('from-deg'):script+='set_angle_mode deg\n'
 for stage,sequence,mode in stages:
  script+=keys(sequence)+'wait 3\nlog PHASE_'+stage+'\nassert_calc_input dump\nscreenshot '+(d/(stage+'.ppm')).as_posix()+'\n'
 script+='assert_angle_mode '+('deg' if name.endswith('from-deg') else 'rad')+'\nlog FORMAT_DONE\n'; replay=d/'repro.numos';replay.write_text(script)
 frames=sum(int(l.split()[1])+1 if l.startswith('wait ') else 1 for l in script.splitlines())+100
 r=subprocess.run([str(a.bin.resolve()),'--headless','--deterministic','--quiet','--frames',str(frames),'--script',str(replay)],env=env,capture_output=True,timeout=60)
 log=(r.stdout+r.stderr).decode('utf-8',errors='replace');(d/'run.log').write_text(log,encoding='utf-8')
 observed=[int(re.search(r'resultMode=(\d+)',chunk).group(1)) for chunk in re.split(r'PHASE_[^\r\n]+',log)[1:] if re.search(r'resultMode=(\d+)',chunk)]
 exact=re.findall(r'^\[RESULT-EXACT\] (.*)$',log,re.M)
 expected=[mode for _,_,mode in stages]
 errors=[]
 if r.returncode or 'FORMAT_DONE' not in log: errors.append('replay failed')
 if observed!=expected:errors.append(f'modes {observed} != {expected}')
 if name!='empty-menu' and len(set(x.strip() for x in exact))!=1:errors.append('canonical result mutated')
 chunks=dict(zip([stage for stage,_,_ in stages],re.split(r'PHASE_[^\r\n]+',log)[1:]))
 def capability(stage):
  match=re.search(r'^\[FORMAT-CAPABILITIES\](.*)$',chunks[stage],re.M)
  if not match:errors.append('missing capability observation');return set()
  return set(map(int,match[1].split()))
 if name in ('plain-half','decimal','sine-ratio','pi','mixed'):
  if capability('exact') & {8,9,11,12,13}:errors.append('unrelated complex/angle format offered')
 if name=='symbolic-menu' and capability('exact')!={0,1}:errors.append('unsupported symbolic conversion offered')
 if name.startswith('complex') and capability('exact')!={0,1,8,9}:errors.append('complex format choices incomplete or unrelated')
 if name=='angle-units' and not {11,12,13}<=capability('exact'):errors.append('angle conversions missing')
 # Independent known values, read from the actual displayed tree. The input
 # tree follows CALC-INPUT and is deliberately excluded from these assertions.
 expectations={
  'angle-units':{'degrees':['Number "30"','Symbol "°"'],
                 'gradians':['Number "100"','Number "3"','gon'],
                 'radians':['Constant π','Number "6"','rad']},
  'complex-phase-deg':{'degrees':['Number "2"','Number "45"','∠','Symbol "°"']},
  'complex-exponential':{'exponential':['Constant e','Constant i','Constant π','Number "4"']},
  'fix-round':{'fixed':['Number "10.00"']},
  'angle-from-deg':{'radians':['Constant π','Number "6"','rad']},
  'complex-from-deg':{'polar':['Number "45"','Symbol "°"'],
                       'exponential':['Constant π','Number "4"','Constant e']},
  'engineering':{'eng':['Number "1.234"','Number "10"','Number "3"'],
                 'eng-shift':['Number "1234"','Number "0"']},
 }
 for stage,tokens in expectations.get(name,{}).items():
  displayed=chunks[stage].split('[CALC-INPUT]')[0]
  if not all(token in displayed for token in tokens):errors.append('wrong displayed value: '+stage)
 images={}
 for stage,_,_ in stages:
  f=d/(stage+'.ppm')
  if not f.exists():errors.append('missing screenshot');continue
  im=Image.open(f).convert('RGB');assert im.size==(320,240);im.save(f.with_suffix('.png'));im.resize((1280,960),Image.Resampling.NEAREST).save(d/(stage+'-4x.png'));images[stage]=im
 if 'back' in images and ImageChops.difference(images['exact'].crop((0,25,320,240)),images['back'].crop((0,25,320,240))).getbbox(): errors.append('exact pixels not restored')
 records.append(dict(id=name,passed=not errors,errors=errors,modes=observed,exact=exact))
 print(name,errors or 'PASS',flush=True)
(a.out/'results.json').write_text(json.dumps(records,indent=2))
raise SystemExit(any(not r['passed'] for r in records))
