# TUTOR-ENGINE-03B — composición mediante una sustitución comprobada

Candidato de implementación, sobre `main@e4f4dcaa679d83184052125814d27e4488b96c01` (padre `f98ff12da58a16342dea03a308a18a14827d044b`). Sin commit ni prueba física de esta fase. No se han modificado dependencias, particiones, clocks, teclado, STIX ni el pool LVGL de 65.536 B.

## Identidad y evidencia

El índice y el árbol estaban limpios. `.vscode/settings.json` sigue ignorado; SHA-256 original `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`. La preservación verificó los 1.286 archivos de `out/tutor-engine-03b/baseline-source.zip`, sus hashes y los parches binarios separados del índice y del árbol. No se reconstruyó el candidato desde un HEAD anterior.

Fingerprint inicial de fuentes: `363d110500bf5119b655e5b0a16cf970595c187b7000e2309ba43ce19bd5d063`; runtime: `add63b49ceaa6350b151bec43986398ec2b675c2cee4fc0f7fc09b06c67b3fed`. El firmware de referencia cerrado medía 5.621.408 B, SHA-256 `12e1277cf0e54c0db004312391d30dede901d67b50dc0571a7063d6282ef847d`; flash enlazada 5.621.041 B, RAM estática 119.160 B, IRAM 60.407 B. Son referencias históricas, no medidas físicas de 03B.

Las identidades finales, el listado exacto de48 archivos y los comandos/salidas se conservan en `out/tutor-engine-03b/final-manifest.json`, `changed-files.txt` y los directorios de evidencia citados abajo. Runtime final: `ddcdcdd5592e2a7056a9ed7b414239cd60dcdd612c11d9872a085d610ae38059`. Las copias de compilación están bajo `C:/.codex-cache/tutor-engine-03b/`, con directorios separados para native, pool fijo, firmware y WASM.

Herramientas: PlatformIO 6.1.19, SCons 4.8.1, Xtensa 8.4.0+2021r2-patch5, Arduino ESP32 3.20017.241212+sha.dcc1105b, LVGL 9.5.0, Giac 1.4.9+khicas.57. Host MinGW GCC 15.2, C++17; Emsdk/CMake fijados, comandos completos en los JSON de construcción. Los probes son privados y no se incluyen en firmware ordinario.

## Contrato y responsabilidades

`GiacComposition.inc` admite estructuralmente una igualdad original y certifica `P(U(x))`, con coeficientes racionales y grado auxiliar ≤2. No obtiene raíces. Se inspecciona el árbol antes de aplicar valores almacenados o normalizar; las divisiones variables y las bases con dominio oculto se rechazan antes de que puedan cancelarse.

`GiacTutorComposition.inc` planifica una única sustitución y reutiliza los planificadores polinómico, exponencial, logarítmico y trigonométrico. Cada hijo conserva su traza completa; el checker reproduce sus reglas, no llama a su planificador. La reconstrucción exacta de `P(U)` y el tratamiento de todas las preimágenes establecen completitud. Comprobar candidatos en el original es una comprobación adicional de validez, no su prueba de completitud.

El certificado pertenece a la derivación: ámbito derivado del snapshot, ID auxiliar, tipo de U, miembro original, definición, tres coeficientes, restricciones originales/auxiliares, hijos y preimágenes. `Snapshot.parentScope` y `childRole` distinguen el problema auxiliar de la vuelta a x. La letra t es presentación; se utiliza quoting con restauración al cruzar Giac, sin asignarla a VariableManager. Los snapshots raíz rechazan roles hijos. Se prueban colisiones t/u/k, generaciones, ángulos y épocas.

Las expresiones que salen del motor son valores propios, no punteros `gen`. La composición posee sus hijos mediante un certificado compartido por las copias de la traza; el checker rechaza composiciones anidadas/cíclicas antes de recorrer o contabilizar sus hijos. Las vistas conservan referencias con índice de hijo y procedencia de estado/paso. No comparten árboles MathAST mutables entre canvases.

## Soporte y límites

| Familia | U y recorrido | Implementado / frontera |
|---|---|---|
| Bicuadrática | `(a*x+b)^2`, t≥0 | a racional no nulo; cuarto y segundo grados compatibles; retorno ±raíz, cero una vez |
| Exponencial | `exp(a*x+b)` o base constante positiva ≠1; t>0 | relación comprobada `exp(2u)` / cuadrado; bases exactas compatibles, incluidos 4 y 2; no bases negativas ni reconocimiento flotante |
| Logaritmo | ln, log10 o logb de argumento afín | argumento>0 y base válida; t puede ser negativo; misma base/argumento en todas las apariciones |
| Trigonométrica | sin/cos/tan afín, RAD o DEG | seno/coseno en [−1,1]; tangente conserva cos(argumento)≠0; hasta cuatro familias completas |
| Aislamiento exterior | función afín en ambos miembros | suma/resta/división racional mediante reglas existentes, operación explícita antes del resultado; no sustituye la ruta corta ya aceptada |
| Objetivo algebraico | racional o radical cuadrático exacto | signo/rango mediante certificado racional de `r+s√q`, reconstrucción y comparación de cuadrados; Unknown fuera de la gramática acotada |

