#!/usr/bin/env python3
"""Real Equations Results/Steps capture, bounded navigation and ownership checks."""
import argparse, importlib.util, json, math, os, subprocess
from pathlib import Path
from PIL import Image, ImageDraw

ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('eq',ROOT/'scripts/test-equations-rebuild.py')
eq=importlib.util.module_from_spec(spec);spec.loader.exec_module(eq)
CASES={
 'sine-zero':'sin x RIGHT = 0',
 'sine':'sin x RIGHT = 1 / 2 RIGHT',
 'cosine':'cos x RIGHT = 1 / 2 RIGHT',
 'cosine-endpoint':'cos x RIGHT = 1',
 'tangent':'tan 3 x RIGHT = 1',
 'affine':'sin 2 x RIGHT = 1 / 2 RIGHT',
 'unfamiliar':'sin 3 x - 1 RIGHT = 1 / 3 RIGHT',
 'sine-deg':'sin x RIGHT = 1 / 2 RIGHT',
 'unfamiliar-deg':'sin 3 x - 1 RIGHT = 1 / 3 RIGHT',
}
def main():
    p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True)
    p.add_argument('--baseline',action='store_true');p.add_argument('--cases',nargs='*');a=p.parse_args()
    out=a.out.resolve();out.mkdir(parents=True,exist_ok=True);binary=str(Path(a.bin).resolve())
    env=dict(os.environ,NUMOS_EQUATIONS_BOUNDS='1');dll=eq.helper.sdl2_dll_dir(binary)
    if dll:env['PATH']=dll+os.pathsep+env.get('PATH','')
    rel=lambda path:os.path.relpath(path,ROOT).replace(os.sep,'/')
    def run(name,script):
        script+='log PERIODIC_UI_DONE\n';file=out/(name+'.numos');file.write_text(script,encoding='utf-8')
        frames=sum(int(x.split()[1]) if x.startswith('wait ') else 1 for x in script.splitlines())+500
        r=subprocess.run([binary,'--headless','--deterministic','--quiet','--frames',str(frames),'--script',rel(file)],cwd=ROOT,env=env,capture_output=True,timeout=180)
        (out/(name+'.log')).write_bytes(r.stdout+r.stderr)
        assert r.returncode==0 and b'PERIODIC_UI_DONE' in r.stdout,(name,r.returncode,r.stdout[-2000:])
        rows=r.stdout.decode(errors='replace').splitlines()
        return {key:[json.loads(x.split(tag,1)[1]) for x in rows if tag in x] for key,tag in [('results','[PERIODIC_RESULT] '),('views','[TUTOR_VIEW] '),('traces','[TUTOR_TRACE] ')]}
    images=[];records=[]
    def shot(name):
        images.append(name)
        return 'wait 4\nscreenshot '+rel(out/(name+'.ppm'))+'\n'
    for name,keys in CASES.items():
        if a.cases and name not in a.cases:continue
        start='set_angle_mode '+('deg' if name.endswith('-deg') else 'rad')+'\n'+eq.single(keys)
        ordinary='' if a.baseline else 'assert_equations periodic equivalent\nassert_equations periodic dump\n'
        discovery=run(name+'-discover',start+ordinary+eq.keys('tools')+'assert_equations view dump\n')
        pageCount=discovery['views'][0]['count']
        bottom=discovery['results'][0]['maxScroll'] if discovery['results'] else 112
        script=start+ordinary+shot(name+'-results-top')
        if not a.baseline and name=='sine':
            for locale in ['es','fr','pseudo','en']:
                script+='assert_equations locale '+locale+'\n'+ordinary+shot(name+'-results-'+locale)
        for i in range(math.ceil(bottom/112)):
            script+=eq.keys('DOWN '*4)+shot(name+'-results-scroll-'+str(i))
        # Horizontal access uses the same canvases; no additional solving.
        script+=eq.keys('RIGHT '*20)+shot(name+'-results-pan')+eq.keys('LEFT '*20)
        script+=eq.keys('tools')+'assert_equations trace check\nassert_equations trace dump\n'+eq.keys('RIGHT '*(pageCount-1))
        script+='assert_equations view dump\n'+shot(name+'-steps-final-top')
        script+=eq.keys('DOWN '*20)+'assert_equations view bounded\n'+shot(name+'-steps-final-bottom')
        script+=eq.keys('VAR '*12)+shot(name+'-steps-final-pan')
        script+=eq.keys('EXE EXE BACK')+ordinary+eq.keys('tools')+'assert_equations trace builds 1\n'
        script+=eq.keys('BACK BACK UP UP UP UP ENTER AC x = 7 BACK')+'assert_equations epochs current\n'
        script+=eq.keys('HOME')+'wait 30\nassert_equations closed\n'
        result=run(name,script)
        if not a.baseline:
            assert all(x['scope']==1 and x['origin']==1 for x in result['results'])
            first,last=result['results'][0],result['results'][-1]
            assert all(x['families']==first['families'] for x in result['results']),'locale/reopening changed owned answer'
            assert 'same complete solution set' in result['views'][0]['prose']
            assert 'representative' not in result['views'][0]['prose'].lower()
        records.append(dict(case=name,**result));(out/'results.json').write_text(json.dumps(records,indent=2),encoding='utf-8')
        print(name,'PASS',flush=True)
    for name in images:
        with Image.open(out/(name+'.ppm')) as im:
            assert im.size==(320,240);im.save(out/(name+'.png'))
    sheet=Image.new('RGB',(960,264*math.ceil(len(images)/3)),'white');draw=ImageDraw.Draw(sheet)
    for i,name in enumerate(images):
        x=(i%3)*320;y=(i//3)*264;draw.text((x+4,y+4),name,fill='black')
        with Image.open(out/(name+'.png')) as im:sheet.paste(im,(x,y+24))
    sheet.save(out/'contact.png')
    (out/'manifest.json').write_text(json.dumps(dict(binary=binary,baseline=a.baseline,images=images),indent=2))
if __name__=='__main__':main()
