# Calculation FORMAT 01

Esta entrega repara las cuatro familias de resultados documentadas en `CALCULATION_RESULTS_BREAKING_01.md`, los índices de raíces señalados por el usuario y el flujo de FORMAT/ENG. El menú muestra opciones pertinentes al resultado. El renderer sigue siendo C++/LVGL/STIX en native, firmware y Wasm.

## Estado conservado

Rama `main`, HEAD `ffb7082ba3b7db7741a8a603a5ad83f40c6c2682`, índice vacío. El working tree ya contenía CALC-CORE-INPUT-01, MATH-TEX-01 y las reparaciones posteriores de corchetes, cursor y viewport. Esta entrega parte de ese estado real, no del HEAD limpio ni del antiguo fingerprint de la primera reparación de entrada.

`out/calculation-format-01/baseline.json` conserva 1338 hashes; `before-source.zip`, `before.patch`, `before-program.exe`, `before-observer.exe`, `before-web.zip` y `before-resources.json` mantienen el baseline inmediatamente anterior. SHA-256 del ejecutable ordinario anterior: `ea5b7f29f4f70cee460acadf76722ccce9fda1525572e1a501512ef31cd5dee8`. El delta exclusivo y su manifiesto se entregan por separado del diff acumulado contra HEAD.

La puerta rápida de entrada se ejecutó antes de editar: 113/113. `.vscode/settings.json` conserva `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`. Sin staging, commit, stash, reset, cambios de dependencias, clocks, particiones, almacenamiento persistente, flash o despliegue.

## Interacción y opciones

- **FORMAT** alterna exacto ↔ decimal. Desde cualquier formato secundario vuelve al exacto. El valor canónico y Ans no se obtienen del texto dibujado.
- **SHIFT + ALPHA + FORMAT** abre el selector. Flechas eligen, EXE aplica y BACK cierra. Se admite pulsar una fila en las superficies con puntero. SHIFT+FORMAT mantiene TABLE; no se añade pulsación larga.
- **SHIFT + ×10^x = ENG** muestra ingeniería. Repetir desplaza el exponente en pasos de −3; LEFT lo devuelve en pasos de +3. Los desplazamientos adicionales están acotados a seis grupos por sentido. FORMAT recupera el exacto.
- El selector usa cinco filas visibles como máximo, altura adaptativa, foco azul, marca del formato activo, contador y scrollbar. No cambia fuentes ni añade frases sin EN/ES. Usa las fuentes de interfaz existentes; las fórmulas conservan STIX.

| Resultado | Opciones pertinentes |
|---|---|
| `0.1`, `1/2` | Exacto/fracción, decimal, periódico/extendido cuando corresponde, SCI, ENG, FIX. Sin RAD/DEG/GRA ni polar |
| Fracción impropia | Además, fracción mixta; el signo afecta a todo el número mixto |
| Entero de magnitud 2…10¹² | Además, factores primos; coste interactivo acotado |
| `2π`, radicales | Exacto por defecto, decimal y formatos numéricos cuando hay un compañero escalar |
| Complejo numérico | Cartesiana, decimal, polar/fasor `r ∠ θ`, exponencial `r·e^(iθ)` |
| Fase polar | RAD, DEG o GRA para la fase; exponencial siempre usa radianes |
| Resultado angular de trigonometría inversa | RAD, DEG, GRA; por ejemplo `asin(0.5)` pasa de `π/6` a `30°` o `100/3 gon` |
| Expresión simbólica | Exacto y aproximación de sus componentes, si Giac proporciona el árbol correspondiente; sin conversiones numéricas inventadas |
| Entrada incompleta o error | No se abre un selector de resultados |

**FIX** elige 0…9 decimales; redondeo decimal determinista, empates alejándose de cero, acarreo y supresión de cero negativo. Ejemplos: `9.995 → 10.00`, `−0.005 → −0.01`, `−0.004 → 0.00`. SCI conserva la precisión del compañero numérico de Giac; no añade todavía un ajuste independiente de cifras significativas. ENG separa mantisa y exponente con dígitos, sin reconvertir a `double`.

