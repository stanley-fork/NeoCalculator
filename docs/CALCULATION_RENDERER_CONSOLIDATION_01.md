# CALCULATION-RENDERER-CONSOLIDATE-01

Fecha: 27 de septiembre de 2026. Cierre de producto de Calculation, renderer y FORMAT. No inicia MATH-TEX-02.

La base funcional acumulada queda guardada en commits locales y contrastada con compilaciones aisladas, eventos de la aplicación y evidencia conservada. La prueba rápida pendiente de WebKit queda sustituida por observaciones separadas de procesamiento, transición y publicación. No se ha escrito la PCB ni publicado la web.

## Estado conservado y fronteras de los commits

El estado inicial era `main`, HEAD `ffb7082ba3b7db7741a8a603a5ad83f40c6c2682`, índice vacío, 37 archivos versionados modificados y 51 nuevos. Los **88 archivos cambiados** incluyen entregas anteriores al manifiesto posterior de 66 archivos; ese manifiesto no se utilizó como inventario completo.

Antes de editar se guardaron `index.patch` y `working-tree.patch` binarios y separados, los archivos nuevos con SHA-256 y un candidato legible completo: **1.351 archivos, 52.336.895 bytes**. Se verificaron los bytes de cada copia. El parche de índice tiene 0 bytes; el del working tree, 276.047. También se comprobaron los seis ZIP históricos de fuentes y sus CRC, sin sustituir por ellos el código final.

Evidencia: [preservación](../out/calculation-renderer-consolidation-01/preservation/manifest.json), [mapa de cambios](../out/calculation-renderer-consolidation-01/change-map.json), parches y `new-files/` en ese mismo directorio. El estado de worktrees inicial se conserva en `preservation/worktrees.txt`; se dejaron intactos los worktrees anteriores, incluido uno ya marcado como prunable.

La reparación de entrada no se reconstruyó ni revirtió. Su fingerprint histórico `5f499072f14891e0df0b40f3532ad6ad68cea723de70094d057b8e6814637647` sigue ligado a la entrega CALC-CORE-INPUT-01, no al árbol posterior completo. Los archivos ejecutables del candidato inicial coincidían con la copia que produjo los últimos artefactos ordinarios documentados.

`.vscode/settings.json` conserva SHA-256 `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`. No se detectó trabajo ajeno entre los 88 cambios. No se usaron stash, reset, clean, staging global, push ni reescritura de commits.

| Commit local | Responsabilidad y validación aislada |
|---|---|
| `0f48c8eb5b9d8e49a7d195918daf14c4e9fdb3d7` — `feat(calculation): consolidate input, renderer and exact result formatting` | Conserva juntos los 88 cambios acumulados A–D. Árbol A creado desde el ancestro y la copia verificada; native, WROOM y CAM con directorios propios, entrada, geometría, resultados, FORMAT, corchetes y viewport aprobados. |
| `2c79d3df81097bd3504463cb18e9e471ae5b466f` — `fix(web): synchronize Calculation input and published frames` | Frontera web, compilación SDL desde SDK vacío, sincronización, secuencias rápidas y medición privada. Árbol B independiente de A, compilaciones ordinarias propias y matriz combinada descrita abajo. |
| Cierre documental posterior — `docs(calculation): record consolidated renderer evidence` | Este informe. Árbol C separado, mismo código ejecutable que B, build native propio y puerta de entrada. Su hash definitivo se registra en `out/calculation-renderer-consolidation-01/final-state.json` para evitar una referencia circular dentro del propio commit. |

No se dividieron A–D por fechas: MathAST, CursorController, MathRenderer, CalculationApp, serialización y pruebas comparten cambios posteriores que sustituyen partes de las primeras reparaciones. Separarlos en cuatro revisiones habría exigido inventar estados intermedios. Se conserva una primera base funcional completa y una segunda corrección acotada reproducida durante el cierre.

