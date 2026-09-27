// Host-only pinned KaTeX reference. It never replaces C++/Wasm rendering.
import assert from 'node:assert/strict';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {chromium} from 'playwright';
import {startStaticServer} from './test-server.mjs';
const engine=resolve(process.env.NUMOS_KATEX_ROOT);
const out=resolve(process.env.NUMOS_REFERENCE_OUT);
const fixtures=JSON.parse(await readFile(new URL('../fixtures/math-spacing.json',import.meta.url),'utf8'));
const pkg=JSON.parse(await readFile(resolve(engine,'package.json'),'utf8'));
assert.equal(pkg.version,'0.16.22');
await mkdir(out,{recursive:true});
const server=await startStaticServer(engine,8898);
const browser=await chromium.launch({headless:true});
const page=await browser.newPage({viewport:{width:320,height:240},deviceScaleFactor:1});
const records=[],errors=[];page.on('pageerror',e=>errors.push(String(e)));
try {
 await page.goto(server.origin+'/dist/katex.min.css');
 await page.setContent(`<link rel="stylesheet" href="${server.origin}/dist/katex.min.css">
 <style>html,body{margin:0;width:320px;height:240px;background:white;overflow:hidden}
 #formula{position:absolute;left:8px;top:120px;transform:translateY(-50%);white-space:nowrap}
 .katex{font-size:18px}</style><div id="formula"></div>`);
 await page.addScriptTag({url:server.origin+'/dist/katex.min.js'});
 for(const f of fixtures) {
  const observed=await page.evaluate(async f=>{
   katex.render(f.latex,document.querySelector('#formula'),{displayMode:false,output:'html',throwOnError:true,strict:'error',trust:false});
   await document.fonts.ready;
   const fonts=[...document.fonts].filter(f=>f.status==='loaded').map(f=>({family:f.family,style:f.style,weight:f.weight,status:f.status}));
   const box=document.querySelector('.katex').getBoundingClientRect();
   return {version:katex.version,width:box.width,height:box.height,fonts};
  },f);
  assert.equal(observed.version,'0.16.22');assert.ok(observed.fonts.some(f=>f.family.includes('KaTeX')));
  await page.screenshot({path:resolve(out,f.id+'.png')});
  records.push({id:f.id,latex:f.latex,style:'TEXT; explicit dfrac where product keeps full-size fraction children',nominalPx:18,scriptRatios:[0.7,0.5],...observed});
 }
 assert.deepEqual(errors,[]);
 await writeFile(resolve(out,'reference.json'),JSON.stringify({engine:'KaTeX',browser:browser.version(),fontModel:'Computer Modern-derived KaTeX Main/Math, not STIX/OpenType MATH',cursor:'No editor; compare only formula views with cursor hidden',records},null,2));
 console.log('PASS pinned reference',records.length);
} finally {await browser.close();await server.close();}