No se admiten denominadores variables, varias funciones independientes, identidades generales de ángulo doble, sustituciones anidadas, cuárticas generales, Lambert W, intervalos/inecuaciones, métodos numéricos ni nuevas composiciones complejas. `sin(x²)` no es `sin(x)²`. Una identidad auxiliar con dominio condicionado sigue siendo Unsupported; no se presenta como todos los reales. Las rutas complejas anteriores permanecen operativas.

Límites conservados: 48 pasos agregados entre padre/hijos, 64 KiB de traza retenida contabilizada, 128 KiB de payload temporal de vectores, 4.096 operaciones simbólicas contabilizadas. Nueva capa: profundidad uno, tres hijos como máximo (auxiliar y dos retornos), dos valores auxiliares, cuatro raíces/familias. Recorridos estructurales con límites de profundidad/nodos antes de expandir. Agotar el presupuesto da Partial, nunca Complete. Un caso repetitivo de logaritmo almacenado alcanza 4.097 en la sonda de límite y queda Partial; un problema sano posterior funciona sin reset de Giac.

El reconocimiento de signo no usa `evalf` como prueba. Se evita exclusivamente en los nuevos hijos la ruta del simplificador vendorizado que convierte ciertas raíces constantes anidadas en expresiones enormes fuera de la gramática acotada. El ejemplo `sqrt(1+sqrt(2))` del probe conserva exactamente el valor (la identidad de sus grandes enteros se comprueba), pero empeora su representación. No se afirma un error matemático de Giac. Se conserva la raíz estructuralmente y se reproduce el teorema de ambos signos. Reproducer: `sign_probe.cpp`, `nested-sign-probe.log`; representación de entradas/salidas: `representation-probe.log`. No se parchea Giac ni se cambian las respuestas ordinarias.

## Reglas y comprobador

| Regla estable | Comprobación independiente |
|---|---|
| `substitution.define` | readmisión completa del original, ámbito, U, coeficientes, restricciones y recorrido exactos |
| `substitution.rewrite` | ecuación auxiliar reconstruida desde los coeficientes ya certificados; mismo dominio |
| `substitution.solve_auxiliary` | snapshot hijo exacto y replay de todas las reglas del polinomio; conjunto finito completo o vacío |
| `substitution.reject_range` | valor auxiliar correcto, signo/rango exacto, motivo y estado rechazado coherentes |
| `substitution.pullback` | U y valor exactos, snapshot propio y replay completo de la preimagen |
| `square.preimage` | potencia realmente de exponente dos, objetivo algebraico cerrado y no negativo; dos signos o un cero; primitivas afines posteriores |
| `substitution.finish` | todos los valores auxiliares contabilizados, hijos únicos en orden, unión exacta, originales/restricciones, sin ramas pendientes |

Los identificadores exactos están en `ruleId()`; no se comparan fórmulas impresas para demostrar una transformación. El replay público empieza siempre en el paso cero: sus premisas inmutables se reutilizan en los pasos siguientes, evitando readmitir el original por cada regla padre. Los hijos mantienen su replay completo. Esta reducción de trabajo permitió conservar el presupuesto de 4.096 también en logb con A racional; no se han relajado verificaciones.

El checker detecta operandos, mensajes, procedencia, ámbitos, restricciones, ramas, relaciones, clasificación final y límites alterados. Los bool del generador no sustituyen al replay. Los rechazos permanecen en la evidencia aunque no se muestren como soluciones finales.

## Results independiente

El adaptador ordinario usa el original y el productor público all-trig de Giac. Comparte únicamente la admisión neutral, nunca hijos/raíces del tutor. Reconoce hasta dos parámetros de procedencia certificada en ramas distintas y hasta cuatro familias, sin clasificar identificadores por su nombre. Cada rama debe pertenecer a un parámetro reconocido; se reproducen las conversiones y se restauran quoting/modos tras los fallos.

La capacidad neutra sube localmente de dos a cuatro familias; se mantienen 16 KiB de payload, 512 nodos, profundidad32 y 64 clases residuales del comparador. Se revisan conversión, normalización, comparación, serialización WASM y presentación. Equivalent/Different/Unknown siguen separados de la validez del tutor. Las 129 trazas Complete del corpus nuevo conservan la respuesta ordinaria idéntica al desactivar el tutor; los casos periódicos completos concilian sus conjuntos exactamente.

