#!/usr/bin/env python3
"""Record mathematical capability failures separately from viewport stress.

Run against a preserved private result-observer executable. No production
math fixes or expected-output promotion are performed by this recorder.
"""
import argparse,json,os,re,subprocess
from pathlib import Path
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--bin',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
p.add_argument('--round',type=int,required=True);p.add_argument('--only',nargs='*')
a=p.parse_args();os.chdir(ROOT);a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,NUMOS_CALC_INPUT_TRACE='1',PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH',''))
records=[]
for f in json.loads((ROOT/'tests/fixtures/calculation-results-breaking.json').read_text()):
 if f['round']!=a.round or a.only and f['id'] not in a.only:continue
 folder=a.out/f['id'];folder.mkdir(exist_ok=True)
 # FACT has no script alias. Use the actual production SHIFT+multiply matrix
 # event, whose published semantic is factorial, without adding an input API.
 keys=lambda s:''.join('calc_physical 4 0\ncalc_physical 1 7\n' if k=='FACT' else 'key '+k+'\n' for k in s.split())
 shot=lambda name:'wait 3\nscreenshot '+(folder/(name+'.ppm')).as_posix()+'\n'
 s='wait 200\nopen_app Calculation\nwait 30\n'+f.get('setup','')+keys(f['keys'])+'assert_calc_input dump\n'+shot('editable')
 s+=keys('ENTER')+'wait 25\nassert_calc_input dump\n'+shot('evaluated')
 if f.get('tail'):s+=keys(f['tail'])+'wait 25\nassert_calc_input dump\n'+shot('tail')
 s+='log RESULTS_BREAKING_DONE\n';path=folder/'repro.numos';path.write_text(s,encoding='utf-8')
 frames=sum(int(l.split()[1])+1 if l.startswith('wait ') else 1 for l in s.splitlines())+100
 try:
  r=subprocess.run([str(a.bin.resolve()),'--headless','--deterministic','--quiet','--frames',str(frames),'--script',str(path)],env=env,capture_output=True,timeout=45);log=r.stdout+r.stderr;code=r.returncode
 except subprocess.TimeoutExpired as e:log=(e.stdout or b'')+(e.stderr or b'');code='timeout'
 (folder/'run.log').write_bytes(log);text=log.decode('utf-8',errors='replace')
 exact=re.findall(r'^\[RESULT-EXACT\] (.*)$',text,re.M);approx=re.findall(r'^\[RESULT-APPROX\] (.*)$',text,re.M)
 record=dict(fixture=f,exit=code,completed='RESULTS_BREAKING_DONE' in text,exact=exact,approximate=approx,status=re.findall(r'^\[RESULT-STATUS\] (.*)$',text,re.M))
 records.append(record);(a.out/'results.json').write_text(json.dumps(records,indent=2),encoding='utf-8')
 for path in folder.glob('*.ppm'):
  im=Image.open(path).convert('RGB');assert im.size==(320,240);im.save(path.with_suffix('.png'));im.resize((1280,960),Image.Resampling.NEAREST).save(path.with_name(path.stem+'-4x.png'))
 print(f['id'],code,exact[-1:] or ['no observation'],flush=True)
