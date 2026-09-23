#!/usr/bin/env python3
"""Strict replay of historical proof fields; enumerate resource-only changes."""
from pathlib import Path
import argparse,copy,json,os,subprocess,concurrent.futures
p=argparse.ArgumentParser();p.add_argument('--bin',required=True);p.add_argument('--baseline',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH']);jobs=[]
for folder in ['historical','02a-seeded','02a-challenge','02b-seeded','02b-challenge','02c-seeded','02c-challenge']:
    for i,line in enumerate((a.baseline/folder/'replay.jsonl').read_text().splitlines()):jobs.append((folder,i,json.loads(line)))
def strip_text(trace):
    result=copy.deepcopy(trace)
    for step in result['steps']:step.pop('text')
    for child in result.get('composition',{}).get('children',[]):
        for step in child['steps']:step.pop('text')
    return result
def run(job):
    folder,index,row=job;old=row['trace'];command=[a.bin,*['='.join(e) for e in old['authored']]]
    if old['complex']:command+=['--complex']
    if old['degrees']:command+=['--degrees']
    r=subprocess.run(command,env=env,capture_output=True,timeout=90);dest=a.out/folder;dest.mkdir(exist_ok=True);(dest/f'{index:03}.log').write_bytes(r.stdout+r.stderr)
    result=dict(folder=folder,index=index,id=row.get('id',row.get('case',{}).get('id')),exit=r.returncode)
    try:
        assert r.returncode==0
        data=json.loads(next(x for x in r.stdout.decode().splitlines() if x.startswith('{')))
        assert data['en']==data['enAgain'];assert strip_text(data['en'])==strip_text(data['es'])
        current=copy.deepcopy(data['en']);before=copy.deepcopy(old);changes={}
        for key in ['bytes','vectorHeapBytes','peakVectorHeapBytes','micros','calls']:
            if current[key]!=before[key]:changes[key]=[before[key],current[key]]
            current.pop(key);before.pop(key)
        # Unsupported inputs can now reach the composition admission boundary.
        # Enumerate that developer diagnostic change; retain status, every
        # equation, condition, step and verdict in the strict comparison.
        if current['status']==before['status']==0 and current['diagnostic']!=before['diagnostic']:
            result['unsupportedDiagnosticChange']=[before['diagnostic'],current['diagnostic']]
            current['diagnostic']=before['diagnostic']
        # Retain complete mathematical comparisons including every condition,
        # family, binder, status, reconciliation and explanatory parameter.
        if current!=before:
            result['differences']=[k for k in before if before.get(k)!=current.get(k)]
            result['before']=before;result['after']=current
            raise AssertionError(result['differences'])
        result['resourceChanges']=changes;result['periodicEquivalent']=current['reconciliation']==1 and current['states'] and current['states'][-1]['conclusion']==5
    except Exception as error:result['failure']=repr(error)
    return result
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:results=list(pool.map(run,jobs))
failed=[r for r in results if 'failure' in r];report=dict(cases=len(results),failed=len(failed),excludedPresentationFields=['steps[*].text','composition.children[*].steps[*].text'],measuredSeparately=['bytes','vectorHeapBytes','peakVectorHeapBytes','micros','calls'],records=results)
(a.out/'results.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
print(len(results),'historical EN/ES comparisons;',len(failed),'differences');raise SystemExit(bool(failed))