Results y el final de Steps paginan dos familias por grupo, reutilizando los cuatro canvases. «Soluciones periódicas 1/2» y «2/2» indican que ambas páginas pertenecen al mismo conjunto, no que haya dos soluciones finitas. VAR cambia el grupo en Results; el pan de Steps conserva su contrato. BACK recupera Results; editar confirma invalidez, cancelar conserva el snapshot; HOME libera todo.

## Enseñanza y ejemplos completos

Se añaden 19 claves EN/ES: 247 claves totales, sin fallback español. Catálogo generado: `docs/TUTOR_I18N_ES_01_CATALOG.md`. No se han retraducido los mensajes históricos. Las cabeceras distinguen «Ecuación auxiliar», «Valor auxiliar N», «Caso N» dentro de una preimagen y «Ecuación para este valor auxiliar»; t nunca se anuncia como respuesta final de x.

Recorridos representativos (cada operación intermedia queda en las trazas JSON y capturas):

* `x⁴−5x²+4=0`: define t=x², t≥0 → t²−5t+4=0 → (t−1)(t−4)=0 → t=1,4 → x²=1 y x²=4 → dos signos en cada retorno → x=−2,−1,1,2, comprobados en el original.
* `x⁴−2x²−1=0`: t=x²≥0 → t=1±√2 → rechaza 1−√2<0 → x²=1+√2 → x=±√(1+√2).
* `e^(2x)−3e^x+2=0`: reconoce el cuadrado y define t=e^x>0 → t²−3t+2=0 → t=1,2 → e^x=1, e^x=2 → x=0,ln2 → unión y comprobaciones originales.
* `e^(2x)+e^x=0`: t(t+1)=0 → t=0,−1 → ambos incumplen t>0 → conjunto vacío, sin inventar retornos.
* `ln(x−1)²−3ln(x−1)+2=0`: x−1>0; t=ln(x−1) → t=1,2 → x−1=e, x−1=e² → suma1 a ambos miembros → x=1+e,1+e²; dominio retenido.
* `6sin(x)²−5sin(x)+1=0`: t=sin(x), −1≤t≤1 → t=1/2,1/3 → dos teoremas completos de seno → cuatro familias; α=asin(1/3) exacto, k entero, unión completa en dos páginas.
* `tan(2x)²=3`: cos(2x)≠0; t=tan(2x) → t=±√3 → 2x=±π/3+πk → divide offset y periodo entre2 → x=±π/6+(π/2)k.
* `2sin(x)+1=0`: resta1 a ambos miembros → divide entre2 → sin(x)=−1/2 → método trigonométrico anterior, sin mostrar una sustitución artificial.

Archivo portable: `out/tutor-engine-03b/review.zip` (HTML, catálogo y 1.012 PNG originales 320×240, todos los enlaces relativos comprobados). `ui-final` incluye 24 secuencias EN/ES, 368 páginas y sus desplazamientos verticales. `pan-final` completa tres relaciones largas de seno DEG, 33 posiciones por página/idioma, contenido accesible y retorno comprobado. Las capturas se han inspeccionado directamente.

Revisiones separadas: matemática (trazas y mutaciones; corrigió admisión de bases con dominio oculto, objetivos no cerrados, exponente incorrecto y ciclo de hijos), propiedad/recursos y enseñanza EN/ES. La revisión docente comprobó las 368 páginas estructuralmente y 38 hojas/4 imágenes en detalle, no cada PNG individual. La reserva sobre el extremo de tres fórmulas DEG se resolvió con capturas de pan inspeccionadas por el agente principal. No se atribuye al usuario revisión humana ni aceptación física.

## Regresión y recuperación

| Control final | Evidencia / resultado |
|---|---|
| 761 trazas históricas EN/ES | `invariance-accepted`: cero diferencias matemáticas; 163 conciliaciones conservadas |
| Corpus nuevo | `seed-accepted-ordinary`:96/0; `challenge-accepted-ordinary`:43/0; total129 Complete y10 negativas correctas |
| Checker de composición | 89 controles, 0 fallos; incluye mutaciones y límite real de presupuesto |
| Checkers anteriores | ABS/radical235 controles/23 mutaciones; exp/log386/25; trig889/166; adaptación/conjuntos2216 controles |
| Catálogo / fuentes | 247 claves completas; retirada aislada de ES detectada; i18n1593/0; notación y MathEnginePhaseRegression PASS |
| Fallos del motor | 1.280 inyecciones persistentes de construcción/retorno, y320 de respuesta ordinaria; recuperación posterior sin reset |
| Fallos de vistas | 150 de Steps y96 de Results; incluye unión de cuatro familias y fallo persistente en publicación |
| Vida útil | 50 ciclos mixtos, ocho métodos, EN/ES, edición/cancelación/BACK/HOME; muestras estables |
| Equations / Calculus / Grapher | flujos de edición, semántica física, F7 y Templates PASS; 50 ciclos propios de las suites existentes |
| Giac / cross-app | semántica ordinaria, Calculus, Neo y contexto cruzado PASS; excepción RSS descrita abajo |
| Native / firmware | emulator_pc y pool fijo PASS; WROOM normal, bring-up, demo y CAM normal/validate PASS (`variants.json`) |
| Web / WASM | build/package/validate/smoke y recorridos completos EN/ES en Chromium/Firefox/WebKit PASS; WASM-MATH Release/Debug PASS, incluidas cuatro familias y polos |

