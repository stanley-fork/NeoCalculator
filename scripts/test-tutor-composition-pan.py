#!/usr/bin/env python3
"""Capture the complete horizontal traversal of three long DEG relations."""
from pathlib import Path
import argparse, importlib.util, json, os, subprocess
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('composition',ROOT/'scripts/test-tutor-composition-ui.py')
c=importlib.util.module_from_spec(spec);spec.loader.exec_module(c)
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
a.out=a.out.resolve();a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'])
records=[]
for locale in ['en','es']:
    for page,scroll in [(11,3),(12,1),(13,2)]:
        label=f'{locale}-{page:02}';frames=[]
        script='set_angle_mode deg\n'+c.t.eq.single(c.t.CASES['composition-sine-deg'][0])+c.t.eq.keys('tools')
        script+=f'assert_equations locale {locale}\n'+c.t.eq.keys('RIGHT '*page+'DOWN '*(3*scroll))
        script+='assert_equations trace dump\n'
        for i in range(33):
            name=f'{label}-pan-{i:02}';frames.append(name)
            script+=(c.t.eq.keys('VAR') if i else '')+f'assert_equations view page {page}\nassert_equations view bounded\nassert_equations view dump\n'
            script+=f'wait 4\nscreenshot {os.path.relpath(a.out/(name+".ppm"),ROOT).replace(chr(92),"/")}\n'
        script+='assert_equations trace dump\nassert_equations trace builds 1\nlog COMPOSITION_PAN_COMPLETE\n'
        path=a.out/(label+'.numos');path.write_text(script)
        count=sum(int(x.split()[1]) if x.startswith('wait ') else 1 for x in script.splitlines())+150
        r=subprocess.run([a.bin,'--headless','--deterministic','--quiet','--frames',str(count),'--script',os.path.relpath(path,ROOT)],cwd=ROOT,env=env,capture_output=True,timeout=180)
        (a.out/(label+'.log')).write_bytes(r.stdout+r.stderr)
        assert not r.returncode and b'COMPOSITION_PAN_COMPLETE' in r.stdout
        traces=[json.loads(x.split('[TUTOR_TRACE] ',1)[1]) for x in r.stdout.decode().splitlines() if '[TUTOR_TRACE] ' in x]
        assert len(traces)==2 and c.t.stable(traces[0])==c.t.stable(traces[1])
        pixels=[]
        for name in frames:
            im=Image.open(a.out/(name+'.ppm'));assert im.size==(320,240);im.save(a.out/(name+'.png'))
            pixels.append(im.crop((0,54,320,220)).tobytes())
        assert any(x!=pixels[0] for x in pixels[1:]) and pixels[0] in pixels[1:]
        records.append(dict(locale=locale,page=page,scroll=scroll,frames=frames,returnAt=pixels[1:].index(pixels[0])+1))
        print(label,'PASS',flush=True)
(a.out/'results.json').write_text(json.dumps(records,indent=2))