| Grupo lógico conservado | Contenido |
|---|---|
| A. Entrada | NEG/NEGATE, base 10 real, vacíos, edición/borrado/unwrapping, operandos con signo agrupados, mapa web y puerta rápida. |
| B. Composición horizontal | Clases efectivas contextuales, estilos, avances proporcionales, posiciones compartidas y menos U+2212 STIX. |
| C. Corchetes y viewport | Agrupación escalar, variantes/ensamblaje STIX, dibujo/cursor, desplazamiento horizontal y vertical acotado. |
| D. Resultados y FORMAT | Factorial, Ans/PreAns/A–F exactos en sesión, dominio/simplificación, exacto/aproximado, capacidades SCI/ENG/FIX y complejos, menú/BACK, placeholders finales y tildes. |

La copia legible `final-source/` y su manifiesto conservan los 1.354 archivos versionados del commit de código B. El contraste con B solo exceptúa `compile_commands.json`: la copia aislada contiene la base de órdenes generada con sus rutas de build; no es una entrada del compilador y la copia versionada principal permanece intacta. La comparación está en `isolated-final-equivalence.json`.

## WebKit: causa observada y cierre

La evidencia histórica de FORMAT_PRESENTATION_02 sigue identificada como histórica. La ejecución antigua, sin instrumentación nueva, pasó en esta máquina; otras cuatro repeticiones trazadas también pasaron. No se presenta ese resultado como reproducción espontánea del fallo anterior.

El reproducer `tests/wasm/calculation-publication.mjs` detiene temporalmente la planificación de frames del navegador en una frontera explícita y la reanuda. No cambia fórmulas, modos, resultados ni llama a `applyFormat`. Usa teclas y clics reales. En WebKit, shell y componente:

1. FORMAT se procesa, pero a los 40 ms sigue publicada la imagen anterior mientras el dibujo está pendiente.
2. El navegador entrega `pointerdown` y `pointerup`; a los 80 ms SDL todavía no ha leído su cola y el menú sigue visible.
3. Tras reanudar los frames, SDL/LVGL lee la pulsación y su liberación, termina la selección y publica la nueva imagen.

Esto demuestra que las pausas antiguas no acreditaban esas fronteras. No demuestra pérdida espontánea de eventos ni justifica atribuir cualquier fallo del producto al test. Se conservan trazas, tiempos, imágenes tempranas y posteriores en `publication-baseline-v3/` y `publication-final/`. Los intentos previos de reproducción también se conservan.

El puente C++ expone observación de solo lectura: número/hash ordenado de flancos procesados, clics completados, estado del menú, transición, invalidación pendiente y frames realmente publicados después de `SDL_RenderPresent`. El contador de vueltas del loop no se trata como contador de imágenes. La consulta de invalidación usa las estructuras de la versión fijada de LVGL 9.5; no fuerza un refresco.

`calculation-driver.mjs` utiliza esas fronteras y plazos máximos de 10 s, más dos oportunidades de composición del navegador. No introduce un sleep global mayor. Comprueba independientemente pérdida, duplicación y orden de PRESS/RELEASE. El recorrido funcional y cinco ráfagas por superficie permanecen separados; cada selección por puntero se compara con su equivalente por teclado.

Se reprodujo además un defecto concreto de la API pública: el catálogo anunciaba códigos hasta FORMAT_MENU=82, pero JavaScript limitaba a 79 y C++ a RBRACKET=81. El paquete previo rechaza 82; el candidato lo acepta. El límite JavaScript se deriva ahora del catálogo auditado y el puente C++ admite su última entrada. Se conserva el detector del catálogo completo. Las pruebas principales siguen pasando por SHIFT→ALPHA→FORMAT y por el puntero.

La compilación limpia detectó otro defecto acotado: `-sUSE_SDL=2` se declaraba solo al enlazar y dependía de que las cabeceras del port ya estuvieran en la caché. Se declara también al compilar el ejecutable web. No se cambió la versión de SDL ni las dependencias del producto.

Un control general del componente falló una vez en Chromium al leer la navegación de teclado después de 60 ms. Se sustituyó esa espera por la transición observable y dos iteraciones de procesamiento SDL, manteniendo la aserción de foco. La repetición final pasa en los tres navegadores. Se conserva como fallo inicial del harness, separado del fallo histórico de FORMAT.

