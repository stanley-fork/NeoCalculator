#!/usr/bin/env python3
"""Original 320x240 + integer nearest-neighbour review ZIP, never goldens/fonts."""
import argparse,html,json,re,zipfile,hashlib
from pathlib import Path
from PIL import Image,ImageDraw,ImageChops
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--work',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
p.add_argument('--baseline-phase',choices=['input-repair','math-spacing'],default='input-repair')
a=p.parse_args()
baseline_title='Baseline · reparación de entrada / input repair' if a.baseline_phase=='input-repair' else 'Antes / Before · MATH-TEX-01 (U+002D)'
out=a.out/'visual';out.mkdir(parents=True,exist_ok=True);assets=out/'assets';assets.mkdir(exist_ok=True)
fixtures=json.loads((ROOT/'tests/fixtures/math-spacing.json').read_text());data=[];sections=[];placeholder_checks=[]
def events(path):return [json.loads(l) for l in path.read_text(encoding='utf-8').splitlines() if l.startswith('{')]
def first(es):
 start=next(i for i,e in enumerate(es) if e['event']=='frame' and e['index']==0)
 end=next((i for i in range(start+1,len(es)) if es[i]['event']=='frame'),len(es))
 return es[start:end]
def save(im,name):
 assert im.size==(320,240),(name,im.size)
 im.save(assets/(name+'.png'));im.resize((1280,960),Image.Resampling.NEAREST).save(assets/(name+'-4x.png'))
def panel(name,title):
 return f'<figure><figcaption>{title}</figcaption><a href="assets/{name}-4x.png"><img src="assets/{name}.png" width="320" height="240" loading="lazy" alt="{html.escape(title)}"></a><p><a href="assets/{name}.png">320×240</a> · <a href="assets/{name}-4x.png">4× nearest-neighbor</a></p></figure>'
for f in fixtures:
 ident=f['id'];old=events(a.work/'baseline-probe'/(ident+'.log'));new=events(a.work/'candidate-probe'/(ident+'.log'));frame=first(new)
 nodes=[n for n in frame if n['event']=='node'];root=next(n for n in nodes if n['path']=='r')
 semantic=next(e for e in new if e['event']=='semantic');engine=next(e for e in new if e['event']=='engineBefore')
 oldRoot=next(n for n in first(old) if n['event']=='node' and n['path']=='r')
 row=dict(id=ident,keys=f['keys'],latex=f['latex'],semantic=semantic,engine=engine,baselineSize={k:oldRoot[k] for k in ['width','ascent','descent']},candidateSize={k:root[k] for k in ['width','ascent','descent']},geometry=frame)
 row['treeBefore']=next(e['tree'] for e in new if e['event']=='treeBefore')
 row['treeAfter']=next(e['tree'] for e in new if e['event']=='treeAfter')
 data.append(row);panels=[]
 for label,title,path in [('baseline',baseline_title,a.work/'baseline-probe'/(ident+'.ppm-formula.ppm')),('new','Nuevo / New · C++ LVGL',a.work/'candidate-probe'/(ident+'.ppm-formula.ppm')),('reference','Referencia / Reference · KaTeX 0.16.22',a.work/'reference'/(ident+'.png')),('cursor','Editor + cursor / Editable cursor',a.work/'candidate-probe'/(ident+'.ppm'))]:
  name=ident+'-'+label;save(Image.open(path).convert('RGB'),name);panels.append(panel(name,title))
 im=Image.open(a.work/'candidate-probe'/(ident+'.ppm')).convert('RGBA');layer=Image.new('RGBA',im.size);d=ImageDraw.Draw(layer)
 for n in nodes:
  x,y,w=n['x'],n['baseline'],n['width'];top=y-n['ascent'];bottom=y+n['descent']-1
  if w>0 and bottom>=top:d.rectangle((x,top,x+w-1,bottom),outline=(0,120,200,130))
  d.line((x,y,x+w,y),fill=(0,140,70,160));d.line((x+w,top,x+w,bottom),fill=(230,115,0,180))
  gap=n.get('spaceBefore',0)
  if gap and n['type']!=0:d.rectangle((x-gap,y-3,x-1,y+2),fill=(255,190,0,115))
  if n['scriptReserve']:d.rectangle((x+w-n['scriptReserve'],top,x+w-1,bottom),fill=(135,0,240,90))
 for g in frame:
  if g['event']=='glyph' and g['inkW']>0 and g['inkH']>0:d.rectangle((g['inkX'],g['inkY'],g['inkX']+g['inkW']-1,g['inkY']+g['inkH']-1),outline=(190,0,180,150))
 cursor=next((e for e in reversed(frame) if e['event']=='cursor'),None)
 if cursor:
  x=cursor['origin']+cursor['offset'];y=cursor['baseline'];d.ellipse((x-2,y-2,x+2,y+2),fill=(240,0,0,255))
 name=ident+'-overlay';save(Image.alpha_composite(im,layer).convert('RGB'),name);panels.append(panel(name,'Geometría / Geometry · host only'))
 # Check actual placeholder borders with the cursor hidden, never just nonzero ink.
 pixels=Image.open(a.work/'candidate-probe'/(ident+'.ppm-formula.ppm')).convert('RGB')
 for n in nodes:
  if n['type']!=3 or (n['path'].count('/')==1 and root['children']==1):continue
  x,y,w,h=n['x'],n['baseline']-n['ascent'],n['width'],n['ascent']+n['descent']
  edge={(x+i,y) for i in range(w)}|{(x+i,y+h-1) for i in range(w)}|{(x,y+j) for j in range(h)}|{(x+w-1,y+j) for j in range(h)}
  visible=[q for q in edge if 0<=q[0]<320 and 0<=q[1]<240];gray=sum(pixels.getpixel(q)==(131,129,131) or pixels.getpixel(q)==(132,130,132) for q in visible)
  placeholder_checks.append(dict(id=ident,path=n['path'],edgePixels=len(visible),grayPixels=gray));assert visible and gray==len(visible),(ident,n,gray,len(visible))
 for phase,title in [('editable','Aplicación editable / App editable'),('evaluated','Después de EXE / After EXE')]:
  for source,label in [('baseline','Baseline'),('candidate','Nuevo / New')]:
   path=a.out/(source+'-spacing-native')/(ident+'-'+phase+'.ppm')
   if path.exists():name=ident+'-'+source+'-'+phase;save(Image.open(path).convert('RGB'),name);panels.append(panel(name,label+' · '+title))
 if ident.startswith('user-'):
  for app in ['Equations','Calculus','Grapher']:
   for phase in ['editable','evaluated','committed']:
    for source,label in [('baseline','Baseline'),('candidate','Nuevo / New')]:
     path=a.work/(source+'-app-spacing')/(ident+'-'+app+'-'+phase+'.ppm')
     if path.exists():
      name=ident+'-'+source+'-'+app+'-'+phase;save(Image.open(path).convert('RGB'),name)
      panels.append(panel(name,label+' · '+app+' · '+phase+(' · x = RHS' if app=='Equations' else ' · y = RHS' if app=='Grapher' else '')))
 sections.append(f'<section id="{ident}"><h2>{html.escape(ident)}</h2><p><b>Teclas / Keys:</b> <code>{html.escape(f["keys"])}</code></p><p><b>Serialización exacta / Exact serialization:</b> <code>{html.escape(semantic["serialized"] or semantic["error"])}</code></p><p>Avance / Advance: {oldRoot["width"]} → {root["width"]} px. TEXT; STIX nominal 18/12/8 px. Referencia / Reference: KaTeX 18/12.6/9 px.</p><div class="panels">'+''.join(panels)+'</div></section>')
