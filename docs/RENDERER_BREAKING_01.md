# Calculation: breaking session y corchetes STIX

26-09-2026. Cuatro rondas, **84 casos distintos introducidos por eventos reales**, cinco hallazgos agrupados por causa. **Solo BR-01, los corchetes, se corrige**. BR-02…BR-05 permanecen reproducibles en el ejecutable final. Las sesiones completas no registraron crashes ni timeouts; esto no implica que su dibujo sea correcto.

Galería: [`out/renderer-breaking-01/visual/index.html`](../out/renderer-breaking-01/visual/index.html). ZIP con todos los recursos relativos: [`renderer-breaking-01-visual.zip`](../out/renderer-breaking-01/renderer-breaking-01-visual.zip). Originales 320×240 y ampliaciones 4× nearest-neighbor, sin archivos de fuente. Se revisaron directamente las capturas de los fallos y los corchetes; no se atribuye esa revisión al LCD físico.

## Preservación y alcance

Rama `main`; HEAD `ffb7082ba3b7db7741a8a603a5ad83f40c6c2682`, índice vacío. El punto de partida es el working tree real: CALC-CORE-INPUT-01, MATH-TEX-01 y la corrección posterior del menos matemático U+2212 ya estaban encima de HEAD. No se sustituyen por el código de HEAD. El informe anterior continúa en [MATH_TEX_01.md](MATH_TEX_01.md).

Antes de modificar producción se conservaron `before-source.zip`, `before.patch`, `baseline.json`, `before-program.exe` y `before-web.zip` en `out/renderer-breaking-01/`. El script inicial de estrés ya existía al hacer ese snapshot; el manifiesto de entrega lo identifica como herramienta de esta sesión. `delta.patch` y `delta-source.zip` contienen exclusivamente los cambios de esta sesión frente a ese snapshot. El archivo `delivery.json` enumera sus hashes.

`.vscode/settings.json` conserva SHA256 `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`. No se hace staging, commit, stash, reset, clean, push, flash ni despliegue. No se cambian dependencias, clocks, particiones, almacenamiento ni el pool LVGL de 64 KiB. El paquete web es local; el binario native no identifica el firmware instalado en una PCB.

## Rondas y hallazgos

El corpus versionado es [calculation-breaking.json](../tests/fixtures/calculation-breaking.json); [test-calculation-breaking.py](../scripts/test-calculation-breaking.py) genera una reproducción `.numos`, AST/serialización, log y capturas por caso, en procesos separados. No inyecta AST ni texto LaTeX.

| Ronda | Casos | Qué se intentó romper |
|---|---:|---|
| 1 | 26 | Aritmética corriente, signos, potencias negativas, productos implícitos, raíces/logaritmos fuera de dominio, división por cero, factorial, trigonometría, resultados grandes/pequeños, huecos, corchetes SDL y matriz física |
| 2 | 30 | Fracciones, raíces, potencias y paréntesis con 2/3/5/8/12 niveles; filas 20/63/64/65/100; periódicos, notación científica, historial e incompletos |
| 3 | 20 | Profundidad 20/40/65/90, filas 199/200/399/400 términos, número de 500 dígitos, decimal de 250, borrado/reentrada y muchos huecos |
| 4 | 8 | Refinamiento de fallos: resultados anchos, overscroll, límite exacto de profundidad 13/14/15 y potencias anidadas |

Los tamaños de las filas del corpus son términos introducidos, no necesariamente el número final de nodos: cada operador también consume presupuesto. Las comprobaciones de espaciado adicionales sí prueban 63/64/65, 399 y 2000 nodos de fila en host.