Resultado: Chromium 139.0.7258.5, Firefox 140.0.2 y WebKit 26.0 pasan en **shell y componente** con el mismo C++/Wasm. `web-formats-v2/events.json` conserva las acciones y sus fronteras; `results.json` conserva los 42 registros visuales de formato. Las seis jornadas incluyen también menú, ENG, puntero y las ráfagas; no se suman sus repeticiones como casos únicos.

## Puertas del candidato combinado

El [ledger completo](../out/calculation-renderer-consolidation-01/ledger.json) indica snapshot, resultado, ejecución nueva o reutilización, alcance y excepción. Los comandos y fechas de las compilaciones/puertas agrupadas están en `builds/` y `gates/`. Las trazas y replays se copiaron a `evidence/` para no depender de la vida de la caché de compilación.

| Puerta ejecutada ahora sobre B | Resultado y alcance |
|---|---|
| Entrada cotidiana | 114 escenarios; rutas lógicas y de producción, controles negativos, reparación de potencia, entrada científica, división agrupada, plantillas e historial. Primero se ejecutó la puerta rápida antes de modificar código. |
| SDL de escritorio | Se conserva `tests/emulator/calculation-desktop-recovery.numos`, ejecutada dentro de su entrada previa a Calculation. También pasa a través del launcher final. |
| Signos/serialización y fallos de asignación | Editor/serializer y 33 posiciones de fallo persistente. |
| Espaciado/medidas | 151.539 checks, corpus de 36 fixtures, clases/estilos/anchuras/cursor y ausencia de asignaciones propias en layout. Límites 63/64/65, 399/400 y fila host de 2.000 elementos. |
| Corchetes | 20 sesiones por eventos y 1.645 checks de geometría; apertura/cierre, ranuras, historial y agrupación escalar. |
| Viewport | Siete detectores por eventos y 2.553 checks; cursor alto, 2^200, decimal periódico, desplazamiento excesivo y regreso. |
| Resultados | 67 casos en cuatro rondas, 11 detectores que fallaban antes y 38 oráculos numéricos; factorial, raíces racionales, simplificación, dominio logarítmico y Ans complejo. |
| FORMAT | 28 sesiones por eventos y 911 checks host. Añadidos fracción mixta negativa, x+i simbólico, FIX negativo en empate, SCI/ENG conservando Ans y dispatch físico SHIFT+FORMAT→TABLE. |
| Placeholders/fuentes | 217 checks y recorridos EN/ES con las fuentes reales del menú; entrada vacía sin caja, ranuras pendientes visibles, EXE e historial. |
| Tutor/composición | 900 trazas comparadas: cero diferencias salvo `micros`, `warmSolveMicros` y ruta de `command[0]`, tras comprobar el nombre del ejecutable. Reglas, condiciones y resultados siguen comparados. |
| Catálogo y Steps | 247 entradas EN/ES, 1.593 checks, detector de traducción ausente, selector real y conservación EN→ES→EN de páginas, AST y scroll. |
| Recuperación | 1.280 inyecciones del tutor, 320 de resultados periódicos y 152 escenarios de fallo persistente en Steps/reflow/Results; recuperación posterior saludable. |
| Pool | 50 ciclos de Calculation y 50 aperturas de FORMAT, con reserva LVGL de 64 KiB. No se amplió el pool. |
| Apps compartidas | Seis controles de fracciones en Equations, Calculus y Grapher; Calculation entrada/resultado y Steps cubiertos por sus puertas específicas. |
| Notación/STIX | Notación, regresión de fases y nueve capturas/medidas de glifos aprobadas. Sin promoción de goldens. |
| Keypad | Suite de 50 mappings; scanner, cola y modificadores. La prueba por eventos confirma además TABLE. |
| Web | Entrada cotidiana, recuperación, FORMAT y componente en los tres navegadores; shell y componente. |
| WASM-MATH | Builds limpios Release y Debug, paquete y regresiones en los tres motores. |
| Plataformas | Native SDL, WROOM ordinario y CAM ordinario, con directorios independientes. |
| Integridad | Diff de cada índice revisado, `git diff --check`, hashes de copias, recursos relativos y CRC del ZIP visual. |

