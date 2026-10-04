// Real keypad/pointer input, with separate processing and presentation barriers.
// No app mutations, refresh calls, expected-result injection or fixed sleeps.
import assert from 'node:assert/strict';
import {NUMOS_LOGICAL_KEYS} from '../../wasm/numos-keypad.js';

const codes=Object.fromEntries(NUMOS_LOGICAL_KEYS.map(k=>[k.id,k.code]));
const aliases={'.':'DOT','^':'POW','/':'DIV','+':'ADD','-':'SUB','*':'MUL',
  pi:'CONST_PI',neg:'NEG',NEGATE:'NEG',x:'VAR_X',FORMAT:'FREE_EQ',BACK:'AC'};
const eventHash=(hash,key,action)=>Math.imul(Math.imul(hash^key,16777619)^action,16777619)>>>0;

export function calculationDriver(page,el,trace,{timeout=10000}={}) {
 const state=()=>el.evaluate(e=>e.diagnosticState());
 const record=async(label,phase)=>{
  const sample={label,phase,at:Date.now(),state:await state()};trace.push(sample);return sample.state;
 };
 const wait=async(label,predicate,arg)=>{
  try {await page.waitForFunction(predicate,arg,{timeout,polling:'raf'});}
  catch(error){await record(label,'timeout');throw error;}
 };
 const settled=async(label='settled',after=null)=>{
  after??=await state();
  assert.ok(after?.render,'candidate lacks presentation diagnostics');
  await wait(label,({published,needsFrame})=>{
   const s=document.querySelector('numos-emulator')?.diagnosticState();
   return s?.render.transitionIdle && !s.render.pending &&
    (!needsFrame || s.render.published>published);
  },{published:after.render.published,needsFrame:after.render.pending});
  // SDL publication and the browser's composition are distinct boundaries.
  // Yield to composition rather than assuming that N milliseconds is enough.
  await page.evaluate(()=>new Promise(resolve=>requestAnimationFrame(()=>requestAnimationFrame(resolve))));
  await wait(label,()=>{
   const r=document.querySelector('numos-emulator')?.diagnosticState()?.render;
   return r?.transitionIdle && !r.pending;
  });
  return record(label,'published');
 };
 const button=token=>{
  const id=/^\d$/.test(token)?'NUM_'+token:aliases[token]||token;
  assert.ok(Number.isInteger(codes[id]),`unknown key ${token}`);
  // The keypad has two POW legends; this is the generic exponent template.
  const physical=token==='BACK'?'r0c4':token==='AC'?'r6c4':id==='POW'?'r2c4':id==='STO'?'r4c0':null;
  return {code:codes[id],locator:physical?el.locator(`[data-physical-id=${physical}]`):
   el.locator(`[data-key-id="${id}"]`).first()};
 };
 const keys=async(text,{rapid=false}={})=>{
  const before=await record(text,rapid?'rapid-start':'start');
  let expectedHash=before.input.hash,events=0;
  for(const token of text.trim().split(/\s+/)) {
   const {locator,code}=button(token);
   assert.equal(Number(await locator.getAttribute('data-key-code')),code);
   await locator.click({force:rapid});
   expectedHash=eventHash(eventHash(expectedHash,code,1),code,2);events+=2;
   if(!rapid)await wait(token,events=>document.querySelector('numos-emulator')
    .diagnosticState().input.events>=events,before.input.events+events);
  }
  const processed=await record(text,'processed');
  assert.equal(processed.input.events,before.input.events+events,'lost or duplicated key edge');
  assert.equal(processed.input.presses,before.input.presses+events/2,'lost or duplicated press');
  assert.equal(processed.input.hash,expectedHash,'key edges arrived out of order');
  return settled(text,processed);
 };
 const clickCanvas=async(x,y,label='canvas-click')=>{
  const before=await record(label,'start'),canvas=el.locator('canvas');
  const bounds=await canvas.boundingBox();
  await canvas.click({position:{x:bounds.width*x/320,y:bounds.height*y/240}});
  await record(label,'delivered');
  await wait(label,clicks=>document.querySelector('numos-emulator')
   .diagnosticState().pointer.completedClicks>clicks,before.pointer.completedClicks);
  const processed=await record(label,'processed');
  assert.equal(processed.pointer.completedClicks,before.pointer.completedClicks+1);
  assert.equal(processed.pointer.downEvents,before.pointer.downEvents+1);
  return settled(label,processed);
 };
 return {keys,clickCanvas,settled,state,record};
}
