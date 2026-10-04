// Product controls, SDL text and pointer only. Diagnostics are read-only.
import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {chromium,firefox,webkit} from 'playwright';
import {startStaticServer} from './test-server.mjs';
import {calculationDriver} from './calculation-driver.mjs';
const out=resolve(process.env.NUMOS_TOOLBOX_OUT||'out/toolbox-01/web');
const server=await startStaticServer(resolve(process.env.NUMOS_WEB_ROOT),8898);
const records=[],trace=[];
try {for(const [name,type] of Object.entries({chromium,firefox,webkit})) {
 if(process.env.NUMOS_BROWSER&&process.env.NUMOS_BROWSER!==name)continue;
 const browser=await type.launch({headless:true});
 try {for(const surface of ['shell','component']) {
  const context=await browser.newContext({viewport:{width:1100,height:1100}}),page=await context.newPage(),errors=[];
  page.on('pageerror',e=>errors.push(String(e)));
  const folder=resolve(out,name,surface);await mkdir(folder,{recursive:true});
  const boot=async()=>{
   await page.goto(server.origin+(surface==='shell'?'/index.html':'/fixture.html'));
   if(surface==='component') {
    await page.waitForFunction(()=>customElements.get('numos-emulator'));
    await page.evaluate(()=>{const e=document.createElement('numos-emulator');e.setAttribute('controls','');document.body.append(e);});
    await page.locator('numos-emulator').locator('[data-action=overlay-start]').click();
   }
   await page.waitForFunction(()=>document.querySelector('numos-emulator')?.diagnosticState()?.ready,null,{timeout:60000});
   const el=page.locator('numos-emulator');
   if(!await el.locator('[data-physical-id=r9c4]').isVisible())await el.locator('[data-action=controls]').click();
   return el;
  };
  let el=await boot(),driver=calculationDriver(page,el,trace),keys=driver.keys;
  const shot=async id=>{const data=await el.locator('canvas').evaluate(c=>c.toDataURL().split(',')[1]);await writeFile(resolve(folder,id+'.png'),Buffer.from(data,'base64'));};
  const expect=async expected=>{
   try {await page.waitForFunction(expected=>{const s=document.querySelector('numos-emulator').diagnosticState().toolbox;return Object.entries(expected).every(([k,v])=>s[k]===v);},expected,{timeout:10000});}
   catch(error){await shot('failure');const state=await driver.state();records.push({browser:name,surface,expected,state,errors});console.error(JSON.stringify({expected,toolbox:state.toolbox}));throw error;}
  };
  // Launch and editing have separate transition/publication boundaries.
  await keys('ENTER');await keys('2 ^ 3');const before=await driver.state();
  await keys('TOOLBOX');await expect({open:true,group:0});await shot('root');
  await driver.clickCanvas(8,45,'dismiss-popup');await expect({open:false});
  await keys('TOOLBOX');await expect({open:true,group:0,id:103});
  await keys('UP RIGHT RIGHT RIGHT ENTER');await expect({queryFocus:true});
  await el.locator('canvas').focus();await page.keyboard.type('raiz');
  await expect({queryBytes:4,count:1});await driver.settled('SDL-search');
  await keys('DOWN');await expect({id:111});await shot('search');
  assert.deepEqual((await driver.state()).giac,before.giac,'search evaluated mathematics');
  await keys('FORMAT');await shot('options');await keys('ENTER BACK BACK');await expect({open:false,favorites:1});
  await keys('ENTER');assert.equal((await driver.state()).calculation.exact,'8','cancel changed exponent');
  await keys('AC TOOLBOX UP RIGHT ENTER');await expect({group:1,id:111});await shot('favorites');
  const prior=(await driver.state()).giac.structuredEvaluations;
  await driver.clickCanvas(150,111,'insert-favorite');await expect({open:false});
  assert.equal((await driver.state()).giac.structuredEvaluations,prior,'click leaked evaluation');
  await keys('3 RIGHT 8 ENTER');assert.equal((await driver.state()).calculation.exact,'2');await shot('inserted');
  await keys('AC TOOLBOX UP RIGHT RIGHT RIGHT ENTER');await el.locator('canvas').focus();await page.keyboard.type('Logarithm');
  await expect({queryBytes:9});await keys('DOWN RIGHT');await expect({group:100,id:120,variant:1});
  await keys('UP ENTER 2 RIGHT 8 ENTER');assert.equal((await driver.state()).calculation.exact,'3');
  await keys('AC TOOLBOX UP RIGHT');await shot('tabs-focus');await keys('BACK');
  await keys('TOOLBOX');await el.locator('canvas').focus();await page.keyboard.type('zzzz');
  await keys('DOWN');await expect({count:0});await shot('empty-search');await keys('BACK BACK');
  await keys('TOOLBOX DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ENTER');await expect({group:18,count:3});await shot('variables');
  await keys('DOWN ENTER');await expect({group:20,count:24,id:300});await shot('greek');
  await keys('DOWN DOWN DOWN RIGHT');await expect({group:1303,id:303,variant:1});await shot('delta-case');
  await keys('ENTER ^ 2 ENTER');assert.equal((await driver.state()).calculation.exact,'Δ^2');await shot('delta-result');
  await keys('AC TOOLBOX');await el.locator('canvas').focus();await page.keyboard.type('Infinity');
  await keys('DOWN');await expect({id:410,variant:0});await keys('RIGHT');await expect({group:101,count:3,id:410,variant:1});await shot('infinity-signs');
  await keys('DOWN ENTER ^ 2 ENTER');assert.equal((await driver.state()).calculation.exact,'+infinity');
  await keys('AC TOOLBOX');await el.locator('canvas').focus();await page.keyboard.type('Both infinities');
  await keys('DOWN ENTER ENTER');assert.equal((await driver.state()).calculation.exact,'[-infinity, +infinity]');await shot('infinity-both');
  // The existing public persistence controller flushes IDBFS; FS and memory
  // stay private. Real reload in the same browser context proves durability.
  const flushed=await el.evaluate(e=>e.flushPersistence());
  records.push({browser:name,version:browser.version(),surface,phase:'before-reload',flushed,state:await driver.state()});
  await shot('log-result');el=await boot();driver=calculationDriver(page,el,trace);keys=driver.keys;
  await keys('ENTER');await keys('TOOLBOX UP RIGHT ENTER');await expect({group:1,id:111,favorites:1});await shot('persisted-after-reload');
  await keys('FORMAT DOWN DOWN DOWN ENTER');await shot('help');await keys('BACK BACK BACK');
  // New presses immediately after closing must survive; shortcuts retain
  // their existing meaning outside the modal.
  await keys('2 + 3 ENTER FORMAT');assert.equal((await driver.state()).calculation.status,'ok');
  assert.deepEqual(errors,[]);records.push({browser:name,surface,passed:true});
  await context.close();
 }}finally{await browser.close();}
}}finally{
 await mkdir(out,{recursive:true});await writeFile(resolve(out,'results.json'),JSON.stringify(records,null,2));await writeFile(resolve(out,'events.json'),JSON.stringify(trace,null,2));await server.close();
}
console.log('PASS Toolbox: all requested browser surfaces, events and real IDBFS reload');