No se suman las repeticiones A/B, los ciclos o los checks de una misma fixture para anunciar un total de casos únicos. La comparación histórica contra la reparación de entrada sigue conservada; el cierre no relabela todas sus ejecuciones como nuevas.

Los cambios matemáticos intencionados —factorial, exactitud en sesión, simplificación y dominio— pertenecen a la reparación acumulada. La capa de cierre no cambia Giac ni la matemática. Los cambios tipográficos y de viewport siguen diferenciados de esas correcciones de resultado. El tutor conserva las 900 trazas.

## Coste final, propiedad y memoria

`scripts/measure-calculation-renderer.py` construye copias privadas instrumentadas de MathAST, MathRenderer y CalculationApp a partir de la base de órdenes del native correspondiente. No enlaza la sonda en el producto. Los replays introducen expresiones mediante eventos; los comandos adicionales solo delimitan la medida y leen propiedad/contadores.

Se midieron 27 combinaciones de carga/acción, tres veces cada una: 81 registros. Los tiempos siguientes son medianas **host instrumentadas**, con compilaciones concurrentes; incluyen sobrecoste de observación y no son latencia ESP32. «Dibujo» excluye el tiempo de layout anidado. Los conteos de glifos separan layout, dibujo y otros consumidores de UI.

| Acción emparejada | Layouts raíz | Glifos layout / dibujo | Draws | Layout / dibujo exclusivo, µs |
|---|---:|---:|---:|---:|
| DEL y reentrada en 2+2 | 4 | 10 / 20 | 2 | 42,8 / 41,4 |
| DEL y reentrada en fracción científica | 4 | 42 / 84 | 2 | 122,8 / 119,6 |
| DEL y reentrada en exponente anidado | 4 | 18 / 36 | 2 | 70,2 / 55,3 |
| DEL y reentrada, 150 términos | 4 | 1.194 / 2.388 | 2 | 2.251,2 / 2.483,7 |
| Parpadeo, 150 términos, ventana de 80 frames | 2 | 598 / 2.392 | 2 | 1.320,1 / 2.533,8 |
| Izquierda/derecha, 150 términos | 4 | 1.196 / 2.392 | 2 | 2.338,1 / 2.296,0 |
| Exacto/decimal y regreso, historial 0.1 | 10 | 30 / 48 | 4 | 72,3 / 60,3 |
| Pan de 2^200, ocho avances y ocho retornos | 12 | 390 / 1.560 | 12 | 622,1 / 1.347,6 |
| Pan periódico de 1/97, misma secuencia | 12 | 606 / 3.024 | 12 | 1.116,8 / 3.439,3 |
| Apertura/cierre de menú sobre 2^200 | 4 | 130 / 520 | 4 | 235,7 / 541,4 |

Hay trabajo redundante confirmado: parpadeo y navegación vuelven a medir la fila sin modificarla. `evidence/profile-paired/cost.numos`, `cost.log` y `cost.json` son el reproducer y la medida. No se introdujo caché global, árbol paralelo ni otro contrato de invalidación. Los 150 términos están dentro del contrato vigente; el microbenchmark final también mantiene crecimiento lineal a ambos lados de 64/65. No se compara su porcentaje con el de otra máquina.

No se observaron asignaciones de nodos en parpadeo, navegación ni pan. La edición asigna un nodo y el cambio de representación puede preparar árboles; estas operaciones no se confunden con layout. Los contadores de nodos no pretenden medir las asignaciones internas de Giac ni las colas de LVGL.

La prueba de propiedad realiza **160 evaluaciones en la misma app activa**, rotando decimal exacto, fracción mixta, complejo polar, 2π y ENG. A partir de la saturación:

- Historial: 50 entradas, capacidad del vector 64; 50 árboles exactos y 50 aproximados.
- 570 nodos del historial; 578 nodos vivos totales, 46.752 bytes de payload de nodos host. Propietarios únicos y enlaces parent correctos.
- Un contexto Giac y cero handles retenidos; reemplazo de entradas antiguas sin aumento de nodos.
- Memoria privada del proceso: 113.131.520 B en ciclos 60/100 y 113.119.232 B en 110/150/160. Es memoria de proceso Windows, no heap ESP32 ni una medida separada del malloc interno de Giac.

