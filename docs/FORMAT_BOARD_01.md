# FORMAT y placeholders: comprobación en placa

## Estado y alcance

Continuación de `CALCULATION_FORMAT_01.md`. Se conserva el trabajo acumulado sin commit sobre `main`, HEAD `ffb7082ba3b7db7741a8a603a5ad83f40c6c2682`, índice vacío. Snapshot independiente de esta intervención: `out/format-board-01/before-source.zip`, `before.patch` y `baseline.json`. No se restaura HEAD ni se altera `.vscode/settings.json` (SHA-256 `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`).

El USB `303a:1001`, número `44:B1:76:A7:B7:2C`, identifica la WROOM de producción en COM9. Lectura completa de 16 MiB conservada localmente en `out/format-board-01/board/flash-before.bin`; contiene datos del dispositivo, no se incluye en el ZIP visual. La aplicación leída tiene 5683936 bytes y SHA-256 `0ed7ba1c63f17bccfeb1ad3f2c47e35b6e0c43fc48021a302673d8f27995846f`: coincide con la entrega anterior de entrada, no con la entrega FORMAT. La compilación de escritorio y los builds WROOM/CAM previos no habían actualizado esta placa.

La petición actual de probar y compilar en la placa incluye la actualización de su aplicación para ejecutar el código actual. Se conserva la tabla real de particiones: aplicación activa OTA0, offset `0x10000`, capacidad `0x640000`. No se escribe bootloader, particiones, NVS, sistema de archivos ni eFuses; se mantienen frecuencia/modo/tamaño de flash. La copia completa y `previous-application.bin` permiten recuperar el firmware previo.

## Cambios

`MathCanvas::drawEmptyBaseline` dibuja cada `NodeEmpty` usando su caja ya medida, con independencia del cursor y de si es la única ranura de la raíz. Se eliminan las dos excepciones que ocultaban la entrada inicial y los canvases de solo lectura. Esto se aplica a todos los consumidores del renderer compartido: edición, historial y vistas previas. No se crean nodos, no se completan entradas incompletas y no cambia la serialización. La geometría y el gris existentes se conservan.

`SystemApp::handleKey` deja que Calculation cierre su menú FORMAT o visor de pasos antes de volver a HOME con BACK. Antes este comportamiento solo se ejercitaba en native/perfil demo; el firmware ordinario salía directamente de la aplicación. EXE, AC, DEL y FORMAT mantienen sus acciones.

No se añaden textos visibles, traducciones, fuentes, dependencias ni asignaciones en layout/draw/cursor. No hay cambios de tamaño de nodos ni nuevas variables de estado de producción.

## Teclas

- ENG: calcular primero, pulsar SHIFT y después `×10^x`. `1234 EXE` se presenta como `1.234 × 10³`. La segunda activación muestra `1234 × 10⁰`; LEFT revierte el desplazamiento. FORMAT recupera el exacto. El exponente inicial es múltiplo de tres, también para valores menores que uno. No modifica el valor canónico ni Ans.
- La tecla `×10^x` sin SHIFT conserva su plantilla de entrada con base 10 real.
- Menú: SHIFT, ALPHA, FORMAT, con un resultado. Son modificadores enclavados; no hace falta mantener tres teclas pulsadas. Flechas seleccionan y EXE aplica. BACK cierra sin perder el resultado.
- SHIFT+FORMAT mantiene su asignación TABLE. ENG y el menú no actúan sobre una entrada todavía no evaluada. Las opciones dependen del resultado; una fracción ordinaria no ofrece RAD/DEG/GRA ni formatos de complejo.

## Pruebas

`tests/host/math_placeholder_checks.cpp` observa el origen medido y comprueba los cuatro bordes reales en el framebuffer RGB565. Cubre raíz vacía, ambos huecos de potencia, numerador/denominador y raíz cuadrada; TEXT, SCRIPT y SCRIPTSCRIPT; con y sin controlador de cursor. La versión previa falla 21 de 109 comprobaciones; la corregida pasa 109/109. Cuenta asignaciones propias en los caminos calientes: cero. Los árboles permanecen incompletos después de medir y dibujar.

Reproducción del detector:

```powershell
python scripts/test-math-spacing.py --source C:/.codex-cache/math-tex-01/source --build C:/.codex-cache/math-tex-01/pinned-build/emulator_pc --lvgl C:/.codex-cache/tutor-03a-es-closeout/combined/source/.pio/libdeps/emulator_pc/lvgl --out C:/.codex-cache/format-board-01/placeholders-after --checks-source tests/host/math_placeholder_checks.cpp
```

La puerta `scripts/test-calculation-input.py` pasa 113 controles antes y después, incluida la secuencia SDL completa del usuario. Las 23 sesiones de `scripts/test-calculation-format.py` pasan, conservando resultados exactos y capacidades contextuales.

