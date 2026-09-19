#!/usr/bin/env python3
"""Seeded periodic theorem corpus. Uses production independent C++ replay.

No root enumeration: family counts, exact units, binders and resource ceilings
are inspected in addition to theorem replay. Challenge inputs remain separate.
"""
from pathlib import Path
from fractions import Fraction
import argparse, json, os, random, subprocess

def families(seed):
    r=random.Random(seed)
    for i in range(180):
        function=['sin','cos','tan'][i%3]
        a=Fraction(r.choice([-7,-3,-2,-1,1,2,3,5]),r.randint(1,3))
        b=Fraction(r.randint(-7,7),r.randint(1,4))
        c=r.choice(['-2','-1','-1/2','0','1/3','1/2','1','2','2/5'])
        lhs=f'{function}(({a})*x+({b}))';rhs=f'({c})'
        if i%4==0:lhs,rhs=rhs,lhs
        yield dict(id=f'seed-{i}',equation=lhs+'='+rhs,degrees=bool(i%2),function=function,target=c)

def main():
    p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True)
    p.add_argument('--seed',type=int,default=20260917);p.add_argument('--challenge',type=Path);a=p.parse_args()
    a.out.mkdir(parents=True,exist_ok=True);env=dict(os.environ);env['PATH']='C:/mingw64/bin;'+env.get('PATH','')
    cases=json.loads(a.challenge.read_text())['cases'] if a.challenge else list(families(a.seed));records=[]
    for c in cases:
        cmd=[a.bin,c['equation']]+(['--degrees'] if c.get('degrees') else [])+(['--complex'] if c.get('complex') else [])
        run=subprocess.run(cmd,capture_output=True,env=env,timeout=30);record=dict(case=c,exit=run.returncode)
        (a.out/(c['id']+'.stderr')).write_bytes(run.stderr)
        try:
            d=json.loads(run.stdout);record['trace']=d;assert run.returncode==0
            if c.get('unsupported'):assert d['status']==0,d['diagnostic']
            else:
                assert d['status']==2,d['diagnostic']
                assert d['validity']==d['completeness']==d['candidates']==1
                assert d['reconciliation']!=2,'ordinary representative outside proven family'
                final=d['states'][-1];target=Fraction(c['target']);function=c['function']
                empty=function!='tan' and abs(target)>1
                expected=0 if empty else 1 if function=='tan' or abs(target)==1 else 2
                assert final['conclusion']==(2 if empty else 5)
                assert len(final['families'])==expected
                for family in final['families']:
                    assert family['lhs']==family['variable']=='x'
                    assert family['degrees']==bool(c.get('degrees'))
                    assert family['binderScope']==d['snapshot'] and family['binderId']==1 and family['domain']==0
                    assert family['originalCheck']==1 and family['period']!='0'
                    assert 'k' not in family['offset']+family['period'],'unbound parser identifier'
                    assert not family['period'].startswith('-'),'negative period not normalized'
                assert d['calls']<=4096 and d['bytes']<=65536 and d['peakVectorHeapBytes']<=131072
                assert len(d['steps'])<=48 and all(s['verdict']==1 for s in d['steps'])
                assert not any(s['rule']=='family.divide' and s['operand']=='1' for s in d['steps'])
                if len(records)%19==0:
                    other=json.loads(subprocess.check_output(cmd,env=env))
                    for key in ['states','steps','snapshot','calls','validity','completeness']:assert d[key]==other[key]
        except Exception as error:record['failure']=str(error);print(c['id'],str(error),flush=True)
        records.append(record)
    (a.out/'replay.jsonl').write_text(''.join(json.dumps(x)+'\n' for x in records))
    bad=[x for x in records if 'failure' in x];(a.out/'summary.json').write_text(json.dumps(dict(cases=len(records),failures=bad),indent=2))
    print(len(records),'cases;',len(bad),'failures');return bool(bad)
if __name__=='__main__':raise SystemExit(main())