La retención legítima del historial no se exige a cero al volver a HOME. Los ciclos con destrucción de app del perfil native se registran por separado de esta prueba de retención. En el pool fijo, los retornos comparables conservan 67 objetos, tres timers y 19.336 B libres según el monitor; sus contadores internos no reemplazan el tamaño reservado de 65.536 B.

Medida final con herramientas Xtensa, en `final-resources.json`:

| Recurso | WROOM | CAM |
|---|---:|---:|
| RAM estática | 119.200 B | 117.792 B |
| `.iram0.text` | 60.407 B | 59.039 B |
| `.iram0.vectors` | 1.027 B | 1.027 B |
| Flash enlazada informada | 5.707.077 B | 5.619.537 B |
| Imagen ordinaria | 5.707.440 B | 5.619.904 B |

Tamaños ESP32 finales: MathNode 44 B, NodeRow 56 B, NodePower 56 B, LayoutResult 16 B, FontMetrics 44 B, MathCanvas 184 B, CursorController 12 B y CalculationApp 840 B. No se extrapolan los tamaños host, donde MathNode es 56 B y MathCanvas 256 B.

Frames propios Xtensa finales: RowLayout medida/clasificación/colocación 32/48/64 B; NodeRow layout 48 B; medida de texto 192 B; cursor 96 B; drawRow 64 B; dibujo de texto 336 B y placeholder 208 B. Se obtuvieron del desensamblado final, no sumándolos como si fueran simultáneos. No se midió el pico físico total de stack/heap ni la latencia del dispositivo. Las observaciones nuevas del navegador están protegidas por `__EMSCRIPTEN__`; no añaden estado de diagnóstico al firmware ordinario.

## Identidad de artefactos y alcance físico

| Artefacto | Bytes | SHA-256 |
|---|---:|---|
| Windows anterior, preservado | 16.849.320 | `51caddd27818b1588abd0b1644d457f810d56cc4299553b66685e9423ab27dd6` |
| Windows ordinario preparado en el launcher | 16.849.320 | `8600ff3f70e3b3b6f27c405ff75d96e38bcd33448aa4077af81f643304970819` |
| WROOM última instalación documentada | 5.707.408 | `251ded7b3264b89b674c5a0495f09a155fa07816610d607873c9296bf39d95c1` |
| WROOM recompilado B, **no instalado** | 5.707.440 | `3ff7dcbd6309f78600767c2915d96ef8a83dc4921edb5be9a7d4d951f54fb6fb` |
| CAM recompilado B, **no instalado** | 5.619.904 | `3c75c8c3b13dade0ad2b0a8b91d1cb9b9b882d63ced330eaa4bbb28352975205` |
| ZIP web preparado, **no publicado** | 2.400.124 | `506ab3fa6f62dead6400da3634582f5db0563e51bce746c28f424ee48f608905` |
| ZIP visual final | 178.845 | `b0ea884a54526e1e31aa4707826429bf9d9c7dd8cd6193606ebabe1fdd8c96d4` |

El launcher usa `C:/.piobuild/numOS/emulator_pc/program.exe`. Se guardó su binario anterior antes de copiar el ordinario comprobado. Arranca mediante `scripts/run-emulator-windows.ps1`; la reproducción SDL final pasó desde esa ruta. Un primer intento lanzó el fragmento de recuperación sin su precondición de entrar en Calculation: ese error de invocación está conservado en `launcher-final.log`, y la ejecución completa correcta en `launcher-recovery-final.log`.

Herramientas: PlatformIO 6.1.19, SCons 4.8.1, espressif32 6.12.0, Arduino ESP32 2.0.17, Xtensa 8.4.0+2021r2-patch5, MinGW 15.2.0, LVGL 9.5.0, TFT_eSPI 2.5.43 y Emscripten 6.0.3. Las cachés anteriores de SCons/SDK/dependencias habían desaparecido; se restauraron las mismas versiones en rutas aisladas. No se editaron manifiestos de dependencias, clocks, particiones ni seguridad.

