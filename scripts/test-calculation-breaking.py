#!/usr/bin/env python3
"""Record adversarial real Calculation key sessions; never bless failures/goldens.

Each case is a fresh process with its own deterministic temporary filesystem.
Timeouts/crashes are evidence and do not abort the remaining rounds.
"""
import argparse,json,os,subprocess,hashlib
from pathlib import Path
from PIL import Image,ImageDraw
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--bin',type=Path,required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('--round',type=int,required=True);p.add_argument('--only',nargs='*')
a=p.parse_args();os.chdir(ROOT);a.out.mkdir(parents=True,exist_ok=True)
cases=json.loads((ROOT/'tests/fixtures/calculation-breaking.json').read_text(encoding='utf-8'));records=[]
env=dict(os.environ,NUMOS_CALC_INPUT_TRACE='1',NUMOS_DELIMITER_METRICS='1',PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH',''))
for f in [f for f in cases if f['round']==a.round and (not a.only or f['id'] in a.only)]:
 folder=a.out/f['id'];folder.mkdir(exist_ok=True)
 def shot(name):return 'wait 3\nscreenshot '+(folder/(name+'.ppm')).as_posix()+'\n'
 def keys(s):return ''.join('key '+k+'\n' for k in s.split())
 script='wait 200\nopen_app Calculation\nwait 30\n'+(f.get('events') or keys(f['keys']))
 script+='assert_calc_input dump\n'+shot('editable')
 if f.get('navigate'):
  script+=keys('LEFT '*f['navigate'])+'assert_calc_input dump\n'+shot('navigated')
 if f.get('evaluate',True):script+=keys('ENTER')+'wait 25\nassert_calc_input dump\n'+shot('evaluated')
 if f.get('tail'):script+=keys(f['tail'])+'assert_calc_input dump\n'+shot('tail')
 script+='log BREAKING_DONE\n'
 replay=folder/'repro.numos';replay.write_text(script,encoding='utf-8')
 frames=sum(int(l.split()[1])+1 if l.startswith('wait ') else 1 for l in script.splitlines())+100
 cmd=[str(a.bin.resolve()),'--headless','--deterministic','--quiet','--frames',str(frames),'--script',str(replay)]
 try:
  r=subprocess.run(cmd,env=env,capture_output=True,timeout=45);log=r.stdout+r.stderr;exit_=r.returncode
 except subprocess.TimeoutExpired as e:log=(e.stdout or b'')+(e.stderr or b'');exit_='timeout'
 (folder/'run.log').write_bytes(log)
 record=dict(id=f['id'],round=a.round,command=cmd,exit=exit_,completed=b'BREAKING_DONE' in log,expected=f.get('expected'),description=f['description'])
 for frame in folder.glob('*.ppm'):
  im=Image.open(frame).convert('RGB');assert im.size==(320,240);im.save(frame.with_suffix('.png'));im.resize((1280,960),Image.Resampling.NEAREST).save(frame.with_name(frame.stem+'-4x.png'))
 records.append(record);(a.out/'results.json').write_text(json.dumps(records,indent=2),encoding='utf-8')
 print(f['id'],exit_,flush=True)
sheet=Image.new('RGB',(960,264*((len(records)+2)//3)),'#e3e6eb');d=ImageDraw.Draw(sheet)
for i,r in enumerate(records):
 x=i%3*320;y=i//3*264;d.text((x+4,y+4),r['id']+' / '+str(r['exit']),fill='black');image=a.out/r['id']/'editable.png'
 if image.exists():sheet.paste(Image.open(image),(x,y+24))
sheet.save(a.out/'contact.png')
(a.out/'binary.json').write_text(json.dumps(dict(path=str(a.bin.resolve()),sha256=hashlib.sha256(a.bin.read_bytes()).hexdigest()),indent=2))
print('RECORDED',len(records),'cases; no automatic classification or golden promotion')