Las unidades de salida no cambian la configuración RAD/DEG de entrada. Un recorrido acotado del AST identifica resultados angulares por trigonometría inversa, signos, agrupación, suma compatible y productos/cocientes escalares. `sin(...)` produce una razón, no un ángulo; las funciones hiperbólicas tampoco adquieren unidades angulares automáticamente. No se infiere una unidad del valor `1/2`, `π` o `30`.

La anotación es conservadora: no es un sistema general de unidades. No propaga dimensión angular a través de Ans/A-F ni interpreta cualquier potencia como una magnitud angular. El historial sí conserva la procedencia angular y la unidad de evaluación. Polar/exponencial se ofrecen a complejos numéricos; las expresiones con variables libres como `x+i` conservan su representación simbólica.

## Reparaciones matemáticas

| Detector | Antes | Ahora |
|---|---|---|
| `5!`, `0!`, `3!+2`, tecla física | 5, 0, 5 | 120, 1, 8 |
| `5 EXE; √(−1) EXE; Ans²` | 25 | −1 |
| `27^(1/3)` | raíz sin reducir | 3 |
| `2x+3x` | suma sin recoger | 5x |
| `sin(1)²+cos(1)²`, identidad con x | sin reducir | 1 en ambos casos |
| `log₁(2)`, `log₁(0.5)`, `log₀(2)` | infinito/infinito/0 con OK | Undefined |

La tecla factorial ahora inserta un nodo postfix real y conserva DEL/unwrapping; sobre un resultado prepara `Ans!`, que se evalúa con EXE. La factorización prima pertenece al menú FORMAT. Los cinco casos históricos `factor-*` del corpus conservan sus teclas anteriores como evidencia del cambio de interacción; ya no se interpretan como pruebas de factorización tras pulsar FACT sin EXE.

Calculation opta por evaluación exacta de decimales escritos: `0.1` se serializa para Giac como una razón de enteros, sin racionalizar un valor binario redondeado. La serialización pública de diagnóstico conserva su contrato anterior. La reducción exacta la realiza Giac; el layout nunca simplifica ni cambia el AST escrito. La simplificación automática evita expandir la respuesta y no cancela denominadores simbólicos: `(x²−1)/(x−1)` mantiene su restricción implícita. Las bases 0 y 1 de `logb` se validan antes de que Giac transforme el logaritmo en un cociente.

Se conserva una pareja de árboles tipados exacto/aproximado, también cuando existe espejo `ExactVal`. Los números en notación `e` se representan como mantisa × potencia de 10; la prueba de notación ahora cuenta sus multiplicaciones internas explícitas. No se ha cambiado la forma de los glifos.

Ans/PreAns y A-F mantienen en sesión el texto exacto validado por revisión y snapshot del almacén numérico. Un complejo o simbólico deja de reutilizar el escalar anterior. Esos valores no representables en `ExactVal` son **solo de sesión**: no se modifica el formato de fichero persistente ni se guarda un escalar falso. x/y/z conservan su papel de símbolos libres en Giac.

Un detector adicional reveló que `arg(1+i)` en contexto DEG podía dar una fase decimal prematura. El formato usa el `ScopedRadianMode` ya existente dentro de una llamada protegida de Giac, restaurando el contexto al salir; por eso la exponencial conserva `π/4` tanto con entrada RAD como DEG. No se cambia globalmente el modo angular.

## Raíces y geometría compartida

Había dos defectos distintos. El índice de una raíz de grado 16 o 128 se levantaba usando em en lugar de la altura del radical y no reservaba adecuadamente su anchura. Las raíces anidadas se contaban con sus filas envolventes y emitían paréntesis redundantes: entradas alrededor de veinte raíces podían acabar como fallo sintáctico del parser.

`NodeRoot` conserva origen del radical, origen/baseline del grado y ascenso del radical. La medida reserva el índice completo y el dibujo/cursor usan esos mismos campos. El ascenso del índice amplía la caja sin mover artificialmente el radicando. Se corrige además el extremo inclusivo de la barra. Las raíces de grado elevado de bases positivas conocidas se presentan como raíces; la potencia canónica de Giac permanece intacta.