La invariancia solo excluye `steps[*].text` al comparar idiomas y mide por separado bytes/capacidades/tiempos/llamadas. Se enumeran las diferencias de diagnóstico de entradas que continúan Unsupported; no se ignoran estados, reglas, condiciones, candidatos, ámbitos ni completitud. Dos antiguas negativas ahora son completas de forma deliberada: `exp(x)+exp(2x)=3` y `sin(x)*sin(x)=1/2`; sus tests comprueban la nueva cobertura. El test de desbordamiento de familias pasa de tres a cinco y añade igualdad válida de cuatro clases.

Se conservan los primeros fallos: una primera ejecución Giac falló el umbral RSS de redibujado (4.984 KiB); repetición del mismo binario179/0, delta0 KiB. Es una medida de working set Windows, no una demostración del pico ni causa identificada. Durante la preparación hubo errores de arnés (dialecto C++/ABI de Giac, cabeceras LVGL9.6 encontradas en una caché abandonada, DLL SDL fuera de PATH, selección de compile_commands de firmware y CLI JSON equivocada); se corrigieron las rutas/ABI, sin cambiar expectativas matemáticas ni dependencias. Las compilaciones aceptadas usan LVGL9.5. No se han promovido goldens, cambiado máscaras ni ocultado códigos de error.

Después de la matriz solo se aclararon tres líneas de comentario sobre el denester, sin cambiar su número de línea. `comment-equivalence.json` verifica igualdad byte a byte de la unidad GiacEngine preprocesada antes/después. La última compilación WROOM confirmó el mismo tamaño y SHA de imagen. El ledger distingue este cambio editorial de una modificación de instrucciones ejecutables. La revisión visual utiliza secuencias nuevas y assertions de geometría/contenido; no pretende que un diff contra todos los goldens históricos sea cero. Las divergencias visuales históricas siguen en sus informes, sin promoción de referencias.

## Recursos y rendimiento

Máximos observados entre los139 casos: traza retenida contabilizada44.812 B, vectores retenidos24.954 B, máximo instrumentado de payload de vectores27.467 B, 3.963 llamadas simbólicas y27 pasos agregados. Son máximos independientes de corpus, no una suma de heap simultáneo. Se conserva la separación entre ese payload y asignaciones internas de Giac/C, cabeceras, fragmentación y LVGL.

Las trazas antiguas incorporan los nuevos campos de snapshot y el puntero propietario: +32 B habituales tanto en el host64bit como en Xtensa; las capacidades de sus vectores no cambian. En Xtensa, Snapshot pasa de60 a80 B y Derivation de136 a168 B; State48, Step136, Branch20 y Condition56 B permanecen iguales. Composition mide160 B y Preimage28 B, antes de sus payloads. Son tamaños medidos compilando también las cabeceras preservadas del baseline. No hay arena global nueva, caché de páginas, objetos ocultos por paso ni trabajo matemático por frame.

Frames propios Xtensa con las opciones de producción y `-fstack-usage`: compose1200 B, verifyComposition768 B, squarePreimage768 B, explainEquations544 B, replace272 B, admit240 B, sign208 B. `GiacEngine.cpp.su` conserva el detalle. No representan profundidad acumulada ni máximo físico de pila; también se ejecutan funciones de Giac. Las profundidades de nuestros recorridos son explícitas y acotadas; no se ha medido la pila de la placa en esta fase.

Firmware WROOM ordinario: flash enlazada **5.682.345 B** (+61.304), imagen **5.682.704 B** (+61.296), RAM estática **119.160 B** (sin delta), IRAM text **60.407 B** (sin delta). Capacidad de aplicación6.553.600 B, margen enlazado871.255 B, sin cambiar particiones. Imagen SHA-256 `dc3337345338fef182e5367ded4dc1f871c4e321e3de6f3c53c2a39ff0732032`. La estabilidad de RAM estática no significa coste dinámico cero.

