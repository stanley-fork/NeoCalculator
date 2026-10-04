// System-language settings, real keyboard paths and real IDBFS reloads.
// Only copied diagnostics and the public flush API; no FS/Wasm memory access.
import assert from 'node:assert/strict';
import {mkdir, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {chromium, firefox, webkit} from 'playwright';
import {startStaticServer} from './test-server.mjs';
import {calculationDriver} from './calculation-driver.mjs';

const out = resolve(process.env.NUMOS_LOCALE_OUT || 'out/unit-catalog-02/web-locales');
const localeNames = ['en-US', 'en-GB', 'es-ES', 'es-419'];
const requestedLocales = (process.env.NUMOS_LOCALES || localeNames.join(',')).split(',');
assert.ok(requestedLocales.includes('en-US') && requestedLocales.includes('en-GB'));
assert.ok(requestedLocales.every(locale => localeNames.includes(locale)));
const server = await startStaticServer(resolve(process.env.NUMOS_WEB_ROOT), 8901);
const records = [], trace = [];
try {
  for (const [name, type] of Object.entries({chromium, firefox, webkit})) {
    if (process.env.NUMOS_BROWSER && process.env.NUMOS_BROWSER !== name) continue;
    for (const surface of ['shell', 'component']) {
      if (process.env.NUMOS_SURFACE && process.env.NUMOS_SURFACE !== surface) continue;
      // Each application fixture gets a fresh browser process; all four locale
      // replacements/reloads still share one context and genuine IDBFS storage.
      const browser = await type.launch({headless: true});
      let closing = false, bootCount = 0;
      const lifecycle = (event, detail = '') => trace.push({source: 'lifecycle',
        browser: name, surface, event, detail, intentionalClose: closing, bootCount, at: Date.now()});
      browser.on('disconnected', () => lifecycle('browser-disconnected'));
      try {
        const context = await browser.newContext({viewport: {width: 1100, height: 1100}});
        const page = await context.newPage(), errors = [];
        page.on('pageerror', error => { errors.push(String(error)); lifecycle('page-error', String(error)); });
        page.on('crash', () => lifecycle('page-crash'));
        page.on('close', () => lifecycle('page-close'));
        context.on('close', () => lifecycle('context-close'));
        const folder = resolve(out, name, surface);
        await mkdir(folder, {recursive: true});
        let el, driver, keys;
        const boot = async () => {
          ++bootCount; lifecycle('boot-start');
          await page.goto(server.origin + (surface === 'shell' ? '/index.html' : '/fixture.html'));
          if (surface === 'component') {
            await page.waitForFunction(() => customElements.get('numos-emulator'));
            await page.evaluate(() => {
              const emulator = document.createElement('numos-emulator');
              emulator.setAttribute('controls', ''); document.body.append(emulator);
            });
            await page.locator('numos-emulator').locator('[data-action=overlay-start]').click();
          }
          await page.waitForFunction(() => document.querySelector('numos-emulator')?.diagnosticState()?.ready,
            null, {timeout: 60000});
          el = page.locator('numos-emulator');
          if (!await el.locator('[data-physical-id=r9c4]').isVisible()) await el.locator('[data-action=controls]').click();
          driver = calculationDriver(page, el, trace); keys = driver.keys;
          await driver.settled('locale-boot');
          lifecycle('boot-ready');
        };
        const waitApp = app => page.waitForFunction(expected =>
          document.querySelector('numos-emulator').diagnosticState().app === expected, app);
        const openApp = async (id, app) => {
          if ((await driver.state()).app !== 'Menu') { await keys('MODE'); await waitApp('Menu'); }
          for (let i = 0; i < 24; ++i) {
            const state = await driver.state();
            if (state.menuFocus === id) break;
            await keys(state.menuFocus < id ? 'RIGHT' : 'LEFT');
          }
          assert.equal((await driver.state()).menuFocus, id, 'launcher focus');
          await keys('ENTER'); await waitApp(app);
        };
        const capture = async id => {
          await driver.settled(id);
          const pixels = await el.locator('canvas').evaluate(canvas => {
            const crop = document.createElement('canvas'); crop.width = 320; crop.height = 212;
            const context = crop.getContext('2d');
            context.drawImage(canvas, 0, 28, 320, 212, 0, 0, 320, 212);
            return {full: canvas.toDataURL().split(',')[1], crop: crop.toDataURL().split(',')[1],
              width: canvas.width, height: canvas.height};
          });
          assert.equal(pixels.width, 320); assert.equal(pixels.height, 240);
          await writeFile(resolve(folder, id + '.png'), Buffer.from(pixels.full, 'base64'));
          return pixels.crop;
        };
        const flush = async () => {
          const state = await el.evaluate(emulator => emulator.flushPersistence());
          assert.equal(state.state, 'persistent_ready'); assert.equal(state.dirty, false);
          return state;
        };
        await boot();
        const spellingFrames = {}, referenceFrames = {};
        let selectedLocaleIndex = 0;
        for (const [index, locale] of localeNames.entries()) {
          if (!requestedLocales.includes(locale)) continue;
          await openApp(10, 'Settings'); await keys('DOWN DOWN DOWN DOWN');
          // Same four-choice path as a user; the default case exercises replacement too.
          const changes = (index - selectedLocaleIndex + localeNames.length) % localeNames.length;
          await keys(changes ? Array(changes).fill('RIGHT').join(' ') : 'RIGHT LEFT');
          selectedLocaleIndex = index;
          const before = await capture(locale + '-settings-selected');
          const persisted = await flush();
          await openApp(0, 'Calculation');
          const giac = (await driver.state()).giac;
          await keys('TOOLBOX'); await el.locator('canvas').focus(); await page.keyboard.type('metre');
          await page.waitForFunction(() => document.querySelector('numos-emulator').diagnosticState().toolbox.queryBytes === 5);
          await keys('DOWN');
          const selection = (await driver.state()).toolbox;
          assert.equal(selection.id, 0x8001); assert.equal(selection.variant, 0);
          spellingFrames[locale] = await capture(locale + '-metre-result');
          await keys('BACK BACK');
          referenceFrames[locale] = {};
          for (const [query, id, label] of [['Normal cubic', 16521, 'normal-gas'], ['Standard cubic m', 16522, 'standard-gas']]) {
            await keys('TOOLBOX'); await el.locator('canvas').focus(); await page.keyboard.type(query);
            await page.waitForFunction(bytes => document.querySelector('numos-emulator')
              .diagnosticState().toolbox.queryBytes === bytes, query.length);
            await keys('DOWN');
            const reference = (await driver.state()).toolbox;
            assert.equal(reference.id, id); assert.equal(reference.variant, 0);
            referenceFrames[locale][label] = await capture(locale + '-' + label + '-result');
            await keys('BACK BACK');
          }
          assert.deepEqual((await driver.state()).giac, giac, 'locale/search performed mathematics');
          await keys('2 + 2 ENTER');
          assert.equal((await driver.state()).calculation.exact, '4');
          await flush();
          // A fresh page in the same context hydrates genuine persistent storage.
          await boot(); await openApp(10, 'Settings'); await keys('DOWN DOWN DOWN DOWN');
          const after = await capture(locale + '-settings-reloaded');
          assert.equal(after, before, locale + ' was not retained after IDBFS reload');
          records.push({browser: name, version: browser.version(), surface, locale,
            persisted, settingsPixelsRetained: true, ordinaryCalculation: '4',
            typedReferenceIds: [137, 138], passed: true});
        }
        // Same ASCII query and same typed unit identity; regional EN labels must differ.
        assert.notEqual(spellingFrames['en-US'], spellingFrames['en-GB'], 'Meter/Metre labels did not change region');
        for (const label of ['normal-gas', 'standard-gas']) assert.notEqual(
          referenceFrames['en-US'][label], referenceFrames['en-GB'][label], label + ' labels did not change region');
        assert.deepEqual(errors, []);
        closing = true;
        await context.close();
      } finally { closing = true; await browser.close(); }
    }
  }
} finally {
  await mkdir(out, {recursive: true});
  await writeFile(resolve(out, 'results.json'), JSON.stringify(records, null, 2));
  await writeFile(resolve(out, 'events.json'), JSON.stringify(trace, null, 2));
  await server.close();
}
console.log('PASS requested system locales, regional unit/reference labels and IDBFS reload on requested browser surfaces');
