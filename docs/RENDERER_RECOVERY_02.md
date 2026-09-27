# Renderer Recovery 02

Continuación: [Calculation FORMAT 01](CALCULATION_FORMAT_01.md) repara los resultados pendientes y el grado de los radicales, conservando este baseline.

Se reparan BR-02…BR-05 de [la sesión anterior](RENDERER_BREAKING_01.md). La búsqueda de fallos matemáticos continúa por separado en [Calculation Results Breaking 01](CALCULATION_RESULTS_BREAKING_01.md); esos fallos no se corrigen en esta entrega.

## Estado y preservación

Inicio en `main`, HEAD `ffb7082ba3b7db7741a8a603a5ad83f40c6c2682`, índice vacío. El working tree ya incluía CALC-CORE-INPUT-01, MATH-TEX-01, menos U+2212 y corchetes STIX. El fingerprint antiguo de la reparación de entrada no identifica este estado posterior. Se conserva el estado real completo: `out/renderer-recovery-02/before-source.zip`, `before.patch`, `baseline.json`, `before-program.exe` y `before-web.zip`. El ejecutable inicial tiene SHA256 `6681464f07e4a2b25452566a27685aa105dcf30c55bfa54931acc934e4b2ac86`.

El delta exclusivo está en `delta.patch`, `delta-source.zip` y `delivery.json` del mismo directorio. `.vscode/settings.json` conserva SHA256 `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`. Sin staging, commits, stash, reset, clean, push, flash ni despliegue. Dependencias, fuentes, clocks, particiones, almacenamiento y configuración del pool no cambian.

## Reparaciones demostradas

| Incidente | Causa | Resultado |
|---|---|---|
| BR-02 | Cursor vertical limitado antes de conocer cuánto había que desplazar; no existía viewport vertical | Se sigue el rectángulo lógico completo. En el reproducer pasa de `y=235…235`, fuera de `25…233`, a `217…233`, con altura completa |
| BR-03 | Guardia de dibujo de 28 niveles, incluyendo filas y estructuras; 14 raíces ocultaban el operando aunque se podían evaluar | Dibujo y búsqueda del cursor dejan de consumir un frame recursivo por nivel. Las 14 raíces muestran el `2`; el recorrido host comprueba hasta 90 raíces |
| BR-04 | Las flechas solo desplazaban el resultado en modo Extended | Exacto, periódico y extendido recorren toda su anchura real; se prueban `2^200` y `1/97` |
| BR-05 | `scrollBy` solo limitaba el extremo positivo | Ambos extremos se limitan a la geometría actual. `1/7`, S⇔D dos veces y RIGHT×120 conserva los dígitos finales |

Hay siete detectores SDL: los cinco controles anteriores —BR-04 tiene dos— y desplazamiento horizontal/vertical de la entrada evaluada con SHIFT y eventos de la matriz física. Los siete detectan el problema en el baseline conservado y pasan con la corrección. Los controles de extremo e ida/vuelta no admiten deriva ni una pantalla vacía como resultado válido.

Uso: durante edición el viewport sigue al cursor. Con resultado ancho, LEFT/RIGHT recorren el resultado en cualquiera de sus tres modos. SHIFT+LEFT/RIGHT recorren la entrada evaluada. SHIFT+UP/DOWN recorren el resultado si es alto; en otro caso, la entrada. UP/DOWN sin SHIFT conservan el historial. Los modificadores se comprueban también mediante `calc_physical`, no solo mediante códigos lógicos.

## Contrato compartido de posición

`MathNode` incorpora una posición de dibujo transitoria, sin propiedad de memoria: origen x, baseline relativo, estilo, nivel de fuente, estado de resaltado e índice de recorrido. `preparePlacements` reutiliza los métodos geométricos de las estructuras con una capa nula: coloca hijos y no emite tareas de dibujo. Las hojas no necesitan esta visita geométrica. A continuación, un recorrido por los enlaces padre/hijo pinta cada nodo exactamente una vez. No hay cola, array proporcional a la profundidad ni nueva capacidad máxima.