Pool fijo: mínimo **muestreado**7.248 B; tras HOME,67 objetos,3 temporizadores,19.160 B libres de57.472 B de payload monitorizado. La configuración reservada sigue siendo65.536 B: payload y reserva no son la misma magnitud. Los handles retornan a0. Cincuenta ciclos sin reset de Giac; estos datos no son memoria total ni pico de PSRAM de una placa.

Medianas host en ms, n=5 tras una vuelta inicial, un mismo contexto sin reset entre problemas. Los ámbitos auxiliar/retorno/conciliación están **incluidos** en tutor; no se suman al total. `return` suma las dos llamadas de generación cuando existen; el replay padre adicional forma parte del resto de tutor.

| Problema | Giac ordinario | Tutor | Auxiliar | Retornos | Conciliación |
|---|---:|---:|---:|---:|---:|
| Bicuadrática |0,330|5,202|1,192|0,211|0,059|
| Exponencial |0,404|5,135|0,876|0,628|0,051|
| Logaritmo |0,396|9,070|1,412|1,799|0,316|
| Seno, cuatro familias |2,766|17,118|2,328|2,629|0,637|
| Tangente |1,999|16,004|1,209|4,646|0,262|
| Cuadrática anterior |0,439|3,373|—|—|0,518|

La primera generación correspondiente midió5,811 /6,116 /10,410 /14,225 /19,029 /3,500 ms. Solo la primera fixture es un proceso recién iniciado; las siguientes primeras vueltas comparten contexto ya calentado. Raw y máximos: `phase-profile/measurement.log`, `measurements.json`.

Preparación/reapertura: `page-benchmark` conserva30 muestras por operación para cuadrática real/compleja, una construcción de prueba y **cero capturas polinómicas redundantes** incluso en el primer final. `composition-page-benchmark` conserva10 por operación para seis problemas nuevos, cinco de calentamiento; incluye primera apertura, primer final, repetición, BACK/reapertura, visita de otra página y guiado/resumen. Son tiempos de función host, no de LCD ni ESP32.

Reapertura de la misma página final, mediana/p95 en ms: cuadrática0,822/1,178; compleja0,657/0,974 (n30). Bicuadrática1,074/1,469; exponencial0,701/0,811; logaritmo0,796/1,426; seno0,928/1,381; senoDEG1,133/2,347; tangente1,060/2,728 (n10). No se ha ganado tiempo ocultando pasos, cambiando Solve/TOOLBOX ni creando caché. Los tiempos absolutos host tienen variación de carga; los contadores y la invariancia son los controles reproducibles.

## Reproducción y revisión futura

Entradas reproducibles: `test-tutor-composition-host.py` (objetos native de la misma compilación), `test-tutor-composition.py --challenge tests/fixtures/tutor-composition-challenge.json` (CLI `periodic_result_main`), `compare-composition-baseline.py`, `test-tutor-composition-ui.py --bilingual`, `test-tutor-composition-pan.py`, `test-tutor-composition-failure.py`, `test-tutor-nonlinear-lifecycle.py --composition`, `profile-tutor-composition.py`, `benchmark-tutor-pages.py`, `package-tutor-composition-review.py`. Los JSON de comandos preservan las rutas concretas y hashes de esta ejecución.

Próximo control físico, separado: WROOM ordinario, bicuadrática de cuatro raíces, logaritmo con dos retornos, seno de cuatro familias RAD/DEG y tangente; comprobar Results/Steps independientes, ambos grupos, pan, cambio de idioma, edición/cancelación/HOME, reapertura sin nueva prueba y telemetría de heap/pila. Esta fase no ha escrito la PCB ni tocado almacenamiento/seguridad. No se reclama latencia, heap, pila ni legibilidad LCD física de03B.

Límite de entrega: una sustitución acotada; una prueba pasada no demuestra corrección universal. No se inicia otra fase ni se crea el commit orientativo `feat(tutor): compose checked methods through substitutions`.

<!-- exact-manifest -->
## Frontera exacta propuesta

Un único candidato03B, sin staging ni commit. Archivos de esta fase:

* `docs/TUTOR_ENGINE_03B.md`
* `docs/TUTOR_I18N_ES_01_CATALOG.md`
* `scripts/benchmark-tutor-pages.py`
* `scripts/build-tutor-ui-allocation-probe.py`
* `scripts/compare-composition-baseline.py`
* `scripts/package-tutor-composition-review.py`
* `scripts/profile-tutor-composition.py`
* `scripts/profile-tutor-pages-native.py`
* `scripts/test-math-notation.py`
* `scripts/test-periodic-results-ui-failure.py`
* `scripts/test-periodic-results-ui.py`
* `scripts/test-tutor-composition-failure.py`
* `scripts/test-tutor-composition-host.py`
* `scripts/test-tutor-composition-pan.py`
* `scripts/test-tutor-composition-ui.py`
* `scripts/test-tutor-composition.py`
* `scripts/test-tutor-nonlinear-lifecycle.py`
* `scripts/test-tutor-teaching-ui.py`
* `scripts/tutor-teaching-web.mjs`
* `src/apps/EquationsApp.cpp`
* `src/apps/TutorPresentation.h`
* `src/apps/TutorPresentation.inc`
* `src/apps/TutorStepsView.inc`
* `src/math/GeneratedMathNotation.h`
* `src/math/PeriodicMath.h`
* `src/math/giac/GiacComposition.inc`
* `src/math/giac/GiacEngine.cpp`
* `src/math/giac/GiacEngine.h`
* `src/math/giac/GiacPeriodic.inc`
* `src/math/giac/GiacTutor.inc`
* `src/math/giac/GiacTutorComposition.inc`
* `src/math/giac/GiacTutorNonlinear.inc`
* `src/math/giac/GiacTutorTranscendental.inc`
* `src/math/giac/GiacTutorTranscendentalChecks.inc`
* `src/math/giac/GiacTutorTranscendentalPlanner.inc`
* `src/math/giac/GiacTutorTrig.inc`
* `src/math/giac/GiacTutorTrigPlanner.inc`
* `src/math/tutor/Derivation.h`
* `src/math/tutor/Messages.inc`
* `src/math/tutor/TeachingPlan.h`
* `tests/fixtures/tutor-composition-challenge.json`
* `tests/host/periodic_results_allocation.cpp`
* `tests/host/periodic_results_checks.cpp`
* `tests/host/tutor_composition_checks.cpp`
* `tests/host/tutor_composition_budget_failure.cpp`
* `tests/host/tutor_nonlinear_allocation.cpp`
* `tests/host/tutor_transcendental_checks.cpp`
* `tests/host/tutor_trig_checks.cpp`
* `tests/wasm/math.mjs`

## Cierre enfocado del candidato (23 de septiembre de 2026)

El árbol inicial seguía en `main@e4f4dcaa679d83184052125814d27e4488b96c01`, con índice vacío y los 48 archivos de 03B sin guardar en Git. Antes de editar se conservaron parches binarios separados, copias legibles y hashes en `out/tutor-engine-03b-closeout/`; `.vscode/settings.json` quedó intacto. El fingerprint de ejecución original fue `ddcdcdd5592e2a7056a9ed7b414239cd60dcdd612c11d9872a085d610ae38059`. El candidato corregido tiene fingerprint `d140125170f6cbfe85876724b7e618518202f99402da2dbe852e95328fc4874c`; entre los 741 archivos de ejecución inventariados solo cambia `src/math/giac/GiacTutor.inc`.

La revisión separada de matemáticas y enseñanza no halló defectos demostrados en sustitución, prueba de hijos, conciliación o paginación. La revisión separada de recursos sí reprodujo un fallo estrecho: una asignación fallida al construir el diagnóstico de agotamiento podía escapar de `explainEquations`, pese a que el agotamiento matemático debía devolver `Partial`. El reproducer contra el código anterior falló en la asignación 56.683. Los dos diagnósticos de presupuesto ahora toleran `std::bad_alloc` sin cambiar la categoría `Partial`, el checker, los límites ni la respuesta ordinaria. `tests/host/tutor_composition_budget_failure.cpp` mantiene el fallo activo, comprueba la restauración de la traza y resuelve luego una ecuación sana sin reset de Giac. Se añadió ese detector a la suite host; la frontera pasa de 48 a 49 archivos.

Con el código corregido: corpus 96 generados y 43 desafíos sin fallos (129 Complete y 10 negativas); 761 trazas históricas sin diferencia matemática; 163 conciliaciones periódicas conservadas; 89 controles de composición: 31 mutaciones explícitas y 58 aserciones de admisión, resultado, alcance y presupuesto; 42 comandos host de compilación/enlace/ejecución sin error, incluida recuperación de asignación; catálogo EN/ES de 247 claves. La fixture almacenada `logb(A*x+1,2)^2-3*logb(A*x+1,2)+2=0`, con `A=1/2`, es la que más se aproxima al límite entre los casos normales: 3.963 llamadas. La variante repetitiva de esfuerzo llega a 4.097, devuelve `Partial`, conserva la respuesta ordinaria y permite un nuevo caso sano. Las 267 imágenes originales de 320×240 de las cuatro secuencias docentes EN/ES son idénticas al candidato anterior al excluir solo la cabecera variable del reloj.

