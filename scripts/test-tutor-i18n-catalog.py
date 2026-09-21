#!/usr/bin/env python3
"""Validate and export the real C++ tutor catalog; never maintain a second catalog."""
from pathlib import Path
import argparse,json,re
ROOT=Path(__file__).resolve().parents[1]
STRING=r'"(?:\\.|[^"\\])*"'
FIELD=r'(?:'+STRING+r'\s*)+|nullptr'
ENTRY=re.compile(r'\{\s*('+FIELD+r')\s*,\s*('+FIELD+r')\s*,\s*('+FIELD+r')\s*,\s*('+FIELD+r')\s*,\s*('+FIELD+r')\s*\}')
def entries(source):
    body=source.split('constexpr CatalogEntry catalog[] = {',1)[1].split('\n};',1)[0]
    def value(s):return None if s=='nullptr' else ''.join(json.loads(x) for x in re.findall(STRING,s))
    return [list(map(value,m.groups())) for m in ENTRY.finditer(body)]
def validate(rows):
    assert rows and len({r[0] for r in rows})==len(rows)
    for key,en,schema,es,fr in rows:
        assert en and es,(key,'missing English/Spanish')
        assert all(c in 'evir' for c in schema),(key,schema)
        for language,text in [('en',en),('es',es),('fr',fr)]:
            if text is None:continue
            assert text.strip(),(key,'empty')
            params=re.findall(r'\{([^}]+)\}',text)
            assert sorted(params)==list(map(str,range(len(schema)))),(key,language,params)
            assert not re.search(r'[{}]',re.sub(r'\{\d\}','',text)),key
            assert '[invalid' not in text and '\ufffd' not in text and not re.search(r'[A-Za-z]\?[A-Za-z]',text),key
    return len(rows)
def main():
    p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--review-file',type=Path);a=p.parse_args()
    source=ROOT/'src/math/tutor/Messages.inc';rows=entries(source.read_text(encoding='utf-8'));n=validate(rows)
    enum=(ROOT/'src/math/tutor/Derivation.h').read_text().split('enum class Message',1)[1].split('\n    Count',1)[0].split('{',1)[1]
    names=[x.strip() for x in enum.split(',') if x.strip()];assert len(names)==n,(len(names),n)
    negative=[r.copy() for r in rows];negative[0][3]=None
    try:validate(negative)
    except AssertionError:negativeDetected=True
    else:raise AssertionError('missing Spanish mutation escaped')
    refs={}
    for path in [*ROOT.glob('src/apps/Tutor*.*'),ROOT/'src/apps/EquationsApp.cpp',ROOT/'src/apps/SettingsApp.cpp',ROOT/'src/math/tutor/TeachingPlan.h',*ROOT.glob('src/math/giac/GiacTutor*.inc')]:
        for line,text in enumerate(path.read_text(encoding='utf-8').splitlines(),1):
            for name in re.findall(r'Message::(\w+)',text):refs.setdefault(name,set()).add(str(path.relative_to(ROOT))+':'+str(line))
    records=[]
    for name,row in zip(names,rows):
        key,en,schema,es,fr=row
        records.append(dict(key=key,enum=name,source='src/math/tutor/Messages.inc',parameters=[{'e':'expression','v':'variable','i':'integer','r':'equation row'}[c] for c in schema],context=sorted(refs.get(name,{'checked rule/message API'})),english=en,spanish=es,french=fr))
    a.out.mkdir(parents=True,exist_ok=True)
    (a.out/'catalog.json').write_text(json.dumps(records,ensure_ascii=False,indent=2),encoding='utf-8')
    escape=lambda s:s.replace('|','\\|').replace('\n','<br>')
    md='# Tutor EN / ES — catálogo generado\n\nFuente: `src/math/tutor/Messages.inc`. No editar esta tabla manualmente.\n\nClave | Contexto y parámetros | English | Español\n--- | --- | --- | ---\n'
    for r in records:md+=' | '.join(escape(x) for x in [r['key'],'; '.join(r['context'])+'; '+','.join(r['parameters']),r['english'],r['spanish']])+'\n'
    (a.out/'catalog.md').write_text(md,encoding='utf-8')
    if a.review_file:a.review_file.write_text(md,encoding='utf-8')
    (a.out/'coverage.json').write_text(json.dumps(dict(keys=n,english=n,spanish=n,missing=0,negativeControlDetected=negativeDetected),indent=2))
    print(n,'EN/ES entries, zero missing; isolated deletion detected')
if __name__=='__main__':main()