El antiguo Finder y sus fórmulas duplicadas desaparecen. El cursor consulta la posición de su fila y el mismo `childXOffset` usado por layout/dibujo. Cada canvas reconstruye las posiciones antes de utilizarlas; son caché de presentación y no estado semántico. La UI sigue siendo secuencial: no se introduce soporte para dibujar simultáneamente el mismo AST desde varios hilos.

`MathViewport.h` separa el límite en píxeles de la tipografía. El desplazamiento se calcula en `int32_t`, se limita y se convierte una vez a las coordenadas existentes `int16_t`. Se reserva el ancho del cursor y su último píxel vertical inclusivo. Los límites se vuelven a calcular tras cambiar expresión, edición o tamaño del widget. Para contenido que cabe se conserva el centrado anterior; el contenido alto empieza anclado arriba y puede recorrerse.

Se mantienen avances, clases contextuales, estilos, fracciones, tamaño mínimo, tinta de placeholders y ensamblado STIX. No se altera la asociación del árbol, el parser ni Giac. Esta entrega resuelve viewport y recorrido; no migra las métricas verticales a otra política TeX.

## Verificación

- **2553 comprobaciones C++** de viewport: recorrido completo, igualdad origen de dibujo/cursor, navegación profunda, altura del cursor, cambios de tamaño, límites, vuelta al origen y serialización. El allocator de nodos y `operator new` están denegados durante los frames: cero asignaciones propias.
- **151539 comprobaciones de espaciado**, 36 fixtures y oráculo independiente de clases/avances/posiciones. AST y resultados Giac antes/después idénticos. 35 imágenes idénticas; la fila larga cambia por el nuevo límite horizontal, con región exacta en `spacing-final.json`.
- Corchetes: **1645 comprobaciones**, 17 fixtures instrumentadas, continuidad de piezas STIX, placeholders y layout/dibujo/cursor; **20 sesiones SDL** ordinarias y otras 20 con pool fijo.
- Entrada: **114 sesiones** ordinarias y otras 114 con pool fijo, incluida `calculation-desktop-recovery.numos`, signos, potencias, plantilla ×10, borrado, serialización, historial y Ans. **50 ciclos** por variante; las muestras del pool permanecen estables.
- Se repiten las **84 sesiones** del corpus anterior. Son reproducciones de estrés, no una declaración de que cada resultado matemático sea correcto.
- Equations, Calculus y Grapher: seis controles de fracciones compartidas. Steps: cuatro recorridos completos EN/ES, 46 páginas. Notación: nueve capturas, suite host y MathEnginePhaseRegression.
- **900 trazas** actuales de tutor/composición idénticas a las de MATH-TEX-01. Solo se excluyen tiempos medidos y la ruta del ejecutable host indicada por el comparador; no se eliminan estados, condiciones o pruebas matemáticas. Además, 1593 controles bilingües, catálogo de 247 entradas y controles host de resultados periódicos/fallos de asignación.
- Web C++/Wasm local: configure/build/package/validate/smoke. Chromium, Firefox y WebKit; shell y componente. Seis recorridos de raíces profundas y 18 de modos de resultado con límites, ida/vuelta y estado matemático invariable. Se repiten también 90 casos evaluados de corchetes, estados incompletos, teclado desktop e historial. Las versiones exactas quedan en los JSON.
- Compilaciones WROOM producción, CAM y native con pool fijo. `git diff --check`.

Las observaciones de Calculation se compilan en una copia privada mediante `scripts/build-calculation-observer.py`. Solo leen campos existentes. Se conserva también el ejecutable ordinario; el observador no entra en firmware ni Wasm. Los `.numos`, logs, JSON y capturas originales permiten reproducir cada conclusión. No se promocionan goldens ni amplían máscaras.

## Memoria, pila y coste

Medición del compilador Xtensa, no extrapolación desde host:

| Objeto | Antes | Después |
|---|---:|---:|
| MathNode | 32 B | 44 B |
| NodeRow | 44 B | 56 B |
| MathCanvas | 180 B | 184 B |
| LayoutResult | 16 B | 16 B |
| FontMetrics | 44 B | 44 B |

El coste persistente es **12 B por nodo** y **4 B por canvas** en ESP32; no se oculta bajo la afirmación de cero asignaciones por frame. No se añade ninguna asignación independiente para posiciones. La preparación y el dibujo recorren linealmente los nodos y aristas; las consultas de espaciado mantienen su tabla O(1). Se conserva el pool LVGL configurado de 64 KiB.

