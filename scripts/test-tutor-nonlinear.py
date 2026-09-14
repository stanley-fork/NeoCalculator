#!/usr/bin/env python3
"""Seeded theorem families; expected finite sets use independent rational arithmetic.

Runs production C++ generation AND independent replay in tutor_engine_main.
No symbolic spot-check claim: exact checker evidence is retained in replay JSON.
"""
import argparse
from fractions import Fraction as Q
import json
from pathlib import Path
import random
import subprocess


def families(seed):
    rng = random.Random(seed)
    nonzero = lambda: rng.choice([-5, -3, -2, -1, 1, 2, 3, 5])
    for i in range(60):
        a, b, c, d = nonzero(), rng.randint(-9, 9), rng.randint(-4, 4), rng.randint(-7, 7)
        if a in (c, -c):
            c = 0
        roots = {Q(d-b, a-c), Q(-d-b, a+c)}
        roots = sorted(x for x in roots if c*x+d >= 0)
        k, shift = Q(nonzero(), rng.randint(1, 4)), rng.randint(-6, 6)
        lhs, rhs = f'({k})*abs({a}*x+({b}))+({shift})', f'({k})*({c}*x+({d}))+({shift})'
        if i % 3 == 0:
            lhs, rhs = rhs, lhs
        yield f'abs-linear-{i}', f'{lhs}={rhs}', roots
    for i in range(20):
        a, b = rng.randint(0, 5), rng.randint(1, 8)
        if a > b:
            a, b = b, a
        middle, rhs = Q(a*a+b*b, 2), Q(b*b-a*a, 2)
        yield f'abs-quadratic-{i}', f'abs(x^2-({middle}))={rhs}', sorted({Q(-a), Q(a), Q(-b), Q(b)})
    for i in range(60):
        r, s, c, d = rng.randint(-6, 6), rng.randint(-6, 6), nonzero(), rng.randint(-6, 6)
        a, b = 2*c*d+c*c*(r+s), d*d-c*c*r*s
        if a == 0:
            d += 1
            a, b = 2*c*d+c*c*(r+s), d*d-c*c*r*s
        k, shift = Q(nonzero(), rng.randint(1, 4)), rng.randint(-4, 4)
        roots = sorted(Q(x) for x in {r, s} if c*x+d >= 0)
        lhs, rhs = f'({k})*sqrt({a}*x+({b}))+({shift})', f'({k})*({c}*x+({d}))+({shift})'
        if i % 3 == 0:
            lhs, rhs = rhs, lhs
        yield f'radical-linear-{i}', f'{lhs}={rhs}', roots


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--bin', required=True)
    p.add_argument('--out', required=True)
    p.add_argument('--seed', type=int, default=20260913)
    p.add_argument('--challenge', type=Path)
    args = p.parse_args()
    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    results, failures = [], []
    corpus = families(args.seed)
    if args.challenge:
        corpus = ((c['id'], c['equation'], c['roots']) for c in json.loads(args.challenge.read_text())['cases'])
    def scalar(value):
        try:
            return str(Q(value))
        except ValueError:
            return str(value)  # exact algebraic canonical spelling, no new parser
    for name, equation, expected in corpus:
        run = subprocess.run([args.bin, equation], capture_output=True, timeout=60)
        (out/(name+'.stderr')).write_bytes(run.stderr)
        record = dict(id=name, equation=equation, expected=list(map(str, expected)), exit=run.returncode)
        try:
            d = json.loads(run.stdout)
            record['trace'] = d
            assert run.returncode == 0, 'independent C++ replay'
            assert d['status'] == 2, d['diagnostic']
            assert all(d[k] == 1 for k in ('validity', 'completeness', 'candidates', 'reconciliation'))
            final = d['states'][-1]
            roots = sorted(scalar(b['equations'][0][1]) for b in final['branches'] if b.get('status', 0) == 0) if final['conclusion'] == 1 else []
            assert roots == sorted(map(scalar, expected)), (roots, expected)
            assert len(roots) == len(set(roots)), 'duplicate final roots'
            if final['conclusion'] == 1:
                assert all(b.get('originalCheck') == 1 for b in final['branches'] if b.get('status', 0) == 0)
            assert final['conclusion'] == (1 if roots else 2)
            assert max(len(s['branches']) for s in d['states']) <= 4
            assert d['bytes'] <= 65536
        except Exception as error:
            record['failure'] = str(error)
            failures.append(record)
            print(name, equation, record['failure'], flush=True)
        results.append(record)
    (out/'replay.jsonl').write_text(''.join(json.dumps(r)+'\n' for r in results), encoding='utf-8')
    (out/'summary.json').write_text(json.dumps(dict(seed=args.seed, cases=len(results), failures=failures), indent=2))
    print(f'{len(results)} cases; {len(failures)} failures', flush=True)
    return bool(failures)


if __name__ == '__main__':
    raise SystemExit(main())
