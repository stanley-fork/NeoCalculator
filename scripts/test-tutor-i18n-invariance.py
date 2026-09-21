#!/usr/bin/env python3
"""Compare all accepted 03A proof fields with one EN/ES/EN rendering of each graph."""
from pathlib import Path
import argparse,copy,json,subprocess,os,concurrent.futures
p=argparse.ArgumentParser();p.add_argument('--bin',type=Path,required=True);p.add_argument('--baseline',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,PATH='C:/mingw64/bin;'+os.environ['PATH']);jobs=[]
for folder in ['historical','02a-seeded','02a-challenge','02b-seeded','02b-challenge','02c-seeded','02c-challenge']:
 for i,line in enumerate((a.baseline/folder/'replay.jsonl').read_text().splitlines()):jobs.append((folder,i,json.loads(line)))
changes=[]
allowed={'trig.principal':'Find the principal inverse angle in the standard real inverse range.','log.base_inverse':'Use each side as the exponent of the valid base. This inverts the logarithm on its positive domain.'}
def run(job):
 folder,i,row=job;old=row['trace'];args=[str(a.bin),*['='.join(e) for e in old['authored']]]
 if old['complex']:args+=['--complex']
 if old['degrees']:args+=['--degrees']
 for name,value in old.get('contextValues',[]):
  if name!=value and len(name)==1 and name in 'ABCDEF':args+=['--set',name+':='+value]
 r=subprocess.run(args,env=env,capture_output=True,timeout=90);target=a.out/folder;target.mkdir(exist_ok=True);(target/f'{i:03}.log').write_bytes(r.stdout+r.stderr)
 assert r.returncode==0,(folder,i,r.returncode)
 data=json.loads(next(x for x in r.stdout.decode().splitlines() if x.startswith('{')))
 assert data['en']==data['enAgain'],(folder,i,'EN roundtrip')
 en=copy.deepcopy(data['en']);es=copy.deepcopy(data['es']);texts=[]
 for x,y in zip(en['steps'],es['steps']):
  texts.append(dict(key=x['key'],en=x.pop('text'),es=y.pop('text')))
 assert en==es,(folder,i,'mathematics changed by locale')
 expected=copy.deepcopy(old);current=copy.deepcopy(data['en']);editorial=[]
 # Exactly one nonpresentation measurement: elapsed wall time from a new solve.
 expected.pop('micros');current.pop('micros')
 for x,y in zip(expected['steps'],current['steps']):
  if x['text']!=y['text']:
   assert y['key'] in allowed and y['text']==allowed[y['key']],(folder,i,'unexpected English change',y['key'])
   editorial.append(y['key']);x['text']=y['text']
 assert expected==current,(folder,i,[k for k in expected if expected.get(k)!=current.get(k)])
 return dict(folder=folder,index=i,id=row.get('id',row.get('case',{}).get('id')),reconciliation=old['reconciliation'],editorial=editorial,texts=texts)
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:results=list(executor.map(run,jobs))
(a.out/'results.json').write_text(json.dumps(dict(cases=len(results),excludedPresentationFields=['steps[*].text'],separatelyMeasured=['micros'],allOtherFieldsIdentical=True,records=results),ensure_ascii=False,indent=2),encoding='utf-8')
print(len(results),'exact EN/ES proof comparisons PASS; baseline 03A reconciliation unchanged')
