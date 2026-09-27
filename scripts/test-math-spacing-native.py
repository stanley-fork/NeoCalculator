#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Key-built spacing corpus on Calculation and the shared Equations editor.

Compare semantic boundary traces with the preserved input-repair executable;
write original 320x240 captures, never promote goldens. No AST/LaTeX injection.
"""
import argparse, importlib.util, json, os, re, subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('input_gate',ROOT/'scripts/test-calculation-input.py')
input_gate=importlib.util.module_from_spec(spec);spec.loader.exec_module(input_gate)

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--bin',type=Path,required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('--baseline',type=Path);p.add_argument('--window',action='store_true')
    p.add_argument('--oracle',type=Path,required=True,help='Preserved baseline probe with full Giac result traces')
    a=p.parse_args();os.chdir(ROOT);a.out.mkdir(parents=True,exist_ok=True)
    env=dict(os.environ,NUMOS_CALC_INPUT_TRACE='1',PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH',''))
    fixtures=json.loads((ROOT/'tests/fixtures/math-spacing.json').read_text());records=[]
    for f in fixtures:
        relation='=' in f['keys']
        start='wait 200\n'+('open_app Equations\nwait 30\nkey ENTER\nkey ENTER\n' if relation else 'key ENTER\nwait 30\nassert_app Calculation\n')
        observe='assert_app Equations\n' if relation else 'assert_calc_input structure\nassert_calc_input dump\n'
        screenshot=lambda state:f'wait 3\nscreenshot {(a.out/(f["id"]+"-"+state+".ppm")).as_posix()}\n'
        body=start+input_gate.keys(f['keys'])+observe+screenshot('editable')
        if not relation:
            body+='key ENTER\n'+observe
            if 'expected' in f:
                result=next(json.loads(l)['result'] for l in (a.oracle/(f['id']+'.log')).read_text().splitlines() if l.startswith('{') and json.loads(l)['event']=='engineBefore')
                value=f['expected'];tolerance=f.get('numericTolerance',max(1e-14,abs(value)*2e-13))
                body+=f'assert_calc_status ok\nassert_calc_exact {result["exact"]}\nassert_calc_input near {value:.17g} {tolerance:.17g}\n'
            body+=screenshot('evaluated')
        body+='log SPACING_NATIVE_DONE\n'
        path=a.out/(f['id']+'.numos');path.write_text(body,encoding='utf-8')
        frames=sum(int(l.split()[1])+1 if l.startswith('wait ') else 1 for l in body.splitlines())+100
        cmd=[str(a.bin.resolve()),'--deterministic','--quiet','--frames',str(frames),'--script',str(path)]
        if not a.window:cmd.append('--headless')
        r=subprocess.run(cmd,env=env,capture_output=True,timeout=90)
        log=r.stdout+r.stderr;(a.out/(f['id']+'.log')).write_bytes(log)
        assert r.returncode==0 and b'SPACING_NATIVE_DONE' in log,(f['id'],r.returncode)
        # Exact result assertions and semantic traces are independent of spacing.
        # The private probe additionally compares every field of Giac's trees.
        semantic=[l for l in log.decode(errors='replace').splitlines() if l.startswith(('[CALC-INPUT]','[CALC-EVAL]'))]
        if a.baseline:
            prior=(a.baseline/(f['id']+'.log')).read_text(encoding='utf-8',errors='replace')
            old=[l for l in prior.splitlines() if l.startswith(('[CALC-INPUT]','[CALC-EVAL]'))]
            assert semantic==old,(f['id'],'semantic trace changed')
        records.append(dict(id=f['id'],surface='Equations editor' if relation else 'Calculation input/result',command=cmd,exit=r.returncode,semantic=semantic))
    (a.out/'results.json').write_text(json.dumps(records,indent=2)+'\n')
    print('PASS',len(records),'key-built spacing cases')

if __name__=='__main__':main()