| ID | Hallazgo, evidencia y reproducción | Estado |
|---|---|---|
| BR-01 | `[2+3]*4` por SDL_TEXTINPUT o SHIFT+paréntesis perdía los corchetes: AST `2+3*4`, resultado **14**. Ahora es `Paren []`, serializa `(2+3)*4` y da **20**. | Corregido |
| BR-02 | `fraction-depth-12`: raíz **57×244** frente a canvas `(6,25)…(313,233)`, de **308×209**. Tras navegar, cursor `(47,235)…(49,235)`, fuera del canvas y reducido a una línea. Fórmula y cursor quedan recortados. `deep-root-65` aporta otro caso de la misma familia. | Sin corregir |
| BR-03 | `root-boundary-13/14/15`: con 13 raíces anidadas se ve el `2`; con **14** se sustituye por `…` rojo. La expresión de 14 raíces mide **163×70**, cabe en la ventana y se evalúa correctamente. `MAX_RENDER_DEPTH=28` cuenta las filas intermedias además de las raíces. | Sin corregir |
| BR-04 | `exact-wide`: `2^200` produce **549×14** en un canvas de 308 px. `periodic-wide`: `1/97`, S⇔D, produce **877×16**. RIGHT×20 no desplaza el resultado en esos modos; mueve el cursor de entrada. Los dígitos finales quedan inaccesibles. | Sin corregir |
| BR-05 | `extended-overscroll`: `1/7`, EXE, S⇔D, S⇔D, RIGHT×120. El resultado extendido ocupa **1813×14**, pero el offset llega a −2400 y el área queda **completamente en blanco**. `scrollBy` solo limita el otro extremo. | Sin corregir |

BR-02 es un defecto de navegación/viewport vertical, no una razón para desplazar glifos horizontalmente. BR-03 conserva un guard de seguridad, pero su límite oculta entradas admitidas por el serializador. No se elimina ese guard. BR-04 y BR-05 tienen causas distintas: el enrutamiento de las flechas y la falta de un límite de desplazamiento, respectivamente.

Las mediciones provienen de una copia host privada de `CalculationApp.cpp`, generada por `out/renderer-breaking-01/observer.py`. Añade observación a `debugInput("dump")`, sin alterar decisiones de layout. No se incorpora al firmware. Las cuatro familias se repiten también con el ejecutable ordinario final: `fraction-final/` y `failures-final/`. Sus cinco imágenes representativas son idénticas a las observadas, excluyendo únicamente la barra del reloj, según `unfixed-preserved.json`.

Se conservan resultados negativos: LEFT×20 desde el origen no causa el overscroll (ya existe clamp a cero); 13 raíces conservan el operando; 63/64/65 no cambian de reglas tipográficas. Los rechazos por profundidad/presupuesto del serializador, los errores matemáticos y el fallback de resultados enormes no se cuentan automáticamente como bugs. La legibilidad vertical de scripts muy anidados queda como observación cualitativa, sin inflar el número de defectos confirmados.

Los tres errores iniciales de runner —alias de variable, alias de logaritmo y coordenadas eléctricas de los corchetes— se corrigieron en el corpus y se repitieron en `round1-replay/`. No se atribuyen al producto. Una primera prueba de browser descubrió que su keypad enviaba códigos legacy: se añadió el manejo de SHIFT+LPAREN/RPAREN, dentro del alcance autorizado de los corchetes.

## Corrección de corchetes

Se pueden introducir con **SHIFT + `(`** y **SHIFT + `)`**, o con `[` y `]` en el teclado de escritorio. Se usan los semantic IDs existentes de la matriz física. Los códigos lógicos 80/81 se añaden al final del catálogo sin renumerar los anteriores; el puente Wasm y los nombres del runner los admiten. Las nuevas etiquetas accesibles están en EN/ES.

El AST reutiliza `NodeParen(DelimKind::Bracket)`: no hay otro tipo de nodo ni cambio al CAS. El cierre busca el corchete ancestro más cercano y sale de él incluso desde una fracción/exponente. No borra placeholders ni convierte huecos en operandos. Un `]` sin corchete abierto no altera el árbol. Clonado e historial conservan `[]`; la serialización para Giac mantiene agrupación escalar con `()`, evitando interpretar el corchete como vector. El resultado de Giac conserva su notación canónica; la forma elegida por el alumno se mantiene en la entrada y al recuperarla del historial.

