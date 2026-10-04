#!/usr/bin/env python3
"""Actual keyboard/modal/editor events; screenshots are original 320x240."""
import tempfile
import argparse,json,os,re,subprocess,time,sys
sys.stdout.reconfigure(encoding="utf8",errors="replace")
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('--only');a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'])
results={}
def keys(*codes):return ''.join('key '+str(c)+'\n' for c in codes)
def search(q):return keys('TOOLBOX')+'sdl_text '+q+'\n'+keys('DOWN')
def selected(item,prefix=0):return f'assert_calc_input toolbox selected {32768|item} {prefix}\n'
def pick(q,item,prefix=0):return search(q)+selected(item,prefix)+keys('ENTER')+'assert_calc_input toolbox closed\n'
def unit(uid,prefix=0,count=1):return f'assert_calc_input unit {uid} {prefix} {count}\n'
def shot(name):return 'wait 2\nscreenshot '+Path(os.path.relpath(a.out/(name+'.ppm'),root)).as_posix()+'\n'
def units():return keys('TOOLBOX',*(['DOWN']*9),'ENTER')
def length():return units()+keys('ENTER')
def language(es):return (keys('HOME')+'wait 20\nopen_app Settings\nwait 20\n'+keys('DOWN','DOWN','DOWN','DOWN','ENTER','ENTER','HOME')+'wait 20\nopen_app Calculation\nwait 20\n') if es else ''
def run(name,body,fs=None,verify=None):
    if a.only and a.only not in name:return
    script='wait 200\nopen_app Calculation\nwait 30\n'+body+'log UNIT_EVENTS_DONE\n'
    path=a.out/(name+'.numos');path.write_text(script,encoding='utf8')
    frames=sum(int(l.split()[1])+1 if l.startswith('wait ') else 1 for l in script.splitlines())+80
    cmd=[a.bin,'--headless','--quiet','--deterministic','--frames',str(frames),'--script',os.path.relpath(path,root)]
    if fs is None:
        filesystem_runs=a.out/'filesystems';filesystem_runs.mkdir(exist_ok=True)
        fs=Path(tempfile.mkdtemp(prefix=name+'-',dir=filesystem_runs))
    if fs:cmd+=['--fs-sandbox-dir',os.path.relpath(fs,root)]
    start=time.perf_counter();r=subprocess.run(cmd,cwd=root,env=env,capture_output=True,timeout=180)
    log=(r.stdout+r.stderr).decode('utf8',errors='replace');(a.out/(name+'.log')).write_text(log,encoding='utf8')
    ok=r.returncode==0 and 'UNIT_EVENTS_DONE' in log and (verify(log) if verify else True)
    results[name]={'passed':ok,'exit':r.returncode,'seconds':time.perf_counter()-start}
    (a.out/'results.json').write_text(json.dumps(results,indent=2))
    print('PASS' if ok else 'FAIL',name,flush=True)
    if not ok:print(log[-2400:])
