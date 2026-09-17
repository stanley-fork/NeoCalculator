#!/usr/bin/env python3
"""Seeded exact exp/log families; production C++ independently replays every trace."""
import argparse
from fractions import Fraction as Q
import json
from pathlib import Path
import random
import subprocess


def families(seed):
    rng = random.Random(seed)
    nonzero = lambda: rng.choice([-5, -3, -2, -1, 1, 2, 3, 5])
    for i in range(48):
        a, b, root = nonzero(), rng.randint(-5, 5), rng.randint(-4, 4)
        k, shift = Q(nonzero(), rng.randint(1, 3)), rng.randint(-4, 4)
        # Same-base injectivity has two occurrences, so outer isolation belongs
        # only to the one-occurrence families below.
        lhs, rhs = f'exp({a}*x+({b}))', f'exp({a*root+b})'
        if i % 2: lhs, rhs = rhs, lhs
        yield dict(id=f'exp-injective-{i}', equation=lhs+'='+rhs, roots=[str(root)])
        base, power = rng.choice([2, 3, 4, Q(1, 2)]), rng.randint(-4, 5)
        lhs = f'({k})*({base})^({a}*x+({b}))+({shift})'
        target = k*Q(base)**power+shift
        rhs = f'({target})'
        if i % 3 == 0: lhs, rhs = rhs, lhs
        yield dict(id=f'fixed-base-{i}', equation=lhs+'='+rhs, roots=[str(Q(power-b, a))])
        # ln(linear)=ln(positive exact rational): independently known rational root.
        target = Q(rng.randint(1, 9), rng.randint(1, 4))
        lhs, rhs = f'ln({a}*x+({b}))', f'ln({target})'
        if i % 2: lhs, rhs = rhs, lhs
        yield dict(id=f'log-injective-{i}', equation=lhs+'='+rhs, roots=[str((target-b)/a)])
        lhs, rhs = f'({k})*ln({a}*x+({b}))+({shift})', str(shift)
        if i % 3 == 0: lhs, rhs = rhs, lhs
        yield dict(id=f'log-isolate-{i}', equation=lhs+'='+rhs, roots=[str(Q(1-b, a))])
    for i in range(16):
        a, b, target = nonzero(), rng.randint(-5, 5), -rng.randint(0, 8)
        yield dict(id=f'exp-range-{i}', equation=f'exp({a}*x+({b}))={target}', roots=[])


def main():
    p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',required=True)
    p.add_argument('--seed',type=int,default=20260914);p.add_argument('--challenge',type=Path)
    args=p.parse_args();out=Path(args.out);out.mkdir(parents=True,exist_ok=True)
    corpus=json.loads(args.challenge.read_text())['cases'] if args.challenge else list(families(args.seed))
    records=[]
    for case in corpus:
        command=[args.bin,case['equation']]+(['--complex'] if case.get('complex') else [])
        run=subprocess.run(command,capture_output=True,timeout=60)
        (out/(case['id']+'.stderr')).write_bytes(run.stderr)
        record=dict(**case,exit=run.returncode)
        try:
            d=json.loads(run.stdout);record['trace']=d
            assert run.returncode==0, 'independent C++ replay failed'
            if case.get('unsupported'):
                assert d['status']==0,d['diagnostic']
            else:
                assert d['status']==2,d['diagnostic']
                assert all(d[k]==1 for k in ('validity','completeness','candidates','reconciliation'))
                final=d['states'][-1]
                roots=[b['equations'][0][1] for b in final['branches'] if b.get('status',0)==0] if final['conclusion']==1 else []
                def scalar(x):
                    try:return str(Q(x))
                    except ValueError:return x
                assert sorted(map(scalar,roots))==sorted(map(scalar,case['roots'])),(roots,case['roots'])
                assert all(b.get('originalCheck')==1 for b in final['branches'] if final['conclusion']==1 and b.get('status',0)==0)
                assert d['calls']<=4096 and d['bytes']<=65536 and d['peakVectorHeapBytes']<=131072
                assert len(d['steps'])<48 and all(s['verdict']==1 for s in d['steps'])
                assert all(len(s['branches'])==1 for s in d['states'])
                assert all(not any(token in s['text'] for token in ('exp(', 'ln(', '^', '>=', '*')) for s in d['steps'])
                assert all(s['prerequisites']==list(range(len(d['states'][s['before']]['conditions']))) for s in d['steps'])
                if len(records)%11==0:
                    replay=subprocess.run(command,capture_output=True,timeout=60);other=json.loads(replay.stdout)
                    assert replay.returncode==0
                    for field in ('states','steps','snapshot','validity','completeness','candidates','reconciliation','calls'):
                        assert d[field]==other[field],('determinism',field)
        except Exception as error:
            record['failure']=str(error);print(case['id'],case['equation'],record['failure'],flush=True)
        records.append(record)
    (out/'inputs.json').write_text(json.dumps(corpus,indent=2))
    (out/'replay.jsonl').write_text(''.join(json.dumps(r)+'\n' for r in records))
    failures=[r for r in records if 'failure' in r]
    (out/'summary.json').write_text(json.dumps(dict(seed=args.seed,cases=len(records),failures=failures),indent=2))
    print(len(records),'cases;',len(failures),'failures');return bool(failures)


if __name__=='__main__':raise SystemExit(main())