Las diferencias binarias se investigaron: los 78 objetos native y 174 objetos WROOM de la aplicación conservan sus secciones de código/datos frente a la última copia ordinaria. Giac y tommath native conservan las secciones de sus miembros. LVGL contiene nuevas rutas `__FILE__`; al excluir exclusivamente los addends de dirección de relocaciones COFF, sus instrucciones y referencias simbólicas coinciden. En WROOM, 87 de 786 objetos de bibliotecas/framework difieren en datos de solo lectura y literales de direcciones, sin diferencias en secciones de instrucciones. Se identificaron las rutas de LVGL/TFT y la cadena del framework `firmware_msc_fat.c`: `Sep 25 2026 22:10:42` frente a `Sep 27 2026 17:28:18` en A. No se atribuye todo el hash al timestamp del archivo. Se conservan comparaciones de objetos, relocaciones, cadenas, secciones y tamaños. No se afirma reproducibilidad binaria byte a byte entre directorios.

El paquete web conserva honestamente la identidad con la que se compiló, `0f48c8eb5b9d-release-dirty`: árbol aislado A más el delta que posteriormente se guardó como B. `web-prepared-identity.json` liga sus hashes a `2c79d3d…` y a los archivos verificados; no se alteró a mano su identidad interna. Su Wasm es `eba2f8a9390e758f49de441060106348b684324fe8389b2f4d4f09295c57a54d` (7.106.754 B). El manifiesto completo está en `web-prepared/numos-assets.json`. No representa un despliegue público.

La identidad instalada continúa siendo la documentada en FORMAT_PRESENTATION_02. No hubo nueva medición física ni lectura/escritura de flash. Los 440 eventos inyectados de la evidencia anterior no se convierten en pulsaciones humanas ni en una revisión completa del LCD. No se incluyen backups completos de flash en las entregas.

## Visuales, decisiones preservadas y pendientes

[Galería final](../out/calculation-renderer-consolidation-01/visual/index.html) y [ZIP visual](../out/calculation-renderer-consolidation-01/CALCULATION_RENDERER_FINAL_VISUAL.zip): 12 originales de 320×240 y ampliaciones ×4 nearest-neighbor. Recursos relativos comprobados y CRC correctos; sin redistribución de archivos de fuente. Se revisó directamente el contacto de signos/exponentes, corchetes, cursor alto, extremos de resultado, FORMAT EN/ES, exacto/decimal, ENG, vacío y ranuras. Son imágenes native, no una revisión humana del LCD.

Se preservan corchetes como agrupación escalar, raíz vacía de Calculation sin caja, todas las ranuras pendientes visibles, presentaciones del mismo valor canónico, complejos y símbolos exactos solo en sesión, procedencia angular conservadora, GRA como representación, controles ENG/FORMAT, límites de serialización y autoridad ordinaria de Giac. No hay frases nuevas visibles en la app. La galería incluye textos EN/ES.

Pendientes explícitos:

- `parMixed` mantiene su discrepancia histórica del detector estricto de tinta. No se tocó tolerancia, máscara ni imágenes; no se declara «todos los goldens aprobados».
- Grapher con pool fijo de 64 KiB conserva la excepción histórica de agotamiento LVGL. Sus recorridos CLIB no certifican ese perfil ni el heap físico ESP32.
- La densidad vertical de scripts muy anidados, el mínimo tipográfico y los límites geométricos extremos siguen fuera del cierre. No se afirma paridad TeX completa ni seguridad para anidación arbitraria del editor.
- El trabajo redundante al parpadear/navegar queda medido y reproducible. Su optimización necesita una decisión posterior de invalidación; no se abrió aquí una caché global.
- No se midieron nuevos picos físicos de stack/heap, latencia ni consumo eléctrico; tampoco se desplegó el paquete web.

Los archivos exactos de cada commit pueden consultarse con `git show --stat` y en `commit-a-paths.txt` / `commit-b-paths.txt`; el mapa de 88 archivos conserva la atribución inicial. El tercer commit contiene únicamente este informe. Los artefactos, cachés, ZIP, binarios, sondas y archivos del editor quedan fuera de los commits.

BASE_CALCULATION_RENDERER_CONSOLIDADA: SÍ