(out/'measurements.json').write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8');(out/'placeholder-ink.json').write_text(json.dumps(placeholder_checks,indent=2)+'\n')
(out/'reference.json').write_bytes((a.work/'reference/reference.json').read_bytes())
intro='''<!doctype html><html lang="es"><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>MATH-TEX-01 · revisión / review</title><style>body{font:16px system-ui;margin:24px;background:#f4f5f7;color:#17212b;line-height:1.5}h1,h2{line-height:1.2}section{background:white;padding:20px;margin:24px 0;border:1px solid #cad3df}.panels{display:flex;flex-wrap:wrap;gap:14px}figure{margin:0;width:320px}figcaption{font-weight:650;min-height:48px}img{image-rendering:pixelated;border:1px solid #bbb}code{overflow-wrap:anywhere}nav a{margin-right:12px}p{max-width:1100px}</style><h1>MATH-TEX-01 · revisión visual / visual review</h1>
<p>Originales 320×240 y ampliaciones enteras 4× nearest-neighbor. Pulse una imagen para ampliarla.<br>Original 320×240 images and integer 4× nearest-neighbor enlargements. Click to enlarge.</p>
<p>Baseline = reparación de entrada preservada; nuevo = mismo renderer C++/LVGL, sin cambios matemáticos. Las dos fracciones del usuario son controles independientes: la captura original no identifica su asociación.<br>Baseline = preserved input repair; new = same C++/LVGL renderer, unchanged mathematics. The two user fractions are independent controls; the original screenshot does not establish association.</p>
<p>Comparación cualitativa, sin igualdad de píxeles entre fuentes. KaTeX usa sus fuentes derivadas de Computer Modern; NumOS usa STIX Two Math. Variables de referencia en cursiva; NumOS conserva su política. Fracciones principales con dfrac para mantener el tamaño de sus hijos; las políticas verticales siguen siendo distintas. Fórmulas sin cursor en los tres primeros paneles; el editor y EXE se muestran aparte.<br>Qualitative comparison, no pixel equality across fonts. KaTeX uses Computer Modern-derived fonts; NumOS uses STIX Two Math. Reference variables are italic; NumOS keeps its policy. Main fractions use dfrac to keep child sizes; vertical policies still differ. First three panels hide the cursor; editor and EXE states appear separately.</p>
<p>Overlay: azul=cajas, verde=baseline, naranja=fin del avance, amarillo=espacios, violeta=reserva del script, magenta=tinta del glifo, rojo=cursor. Solo herramienta host.<br>Overlay: blue=boxes, green=baseline, orange=advance end, yellow=spacing, violet=script reserve, magenta=glyph ink, red=cursor. Host tool only.</p>
<p>No se afirma paridad TeX completa ni revisión del LCD físico. No se incluyen fuentes ni se ha publicado este paquete.<br>No claim of full TeX parity or physical LCD review. No font files included; this package has not been deployed.</p>
<p><a href="measurements.json">AST, Giac y geometría / geometry</a> · <a href="reference.json">Configuración de referencia / reference configuration</a> · <a href="placeholder-ink.json">Tinta de huecos / placeholder ink</a> · <a href="visual-changes.json">Cambios de píxeles sin máscaras / Unmasked pixel changes</a></p><nav>'''
if a.baseline_phase=='math-spacing':
 intro=intro.replace('Baseline = reparación de entrada preservada;', 'Baseline = MATH-TEX-01 completado, antes de sustituir el guion por el menos matemático;').replace('Baseline = preserved input repair;', 'Baseline = completed MATH-TEX-01, before replacing the hyphen with mathematical minus;')
 # An identical crop enlarged by an integer factor makes the user's small
 # exponent legible immediately; complete 320x240 originals remain below.
 detail=Image.new('RGB',(1440,272),'#edf0f3');labels=ImageDraw.Draw(detail)
 for i,(phase,title) in enumerate([('baseline','Antes / Before: U+002D'),('new','Ahora / Now: U+2212'),('reference','KaTeX 0.16.22')]):
  im=Image.open(assets/('five-linear-exponent-'+phase+'.png')).convert('RGB')
  detail.paste(im.crop((0,92,120,152)).resize((480,240),Image.Resampling.NEAREST),(480*i,32))
  labels.text((480*i+8,8),title,fill='black')
 detail.save(assets/'minus-detail-4x.png')
 sections.insert(0,'<section><h2>5^(−5−6) · detalle / detail</h2><p>Mismo recorte, 4× nearest-neighbor / Same crop, 4× nearest-neighbor.</p><div style="overflow-x:auto"><img src="assets/minus-detail-4x.png" width="1440" height="272" alt="Antes / Before · Ahora / Now · KaTeX"></div></section>')
