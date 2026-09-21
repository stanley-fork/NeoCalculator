#!/usr/bin/env python3
"""Giac-origin answers, independent tutor sets and explicit fallback corpus."""
from pathlib import Path
import argparse,importlib.util,json,os,subprocess
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('trig',ROOT/'scripts/test-tutor-trig.py');trig=importlib.util.module_from_spec(spec);spec.loader.exec_module(trig)
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,PATH='C:/mingw64/bin;'+os.environ.get('PATH',''))
cases=list(trig.families(20260917))+json.loads((ROOT/'tests/fixtures/tutor-trig-challenge.json').read_text())['cases']
cases += [dict(id='stored-rational',equation='sin(2*x)=A',assign=['A:=1/2']),
          dict(id='binder-collision',equation='sin(x)=1/2',assign=['k:=19','n_0:=27']),
          *[dict(id='domain-'+str(i),equation=e,unsupported=True,assign=['A:=0']) for i,e in enumerate(['sin(x)=0*x/x','sin(x)=A*x/x','sin(x)=0/(x-1)','sin(x/x)=0','sin(x)=0/0'])]]
records=[]
for case in cases:
    cmd=[a.bin,case['equation']]+(['--degrees'] if case.get('degrees') else [])+(['--complex'] if case.get('complex') else [])+case.get('assign',[])
    r=subprocess.run(cmd,capture_output=True,env=env,timeout=30);record=dict(case=case,exit=r.returncode)
    (a.out/(case['id']+'.stderr')).write_bytes(r.stderr)
    try:
        assert r.returncode==0
        value=json.loads(r.stdout);record['result']=value
        if case.get('unsupported'):
            assert value['scope']!=1,'out-of-contract authored form claimed complete'
        else:
            assert value['status']==0 and value['scope']==value['origin']==1
            assert value['payload']<=16384 and value['calls']<=256 and len(value['families'])<=2
            t=value['tutor'];assert t['status']==2 and t['validity']==t['completeness']==t['candidates']==t['reconciliation']==1
            for f in value['families']:
                assert f['binderScope']==value['binderScope'] and f['binderId']==1 and f['domain']==0
                assert 'n_' not in f['offset']+f['period'] and 'k' not in f['offset']+f['period']
            # Each supported ordinary answer is also built with NO tutor call.
            alone=json.loads(subprocess.check_output(cmd+['--no-tutor'],env=env,timeout=30))
            assert alone['tutor'] is None
            assert {k:v for k,v in value.items() if k not in ('tutor','micros')}=={k:v for k,v in alone.items() if k not in ('tutor','micros')}
    except Exception as error:record['failure']=repr(error);print(case['id'],record['failure'],flush=True)
    records.append(record)
(a.out/'replay.jsonl').write_text(''.join(json.dumps(r)+'\n' for r in records))
bad=[r for r in records if 'failure' in r]
(a.out/'summary.json').write_text(json.dumps(dict(cases=len(records),failures=bad),indent=2));print(len(records),'cases;',len(bad),'failures')
raise SystemExit(bool(bad))
