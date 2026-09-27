#!/usr/bin/env python3
"""Fast Calculation gate: real app keys -> authored AST -> unchanged serializer/Giac.
No expression/AST injection. Decimal oracles use Python Fraction, not the CAS.
"""
import argparse, importlib.util, json, os, re, subprocess
from fractions import Fraction
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('candidates', ROOT/'scripts/generate-emulator-candidates.py')
helper = importlib.util.module_from_spec(spec); spec.loader.exec_module(helper)
KEYS = {'^':'POW','exp':'EXP','neg':'NEGATE','-':'SUB','+':'ADD','*':'MUL','/':'FRAC',
        'divide':'DIVIDE','.':'DOT','(':'LPAREN',')':'RPAREN','log':'LOG','sin':'SIN','ENTER':'EXE'}
mapping = {code:(row,col) for row,col,code in re.findall(
    r'\{(\d+), (\d+), \d+, \d+, \d+, \d+, \d+, -?\d+, SemanticId::\w+, KeyCode::(\w+),',
    (ROOT/'src/input/generated/ProductionKeypadMap.generated.h').read_text(encoding='utf-8'))}
def keys(text, physical=False):
    output = ''
    for key in text.split():
        code = 'NUM_'+key if key.isdigit() else KEYS.get(key,key)
        if physical and code in mapping:
            row,col = mapping[code]; output += f'calc_physical {row} {col}\n'
        else: output += 'key '+key+'\n'
    return output
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--bin');p.add_argument('--out',type=Path,default=Path('out/calc-core-input-01/native'));p.add_argument('--cycles',type=int,default=0)
    a=p.parse_args();os.chdir(ROOT);a.out.mkdir(parents=True,exist_ok=True)
    binary=helper.resolve_binary(a.bin);env=dict(os.environ,NUMOS_CALC_INPUT_TRACE='1');dll=helper.sdl2_dll_dir(binary)
    if dll:env['PATH']=dll+os.pathsep+env.get('PATH','')
    records={}
    def pending_box_visible(path):
        # Original RGB565 framebuffer: require the four edges of an actual
        # placeholder, not merely a nonempty screenshot or absence of an error.
        magic,size,maximum,pixels=path.read_bytes().split(b'\n',3)
        width,height=map(int,size.split())
        assert magic==b'P6' and maximum==b'255' and (width,height)==(320,240)
        gray=bytes((132,130,132))
        at=lambda x,y:pixels[(y*width+x)*3:(y*width+x)*3+3]==gray
        for y in range(30,110):
            for x in range(1,160):
                if not at(x,y):continue
                for w in range(5,20):
                    if not all(at(x+i,y) for i in range(w)):break
                    for h in range(5,22):
                        if all(at(x+i,y+h-1) for i in range(w)) and all(
                            at(x,y+j) and at(x+w-1,y+j) for j in range(h)):
                            return True
        return False
    def run(name,body,negative=False):
        script='wait 200\nkey ENTER\nwait 30\nassert_app Calculation\n'+body+'\nlog CALC_INPUT_DONE\n'
        path=a.out/(name+'.numos');path.write_text(script,encoding='utf-8')
        # A wait command itself also consumes a script-dispatch frame.
        frames=sum(int(line.split()[1])+1 if line.startswith('wait ') else 1 for line in script.splitlines())+200
        r=subprocess.run([binary,'--headless','--deterministic','--quiet','--frames',str(frames),'--script',os.path.relpath(path,ROOT)],env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=120)
        (a.out/(name+'.log')).write_bytes(r.stdout)
        ok=(r.returncode==4 if negative else r.returncode==0 and b'CALC_INPUT_DONE' in r.stdout)
        if name=='desktop-pending-visible' and ok:
            ok=pending_box_visible(a.out/'desktop-pending-visible.ppm')
        if name.startswith('lifecycle-') and ok:
            # Compare identical HOME boundaries, not unrelated in-app maxima.
            homes = [dict(re.findall(r'(\w+)=(\w+)', line)) for line in
                     r.stdout.decode(errors='replace').splitlines()
                     if line.startswith('CALCULUS_PROBE|app=Menu|')]
            stable = ('objects', 'timers', 'handles', 'pool_total', 'pool_free')
            ok = len(homes) == a.cycles and all(
                all(h[key] == homes[0][key] for key in stable) and h['handles'] == '0'
                for h in homes)
            (a.out/'lifecycle-resources.json').write_text(json.dumps(homes,indent=2)+'\n')
        records[name]=dict(passed=ok,exit=r.returncode,expectedRejection=negative);print('PASS' if ok else 'FAIL',name,flush=True)
    def check(value):
        exact=Fraction(value);number=float(exact)
        return f'assert_calc_status ok\nassert_calc_input near {number:.17g} {max(1e-14,abs(number)*2e-13):.17g}\n'
    def sdl_press(name):
        return f'sdl_down {name}\nsdl_up {name}\n'
    desktop=(ROOT/'tests/emulator/calculation-desktop-recovery.numos').read_text()
    run('desktop-user-recovery',desktop+f'screenshot {(a.out/"desktop-user-after.ppm").as_posix()}\n')
    run('desktop-pending-visible','sdl_text 2\nsdl_text ^^\nsdl_text -2\n'
        +'assert_calc_input incomplete\n'+sdl_press('Return')+'assert_calc_status parse_error\n'
        +f'wait 3\nscreenshot {(a.out/"desktop-pending-visible.ppm").as_posix()}\n'
        +keys('AC 2 ^ - 2 ENTER')+check('0.25'))
    # Independent rational oracle; signed bases, both sign keys, multi-digit
    # exponents and cursor editing. Same production path, no prepared AST.
    for physical in (False,True):
        body=''
        for base in (-3,-2,2,3,10):
            for exponent in (1,2,3,4,6,12):
                basekeys=f'( neg {abs(base)} )' if base < 0 else ' '.join(str(base))
                for sign in ('neg','-'):
                    body+=keys(f'AC {basekeys} ^ {sign} '+ ' '.join(str(exponent)),physical)
                    body+='assert_calc_input complete\nassert_calc_input structure\n'
                    exact=Fraction(base)**(-exponent)
                    body+=keys('ENTER',physical)+check(exact)+f'assert_calc_input rational {exact.numerator} {exact.denominator}\n'
        run('signed-power-matrix-'+('production' if physical else 'logical'),body)
    fixtures=json.loads((ROOT/'tests/emulator/calculation-input.json').read_text())
    for route in ['logical','production']:
        for f in fixtures:
            stem=f['id']+'-'+route;shot=(a.out/stem).as_posix()
            body=keys(f['keys'],route=='production')+'assert_calc_input dump\nassert_calc_input complete\nassert_calc_input structure\n'
            if 'serialized' in f: body+='assert_calc_input serialized '+f['serialized']+'\n'
            body+=f'wait 3\nscreenshot {shot}-before.ppm\n'+keys('ENTER',route=='production')+check(f['expected'])+'assert_calc_input structure\n'
            if 'serialized' in f: body+='assert_calc_input serialized '+f['serialized']+'\n'
            run(stem,body+f'wait 3\nscreenshot {shot}-after.ppm\n')
    for name,partial,finish,value in [('exponent','2 ^','neg 3','0.125'),('sign','1 0 ^ neg','3','0.001'),('denominator','6 /','2','3'),('paren','(','5 )','5')]:
        run('recover-'+name,keys(partial)+'assert_calc_input incomplete\nkey ENTER\nassert_calc_status parse_error\n'+keys(finish+' ENTER')+check(value))
    run('edit-sign',keys('1 0 ^ 3 ENTER DEL neg 3 ENTER')+check('0.001')+keys('LEFT DEL ENTER')+check('1000'))
    run('edit-mantissa',keys('2 exp neg 3 ENTER RIGHT LEFT LEFT LEFT LEFT LEFT LEFT LEFT DEL 5 ENTER')+check('0.005'))
    run('history-ans',keys('2 exp neg 3 ENTER AC 5 ENTER UP')+check('5')+keys('UP')+check('0.002')+'assert_calc_input serialized 2*((10)^((-1)*3))\n'+keys('sd sd AC ans + 1 ENTER')+check('6')+keys('AC 2 ^ ENTER')+'assert_calc_status parse_error\n'+keys('AC ans ENTER')+check('6'))
    # Detecting controls: neither incomplete powers nor wrong grouping/sign may pass.
    run('reject-missing-base',keys('^ 5')+'assert_calc_input complete\n',True)
    run('reject-lost-sign',keys('2 exp 3 ENTER')+check('0.002'),True)
    run('reject-denominator-group',keys('6 6 . 6 3 / 2 RIGHT exp neg 3 ENTER')+check('33315'),True)
    run('reject-stale-result',keys('5 ENTER AC 2 ^ ENTER')+check('5'),True)
    if a.cycles:
        body='calculus_probe\n'
        for i in range(a.cycles):
            body+=keys('AC 6 6 . 6 3 / 2 exp neg 3 ENTER')+check('33315')+'calculus_probe\n'+keys('DEL ENTER')+'assert_calc_status parse_error\n'+keys('3 ENTER')+check('33315')+keys('AC HOME')+'wait 60\nassert_app Menu\ncalculus_probe\nkey ENTER\nwait 30\nassert_app Calculation\n'
        run('lifecycle-'+str(a.cycles),body)
    try:
        from PIL import Image
        for path in a.out.glob('*.ppm'):Image.open(path).save(path.with_suffix('.png'))
    except ImportError: pass
    (a.out/'results.json').write_text(json.dumps(records,indent=2)+'\n')
    return 0 if all(r['passed'] for r in records.values()) else 1
if __name__=='__main__':raise SystemExit(main())