Los originales SDL de 320×240 y ampliaciones ×4 nearest-neighbor se guardan en `out/format-board-01/visual/`: entrada inicial, base/exponente vacíos, fracción antes/después de EXE, reparación, plantilla anidada e historial. Son capturas native del renderer compartido; no fotografías ni revisión humana del LCD.

La instrumentación de placa es privada, añadida únicamente a una copia de fuentes en caché; inyecta coordenadas del mapa de producción por `SystemApp::injectKey`, que recorre la resolución real de planos del firmware WROOM. No simula accionamiento eléctrico de interruptores ni demuestra por sí sola el debounce o los contactos. La imagen ordinaria final queda sin los comandos `FB`, comprobado en el binario y en el arranque.


## Compilaciones y recursos

Compilan el emulador native, WROOM de producci?n, CAM y Wasm. El paquete web local pasa compilaci?n, empaquetado, validaci?n y smoke; adem?s se ejecutan FORMAT/ENG en Chromium, Firefox y WebKit, shell y componente. No hay publicaci?n web. El emulador que encuentra `scripts/run-emulator-windows.ps1` se ha actualizado en `C:/.piobuild/numOS/emulator_pc/program.exe`.

Respecto al build FORMAT inmediatamente anterior, WROOM suma 168 bytes de `.flash.text` y 28 de `.flash.rodata`; RAM est?tica e IRAM no cambian (119200 bytes RAM est?tica). Binario: 5707376 bytes, SHA-256 `78f954531a58e1d25fb01082c92a62889ec39d31edea59d3afef80d927423fbb`. CAM suma 160 y 28 bytes respectivamente, con RAM e IRAM tambi?n constantes. No se instala CAM en la WROOM.

El desensamblado Xtensa muestra que GCC separa ahora el dibujo de la casilla en un wrapper de 32 bytes y cuerpo de 208: hasta 240 bytes de frames propios frente a 208 antes, sin recursi?n nueva; excluye los callees de LVGL. `SystemApp::handleKey` tiene frame propio de 80 bytes. Delta de tama?os de objetos: cero (ning?n cambio de miembros ni cabeceras de producci?n). No se afirma pico total de heap ni latencia f?sica a partir de estas cifras.

Archivos de producto de esta intervenci?n: `src/ui/MathRenderer.cpp`, `src/SystemApp.cpp`. Detector: `tests/host/math_placeholder_checks.cpp`. Informe: `docs/FORMAT_BOARD_01.md`. Los dem?s cambios sin commit pertenecen a las entregas previas y se conservan.

Galer?a: `out/format-board-01/visual/index.html`; ZIP `out/format-board-01/format-board-visual.zip`, con todos sus recursos relativos comprobados y sin archivos de fuentes. Baseline y nuevo resultado usan las mismas secuencias, resoluci?n y pol?tica tipogr?fica. No se promueven goldens.


## Resultado en la placa

La imagen privada WROOM ejecut? 440 eventos con coordenadas de producci?n y pas? 22 puntos de control: ENG inicial/repetido/reverso, exacto conservado, apertura/selecci?n/cierre del men?, BACK sin salir de Calculation, capacidades de `1/2`, potencia incompleta y recuperaci?n, reparaci?n de entrada cient?fica, valores peque?os/negativos/cero, conservaci?n de la asignaci?n SHIFT+FORMAT, ausencia de acciones de formato sin resultado y 50 ciclos abrir/cerrar. Los 50 ciclos mantienen exactamente `free=38164`, `largest=31960` y `peak=29532` en los muestreos de LVGL de los ciclos 1, 10 y 50. El arena configurado sigue en 65536 bytes; `lv_mem_monitor` informa carga ?til y metadatos, no ese tama?o nominal. Sin panic ni corrupci?n de heap en el registro.

Esta prueba prueba el despachador y la aplicaci?n ejecut?ndose en el ESP32, no contactos el?ctricos accionados por una persona. Durante la preparaci?n del harness se corrigieron su espera entre l?neas (respeta el debounce de 50 ms del puente serie), la espera de transici?n HOME y el signo esperado de la variable interna del desplazamiento ENG. No se modific? el comportamiento de producto para acomodar esas expectativas.


La aplicaci?n ordinaria final qued? instalada y verificada por digest en la WROOM, y arranc? con `[BOOT] OK`. Identidad y registro: `out/format-board-01/board/final-install/identity.json`, `verify.log`, `boot.log`. El prefijo de flash de 64 KiB es id?ntico antes/despu?s de cada escritura; el instalador solo direcciona OTA0. La placa queda en HOME, con el puerto serie cerrado. No se efectu? staging, commit, push ni despliegue web.

Limitaciones conservadas: no se ampl?a el alcance a contactos/debounce del teclado ni revisi?n humana del LCD; no se declara paridad TeX completa ni se cambia la pol?tica vertical. Esta entrega restaura visibilidad y navegaci?n, sin reabrir el motor matem?tico ni la pol?tica de fuentes.