La compilación WROOM ordinaria corregida usa `xtensa-esp32s3-elf-g++ 8.4.0` y la plataforma fijada `espressif32 6.12.0`: flash enlazada 5.682.397 B, imagen 5.682.768 B, RAM estática 119.160 B, IRAM 60.407 B. SHA-256 de imagen: `693b8d759ac3878f0d69e9b956d6ed7c02caf86720a1ea7001954bf45809f160`. Son +52 B de flash enlazada y +64 B de imagen respecto del informe de implementación; la RAM estática y la IRAM no cambian. La sonda temporal, compilada fuera del árbol y excluida del commit, tiene SHA-256 `6a5ba1d4df0970dcecfb13939c4be37c177ade0da9461f24c70122d14d7cdba6`; estos hashes no sustituyen la verificación de la imagen instalada.

Las regresiones posteriores a la corrección compilan native, WROOM ordinario, CAM normal y CAM validación. WASM-MATH Release/Debug pasan compilación, empaquetado, validación y regresión; el web pasa compilación, empaquetado, smoke y seis recorridos docentes completos (Chromium, Firefox y WebKit × EN/ES). `git diff --check` pasa. El código de vistas no cambió en el cierre y la matriz original de 50 ciclos del pool fijo, fallos de publicación y aplicaciones cruzadas conserva su identidad de implementación; el nuevo fallo persistente de diagnóstico se ensayó específicamente con el runtime corregido. La sonda de subtiempos registra resolución auxiliar, retornos y replay de hijos como ámbitos internos de `tutor_us`; no mide por sí misma el primer píxel del LCD ni un pico total de asignaciones.

**Estado anterior a la autorización física:** se había preparado y compilado la sonda, sin escribir todavía la PCB ni obtener observación humana del LCD. Los resultados host y de emulador de esa etapa no se presentan como aceptación física; el ciclo posterior se documenta a continuación.

## Aceptación física y cierre (23 de septiembre de 2026)

El usuario autorizó expresamente este ciclo. Se redescubrió la WROOM de producción por USB `303a:1001`, serie `44:B1:76:A7:B7:2C`, puerto `COM9` en esta sesión. La entrada OTA activa era la aplicación en `0x10000`, con capacidad de 6.553.600 B; Secure Boot y Flash Encryption estaban desactivados. Se guardó una copia completa del slot anterior, SHA-256 `11530e10d75553abef85a6ab3f7fcd31b77204e3407ce3502acfa183cc707656`. El prefijo de 64 KiB, que incluye arranque y particiones, se comparó byte a byte antes/después de cada escritura y quedó intacto. No se escribió el bootloader, la tabla de particiones, NVS, LittleFS ni eFuses.

La sonda privada y desactivada por defecto (`6a5ba1d4df0970dcecfb13939c4be37c177ade0da9461f24c70122d14d7cdba6`) se instaló solo en el slot de aplicación; `verify_flash` y una lectura independiente reprodujeron ese SHA-256. Arrancó con `[BOOT] OK` y 8.182 KiB de PSRAM libre. El replay usó el teclado semántico físico de producción en Equations, no una llamada directa al planificador. Se ejecutaron 17 casos: tres bicuadráticos (incluido el retorno afín y la raíz anidada), dos exponenciales, dos logarítmicos, seno de cuatro familias en RAD y DEG, tangente compuesta, seno imposible y duplicado, aislamiento simple, cuadrática y logaritmo anteriores, y dos fronteras con dominio cancelable. Los 15 casos admitidos terminaron Complete con estados idénticos al replay host de la entrada realmente tecleada; las dos fronteras no publicaron una composición Complete. La respuesta ordinaria se produjo por la ruta Giac, con familias periódicas de su productor independiente; el tutor conservó su construcción propia y la conciliación trivaluada. Ninguna apertura válida reconstruyó la prueba. La variante `sin(x)^2=2` concluyó sin solución y `(sin(x)-1)^2=0` mostró una sola familia final.

Se recorrieron todas las páginas de Steps, scroll vertical hasta el fondo y vuelta, guiado/resumen, BACK, cancelación, una edición confirmada, HOME y reentrada. En las cuatro familias se comprobaron ambos grupos de Results; en DEG se recorrió el pan de las fórmulas largas. El cambio RAD→DEG produjo fingerprints distintos, sin reutilización de la prueba RAD. Las colisiones t/u/k y el agotamiento simbólico se mantienen como pruebas host; no se fabricó una variable física ni se agotó la placa. La inspección visual del emulador y sus capturas 320×240 pertenece a la evidencia de implementación. **Revisión humana del LCD: diferida**; la telemetría no demuestra legibilidad subjetiva.

Tiempos físicos de funciones instrumentadas, en ms. `auxiliar`, `retornos` y `replay de hijos` están incluidos en `tutor`, por lo que no se suman. `reapertura` es mediana/máximo de la misma página final; no mide el primer píxel LCD ni separa por sí sola la conciliación del resto del planificador.

