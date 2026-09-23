#!/usr/bin/env python3
"""Seeded composition proofs and independent ordinary Giac results, no sampling proof."""
from pathlib import Path
from fractions import Fraction
import argparse,json,random,subprocess,os

def seeded():
    rng=random.Random(20260922)
    for i in range(96):
        kind=['square','exp','log','sin','cos','tan'][i%6]
        slope=rng.choice([-2,-1,1,2]);offset=rng.randint(-2,2)
        inner=f'({slope}*x+({offset}))'
        atom={'square':f'{inner}^2','exp':f'exp({inner})','log':f'ln({inner})'}.get(kind,f'{kind}({inner})')
        choices={'square':[-1,0,1,4],'exp':[-1,0,1,2,3],'log':[-2,-1,0,1,2],
                 'sin':['-1','-1/2','0','1/3','1/2','1'],'cos':['-1','-1/2','0','1/3','1/2','1'],'tan':['-1','0','1','2']}[kind]
        first,second=rng.choice(choices),rng.choice(choices)
        lhs=f'({atom}-({first}))*({atom}-({second}))';rhs='0'
        if i%3==0:lhs=f'({lhs})/2'
        if i%4==0:lhs,rhs=rhs,lhs
        values=set(map(Fraction,[str(first),str(second)]));count=0
        for v in values:
            count+= (0 if v<0 else 1 if v==0 else 2) if kind=='square' else (int(v>0) if kind=='exp' else 1 if kind in ['log','tan'] else 1 if abs(v)==1 else 2)
        yield dict(id=f'seed-{i:03}',family=kind,equation=lhs+'='+rhs,degrees=kind in ['sin','cos','tan'] and bool((i//6)%2),periodic=kind in ['sin','cos','tan'],count=count)

def main():
    p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('--challenge',type=Path);a=p.parse_args()
    a.out.mkdir(parents=True,exist_ok=True);env=dict(os.environ);env['PATH']='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+env.get('PATH','')
    cases=json.loads(a.challenge.read_text(encoding='utf-8')) if a.challenge else list(seeded());records=[]
    for c in cases:
        command=[a.bin,c['equation']]+(['--degrees'] if c.get('degrees') else [])+(['--complex'] if c.get('complex') else [])+c.get('assign',[])
        r=subprocess.run(command,env=env,capture_output=True,timeout=60);(a.out/(c['id']+'.stderr')).write_bytes(r.stderr)
        record={'case':c,'command':command,'exit':r.returncode}
        try:
            d=json.loads(r.stdout);record['result']=d;t=d['tutor'];assert r.returncode==0
            if c.get('unsupported'):assert t['status']==0,t['diagnostic']
            else:
                assert t['status']==2,t['diagnostic'];assert t['validity']==t['completeness']==t['candidates']==1
                assert t['reconciliation']!=2
                end=t['states'][-1];periodic=end['conclusion']==5
                count=len(end.get('families',[])) if periodic else sum(not b['rejected'] for b in end['branches']) if end['conclusion']==1 else 0
                assert count==c['count'],(count,c['count'])
                if periodic:
                    assert d['origin']==1 and d['scope']==1 and len(d['families'])<=4,'ordinary complete families missing'
                    assert t['reconciliation']==1,'set equality not proved'
                def checked(trace):
                    assert all(s['verdict']==1 for s in trace['steps']);total=len(trace['steps'])
                    for child in trace.get('composition',{}).get('children',[]):total+=checked(child)
                    return total
                assert checked(t)<=48 and t['calls']<=4096 and t['bytes']<=65536 and t['peakVectorHeapBytes']<=131072
                independent=json.loads(subprocess.check_output(command+['--no-tutor'],env=env))
                for key in ['status','kind','scope','origin','degrees','raw','families','restrictions']:assert d[key]==independent[key],('ordinary dependence',key)
                record['ordinaryWithoutTutorIdentical']=True
        except Exception as error:record['failure']=repr(error);print(c['id'],record['failure'],flush=True)
        records.append(record)
        (a.out/(c['id']+'.json')).write_bytes(r.stdout)
    failed=[r for r in records if 'failure' in r];(a.out/'replay.jsonl').write_text(''.join(json.dumps(r,ensure_ascii=False)+'\n' for r in records),encoding='utf-8')
    (a.out/'summary.json').write_text(json.dumps(dict(cases=len(records),failed=len(failed),failures=failed),ensure_ascii=False,indent=2),encoding='utf-8')
    print(len(records),'composition cases;',len(failed),'failed');return bool(failed)
if __name__=='__main__':raise SystemExit(main())