`RadicalDegreeBottomRaisePercent` se aplica sobre ascenso + descenso del radical, según la [especificación primaria OpenType MATH](https://learn.microsoft.com/en-us/typography/opentype/spec/math). Adaptación explícita: NumOS conserva aquí su radical vectorial existente; su bearing no es el de un glifo radical STIX. Se limita el kern negativo del grado a la envolvente real del gancho, con separación derivada de mu. No se introduce otra fuente ni se afirma paridad completa OpenType.

La serialización cuenta profundidad semántica y, por separado, frames C++ acotados. Mantiene 400 nodos, 2000 bytes y profundidad semántica 40; la guardia de frames es 82. Para el control `√…√2`, 39 raíces evalúan; 40 y 41 exceden ese presupuesto y muestran **Expression limit / Limite de expresion**, no Syntax ERROR. No se promete anidamiento ilimitado. Continúan pendientes la densidad visual de scripts extremos y otras métricas verticales ajenas al grado del radical.

## Validación

Evidencia en `out/calculation-format-01/` y builds privados en `C:/.codex-cache/calculation-format-01/`. Los observadores solo leen estado; no se incorporan al firmware ni al Wasm.

- Entrada: baseline y candidato 113/113, secuencia completa `calculation-desktop-recovery.numos`, eventos SDL reales, placeholders e historial. También se ejecutó la reproducción en SDL visible con backend Direct3D; no es una inspección del LCD.
- Resultados: 67 sesiones de teclas; **11 detectores fallan en el baseline y pasan en el candidato**, 38 oráculos numéricos independientes, controles de dominio y Ans simbólico. Se verifica también STO A complejo y PreAns al pasar de complejo a real. `check-calculation-result-recovery.py` comprueba valores, no que el recorder termine.
- FORMAT: 23 sesiones native, capacidades permitidas/prohibidas, AST dibujado para conversiones conocidas, ambos modos de entrada, conservación del texto canónico y restauración de los píxeles exactos al volver.
- Geometría/formato: 911 checks, cuatro estilos y varios grados; inyección de fallo al insertar factorial; cero asignaciones propias durante layout/dibujo de esos casos.
- Espaciado: 151539 checks; corchetes: 1645; notación/STIX y regresión de fases pasan. Los siete detectores de viewport siguen reparados; seis recorridos de Equations/Calculus/Grapher pasan.
- Tutor: 900 trazas sin diferencias matemáticas contra el baseline inmediato; se excluyen únicamente campos de tiempo y ruta del ejecutable. 1593 checks i18n, catálogo de 247 mensajes EN/ES, 1280 fallos de asignación del tutor y 320 de resultados periódicos. Se recorre además Steps en EN/ES.
- Pool fijo de 64 KiB: 50 ciclos de entrada y otros 50 ciclos de selector/mixta/FIX/cierre; HOME conserva objetos, timers, handles y memoria utilizable. La cifra `pool_total` de LVGL excluye metadatos del allocator y no equivale a `LV_MEM_SIZE`.
- Web local: FORMAT/ENG/FIX/complejos/ángulos, recuperación de entrada y viewport en Chromium, Firefox y WebKit, shell y componente. Seleccionar polar con puntero produce los mismos píxeles que elegirlo con teclas. El harness espera la transición existente del launcher antes de comparar píxeles; no se amplía una máscara para ocultarla. Un arranque Firefox agotó el timeout en la última repetición conjunta; la repetición aislada pasó ambas superficies.
- Compilan native normal, native con pool fijo, WROOM y CAM; Wasm compila, empaqueta, valida y pasa smoke. Keypad físico y catálogo web de teclas conservan su correspondencia; SHIFT+FORMAT sigue TABLE.

No se promocionan goldens ni se amplían tolerancias globales. La galería conserva originales 320×240 y ampliaciones enteras nearest-neighbor, sin archivos de fuente. Su ZIP verifica los recursos HTML relativos. Las capturas son candidatos de revisión.

## Coste medido

Medición con el compilador Xtensa, respecto al snapshot anterior:

| Recurso | Delta |
|---|---:|
| WROOM flash text / rodata | +13012 / +2756 B |
| CAM flash text / rodata | +13128 / +2752 B |
| DRAM estática | +40 B |
| IRAM / vectores | 0 B |
| `NodeRoot` | 64 → 72 B |
| `CalculationApp` | 736 → 840 B |
| `VariableManager` | 800 → 840 B |
| `MathNode`, `NodeRow`, `NodeFunction`, `MathCanvas` | sin cambio en esta entrega |

Frames propios: raíz 80 B, fila 48 B, cursor 96 B, apertura del selector 64 B, conversión de formato 528 B, anotación angular 48 B por nivel. Esta última se corta después de 40 niveles, alrededor de 2 KiB propios antes de llamadas auxiliares. Layout conserva su recursión anterior; estas cifras no son el pico total de pila. Imágenes WROOM/CAM: 5707184 / 5619664 bytes.

No hay asignaciones nuevas por frame en layout/draw/cursor. Abrir el menú crea objetos LVGL en el pool y convertir un resultado crea AST/cadenas fuera de esas rutas. Retener ahora ambos árboles tipados puede aumentar memoria de resultado/historial: no se declara heap delta total cero. No se ha medido el pico físico de heap ni la latencia en ESP32. Los números host no se atribuyen a la PCB.

## Referencias y límites

Las opciones toman como referencia la documentación oficial de [FORMAT de Casio](https://support.casio.com/en/manual/004/fx-570CW_991CW_EN.pdf), [ENG](https://support.casio.com/global/en/calc/manual/fx-570CW_991CW_en/changing_calculation_result_format/engineering_notation.html) y [FIX/SCI](https://support.casio.com/global/en/calc/manual/fx-991CW%2BUK_en/calculator_apps_and_menus/using_the_settings_menu.html). Son referencias de interacción; no se afirma identidad de funcionalidades o resultados entre motores. La separación entre fase y unidad se corresponde con la [definición de fase principal](https://dlmf.nist.gov/4.2.E5).

Quedan explícitamente fuera: persistencia compleja nueva, sistema general de unidades, GRA como modo global de entrada, ajuste de cifras SCI, prefijos SI, DMS y cálculo arbitrario más allá de los límites del motor. FIX conserva el valor canónico y limita su expansión a 256 dígitos enteros; las aproximaciones dependen de la precisión/rango numéricos actuales de Giac. No se presenta este trabajo como corrección de todos los posibles errores del CAS.

## Reproducción y archivos

```powershell
python scripts/test-calculation-format.py --bin OBSERVER/observer.exe --out out/format-check
python scripts/test-calculation-format-lifecycle.py --bin POOL/program.exe --out out/format-pool-check
python scripts/check-calculation-result-recovery.py --candidate out/calculation-format-01 --baseline out/renderer-recovery-02
```

El manifiesto `out/calculation-format-01/delta-manifest.json` enumera exactamente los archivos exclusivos de esta entrega frente al snapshot conservado. El ZIP visual y `visual/index.html` están en ese mismo directorio. El ejecutable validado se copia al directorio que resuelve `scripts/run-emulator-windows.ps1`; el paquete web se prepara localmente. **No hay publicación ni flash.**

### Lista exacta del delta

```text
docs/CALCULATION_FORMAT_01.md
docs/CALCULATION_RESULTS_BREAKING_01.md
docs/RENDERER_RECOVERY_02.md
scripts/build-calculation-observer.py
scripts/check-calculation-result-recovery.py
scripts/package-calculation-format-visuals.py
scripts/test-calculation-format-lifecycle.py
scripts/test-calculation-format.py
scripts/test-calculation-viewport.py
src/apps/CalculationApp.cpp
src/apps/CalculationApp.h
src/input/KeyCodes.h
src/input/KeySemanticResolver.cpp
src/math/CalculationEngine.cpp
src/math/CalculationEngine.h
src/math/CalculationFormat.h
src/math/CursorController.cpp
src/math/CursorController.h
src/math/MathAST.cpp
src/math/MathAST.h
src/math/MathEvaluator.cpp
src/math/VariableManager.cpp
src/math/VariableManager.h
src/math/cas/ASTFlattener.cpp
src/math/giac/GiacEngine.cpp
src/math/giac/GiacEngine.h
src/ui/MathRenderer.cpp
tests/host/calculation_format_checks.cpp
tests/host/math_notation_fixtures.h
tests/host/production_keypad_test.cpp
tests/wasm/calculation-format.mjs
tests/wasm/calculation-viewport.mjs
wasm/numos-keypad.js
```