| Caso | Giac ordinario | Tutor | Auxiliar / retornos / replay hijos | Primer Steps / primer final | Reapertura n, mediana / máximo |
|---|---:|---:|---:|---:|---:|
| Bicuadrática | 38,5 | 940,2 | 215,2 / 43,8 / 226,0 | 48,5 / 33,2 | 3; 61,5 / 61,5 |
| Exponencial compuesta | 71,2 | 2057,9 | 215,1 / 160,6 / 367,3 | 48,6 / 22,7 | 3; 49,4 / 49,4 |
| Logaritmo compuesto | 62,9 | 1939,2 | 215,2 / 449,2 / 703,9 | 49,6 / 26,1 | 3; 53,6 / 53,6 |
| Logaritmo con ±√2 | 91,7 | 3853,9 | 223,1 / 1003,9 / 1548,0 | 48,3 / 27,0 | 3; 53,1 / 53,1 |
| Seno, cuatro familias RAD | 508,7 | 3041,0 | 401,1 / 513,8 / 1044,2 | 49,2 / 43,1 | 10; 74,4 / 74,5 |
| Seno, cuatro familias DEG | 905,6 | 2989,4 | 401,1 / 509,7 / 1076,9 | 49,1 / 35,4 | 10; 67,7 / 67,7 |
| Tangente compuesta | 270,5 | 2841,4 | 222,8 / 813,6 / 1389,3 | 49,9 / 55,6 | 10; 81,2 / 81,3 |
| Cuadrática anterior | 47,8 | 457,0 | — | 59,7 / 35,2 | 3; 58,9 / 58,9 |

Los datos crudos, tamaños de muestra y todos los casos están en `out/tutor-engine-03b-closeout/physical-analysis.json`; la preparación de Results se conserva allí como ámbito distinto. Son una primera ejecución por caso en un Giac compartido ya calentado por casos anteriores, no distribuciones de generación fría. La conciliación no tenía sonda propia; no se le atribuye tiempo aislado. El caso de logaritmo con radicales fue el más lento, pero terminó dentro de los límites sin watchdog. No se cambió la política de generación ni los presupuestos.

Muestras físicas: antes de la primera bicuadrática, heap interno libre/bloque mayor `143.856/102.388 B`, PSRAM `8.224.979/8.126.452 B`; después de HOME fueron los mismos valores, con 71 objetos, 5 pantallas y 3 temporizadores. Durante el seno RAD de cuatro familias: heap interno `143.856/102.388 B`, PSRAM `8.203.715/8.126.452 B`, traza contabilizada 15.691 B y vector máximo contabilizado 12.688 B. La menor memoria libre LVGL **muestreada** en las vistas fue 33.504 B de 61.060 B de payload monitorizado, con hasta 100 objetos, 6 pantallas y 3 temporizadores. La reserva LVGL sigue siendo 65.536 B. Al HOME final: heap interno `143.856/102.388 B`, PSRAM `8.224.059/8.126.452 B`, LVGL libre 40.784 B, 71/5/3 objetos/pantallas/temporizadores, traza y vectores contabilizados a cero. La PSRAM libre bajó inicialmente unos 0,7–0,9 KiB y luego osciló dentro de ese margen durante más de diez casos sin descenso monótono. Es retención/calientamiento estable **observado**, no prueba de ausencia de fugas en cualquier ejecución. La pila mínima sin usar al final fue 48.856 **bytes** según el puerto ESP32-S3 fijado (`stack_unit_bytes=1`); no se multiplica por cuatro. Las lecturas periódicas no capturan el pico total del asignador ni permiten sumar mínimos de instantes distintos. No hubo reset, Guru Meditation ni watchdog en los registros de ciclos.

Tras aceptar la sonda se reinstaló la imagen ordinaria **sin instrumentación**, SHA-256 `693b8d759ac3878f0d69e9b956d6ed7c02caf86720a1ea7001954bf45809f160`, con `verify_flash`, lectura independiente idéntica y `[BOOT] OK`. Se hizo un smoke corto de entrada por SerialBridge normal para una composición finita y un seno periódico, apertura de Steps y HOME, sin comandos privados de la sonda ni fallos de arranque. La última preferencia medida por la sonda antes de restaurar fue HOME/RAD/Español y el smoke ordinario no cambió Settings; no hay una lectura de preferencia posterior a la restauración con la sonda ya retirada. El binario físico probado corresponde al fingerprint de ejecución `d140125170f6cbfe85876724b7e618518202f99402da2dbe852e95328fc4874c`; el cambio de documentación y el hash de commit posterior no convierten el binario anterior en otro. La sonda, los backups, los logs y las capturas permanecen ignorados bajo `out/` y no pertenecen al commit.