for es in (False,True):
    lang='es' if es else 'en';pre=language(es)
    run(lang+'-browse',pre+keys('2','ADD','3')+units()+shot(lang+'-units')+keys('ENTER')+selected(1)+shot(lang+'-length')+keys('RIGHT')+selected(1)+'assert_calc_input toolbox geometry\nassert_calc_input toolbox dump\n'+shot(lang+'-metre-centred')+keys(*(['UP']*3))+selected(1,10)+shot(lang+'-km')+keys(*(['DOWN']*7))+selected(1,16)+shot(lang+'-micrometre')+keys(*(['DOWN']*8))+selected(1,24)+shot(lang+'-quectometre')+keys(*(['UP']*24))+selected(1,1)+shot(lang+'-quettametre')+keys('BACK')+selected(1)+keys('BACK','BACK')+'assert_calc_input serialized 2+3\n',verify=lambda log:'selection=12 top=10 count=25' in log)
    run(lang+'-quantity',pre+keys('2')+pick('Segundo' if es else 'Second',3)+unit(3)+keys('FRAC')+pick('Metro' if es else 'Metre',1)+unit(1)+keys('POW','2')+'assert_calc_input structure\n'+shot(lang+'-edited-quantity')+keys('ENTER')+'assert_calc_status ok\n'+shot(lang+'-unavailable')+keys('UP')+unit(1)+unit(3)+keys('AC','2','ADD','2','ENTER')+'assert_calc_exact 4\n')
    run(lang+'-electricity',pre+units()+keys(*(['DOWN']*6),'ENTER')+shot(lang+'-electricity')+keys('DOWN','DOWN','DOWN','ENTER')+selected(18)+shot(lang+'-resistance')+keys('RIGHT',*(['UP']*3))+selected(18,10)+shot(lang+'-kohm')+keys('ENTER')+unit(18,10)+keys('AC')+search('µF')+selected(17,16)+shot(lang+'-microfarad')+keys('ENTER')+unit(17,16))
    run(lang+'-powers',pre+search('cm²')+selected(101,14)+shot(lang+'-square-cm')+keys('ENTER')+unit(1,14)+keys('AC')+search('cm³')+selected(102,14)+shot(lang+'-cubic-cm')+keys('ENTER')+unit(1,14)+keys('ENTER')+'assert_calc_status ok\n')
    run(lang+'-infinity',pre+search('±∞')+'assert_calc_input toolbox selected 410 3\n'+keys('ENTER','ENTER')+'assert_calc_exact [-infinity, +infinity]\n'+shot(lang+'-both-infinities'))
for q,item,prefix in [('metro',1,0),('metre',1,0),('meter',1,0),('milimetro',1,15),('milímetro',1,15),('millimeter',1,15),('resistencia',18,0),('ohm',18,0),('ohmio',18,0),('Ω',18,0),('microsegundo',3,16),('µs',3,16),('μs',3,16),('mA',4,15),('MA',4,9),('MHz',10,9),('mHz',10,15),('Pa',12,0),('pA',4,18),('s',3,0),('S',19,0)]:
    # s also names a Latin variable; keep both results with their own identities.
    extra=keys('DOWN') if q in ('s','S','Ω') else ''
    run('search-'+q.encode().hex(),search(q)+extra+selected(item,prefix)+shot('search-'+str(item)+'-'+str(prefix))+keys('ENTER')+unit(item,prefix))
fs=a.out/'favorites-fs'
run('favorite-write',search('mm')+selected(1,15)+keys('FORMAT','ENTER','BACK','BACK')+search('km/h')+selected(111)+keys('FORMAT','ENTER','BACK','BACK')+search('kΩ')+selected(18,10)+keys('FORMAT','ENTER','BACK','BACK')+search('µF')+selected(17,16)+keys('FORMAT','ENTER','BACK','BACK'),fs)
run('favorite-read',keys('TOOLBOX','UP','RIGHT','ENTER')+selected(1,15)+shot('favorite-mm')+keys('RIGHT')+selected(1,15)+'assert_calc_input toolbox dump\n'+keys('BACK','DOWN')+selected(111)+shot('favorite-kmh')+keys('ENTER')+unit(1,10)+unit(48)+keys('AC','TOOLBOX','UP','RIGHT','ENTER','DOWN','DOWN')+selected(18,10)+keys('DOWN')+selected(17,16)+keys('ENTER')+unit(17,16),fs)
run('suppressed-exe',length()+selected(1)+'keydown ENTER\nkeyrepeat ENTER\nkeyrepeat ENTER\nkeyup ENTER\nassert_calc_input no_result\n'+unit(1)+keys('ENTER')+'assert_calc_status ok\n')
run('ordinary-unit-coexistence',search('m')+'assert_calc_input toolbox selected 212 0\n'+keys('ENTER')+pick('Metre',1)+unit(1)+'assert_calc_input dump\nassert_calc_input structure\n'+keys('ENTER')+'assert_calc_status units_unavailable\n'+keys('AC','2','ADD','2','ENTER')+'assert_calc_exact 4\n')
run('zero-and-cancellation',keys('0','MUL')+pick('Metre',1)+keys('ENTER')+'assert_calc_status ok\n'+keys('AC')+pick('Metre',1)+keys('FRAC')+pick('Metre',1)+unit(1,0,2)+keys('ENTER')+'assert_calc_status ok\n')
raise SystemExit(0 if all(r['passed'] for r in results.values()) else 1)
