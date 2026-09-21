#!/usr/bin/env python3
"""Compare every old proof field; allow only enumerated 03A reconciliation changes."""
from pathlib import Path
import argparse,json
p=argparse.ArgumentParser();p.add_argument('--baseline',type=Path,required=True);p.add_argument('--candidate',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
changed=[];resources=[];count=0
for folder in ['historical','02a-seeded','02a-challenge','02b-seeded','02b-challenge','02c-seeded','02c-challenge']:
    old=[json.loads(x) for x in (a.baseline/folder/'replay.jsonl').read_text().splitlines()]
    new=[json.loads(x) for x in (a.candidate/folder/'replay.jsonl').read_text().splitlines()]
    assert len(old)==len(new)
    for before,after in zip(old,new):
        identity=before.get('id',before.get('case',{}).get('id'))
        assert identity==after.get('id',after.get('case',{}).get('id'))
        x,y=before['trace'].copy(),after['trace'].copy();count+=1
        # Timing and measured container payload are checked separately, never
        # used to mask a state, condition, step, parameter or proof fingerprint.
        for key in ['micros','vectorHeapBytes','peakVectorHeapBytes','bytes']:
            if x.get(key)!=y.get(key):resources.append(dict(folder=folder,id=identity,field=key,before=x.get(key),after=y.get(key)))
            x.pop(key,None);y.pop(key,None)
        if x['reconciliation']!=y['reconciliation']:
            assert x['reconciliation']==0 and y['reconciliation']==1
            assert x['states'][-1]['conclusion']==5
            changed.append(dict(folder=folder,id=identity,field='reconciliation',before=0,after=1))
            x['reconciliation']=1
        if x['calls']!=y['calls']:
            assert x['states'][-1]['conclusion']==5
            resources.append(dict(folder=folder,id=identity,field='calls',before=x['calls'],after=y['calls']))
            x['calls']=y['calls']
        assert x==y,(folder,identity,[k for k in x if x.get(k)!=y.get(k)])
a.out.parent.mkdir(parents=True,exist_ok=True)
a.out.write_text(json.dumps(dict(cases=count,proofFieldsIdentical=True,reconciliationChanges=changed,resourceFields=resources),indent=2))
print(count,'proofs unchanged;',len(changed),'enumerated reconciliation upgrades')