Frames propios WROOM: `preparePlacements` 96 B, `drawNodeBaseline` 112 B, `paintNode` 48 B, `nextPaintNode` 32 B, cursor 96 B y `onDraw` 96 B. El Finder anterior consumía 224 B por nivel. **El layout recursivo existente no se elimina**: `NodeRow::calculateLayout` 48 B, raíz 80 B y fracción 144 B, más sus llamadas. Estas cifras no son el pico total de pila. El loop conserva su configuración de 64 KiB; no se modifica ni se promete profundidad ilimitada.

WROOM: flash text −2220 B, rodata +188 B; total **−2032 B**. CAM: text −2256 B, rodata +188 B; total **−2068 B**. DRAM estática e IRAM sin cambio. El tamaño extra de los objetos dinámicos se declara aparte. No se mide ni afirma latencia física, pico total de heap o inspección del LCD.

Se descarta ampliar simplemente la guardia de profundidad: mantendría frames recursivos y fórmulas duplicadas. También se descarta un vector nuevo por frame o un buffer con otro límite silencioso. El pequeño almacenamiento en los nodos existentes permite un recorrido completo sin esos costes.

## Reproducción y material visual

```powershell
python scripts/build-calculation-observer.py --source SOURCE --build BUILD --compile-commands COMPILE_COMMANDS.json --out OBSERVER_ASCII
python scripts/test-calculation-viewport.py --bin OBSERVER_ASCII/observer.exe --out out/viewport-check
# Contra el observador conservado del baseline: añadir --baseline.
python scripts/test-math-spacing.py --source SOURCE --build BUILD --lvgl LVGL --out CHECKS_ASCII --checks-source tests/host/math_viewport_checks.cpp
$env:NUMOS_WEB_ROOT='PAQUETE_WEB_LOCAL'
$env:NUMOS_VIEWPORT_OUT='SALIDA_BROWSER'
node tests/wasm/calculation-viewport.mjs
```

Galería: `out/renderer-recovery-02/visual/index.html`. ZIP: `renderer-recovery-02-visual.zip`, con recursos relativos verificados y sin archivos de fuente. Incluye antes/después 320×240, ampliaciones enteras 4× nearest-neighbor, reproducciones y los resultados matemáticos pendientes. Se revisan directamente las capturas SDL; no equivalen a revisar una PCB. `after-web.zip` es preparación local, sin publicación.

El launcher de Windows usa el ejecutable comprobado en `C:/.piobuild/numOS/emulator_pc/program.exe`; se registran sus hashes antes/después en `launcher.json`. Se abre con `./scripts/run-emulator-windows.ps1`. La identidad de este binario no se atribuye al firmware instalado.

## Límites y pendientes

No se cambian los presupuestos del editor, los límites del serializador ni las reglas de expresiones incompletas. Se ejercitan filas 63/64/65 y los límites existentes mediante las suites previas; ningún buffer nuevo introduce una discontinuidad. Las comprobaciones host largas no significan que toda entrada de esa longitud deba ser evaluable en producto.

La densidad vertical de scripts muy anidados y el mínimo de fuente permanecen como limitaciones visuales de la política actual. La discrepancia histórica del detector `parMixed` sigue documentada en la entrega anterior; no se declara reparada aquí. Los once ejemplos matemáticos de las cuatro familias confirmadas permanecen sin corregir, según la instrucción del usuario.

Archivos de esta entrega: `src/math/MathAST.h`; `src/ui/MathRenderer.h`, `MathRenderer.cpp`, `MathViewport.h`; `src/apps/CalculationApp.cpp`; `scripts/build-calculation-observer.py`, `test-calculation-viewport.py`, `test-calculation-results-breaking.py`, `test-math-spacing.py`; `tests/host/math_viewport_checks.cpp`, `calculation_result_capabilities.cpp`; `tests/fixtures/calculation-results-breaking.json`; `tests/wasm/calculation-viewport.mjs`; este documento y `docs/CALCULATION_RESULTS_BREAKING_01.md`. El manifiesto compara con el snapshot inmediatamente anterior, no con HEAD.
