#!/usr/bin/env python3
"""Independent mathematical detectors over the real-key observer corpus.

The preserved pre-repair corpus must fail all eleven recovery detectors;
running a recorder successfully is not a mathematical pass.
"""
import argparse, json, math, re
from pathlib import Path

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--candidate',type=Path,required=True)
p.add_argument('--baseline',type=Path,required=True)
a=p.parse_args()
def load(folder,prefix):
 records=[]
 for i in range(1,5):records+=json.loads((folder/(prefix+str(i))/'results.json').read_text())
 assert len(records)==67 and all(r['exit']==0 and r['completed'] for r in records)
 return {r['fixture']['id']:r for r in records}
old=load(a.baseline,'results-final-r');new=load(a.candidate,'results-final-')
expected={'factorial-five':'120','factorial-zero':'1','factorial-sum':'8',
 'ans-complex':'-1','rational-power':'3','symbol-collect':'5*x',
 'trig-identity-numeric':'1','trig-identity-symbolic':'1',
 'log-base-one-control':None,'log-base-one-half':None,'log-base-zero-two':None}
def correct(record,value):
 return int(record['status'][-1])==1 if value is None else int(record['status'][-1])==0 and record['exact'][-1].strip()==value
detectors=[]
for name,value in expected.items():
 before=correct(old[name],value);after=correct(new[name],value)
 assert not before and after,(name,before,after)
 detectors.append(dict(id=name,expected=value,baselinePass=before,candidatePass=after))
numeric=[]
for name,record in new.items():
 fixture=record['fixture'];expectedNumber=fixture.get('tailNumber',fixture.get('number'))
 if expectedNumber is None:continue
 raw=record['approximate'][-1].strip()
 if not re.fullmatch(r'[+-]?\d+(?:\.\d*)?(?:e[+-]?\d+)?',raw,re.I):
  raw=record['exact'][-1].strip()
  assert re.fullmatch(r'-?\d+(?:/\d+)?',raw),(name,raw)
  numerator,_,denominator=raw.partition('/');actual=int(numerator)/int(denominator or 1)
 else:actual=float(raw)
 assert math.isclose(actual,expectedNumber,rel_tol=5e-12,abs_tol=1e-18),(name,actual,expectedNumber)
 numeric.append(dict(id=name,expected=expectedNumber,actual=actual))
assert new['symbol-quotient']['exact'][-1].strip()=='(x^2-1)/(x-1)','lost domain hole'
assert new['ans-symbolic']['exact'][-1].strip()=='x+1','lost symbolic Ans'
result=dict(corpus=67,detectors=detectors,numericControls=numeric,domainPreserved=True)
(a.candidate/'result-detectors.json').write_text(json.dumps(result,indent=2))
print('PASS 11 baseline-failing detectors;',len(numeric),'independent numeric controls; symbolic domain and Ans')