La fuente local verificada es **STIX Two Math 2.12 b168a**, SHA256 `562551b15b836e6e01d1b7350909baf3c8c8d83260c1190fbf4544333e6936de`, UPM 1000. Se extendió el generador suplementario ya existente, con `lv_font_conv 1.5.3` instalado, para extraer **13 variantes y 3 piezas por lado** en 18/12/8 px. Los paréntesis mantienen U+E000…E01F; los corchetes usan U+E020…E03F. Los 32 glifos anteriores conservan exactamente sus bitmaps.

La tabla MATH de STIX contiene piezas en orden inferior→superior, con flags `[0,1,0]`: se convierten a superior/extensor/inferior. Para los corchetes, MinConnectorOverlap=100 DU; los conectores permiten al menos 1000 DU. El solape se redondea hacia arriba una vez a píxeles. La selección usa las cajas raster reales de la misma fuente; si ninguna variante cubre la altura, se ensamblan las piezas auténticas con recorte del último extensor. Esta es la ruta existente de los paréntesis aplicada también a los corchetes, sin estirar bitmaps y sin sustituirlos por líneas vectoriales.

La [especificación primaria OpenType MATH](https://learn.microsoft.com/en-us/typography/opentype/spec/math) documenta variantes verticales, orden de las piezas, extensores y mínimos de conexión. No se afirma una implementación general nueva de todo OpenType MATH. Se conserva la política actual de tamaño mínimo y geometría vertical de los paréntesis.

`stixParenthesisPlan` comparte ancho y selección entre medida y dibujo. `NodeParen` guarda las mismas dimensiones que consumen `drawParen` y Finder. Los corchetes pasan a la geometría que ajusta los paréntesis a la tinta de su contenido; no se cambia la política vertical de fracciones o scripts. La nueva clasificación de espaciado, el menos U+2212, Giac y el tutor no se modifican.

## Verificación y límites

- Detector real SDL y matriz física: las dos secuencias que exigen 20 fallan contra `before-program.exe` con exit 4; pasan después. No se genera el valor esperado desde el renderer o Giac.
- 17 fixtures de corchetes, más SDL directo, matriz física e historial: **20 sesiones native**. Cubre cierre dentro de fracción/raíz/exponente, mezcla y anidamiento, producto implícito, borrado, huecos y cierre sin apertura.
- **1645 comprobaciones C++ independientes**: descriptores de los glifos reales para objetivos de 1…512 px en los tres tamaños, agrupación, clonación y cero asignaciones propias durante layout repetido con allocators denegados.
- Observador de dibujo: 17 fixtures, glifos STIX reales, variantes y ensamblado también en SCRIPT_SCRIPT, continuidad de tinta, borde completo de placeholders, igualdad exacta layout/draw/cursor y AST/serialización/Giac antes/después. No usa el helper probado para generar esperados. Un pixel de antialiasing de cobertura mínima cuenta como tinta; una fila blanca completa en un empalme falla.
- Regresión MATH-TEX-01: **151539 comprobaciones**, 36 fixtures y comparación independiente de reglas, posiciones y semántica contra el baseline del menos ya corregido. Cero fallos.
- Puerta Calculation: **114 sesiones**, reproducción SDL `calculation-desktop-recovery.numos`, controles negativos e historial/Ans. 50 ciclos de apertura/cierre. Además se reconstruye native con el pool LVGL fijo y se repiten la puerta y los corchetes; sus resultados se guardan aparte.
- Web local C++/Wasm: build, package, validate y smoke. Chromium **139.0.7258.5**, Firefox **140.0.2** y WebKit **26.0**, **shell y componente**, keypad SHIFT y teclado desktop; 90 casos evaluados (15×6), además de 12 casos de scripts sin evaluar y las comprobaciones de escritorio/historial. El canvas capturado es 320×240, sin depender del zoom CSS. Catálogo lógico: 81/81.
- WROOM producción y CAM compilados. No se flashean. No se vuelven a atribuir a esta revisión las 900 trazas del tutor ni los recorridos de todas las apps de la entrega MATH-TEX-01: esa batería no se repitió para este delta.

El runner estricto histórico de paréntesis presenta una discrepancia en `parMixed`: detecta límites de tinta distintos a izquierda y derecha. La posible contaminación por tinta vecina queda pendiente de aislar. No se oculta como PASS ni se amplía su tolerancia. Las 16 capturas de paréntesis, incluidas `parMixed`, se grabaron con los ejecutables antes/después: **idénticas fuera de la barra del reloj**. `parenthesis-preservation.json` registra esa comparación y la igualdad de los glifos. No se cambia el dibujo para solucionar el detector histórico.

## Recursos

Frente al baseline inmediato, tanto WROOM como CAM suman **428 B de `.flash.text` y 8156 B de `.flash.rodata`**, total **8584 B**. `.dram0.data`, `.dram0.bss` e IRAM no cambian. La fuente suplementaria permanece en flash. No se añaden miembros a los nodos, arrays por fila ni asignaciones por frame.

Frames propios medidos en el objeto Xtensa WROOM: `NodeParen::calculateLayout` **48 B**, `closeBracket` **32 B**, `insertParen` **48 B**, `drawDelimiterGlyph` **384 B** más llamadas, lambda de pieza STIX **144 B**; Finder **224 B**, envoltorio de cursor **96 B**. No son un máximo de pila total ni una medición del heap/latencia físicos. El cierre es iterativo O(profundidad + hermanos); la selección examina como máximo 13 variantes. El ensamblado recorre linealmente las piezas necesarias, sin almacenamiento proporcional a la altura.

## Reproducción

```powershell
python scripts/test-calculation-breaking.py --bin out/renderer-breaking-01/after-program.exe --out out/replay-breaking-r1 --round 1
# Repetir con --round 2, 3 y 4 en directorios distintos.
python scripts/test-calculation-brackets.py --bin out/renderer-breaking-01/before-program.exe --out out/replay-before --baseline
python scripts/test-calculation-brackets.py --bin out/renderer-breaking-01/after-program.exe --out out/replay-brackets
python scripts/test-math-spacing.py --source SOURCE --build BUILD --lvgl LVGL --out PROBE --brackets --fixtures tests/fixtures/math-brackets.json --checks-source tests/host/math_brackets_checks.cpp
python scripts/compare-math-brackets.py --probe PROBE --out geometry-brackets.json
# Node: desde un árbol con las dependencias host existentes de tests/wasm.
$env:NUMOS_WEB_ROOT='RUTA_AL_PAQUETE_LOCAL'
$env:NUMOS_BRACKET_OUT='RUTA_DE_RESULTADOS'
node tests/wasm/calculation-brackets.mjs
```

El ejecutable probado se copia también a `C:/.piobuild/numOS/emulator_pc/program.exe`, donde lo busca `scripts/run-emulator-windows.ps1`; se conserva su hash y el anterior. Los DLL existentes no cambian.

## Archivos de esta sesión

Producción/generación (14): `scripts/generate-stix-parentheses.py`; `src/apps/CalculationApp.cpp`; `src/fonts/stix_parens_18.c`, `stix_parens_12.c`, `stix_parens_8.c`; `src/hal/NativeHal.cpp`; `src/input/KeyCodes.h`; `src/math/CursorController.cpp`, `CursorController.h`, `MathAST.cpp`; `src/math/font/StixParentheses.h`, `stix_parenthesis_ink.h`; `src/ui/MathRenderer.cpp`; `wasm/numos-keypad.js`.

Pruebas/documentación (10): este informe; `scripts/compare-math-brackets.py`, `test-calculation-brackets.py`, `test-calculation-breaking.py`, `test-math-spacing.py`; `tests/fixtures/calculation-breaking.json`, `math-brackets.json`; `tests/host/math_brackets_checks.cpp`, `math_spacing_probe.cpp`; `tests/wasm/calculation-brackets.mjs`.

Los demás archivos ya modificados en el working tree pertenecen a las reparaciones anteriores. El manifiesto incremental permite distinguirlos. Los fallos BR-02…BR-05 quedan documentados para una tarea posterior, sin parche, cambio de goldens ni máscara nueva.
