#!/usr/bin/env python3
"""Row factory boundary faults on the private Toolbox overlay, never firmware."""
import argparse,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
root=Path(__file__).resolve().parents[1];results=[]
for row in range(1,5):
    script=a.out/f'row-{row}.numos'
    script.write_text('wait 200\nopen_app Calculation\nwait 30\nkey 2\nkey POW\nkey 3\nkey TOOLBOX\nassert_calc_input toolbox closed\nassert_calc_input serialized ((2)^(3))\nassert_calc_input structure\nkey ENTER\nassert_calc_exact 8\nkey HOME\nwait 20\nassert_app Menu\ncalculus_probe\nlog ROW_FAULT_DONE\n')
    env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'],NUMOS_TOOLBOX_FAIL_ROW=str(row))
    r=subprocess.run([a.bin,'--headless','--quiet','--deterministic','--frames','400','--script',os.path.relpath(script,root)],cwd=root,env=env,capture_output=True,timeout=60)
    (a.out/f'row-{row}.log').write_bytes(r.stdout+r.stderr)
    results.append({'row':row,'passed':r.returncode==0 and b'ROW_FAULT_DONE' in r.stdout,'exit':r.returncode})
(a.out/'results.json').write_text(json.dumps(results,indent=2));assert all(r['passed'] for r in results),results
print('PASS four row creation failures: prior AST retained, partial modal destroyed, ordinary calculation and HOME recovered')
