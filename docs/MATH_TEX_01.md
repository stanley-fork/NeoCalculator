# MATH-TEX-01 — espaciado contextual compartido

Entrega local, 25-09-2026. Implementada y comprobada; **sin commit, staging, push, publicación ni flash**. Primera ola horizontal, no paridad TeX completa.

**Actualización 26-09-2026: menos matemático.** Se sustituye la presentación del guion ASCII por el glifo U+2212 existente en STIX. La [galería actualizada](../out/math-tex-01/minus-followup/visual/index.html) y su [ZIP visual](../out/math-tex-01/minus-followup/MATH-TEX-01-visual.zip) comparan contra la entrega de espaciado ya completada. El [apéndice](#actualización-menos-matemático-26-09-2026) separa esta corrección de los resultados históricos que siguen abajo.

Abrir la [revisión visual HTML](../out/math-tex-01/visual/index.html) o descargar el [ZIP visual autocontenido](../out/math-tex-01/MATH-TEX-01-visual.zip). Contiene originales 320×240, ampliaciones enteras 4× nearest-neighbor, comparación con referencia, overlays, edición y EXE. No contiene archivos de fuentes. Todas las rutas locales del HTML se verifican al empaquetar. Son candidatos de revisión; no se promueven goldens ni máscaras.

## Estado inicial y preservación

HEAD y rama iniciales: `main`, `ffb7082ba3b7db7741a8a603a5ad83f40c6c2682`; índice vacío. El working tree ya contenía CALC-CORE-INPUT-01 y su reparación posterior de edición de potencias. El fingerprint de runtime coincidía exactamente con el comunicado:

`5f499072f14891e0df0b40f3532ad6ad68cea723de70094d057b8e6814637647`

Antes de modificar se guardaron en `out/math-tex-01/`:

| Artefacto | Contenido |
|---|---|
| `baseline.json` | HEAD, rama, manifiesto y hashes de cada archivo inicial |
| `input-repair-source.zip` | Snapshot completo del estado inicial, incluidos archivos no registrados |
| `input-repair-new-files.zip` | Archivos nuevos de la reparación, separados |
| `input-working.patch`, `input-index.patch` | Diffs iniciales; el segundo está vacío |
| `baseline-program.exe` | Ejecutable native de la reparación final, no un binario anterior |
| `baseline-web.zip` | Paquete web de esa reparación |
| `baseline-calculation/` | Puerta rápida inicial, 114 comprobaciones aprobadas |
| `baseline-spacing-native/`, `evidence/baseline-probe/` | Teclas, AST, serialización, geometría y capturas anteriores |
| `MATH-TEX-01.patch`, `delta.json` | Solo la diferencia de esta entrega respecto al snapshot inicial |

SHA-256 del ejecutable preservado: `94fcccfc5bbe88f4cb5d185559c430cec9d47cdf9d363713d9c5068bfd227fba`.
SHA-256 del ZIP web preservado: `95a56fb5e0ea1d7ea292775105e3c60e34bf4b34a64ba65f051252df7273f727`.
El manifiesto fuente inicial es `38c501b46be03c8b82add1e7f119ac2ae1304bc1e2a3b14f22369e50898628d6`.

`.vscode/settings.json` permanece idéntico, SHA-256 `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`. No se ha usado stash/reset/clean ni tocado dependencias, clocks, particiones, seguridad, almacenamiento o el pool LVGL. Los builds se hacen en copias ASCII bajo `C:/.codex-cache/math-tex-01/`; esto evita problemas históricos de MinGW y del runner con espacios/Unicode en rutas. El checkout original conserva su trabajo sin commit.

La reparación de escritorio anterior no estaba flasheada según su informe. No se ha inspeccionado ni actualizado una PCB en esta tarea. Ninguna identidad de native/Wasm se atribuye al firmware instalado.

## Especificaciones y estado real

Se han contrastado la cabecera V/P/U, A.2/A.3, PD-1/2/8/10, G.6, H.5, TXP-0, TXP-2 y el corpus de `NUMOS_TEX_PARITY_TYPESETTING_SPEC.md`, junto con las especificaciones de arquitectura V2, layout/tipos, oráculo/goldens, semántica/serialización y MR02, y los informes actuales `CALC_CORE_INPUT_01.md` y `MATH_NOTATION_01.md`.

| Afirmación o propuesta | Estado contrastado/adaptación |
|---|---|
| Clases TeX, MathStyle, proveedor MATH y conversiones | Ya existen; se reutilizan. La tabla efectiva anterior tenía entradas de script incorrectas |
| Clasificación binaria contextual | Pendiente en el baseline real; implementada ahora en presentación |
| Métricas `charWidth × longitud` | Persistían en números, signos, variables/constantes y nombres de función; corregidas donde dibuja texto proporcional |
| Paréntesis, STIX y Delta | Corregidos después de las revisiones antiguas: se conservan glifos/ensamblaje STIX y el fallback puntual de Delta |
| NEG, ×10^, vacíos y borrado de plantillas | Reparados después; se preservan y se ejecutan sus pruebas de teclas |
| MathEvaluator como autoridad | Descripción histórica, no vigente. Giac mantiene la evaluación; esta entrega no modifica backend/tutor |
| Buffer local de 64 clases + fallback | Propuesta descartada: tres recorridos lineales y caché pequeña en cada nodo, sin corte en 65 |
| Dimensión em inferida de cualquier fuente pequeña | No se adopta. Estilo y fuente son variables distintas |
| Límites antiguos de filas/nodos | No se toman como hechos actuales: el editor no impone un máximo de elementos por fila; el serializador sí tiene presupuesto propio |
| Tabla reconstruida y asociaciones inferidas de capturas [U] | No se aceptan como evidencia. Se verifican fuentes primarias y se separan los árboles de control |
| TXP-0 completo / migración vertical | Fuera de esta ola: se adapta el mínimo de oráculo y observación necesario; no se afirma completar todo TXP-0…10 |

Los marcadores [V] de julio y sus números de línea no certifican el código de septiembre. La política vigente usa perfiles STIX nominales **18/12/8 px**, con ascent/descent y avance del glifo real. `lv_font_t::line_height` de estas fuentes incluye extensiones matemáticas y no equivale a su em nominal; sustituirlo ciegamente cambiaría los tamaños del producto. No se añade una fuente ni un tamaño.

## Causas medidas

La observación privada se inserta en una copia del renderer real y se enlaza con los objetos/LVGL de cada build. Registra tipo, clase estática/efectiva, estilo, em, baseline, avance, tinta del glifo, espacio entrante, reserva de script, origen, cursor y tamaño de fila. No hay hooks ni overlays nuevos en firmware.

| Medida TEXT, em 18 | Baseline | Nuevo |
|---|---:|---:|
| `-5`, avance de fila | 22 px | 15 px |
| `5-6`, avance de fila | 35 px | 32 px |
| `5^(-5-6)`, avance total | 40 px | 30 px |
| `5^(-5^(-6))`, avance total | 34 px | 28 px |
| Primera fracción de control | 120 px | 115 px |
| Segunda fracción de control | 114 px | 113 px |
| Fila larga de sumas | 815 px | 939 px |

La fila larga **debe ensancharse**: `+` avanza 13 px, pero antes se reservaban 9 px. Compactar todo habría ocultado este defecto.

Las causas no eran una sola:

1. El menos inicial reservaba 9 px para un glifo de avance 6 px, y añadía 4 px de espacio binario: 7 px sobrantes antes del operando.
2. La fila del exponente `-5-6` reservaba 30 px frente a 20 px de avances reales: 6 px de espacios binarios de texto y 4 px de medida excesiva de los signos.
3. Los operadores después de una potencia recibían la frontera estática INNER del contenedor; el exponente no es el contexto externo. Las filas envolventes tampoco son grupos visibles.
4. `66.63` se medía como 45 px en vez de 40; `sin` como 27 en vez de 22. `+`, `×` y `=` podían dibujar más ancho que lo reservado. Medir tinta en lugar de avance tampoco soluciona esto.
5. El cursor se recortaba horizontalmente **antes** de calcular el scroll. En filas largas perdía la distancia fuera de pantalla y alcanzaba su destino por pasos. Ahora el scroll recibe la coordenada lógica completa.
6. A partir del tercer script, las métricas sintéticas seguían bajando aunque el renderer continuaba usando la fuente mínima de 8 px. Se mantiene esa fuente y se conserva su perfil de métricas.

`SpaceAfterScript` ya estaba presente una sola vez: no se demostró duplicación. Se conserva y se comprueba explícitamente. El padding interno vigente de funciones/paréntesis y la política vertical de fracciones no se cambian. La reserva tras script, el espacio entre átomos, el avance, el kerning y el padding de interfaz siguen siendo conceptos separados.

## Reglas y fuentes verificadas

La autoridad para la reclasificación ordenada y los espacios es [TeX, `tex.web`, mlist_to_hlist / math_spacing](https://raw.githubusercontent.com/TeX-Live/texlive-source/trunk/texk/web2c/tex.web), versión indicada en el archivo: 3.141592653. Se conserva el archivo consultado y su SHA-256 en `sources/manifest.json`, porque la URL de trunk puede avanzar.

La cadena primaria verificada, en orden ORD OP BIN REL OPEN CLOSE PUNCT INNER, es:

```text
0234000122*4000133**3**344*0400400*000000234000111*1111112341011
```

Sus códigos no son los del helper NumOS: `1` = thin condicional, `2` = thin incondicional, `3` = medium condicional, `4` = thick condicional; `*` = combinación imposible tras clasificación. Se traducen a 3/4/5 mu y al encoding positivo/negativo existente. Un espacio condicional desaparece en SCRIPT/SCRIPTSCRIPT; los thin incondicionales, por ejemplo ORD→OP, permanecen. Las combinaciones imposibles conservan el comportamiento seguro sin espacio, pero se prueban también las secuencias que las reclasifican.

Reclasificación, en el orden de TeX:

- BIN al inicio o después de BIN/OP/REL/OPEN/PUNCT pasa a ORD.
- Ante REL/CLOSE/PUNCT se reclasifica el BIN anterior.
- El BIN final pasa a ORD.

Se cambia únicamente la clase **efectiva de presentación**. `OpKind::Sub` permanece Sub; `a--b` no se reescribe; no se parsea texto ni se llama a Giac desde layout/draw.

[OpenType MATH](https://learn.microsoft.com/en-us/typography/opentype/spec/math) se usa para las constantes/métricas y `SpaceAfterScript`, no como sustituto de la clasificación TeX. Se mantiene UPM=1000 y las conversiones enteras existentes: truncamiento hacia cero en `duToPx`/`muToPx`, reserva de script al menos 1 px y avances enteros redondeados por LVGL. Cada espacio se convierte una vez y se reutiliza en los consumidores.

La referencia visual host es **KaTeX 0.16.22**: [implementación de espacios](https://github.com/KaTeX/KaTeX/blob/v0.16.22/src/spacingData.js), [clasificación/HTML](https://github.com/KaTeX/KaTeX/blob/v0.16.22/src/buildHTML.js), [opciones](https://katex.org/docs/options.html) y [fuentes](https://katex.org/docs/font.html). Se verificaron versión y fuentes cargadas. Usa fuentes KaTeX derivadas de Computer Modern, **no STIX ni OpenType MATH**. Nominal 18 px, `displayMode:false`, salida HTML, sin macros de espacio manuales, `trust:false`, `strict:error`. Scripts 0,7/0,5 frente a los perfiles discretos NumOS 12/8. Las fracciones principales usan `\dfrac` para igualar el tamaño de sus hijos; esto no iguala políticas verticales. Las variables de referencia conservan su cursiva; NumOS conserva su forma actual.

Excepción explícita: KaTeX 0.16.22 prescribe thick en PUNCT→REL, mientras que la cadena primaria de TeX prescribe thin. El oráculo de reglas sigue TeX. Las capturas de KaTeX son comparación cualitativa, no valores esperados para NumOS ni comparación píxel a píxel. Las filas largas pueden mostrar distinta porción por el scroll de edición y la anchura de cada motor; su comprobación cuantitativa usa la traza íntegra, no ese recorte.

## Contrato horizontal compartido

`MathRowLayout.h` calcula las fronteras, mide, reclasifica y coloca. Solo atraviesa filas transparentes como partes de la misma lista. Las ranuras de una estructura se calculan en su propio contexto:

| Nodo real | Fronteras de presentación |
|---|---|
| Number, Variable, Constant, Symbol | Sus clases existentes, normalmente ORD |
| Operator | Clase estática, reclasificada contextualmente si BIN |
| Row envolvente | Lista transparente; no adopta sin más la clase del primer hijo |
| Power/Subscript | Fronteras de su base; jamás de su exponente/subíndice. Base multielemento encajada: ORD |
| Paren visible | INNER, regla 15; los signos interiores no salen a la fila padre |
| Function/LogBase | OP hacia la izquierda, operando completado ORD hacia la derecha; mismo criterio para entrada y resultados |
| Empty | Conserva nodo, ancho, tinta y validación incompleta; no altera el vecino tipográfico al buscar contexto |

La clasificación se guarda junto a `rowX` y `spaceBefore` en `LayoutResult`. `NodeRow::calculateLayout`, `drawRow`, Finder y `childXOffset` consumen esas posiciones. El final lógico es `row.layout().width`. El Finder usa además el helper compartido de elevación de potencia. Los consumidores de `cursorBounds` en Equations y el scroll del canvas reciben esas mismas coordenadas; el resaltado dibuja en los mismos orígenes. No hay otra implementación de hit-testing matemático por píxel que requiera una tabla separada.

`MathTextMetrics.h` vincula los perfiles a la fuente real mediante un puntero no propietario y una función de medida sin asignación. Comparte normalización, resolución de glifo/fallback y kerning por pareja con el dibujo. No crea otro árbol ni una interfaz virtual. El núcleo sigue pudiendo usar métricas sintéticas para pruebas sin LVGL; los canvases del producto siempre vinculan sus fuentes reales.

Los números, operadores, constantes, variables, etiquetas de función y decimales periódicos reservan el avance de las mismas secuencias que dibujan. El decimal periódico mide sus fragmentos por separado, tal como se dibujan; no aplica kerning a través de una frontera de run. La posición interna de `lv_draw_letter` usa el avance sin siguiente carácter para su pivote, y el lápiz avanza con el kerning de pareja: no se aplica dos veces. No se cambia la forma ni la cursiva de glifos.

Calculation entrada/resultado mantiene TEXT. Los demás canvases mantienen su estilo vigente, generalmente DISPLAY; en ambos estilos los espacios de esta entrega coinciden. Numeradores/denominadores siguen `fractionPartMetrics`; potencias usan SCRIPT y luego SCRIPTSCRIPT. Una función/fracción en un exponente recibe ese estilo, no lo infiere de su tamaño físico. Se preserva el mínimo físico 8 px incluso a mayor profundidad.

## Reconstrucción, corpus y matemática

Las capturas originales y `specs.rar` no estaban disponibles como adjuntos accesibles de esta sesión; las especificaciones pertinentes sí estaban extraídas en `docs/specs`. No se afirma identificar inequívocamente el árbol de la foto. Los dos controles pedidos se introducen por el editor actual, no por LaTeX:

```text
8 / 5 ^ neg 5 - 6 RIGHT - 5 + 5 * 6
((8)/(((5)^((-1)*5-6))-5+5*6))

8 / 5 ^ neg 5 ^ neg 6 RIGHT RIGHT - 5 + 5 * 6
((8)/(((5)^((-1)*((5)^((-1)*6))))-5+5*6))
```

La primera contiene la resta `-5-6` dentro del exponente; la segunda una potencia anidada dentro de la negación. El control `(5^(-5))^(-6)` es un tercer árbol, con grupo visible. Todos tienen AST completos en `visual/measurements.json` y las trazas. Las letras abstractas a/b se representan con x/y, que admiten los caminos de entrada probados. El corpus tiene 36 fixtures y combinaciones parametrizadas de las ocho clases y cuatro estilos.

Se compara la serialización antes/después de layout, dibujo y **377 frames de navegación**. De `dumpTree` solo se excluyen sus cinco números geométricos; no se excluyen operadores, valores, agrupaciones ni hijos. El probe compara asimismo todos los campos del resultado estructurado Giac —árbol exacto/aproximado, textos, status, diagnósticos, flags y fallback— antes/después y contra el baseline. No cambia ningún campo matemático.

Las 900 trazas de tutor/composición coinciden: solo se excluyen los campos temporales `micros`/`warmSolveMicros` y, en 139 registros de composición, `command[0]` tras comprobar el nombre del ejecutable. Condiciones, estados, pasos, reglas, candidatos, bindings y mediciones de memoria del tutor permanecen comparados. La lista exacta de exclusiones está en `corpus/invariance.json`.

Incidencia numérica histórica separada: `assert_calc_input near` consulta el espejo ExactVal, que convierte algunos resultados mediante `fromDouble` con diez decimales, después del texto aproximado Giac de doce cifras. Las fixtures afectadas declaran su tolerancia numérica y causa; **el texto exacto y los árboles Giac se comparan sin tolerancia**. No se ha modificado esa política ni usado sus tolerancias para geometría. Grapher devuelve muestras float32: sus dos controles comparan exactamente el esperado redondeado independientemente a float32, con tolerancia cero.

## Pruebas y plataformas

| Puerta | Resultado |
|---|---|
| Puerta rápida `scripts/test-calculation-input.py` | 114/114 en baseline y candidato; signos/×10^/borrado/unwrapping/serialización/Ans/historial/ranuras conservados |
| `tests/emulator/calculation-desktop-recovery.numos` | Reproducción SDL completa preservada y ejecutada; también el corpus nuevo con ventana SDL real |
| Reglas y posiciones host | 151.490 comprobaciones; cuatro estilos, 4.096 secuencias de cuatro clases, tres em y wrappers transparentes |
| Detector contra baseline | 818 comprobaciones comunes, 184 fallan como se esperaba |
| Geometría observada | 36 fixtures; baseline detectado en 33; candidato sin fallos; ancho/origen/cursor exactos, sin tolerancia global |
| Mutaciones privadas | Detectadas las cuatro: tabla antigua, clase estática, TEXT dentro de todos los scripts, cursor con suma estática antigua |
| Tinta de Empty | Tres ranuras: 46/46, 48/48 y 46/46 píxeles de borde presentes; además detector de placeholder del gate de entrada y EXE/historial |
| Mismas fracciones en otras apps | Seis recorridos baseline y seis candidato: Equations, Calculus y Grapher; teclas reales, cursores/capturas y resultados donde corresponde |
| Notación/STIX | Suite de notación, MathEnginePhaseRegression, nueve capturas/medidas de glifos y 16 casos de delimitadores aprobados |
| Backend | Suite Giac ordinaria y suites Calculus/cross-app/Neo aprobadas; sin cambios de autoridad |
| Tutor/composición | 900/900 trazas invariantes, suites de composición/no lineales/trig/transcendentales y controles de asignación aprobados |
| EN/ES | 247 entradas, cero ausencias; mutación de traducción detectada; selector EN/ES/EN y páginas Steps bilingües aprobados |
| Pool fijo | Calculation 50 ciclos + 114 checks y Equations 50 ciclos; pool reservado sigue 65.536 bytes |
| Calculus / Grapher Templates | Recorridos y 50 ciclos aprobados con el perfil native habitual |
| Web de entrada anterior | Chromium, Firefox, WebKit: teclas virtuales, teclado, shell y componente aprobados |
| Web de espaciado | 23 fixtures compatibles × 2 superficies × 3 navegadores = 138 casos, baseline/candidato con resultados iguales |
| Wasm | Shell Release + smoke; componente matemático Release y Debug, pruebas en los tres navegadores; mismo C++/Wasm |
| Firmware, solo compilación | WROOM producción/bringup/demo y CAM normal/validate aprobados |
| Integridad | `git diff --check`; índice/HEAD y settings preservados; recursos relativos y ZIP verificados |

Los tests ordinarios fallaron inicialmente al recibir rutas de captura absolutas con espacios, y un link native coincidió con un proceso que mantenía abierto el EXE. Se conservaron los logs; los reintentos usan rutas compatibles y un ejecutable separado. No son fallos matemáticos ni se han ocultado ampliando máscaras.

**Incidencia previa comprobada:** abrir Grapher con el pool fijo de 64 KiB provoca la misma aserción LVGL por agotamiento en el ejecutable preservado y en el candidato. Esto interrumpe también el recorrido general de Calculus cuando visita Grapher. Se guarda el reproducer `modifier-Grapher.numos` y `baseline-fixedpool-grapher.log`. No se amplía el pool ni se declara aprobado ese recorrido con pool fijo. Los 50 ciclos propios de Calculation/Equations sí pasan; los recorridos completos Calculus/Grapher se validan con el perfil CLIB de escritorio, cuyos contadores de heap/pool no representan mediciones ESP32.

## Límites y coste

No hay array nuevo de 64 elementos, truncamiento por ese tamaño ni búsqueda de vecino desde cero por átomo. Se prueban 63, 64, 65, 399, 400 y 2.000 elementos. El presupuesto de serialización es **400 nodos contando la raíz**, profundidad 40 y 2.000 caracteres; la prueba distingue 399 hijos aceptados de 400 rechazados por ese contrato existente. El editor no tiene un máximo finito de longitud de fila equivalente: no se inventa uno ni se rechazan entradas por un buffer nuevo.

El almacenamiento geométrico histórico usa `int16_t`; expresiones extremas que excedan 32.767 px y anidaciones arbitrarias del editor siguen siendo límites pendientes, no se certifican como seguras por pasar la puerta del serializador. El renderer mantiene su límite de profundidad existente. No se migra todo el motor a int32/fijo en esta entrega.

Tres pasadas por fila: medida, reclasificación ordenada, colocación. O(n) en la longitud de fila, O(profundidad) de stack; las fronteras de núcleos con scripts siguen solo su cadena de bases. Cero asignaciones propias por frame/layout: se deniega tanto `new` como el allocator AST durante layouts repetidos. Las colas de dibujo existentes de LVGL no se confunden con asignaciones introducidas aquí.

| ESP32, WROOM producción | Baseline | Nuevo | Delta |
|---|---:|---:|---:|
| Flash enlazada informada por build | 5.682.929 B | 5.684.441 B | +1.512 B |
| Imagen firmware.bin | 5.683.296 B | 5.684.800 B | +1.504 B |
| RAM estática | 119.160 B | 119.160 B | 0 B |
| IRAM `.iram0.text` | 60.407 B | 60.407 B | 0 B |
| `sizeof(MathNode)` | 24 B | 32 B | +8 B |
| `sizeof(NodeRow)` | 36 B | 44 B | +8 B |
| `sizeof(LayoutResult)` | 10 B | 16 B | +6 B |
| `sizeof(FontMetrics)` | 36 B | 44 B | +8 B |
| `sizeof(MathCanvas)` | 156 B | 180 B | +24 B |

Los tamaños ESP32 proceden del compilador Xtensa real; en host MathNode es 40→48 B, NodeRow 64→72 B y FontMetrics 40→56 B. El coste residente adicional del AST es 8×N bytes por el alineamiento de nodos; no son asignaciones durante la maquetación. Un árbol de 400 nodos añade 3.200 B al almacenamiento de sus nodos, no al pool LVGL. No se declara el pico total de heap del dispositivo.

Frames propios Xtensa medidos por desensamblado: RowLayout measure/classify/place = **32/48/64 B**, NodeRow::calculateLayout 48 B, medida de texto 208 B (incluye buffer acotado de normalización), Finder 224 B, computeCursorPosition 96 B, drawRow 64 B y drawTextBaseline 352 B (antes 304 B). No se suman como si fueran todos simultáneos: las tres pasadas se ejecutan consecutivamente. La colocación añade como máximo 64 B por nivel de Row transparente; la pila total incluye llamadas existentes de estructuras/LVGL y no ha sido medida físicamente. No se cambia el stack de tareas.

Microbenchmark host registrado, 1.000 layouts por tamaño: filas nuevas de 64/399/2.000 nodos ≈54,6/304/1.587 µs por layout frente a 0,48/3,05/16,2 µs del baseline. El coste real de consultar los glifos proporcionales es superior al antiguo cálculo aproximado. Es una medición host con otros builds concurrentes, no latencia ESP32 ni garantía de FPS. El crecimiento observado es lineal y no presenta discontinuidad en 65. Se deja explícito el coste para una futura caché con invalidación justificada; no se introduce una caché por frame ni una tabla grande silenciosa.

Los 50 retornos HOME de Calculation conservaron 67 objetos, 3 timers, 0 handles, payload monitorizado 57.704 B y 19.336 B libres en el allocator, con reserva fija de 65.536 B. No se interpreta la suma de esos contadores internos como tamaño físico del pool. Recursos completos por los cinco targets, tamaños, hashes y frames: `resources.json`.

## Revisión visual y siguiente fase

Se han inspeccionado directamente las comparaciones y overlays: menos/operando, unión base-script, salida del script, separación de operadores, ancho de fracción, cursor, filas largas y placeholders. El ZIP contiene también las páginas Steps EN/ES y las otras superficies de las dos fracciones. `visual-changes.json` enumera cada pareja baseline/candidato, número de píxeles cambiados y caja de diferencias, sin máscaras; incluye también las diferencias del reloj de la aplicación. El HTML indica los tamaños y las diferencias de estilo/fuente, y deja separados edición, EXE y referencia sin editor. No constituye revisión humana del LCD.

Las dos fracciones principales mantienen **exactamente** su geometría vertical previa: primera ascent/descent 21/14 px; segunda 21/16 px. En la segunda, los baselines son base 136, exponente 130, exponente anidado 126; la tinta del `6` mínimo ocupa y=121…125, 4×5 px, cerca de la barra. La legibilidad vertical del anidamiento mínimo sigue siendo inferior a la referencia: se conserva este reproducer para estudiar superscriptShiftUp, bottomMin y separación de barra en la siguiente fase, sin compensarlo horizontalmente.

Excepción acotada: `minimum-font` (`2^(2^(2^(-3)))`) pasa de ascent 21 a 19 porque se deja de medir una fuente sintética menor que la de 8 px realmente dibujada. No se cambia el escalado físico ni la política vertical general. Debe permanecer como control de regresión de esa coherencia.

También quedan fuera: migración vertical completa, nueva cursiva, line breaking, nuevos símbolos/matrices, otra fuente, métodos nuevos del tutor y corrección del espejo numérico ExactVal. No se atribuyen a esta ola las discrepancias históricas de esos subsistemas.

## Archivos de esta entrega y reproducción

Delta exclusivo, además de este informe:

- Producción: `src/math/MathAST.cpp`, `src/math/MathAST.h`, `src/math/MathTypography.h`, **nuevo** `src/math/MathRowLayout.h`, `src/ui/MathRenderer.cpp`, **nuevo** `src/ui/MathTextMetrics.h`.
- Ajuste de expectativas antiguas: `tests/MathEnginePhaseRegression.cpp`, `tests/host/math_notation_main.cpp`.
- Nuevos controles: `tests/fixtures/math-spacing.json`, `tests/host/math_spacing_checks.cpp`, `tests/host/math_spacing_probe.cpp`, `tests/wasm/math-spacing.mjs`, `tests/wasm/math-spacing-reference.mjs`.
- Nuevos runners: `scripts/test-math-spacing.py`, `scripts/compare-math-spacing.py`, `scripts/test-math-spacing-native.py`, `scripts/test-math-spacing-apps.py`, `scripts/test-math-spacing-mutations.py`, `scripts/package-math-spacing-visuals.py`.

No hay frases nuevas en la interfaz de firmware que requieran ampliar el catálogo. La galería de revisión tiene textos EN/ES. Los archivos originales de la reparación de entrada que aparecen en `git status` no deben atribuirse a este delta.

Después de construir `emulator_pc`, usar fuentes/objetos/LVGL coincidentes, sin mezclar ABI Giac ni cabeceras LVGL de otros builds:

```powershell
python scripts/test-calculation-input.py --bin EXE_ABSOLUTO --out out/math-tex-01/input --cycles 50
python scripts/test-math-spacing.py --source SOURCE --build NATIVE_BUILD --lvgl LVGL --out PROBE
# Para el snapshot inicial, añadir --baseline: sus detectores deben fallar.
python scripts/compare-math-spacing.py --baseline BASELINE_PROBE --candidate PROBE --out out/math-tex-01/geometry-comparison.json
python scripts/test-math-spacing-native.py --bin EXE_ABSOLUTO --out out/math-tex-01/candidate-spacing-native --oracle BASELINE_PROBE --baseline out/math-tex-01/baseline-spacing-native --window
python scripts/test-math-spacing-apps.py --bin EXE_ABSOLUTO --out OUTPUT_ASCII
python scripts/test-math-spacing-mutations.py --source SOURCE --baseline-source BASELINE_SOURCE --probe PROBE --out MUTANTS_ASCII
python scripts/package-math-spacing-visuals.py --work C:/.codex-cache/math-tex-01 --out out/math-tex-01
```

Los comandos exactos, versiones y hashes de objetos están en los JSON de evidencia. Los scripts browser usan `NUMOS_WEB_ROOT`, `NUMOS_SPACING_WEB_OUT` y opcionalmente `NUMOS_SPACING_WEB_BASELINE`; la referencia usa `NUMOS_KATEX_ROOT`/`NUMOS_REFERENCE_OUT`. KaTeX y Playwright son herramientas host aisladas, sin cambios al manifiesto de dependencias del producto.

Rollback preparado: revisar `MATH-TEX-01.patch` contra `delta.json`; su inversa restaura solo los archivos de esta ola al snapshot inicial. Se comprueba con `git apply --reverse --check`, pero **no se aplica**. No hay flag permanente ni dos renderers de producción. No usar reset a HEAD: perdería CALC-CORE-INPUT-01. El paquete web nuevo queda preparado localmente; su corrección pública no está desplegada.

## Actualización: menos matemático (26-09-2026)

La comparación aportada de `5^(-5-6)` identifica otro defecto concreto: el renderer dibujaba U+002D (guion ASCII), cuya tinta es mucho más corta que U+2212. No era un problema adicional de espacio entre átomos. Se usa ahora **el glifo U+2212 ya incluido en las tres fuentes STIX**, sin modificar sus bitmaps, tamaños ni grosor. La tabla de símbolos del motor de referencia también traduce `-` a U+2212 en modo matemático: [KaTeX 0.16.22, symbols.js](https://github.com/KaTeX/KaTeX/blob/v0.16.22/src/symbols.js). Se verificó la implementación primaria y el paquete host aislado; no se incorpora KaTeX al producto.

Medidas consultadas directamente con LVGL sobre los glifos compilados, sin usar el helper de producción para obtener los valores esperados:

| Em nominal | Tinta U+002D, ancho×alto | Tinta U+2212, ancho×alto | Avance anterior → actual |
|---|---:|---:|---:|
| 18 px | 5×3 px | 11×2 px | 6 → 13 px |
| 12 px | 4×1 px | 8×2 px | 4 → 9 px |
| 8 px | 3×1 px | 6×1 px | 3 → 6 px |

El menos tiene ahora el mismo avance que `+` y `=` en cada tamaño. Son cajas de tinta y avances distintos: no se estira el guion ni se añade padding para imitar la referencia. El ancho total de `5^(-5-6)` pasa de 30 a 40 px; el de `5^(-5^(-6))`, de 28 a 36 px. Ascent y descent permanecen idénticos en los 36 casos.

**Contrato compartido.** `mathTextCodepoint` resuelve el carácter después de decodificar UTF-8. La medida aplica esa resolución al glifo actual y al siguiente antes del kerning. El dibujo envía el mismo carácter a LVGL y usa su descriptor sin kerning para el pivote. La fila y el cursor consumen los avances existentes del contrato MATH-TEX-01. Cubre operadores, números de resultado con signo, decimales periódicos y demás texto matemático del canvas. Los widgets de texto ordinario no cambian. No hay expansión de cadenas, buffer nuevo ni discontinuidad al superar los 96 bytes de normalización; se prueba una secuencia de 100 signos.

El operador semántico y `NodeOperator::symbol()` siguen siendo ASCII `-`; no se cambian nodos, parser, serialización, evaluación, clasificación contextual, SpaceAfterScript ni política vertical. La forma del glifo sí cambia, autorizada por esta observación posterior del usuario; es una excepción explícita al alcance original que preservaba las formas. La reserva horizontal crece conforme al nuevo avance, incluido el centrado natural de fracciones.

**Preservación incremental.** HEAD y rama siguen siendo `main` / `ffb7082ba3b7db7741a8a603a5ad83f40c6c2682`, índice vacío. `minus-followup/before-source.zip`, `baseline.json`, `working.patch` e `index.patch` conservan el estado anterior completo, incluidos archivos nuevos. También se preservan el ejecutable previo (SHA-256 `7f34be50f9bb360d889ea99f28855f1747de6b41c76c993aa0616daa5c8e7015`), el paquete web, recursos y ZIP visual de la primera entrega. Los archivos iniciales de CALC-CORE-INPUT-01 y `.vscode/settings.json` permanecen intactos. El delta y su inversa comprobable se empaquetan aparte en `minus-followup/MATH-TEX-01-minus-source-delta.zip`; no se aplicó rollback.

**Validación de esta actualización:**

- 151.539 comprobaciones independientes de clases, estilos, métricas, límites y cero asignaciones propias: 0 fallos. Contra el renderer anterior, la misma batería conserva **19 fallos detectores**, incluidos los tres tamaños y los números negativos de resultado.
- 36 fixtures, navegación, AST, serialización y árboles completos de Giac invariantes; layout/draw/cursor coincidentes. La comprobación adicional del carácter realmente enviado a `lv_draw_letter` detecta U+002D en 24 fixtures anteriores y ninguno en el candidato. El observador privado registra ahora el descriptor/codepoint real, en vez de reconstruirlo desde el texto.
- Puerta rápida Calculation: 114 casos, reproducción SDL completa, EXE, historial/Ans, edición de potencias y 50 ciclos. Corpus de 36 entradas por teclas con SDL real, trazas semánticas idénticas al ejecutable de espaciado previo.
- Equations/Calculus/Grapher: seis controles de fracción. Steps: 12 páginas EN/ES. Notación/STIX y MathEnginePhaseRegression aprobadas; tres comprobaciones de bordes de placeholders conservadas en las capturas.
- C++/Wasm reconstruido y paquete local validado; 138 casos en Chromium/Firefox/WebKit, shell y componente, con estado de Calculation idéntico al baseline. Un primer intento apuntó al directorio de build sin la fixture del componente y terminó en timeout; la ejecución válida usa `out/wasm/dist/release`, sin cambiar el producto para esa incidencia del runner.
- Builds WROOM producción y CAM aprobados; `git diff --check` aprobado. Las 900 trazas de tutor, otros perfiles de build, catálogo completo y ciclos de pool fijo descritos en la entrega original **no se vuelven a atribuir a este ejecutable**: no se repitieron para este delta de presentación. No se modifican sus fuentes ni dependencias.

**Recursos incrementales frente al MATH-TEX-01 ya completado:** WROOM y CAM suman 28 B de `.flash.text` y 28 B de `.flash.rodata` (+56 B). `.dram0.data`, `.dram0.bss` e `.iram0.text` no cambian. No se añaden miembros ni tipos de layout; sus tamaños permanecen iguales. La conversión es O(1) por glifo, el recorrido sigue siendo lineal y no introduce asignaciones ni recursión. Frames propios Xtensa: medida de texto 208→192 B, dibujo de texto 352→336 B; Finder 224 B y fila 64 B, sin cambios. Esto no mide el pico total de pila/heap ni la latencia física del ESP32. LVGL conserva 64 KiB.

**Visuales.** La galería actualizada conserva 320×240, 4× nearest-neighbor, referencia, overlays y estados editable/EXE; incluye un detalle legible de la expresión aportada, con el mismo recorte y escala entera en las tres columnas. Todos los recursos relativos del ZIP se comprueban y no se incluyen archivos de fuente. Se revisaron directamente los signos a 18/12/8 px, las potencias anidadas, el cursor y el resultado negativo. KaTeX mantiene su fuente y escalas propias ya documentadas; no se afirma igualdad de píxeles ni revisión física del LCD. La altura de los exponentes y legibilidad vertical extrema siguen pendientes por separado.

**Archivos de este delta incremental (8):** `src/ui/MathTextMetrics.h`, `src/ui/MathRenderer.cpp`, `tests/host/math_spacing_checks.cpp`, `tests/host/math_spacing_probe.cpp`, `scripts/test-math-spacing.py`, `scripts/compare-math-spacing.py`, `scripts/package-math-spacing-visuals.py` y este informe. Solo los dos primeros afectan a producción. No hay nuevas frases en la app; la galería está en EN/ES. Los artefactos y logs están en `out/math-tex-01/minus-followup/`, con `delivery.json` como manifiesto final. Preparado localmente; sin staging, commit, push, publicación ni flash.

Reproducción adicional sobre los runners anteriores:

```powershell
# Fuente/objetos del MATH-TEX-01 completado: los nuevos detectores deben fallar.
python scripts/test-math-spacing.py --source BEFORE_SOURCE --build BEFORE_BUILD --lvgl LVGL --out BEFORE_PROBE --expect-check-failure
# Candidato: todos deben pasar.
python scripts/test-math-spacing.py --source SOURCE --build BUILD --lvgl LVGL --out AFTER_PROBE
python scripts/compare-math-spacing.py --baseline BEFORE_PROBE --candidate AFTER_PROBE --out geometry.json --require-math-minus
python scripts/package-math-spacing-visuals.py --work REVIEW_WORK --out REVIEW_OUT --baseline-phase math-spacing
```
