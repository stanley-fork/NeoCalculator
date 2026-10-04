#!/usr/bin/env python3
"""Strictly replay the preserved 900 mathematical traces. No golden promotion."""
import argparse,concurrent.futures,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--baseline',type=Path,required=True);p.add_argument('--bin-dir',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'])
def normalize(value):
    if isinstance(value,dict):return {k:normalize(v) for k,v in value.items() if k not in ('micros','warmSolveMicros')}
    if isinstance(value,list):return [normalize(v) for v in value]
    return value
jobs=[]
for folder in a.baseline.iterdir():
    path=folder/'replay.jsonl'
    if path.is_file():
        jobs.extend((folder.name,i,json.loads(line)) for i,line in enumerate(path.read_text(encoding='utf8').splitlines()))
def run(job):
    suite,index,row=job
    if suite.startswith('composition-'):
        before=row['result'];binary=Path(row['command'][0]).name;command=[str(a.bin_dir/binary),*row['command'][1:]]
        assert binary=='periodic_result_main.exe'
    else:
        before=row['trace'];binary='tutor_engine_main.exe';command=[str(a.bin_dir/binary),*['='.join(pair) for pair in before['authored']]]
        if before['complex']:command+=['--complex']
        if before['degrees']:command+=['--degrees']
    r=subprocess.run(command,env=env,capture_output=True,timeout=90)
    path=a.out/suite;path.mkdir(exist_ok=True);(path/f'{index:03}.log').write_bytes(r.stdout+r.stderr)
    record={'suite':suite,'index':index,'command':command,'exit':r.returncode,'passed':False}
    try:
        after=json.loads(next(line for line in r.stdout.decode('utf8').splitlines() if line.startswith('{')))
        assert r.returncode==0
        assert normalize(before)==normalize(after)
        record['passed']=True
    except (AssertionError,ValueError,StopIteration) as error:record['error']=repr(error)
    return record
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:results=list(executor.map(run,jobs))
report={'cases':len(results),'passed':all(r['passed'] for r in results),'ignoredFields':['micros','warmSolveMicros'],'results':results}
(a.out/'comparison.json').write_text(json.dumps(report,indent=2))
assert len(results)==900 and report['passed'],[r for r in results if not r['passed']][:3]
print('PASS 900 preserved traces: all non-timing mathematical and resource fields identical')