steps=[]
for path in sorted((a.out/'steps').glob('*.ppm')):
 for folder,label in [('baseline-steps','Baseline'),('steps','Nuevo / New')]:
  source=a.out/folder/path.name
  if source.exists():
   name=folder+'-'+path.stem;save(Image.open(source).convert('RGB'),name);steps.append(panel(name,label+' · '+path.stem))
if steps:sections.append('<section><h2>Steps · EN / ES</h2><p>Mismas teclas y páginas, idioma indicado en el nombre / Same keys and pages, locale shown in the name.</p><div class="panels">'+''.join(steps)+'</div></section>')
document=intro+' '.join(f'<a href="#{f["id"]}">{f["id"]}</a>' for f in fixtures)+'</nav>'+''.join(sections)+'</html>'
(out/'index.html').write_text(document,encoding='utf-8')
changes=[]
for before in sorted(assets.glob('*baseline*.png')):
 if before.stem.endswith('-4x'):continue
 name=before.name
 afterName=name.replace('baseline-steps-','steps-') if name.startswith('baseline-steps-') else name.replace('-baseline.png','-new.png').replace('-baseline-','-candidate-')
 after=assets/afterName
 if after==before or not after.exists():continue
 old=Image.open(before).convert('RGB');new=Image.open(after).convert('RGB');difference=ImageChops.difference(old,new)
 changes.append(dict(baseline=before.name,candidate=after.name,changedPixels=sum(p!=(0,0,0) for p in difference.getdata()),boundingBox=difference.getbbox(),mask=None))
(out/'visual-changes.json').write_text(json.dumps(dict(note='Raw pixels, including app clock/cursor state. Review candidates, never accepted goldens. / Píxeles íntegros, incluido reloj/cursor; candidatos, no goldens aprobados.',pairs=changes),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
for ref in re.findall(r'(?:src|href)="([^"]+)"',document):
 if not ref.startswith('#'):assert (out/ref).is_file(),ref
manifest=[dict(path=str(f.relative_to(out)).replace('\\','/'),sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in out.rglob('*') if f.is_file() and f.name!='manifest.json']
(out/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
zip_path=a.out/'MATH-TEX-01-visual.zip'
with zipfile.ZipFile(zip_path,'w',zipfile.ZIP_DEFLATED) as z:
 for f in out.rglob('*'):
  if f.is_file():assert f.suffix not in ['.ttf','.otf','.woff','.woff2'];z.write(f,f.relative_to(out))
with zipfile.ZipFile(zip_path) as z:assert z.testzip() is None
print('PASS',len(fixtures),'fixtures',len(manifest),'relative resources;',len(placeholder_checks),'placeholder ink checks;',zip_path)
