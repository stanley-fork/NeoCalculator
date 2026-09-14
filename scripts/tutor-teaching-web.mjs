// Actual C++/LVGL browser sequences; no runtime JS mathematics or solver.
import assert from 'node:assert/strict';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {spawn} from 'node:child_process';
import {chromium,firefox,webkit} from '../tests/wasm/node_modules/playwright/index.mjs';
const snapshot=process.argv[2];assert.ok(snapshot,'pass the explicit ASCII snapshot');
const baseline=process.argv.includes('--baseline');
const browserName=process.argv.find(x=>x.startsWith('--browser='))?.split('=')[1]||'chromium';
const browserType={chromium,firefox,webkit}[browserName];assert.ok(browserType,'unknown browser');
const folder=resolve('out/tutor-teaching-ux-01/review-math/web',baseline?'baseline-sequence':('candidate-sequence'+(browserName==='chromium'?'':'-'+browserName)));
await mkdir(folder,{recursive:true});
const manifest=JSON.parse(await readFile(resolve(snapshot,'out/wasm/dist/release/numos-assets.json'),'utf8'));
const rawHash=createHash('sha256').update(await readFile(resolve(snapshot,'out/wasm/release/numos-emulator.wasm'))).digest('hex');
assert.equal(manifest.assets.wasm.sha256,rawHash,'package the freshly compiled WASM');
const server=spawn('python',['-m','http.server','8796','--bind','127.0.0.1','--directory',resolve(snapshot,'out/wasm/dist/release')],{stdio:'ignore',windowsHide:true});
const delay=ms=>new Promise(r=>setTimeout(r,ms));let browser,page,canvas;
try{
  for(let i=0;i<100;++i){try{if((await fetch('http://127.0.0.1:8796/index.html')).ok)break;}catch{}await delay(50);}
  browser=await browserType.launch({headless:true});page=await browser.newPage({viewport:{width:900,height:720}});
  const errors=[];page.on('pageerror',e=>errors.push(String(e)));
  await page.goto('http://127.0.0.1:8796/index.html?persistence=disabled');
  await page.waitForFunction(()=>window.numos?.isReady(),null,{timeout:30000});
  canvas=page.locator('numos-emulator').locator('canvas');const box=await canvas.boundingBox();
  if(process.argv.includes('--nonlinear')) {
    // Select the real policy through the actual Settings UI, just as a user
    // does. Complex-mode refusal is tested separately by the native checker.
    const focusApp=async id=>{
      for(let i=0;i<24;++i) {
        const s=await page.evaluate(()=>window.numos.diagnosticState());
        if(s.menuFocus===id)break;
        await page.evaluate(c=>window.numos.pressLogicalKey(c),s.menuFocus<id?16:13);await delay(80);
      }
      await delay(500);
      const s=await page.evaluate(()=>window.numos.diagnosticState());
      assert.equal(s.menuFocus,id);
      await canvas.click({position:{x:s.menuFocusPoint.x*box.width/320,y:s.menuFocusPoint.y*box.height/240},delay:40});
    };
    await focusApp(10);await page.waitForFunction(()=>window.numos.diagnosticState().app==='Settings');await delay(300);
    await page.evaluate(()=>window.numos.pressLogicalKey(15));await delay(80);
    await page.keyboard.press('Enter');
    await page.waitForFunction(()=>window.numos.diagnosticState().storage.complexEnabled===false);
    await page.evaluate(()=>window.numos.pressLogicalKey(69));
    await page.waitForFunction(()=>window.numos.diagnosticState().app==='Menu');await delay(400);
    await focusApp(2);
  } else
  await canvas.click({position:{x:255*box.width/320,y:72*box.height/240},delay:40});
  await page.waitForFunction(()=>window.numos.diagnosticState().app==='Equations');
  await delay(300); // complete the production launcher transition before typing
  const press=async(code,ms=80)=>{assert.equal(await page.evaluate(c=>window.numos.pressLogicalKey(c),code),true);await delay(ms);};
  const capture=async name=>{await delay(150);const uri=await canvas.evaluate(c=>c.toDataURL('image/png'));await writeFile(resolve(folder,name+'.png'),Buffer.from(uri.split(',')[1],'base64'));};
  const fullPage=async name=>{await capture(name+'-top');for(let i=0;i<24;++i)await press(15,20);await capture(name+'-bottom');for(let i=0;i<24;++i)await press(14,20);};
  for(const key of [50,50,43,36,17,44,34,78,42,46,50,15,15,50])await press(key);
  await page.waitForFunction(()=>window.numos.diagnosticState().equations.x0Exact==='5',null,{timeout:20000});
  await capture('linear-result');await press(72);
  for(let i=0;i<(baseline?2:3);++i){await fullPage('linear-guided-'+i);if(i<(baseline?1:2))await press(16);}
  for(let i=0;i<4;++i)await press(13);await press(50);
  await fullPage('linear-summary');await press(70);
  await page.waitForFunction(()=>window.numos.diagnosticState().frameMs.samples===512,null,{timeout:20000});
  const before=await page.evaluate(()=>window.numos.diagnosticState());const heap=[];
  for(let i=0;i<12;++i){await press(72);await press(70);heap.push((await page.evaluate(()=>window.numos.diagnosticState())).usedHeapBytes);}
  const after=await page.evaluate(()=>window.numos.diagnosticState());
  assert.equal(after.giac.structuredSolves,before.giac.structuredSolves);assert.equal(after.giac.generation,before.giac.generation);
  assert.ok(Math.max(...heap)-Math.min(...heap)<4096);assert.equal(heap.every((v,i)=>i===0||v>heap[i-1]),false);
  // Edit and commit through the physical VPAM key path.
  for(const key of [70,14,14,50,10,42,36,17,75,44,43,36,17,37,33,78,46,50,15,15,50])await press(key);
  await page.waitForFunction(()=>window.numos.diagnosticState().equations.solutionCount===2,null,{timeout:20000});
  await capture('quadratic-result');await press(72);
  for(let i=0;i<(baseline?2:5);++i){await fullPage('quadratic-guided-'+i);if(i<(baseline?1:4))await press(16);}
  await press(70);
  for(const key of [70,14,14,50,10,17,78,37,43,50,15,15,50])await press(key);
  await page.waitForFunction(()=>window.numos.diagnosticState().equations.x0Exact==='-3',null,{timeout:20000});
  await press(72);await fullPage('negative-isolated');await press(70);
  const nonlinear=[];
  if(process.argv.includes('--nonlinear')) {
    const fixtures=[
      ['abs-linear',[1,32,42,17,37,43,16,78,34],2,'-1'],
      ['abs-variable',[1,32,17,37,41,16,78,17,44,43],1,'-1'],
      ['abs-negative',[1,32,17,16,78,37,43],0,''],
      ['radical-extraneous',[32,17,44,41,16,78,17,37,41],1,'3'],
      ['radical-linear',[32,42,17,44,43,16,78,17],1,'3'],
      ['radical-isolate',[42,32,17,44,41,16,44,41,78,25],1,'8'],
    ];
    for(const [name,keys,count,first] of fixtures) {
      for(const key of [70,14,14,50,10,...keys,50,15,15,50])await press(key);
      await page.waitForFunction(expected=>{
        const e=window.numos.diagnosticState().equations;
        return e.tutorStatus==='complete' && e.solutionCount===expected.count && e.x0Exact===expected.first;
      },{count,first},{timeout:20000});
      const solved=await page.evaluate(()=>window.numos.diagnosticState());
      await capture(name+'-result');await press(72);
      for(let i=0;i<solved.equations.teachingPages;++i) {
        const state=await page.evaluate(()=>window.numos.diagnosticState().equations);
        assert.equal(state.teachingPage,i);assert.ok(state.teachingFormulas>0 && state.teachingFormulas<=4);
        await fullPage(name+'-guided-'+i);
        if(i+1<solved.equations.teachingPages)await press(16);
      }
      const final=await page.evaluate(()=>window.numos.diagnosticState());
      await press(70);await press(72);
      const reopened=await page.evaluate(()=>window.numos.diagnosticState());
      assert.equal(reopened.equations.teachingPage,final.equations.teachingPage);
      assert.equal(reopened.equations.tutorBuilds,solved.equations.tutorBuilds);
      assert.equal(reopened.giac.structuredSolves,solved.giac.structuredSolves);
      nonlinear.push({name,solved,final,reopened});await press(70);
    }
  }
  await press(69);
  await page.waitForFunction(()=>window.numos.diagnosticState().app==='Menu');await delay(400);
  const home=[];for(let i=0;i<5;++i){home.push((await page.evaluate(()=>window.numos.diagnosticState())).usedHeapBytes);await delay(250);}
  assert.ok(Math.max(...home)-Math.min(...home)<=128);assert.equal(errors.length,0,errors.join('\n'));
  const result={pass:true,baseline,browser:browserName,wasmSha256:rawHash,logicalSize:[320,240],cycles:12,heap,home,before,after,errors,nonlinear};
  await writeFile(resolve(folder,'result.json'),JSON.stringify(result,null,2));console.log(JSON.stringify(result));
}catch(error){
  if(page){await writeFile(resolve(folder,'failure.json'),JSON.stringify({error:String(error),state:await page.evaluate(()=>window.numos?.diagnosticState())},null,2));}
  if(canvas){const uri=await canvas.evaluate(c=>c.toDataURL('image/png'));await writeFile(resolve(folder,'failure.png'),Buffer.from(uri.split(',')[1],'base64'));}
  throw error;
}finally{if(browser)await browser.close();server.kill();}
