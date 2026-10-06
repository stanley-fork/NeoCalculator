# UNIT-PICKER-UX-PERF-01

Implementación, regresión y medición física emparejada terminadas. Los dos recorridos manuales en el ordinario final están **aprobados por el usuario**. UNIT-PICKER-FINALIZE-01 guarda este candidato en un único commit local, sin recompilar ni reinstalar el runtime.

## Candidato preservado y alcance

HEAD inicial y rama: `7fcdced6a5092e7521855354fb8a0bbc6f0b45c5`, `main`. Son ancestros los commits `7a21767f3612aea0a13a676edc8b0f605f544b95` y `05fe60ddde3814ca4fe07069b7f502a30405be6a`. No se reconstruyó el trabajo desde un ancestro ni se modificó el índice. `docs/RESUMEN_CHAT_TOOLBOX.png` era un archivo nuevo anterior a esta tarea y queda separado del delta.

Evidencia local ignorada: `out/unit-picker-ux-perf-01/`. Los checkpoints `A-baseline`, `B-provider`, `C-context`, `C-context-final` y `D-final` contienen las fuentes completas, archivos nuevos, hashes, parches binarios separados de índice/working tree, estado de Git/worktrees y el emulador publicado con sus runtimes y copia anterior. Se conservan también los intentos y las evidencias anteriores. `.vscode/settings.json` mantiene SHA-256 `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.

La lectura independiente inicial de la WROOM confirmó aplicación `app0`, offset `0x10000`, capacidad 6.553.600 B, imagen de 6.260.112 B y SHA-256 `5f3a30891114e5d8aa25e6a9a369c6fb884b76dcd71f7d1e8e962d84c0215499`. La placa y el puerto se redescubrieron; no se asumieron desde el cierre. `board-before/` contiene la recuperación local, que no se distribuye. La autorización nueva y explícita está en `authorization.json`.

Esta tarea cambia proveedor y presentación. `runtime-boundaries.json` comprueba la identidad respecto de A de Quantity, CalculationEngine, InputRowSyntax, AST y las demás fuentes sin cambios. No se añaden unidades, factores, temperaturas, incertidumbre, conversiones contextuales ni nuevas teclas. Se mantienen clocks, perfil de pantalla, dependencias, particiones y pool LVGL de 64 KiB. Durante UNIT-PICKER-UX-PERF-01 no hubo staging ni commits. UNIT-PICKER-FINALIZE-01 autoriza un único commit local después de la revisión manual; no hay push ni despliegue.

## Arquitectura y causa del coste

Antes, contar las categorías visibles y obtener sus filas recorría repetidamente el proveedor completo. Cada pasada volvía a construir descriptores y comprobar prefijos equivalentes para el filtro. Buscar recalculaba el ranking al contar y obtener filas visibles.

B conserva el mismo recorrido, orden y resultados, con tres cambios:

1. `UnitNavigationIndex.inc`, generado desde el registro existente, permite acceder al rango de hijos de una categoría sin recorrer todas las definiciones. No contiene factores nuevos ni una segunda tabla de conversiones.
2. `toolbox::View` prepara una admisión por sesión. Usa un byte por identidad para admisión y ranking; las variantes ofrecidas de una misma familia comparten la comprobación de compatibilidad. Esta optimización es optativa para el receptor de unidades de salida. El resto de receptores mantiene sus filtros propios.
3. El conteo memoriza hasta 64 pares grupo/profundidad. Si se llena, calcula la respuesta completa: no limita destinos. Una consulta calcula cada ranking admitido una vez y conserva los cuatro grupos de orden anteriores. La navegación no crea coeficientes convertidos ni llama a Giac.

El filtro sigue usando `quantity::descriptor`, `descriptorDimension` y la firma completa de 17 componentes: siete SI, ángulos plano/sólido, información y dominios de recuento. Mantiene admisibilidad, potencias, exponentes de denominadores y restricciones pendientes. La confirmación ejecuta de nuevo la validación exacta existente. Ningún hash sustituye una comparación de identidad/dimensión.

En la apertura física A de longitud se observan 3.765 construcciones de descriptor, 3.991 accesos combinados a filas y 4.200 conteos combinados. La misma apertura física B reduce esos contadores a 312, 563 y 74 respectivamente. Los tiempos native no certifican la latencia de la WROOM; aquí se comparan muestras de la misma placa.

## Propiedad e invalidación

La vista pertenece al modal y se destruye al cerrarlo/HOME. Contiene flags y posiciones, sin punteros a AST, cantidades compartidas ni historial. La admisión queda ligada al receptor, capacidades y modo completo/componente de esa apertura. La profundidad forma parte de la clave de conteo.

Los textos se obtienen del locale vigente; no se almacenan cadenas traducidas. El ranking conserva la búsqueda bilingüe/regional existente. Cada consulta reemplaza los rangos de búsqueda sin asignar otro buffer. Los favoritos se leen en su orden actual y no se incluyen en el caché de categorías. No se escribe ningún caché de rendimiento en almacenamiento. El clasificador de símbolos de 66 B anterior a esta tarea sigue siendo independiente del idioma.

El receptor conserva la generación del resultado. La selección rápida también la guarda y rechaza un resultado obsoleto antes de publicar. El cierre destruye las tres previews antes de eliminar sus objetos. Los fallos de preparación conservan la expresión y el resultado; un índice incompleto no se publica como una lista vacía.

## Recorrido contextual

Se conserva `SHIFT → ALPHA → FORMAT`. Para una cantidad, «Unidad de salida» es la primera elección. Al entrar aparecen la unidad actual, hasta tres favoritos compatibles y unas pocas alternativas de la familia de representación actual, seguidas de «Todas las unidades», «Por componentes» y «SI predeterminado».

El orden es estable. Los duplicados se eliminan por descriptor normalizado, incluidos productos reordenados, sin confundir unidades dimensionalmente iguales. Las sugerencias referencian identidades existentes; no contienen factores. No se deduce Hz/Bq, J/N·m ni Gy/Sv desde los exponentes. No hay selección automática de prefijos por el valor del coeficiente. Sin alternativas apropiadas quedan visibles los accesos al selector compatible y a componentes; no se ocultan destinos válidos.

Las tres filas visibles reutilizan MathCanvas, el AST de unidad y las métricas MATH existentes. Se corrigieron dos recortes encontrados durante desarrollo: fracciones en filas demasiado bajas y una composición de siete unidades más ancha que la columna de símbolo. La composición larga usa el ancho de la fila con SCRIPTSCRIPT y el nombre encima; si aun así no cabe, se conserva el nombre completo de la opción en lugar de mostrar una fórmula cortada. No cambia la política tipográfica del editor.

Estas son secuencias de producto comprobadas por eventos, desde el resultado, con favoritos inicialmente vacíos salvo la prueba del favorito. Cada SHIFT, ALPHA y FORMAT cuenta como una pulsación. No se contabilizan las pulsaciones de preparación del cálculo ni los desplazamientos de saturación usados por otros harnesses.

| Recorrido comprobado | Antes | Después |
|---|---:|---:|
| m → cm | 16 | 6 |
| m/s → km/h, unidad completa | 15 | 6 |
| J → kW·h | 20 | 6 |
| Elegir favorito compatible mm | 15 | 6 |
| Cambiar componente m → km en m/s | 18 | 15 |

Las secuencias exactas están en `D-before-steps/counts.json` y `D-after-steps/counts.json`. B comprueba el recorrido anterior, preservado por el oráculo diferencial. Otros favoritos compatibles pueden desplazar una alternativa común; no se promete seis pulsaciones para cualquier estado del almacén.

FORMAT corto, SHIFT+FORMAT/TABLE, SHIFT+×10^/ENG, TOOLBOX de inserción, Steps de Equations, VAR/STO y ALPHA conservan sus funciones. No se ha añadido un atajo físico no acordado.

## Invariancia y controles de software

El oráculo diferencial mantiene en pruebas el filtrado anterior y enumera categorías directamente desde los datos originales, sin usar el índice generado nuevo. Compara todas las identidades, variantes, categorías, orden, selección inicial, último resultado y componentes. Recorre los cuatro locales y favoritos vacíos/poblados.

Resultado: **2.860 contextos**, **11.377.080 comparaciones de variantes**, **3.040 consultas** y **24.169.552 aserciones**. No son definiciones nuevas. Se conserva el inventario de unidades de UNIT-CATALOG-02. Una primera optimización incorrecta de la consulta vacía fue detectada por este oráculo y retirada antes de B.

Controles conservados o repetidos:

| Frontera | Evidencia |
|---|---|
| Constructor/filtro/semántica de sugerencias | `B-differential/`, `essential/C-quick-model/` |
| Calculation rápida | 113 eventos preservados en `D-quick-gate/`, incluidos rechazos esperados |
| Cantidades, Ans/PreAns, memoria de sesión, conversión, formatos | 23 casos en `D-quantities-final/` |
| Menú, puntero/teclado, EXE mantenido, cancelación, prefijos, búsqueda física | `D-picker-final/` y matriz web final |
| Núcleo | 42 casos dirigidos, 60 escenarios racionales generados, 6.330 aserciones; 105 factores independientes |
| Datos y mutaciones | generador `--check`, oráculos de datos, 12 mutaciones rechazadas en `essential/D-mutations/` |
| Matemática sin unidades | `D-corpus900-valid/`: 900 trazas; todos los campos no temporales coinciden |
| Cuatro regiones y persistencia | `D-native-locales/`, `D-web-locales/` |
| FORMAT escalar/TABLE/ENG | `D-scalar-format-final/`, 28 casos con observador privado de lectura |
| Fallos | 28 puntos de asignación del menú rápido, cinco filas, sesión/índice, receptor obsoleto y confirmación fallida en `D-faults-final/`; filas Toolbox en `D-toolbox-row-faults/` |
| Pool fijo | `D-cycles-final/`: cuatro calentamientos y 100 ciclos con historial lleno, búsqueda, prefijos, favoritos, conversión/inserción/cancelación y HOME |

La búsqueda conserva `mA/MA`, `MHz/mHz`, `Pa/pA`, `pH`, `µs/μs`, metro/metre/meter, milímetro/millimeter y ohmio/Ω. El teclado físico se resuelve contacto a contacto con ALPHA, no mediante una consulta completa inyectada. Los aliases de símbolos mantienen mayúsculas; no se normaliza globalmente el griego.

Los fallos de C++/AST son persistentes dentro del ámbito inyectado. No se anuncian como fallos exhaustivos del malloc de LVGL o Giac. Un intento del harness pidió puntos de asignación inexistentes (3 y 8); el contador mostró solo dos asignaciones de sesión/índice, se conservaron esos logs y la prueba final exige que los dos fallos reales se hayan disparado.

Los primeros intentos de la prueba escalar de FORMAT usaron un ejecutable sin su observador privado y no aportan resultados de producto. La ejecución válida usa el observador reconstruido con fuentes/objetos correspondientes. También se conserva el intento que apuntó al corpus histórico preliminar; las 900 trazas válidas proceden del corpus congelado de UNIT-CATALOG-01.

## Medición física

`paired-campaign.py` registra por comando la entrada/canónico, descriptor, componente y exponente, locale, favoritos, consulta y filas válidas. A/B conservan el mismo menú. C mide por separado el selector completo y la experiencia rápida. Se mantuvieron placa, compilador/optimización, reloj, perfil display SAFE 40 MHz, instrumentación, es-ES/RAD, favoritos vacíos e historial de 50 entradas. Las entradas se construyeron mediante Toolbox y se verificó el AST; no se cargaron cantidades terminadas.

La sonda privada separa receptor, proveedor, enumeración, admisión, conteos, ranking/normalización, previews y actualización del modal. `PickerProbe` registra ámbitos exclusivos y contadores; no se suman sus totales anidados. `CL-TIME` conserva los ámbitos históricos. Transporte y ACK se guardan aparte. El intervalo mínimo de 55 ms de la línea serie del harness no es debounce del producto. La llamada explícita al refresco al final del ACK puede encontrar el frame ya dibujado por el ciclo normal: **no mide el primer píxel LCD**.

Mediana / p95 / máximo en **ms**, n=30 aperturas calientes por celda. p95 usa el rango superior `ceil(0,95·n)`. Cada apertura construye una sesión nueva, sin un índice preparado por una apertura anterior; ninguna muestra se descarta.

| Selector completo | A, anterior | B, solo proveedor | C, candidato final |
|---|---:|---:|---:|
| Longitud, componente de velocidad | 774,840 / 774,900 / 774,907 | 51,203 / 51,236 / 51,239 | 51,130 / 51,198 / 51,247 |
| Tiempo, componente de velocidad | 859,569 / 859,653 / 859,659 | 51,008 / 51,047 / 51,058 | 50,965 / 51,009 / 51,019 |
| Velocidad, unidad completa | 974,143 / 974,242 / 974,253 | 55,234 / 55,269 / 55,275 | 55,153 / 55,222 / 55,222 |
| Energía | 1154,989 / 1155,023 / 1155,039 | 55,038 / 55,078 / 55,081 | 54,926 / 54,986 / 54,999 |
| Tiempo inverso | 1459,664 / 1459,719 / 1459,746 | 55,657 / 55,700 / 55,712 | 55,655 / 55,731 / 55,739 |
| Superficie, caso históricamente lento | 2635,416 / 2635,491 / 2635,497 | 59,782 / 59,850 / 59,852 | 59,739 / 59,812 / 59,816 |

Las tres primeras sesiones por contexto se conservan aparte; son primeras aperturas tras preparar el contexto, **no tres arranques fríos del dispositivo**. Mediana / máximo (ms), n=3:

| Selector completo | A | B | C |
|---|---:|---:|---:|
| Longitud | 774,845 / 774,869 | 51,239 / 51,246 | 51,146 / 51,153 |
| Tiempo | 859,518 / 859,673 | 51,021 / 51,030 | 50,981 / 50,990 |
| Velocidad | 974,161 / 974,231 | 55,247 / 55,271 | 55,199 / 55,274 |
| Energía | 1155,024 / 1155,045 | 55,049 / 55,060 | 54,943 / 55,003 |
| Tiempo inverso | 1459,636 / 1459,724 | 55,619 / 55,651 | 55,696 / 55,704 |
| Superficie | 2635,411 / 2635,446 | 59,815 / 59,844 | 59,739 / 59,779 |

La experiencia contextual C abre una lista útil ya preparada, con las previews visibles. No publica un popup vacío mientras prepara el catálogo.

| Acceso rápido C | Primeras sesiones, n=3, mediana/máximo | Caliente, n=30, mediana/p95/máximo |
|---|---:|---:|
| Velocidad | 18,936 / 19,011 | 18,823 / 18,987 / 19,043 |
| Longitud | 19,240 / 19,365 | 19,233 / 19,322 / 19,380 |
| Energía | 19,581 / 19,672 | 19,570 / 19,660 / 19,680 |

Otros ámbitos emparejados, mediana / p95 / máximo (ms):

| Operación | n | A | B | C |
|---|---:|---:|---:|---:|
| Búsqueda por carácter | 45 | 546,517 / 1851,236 / 1851,263 | 48,051 / 55,224 / 55,257 | 48,115 / 55,352 / 55,369 |
| Abrir prefijos de Metro | 30 | 31,968 / 31,985 / 31,986 | 18,305 / 18,317 / 18,339 | 18,406 / 18,420 / 18,424 |
| Prefijo arriba | 30 | 25,918 / 25,949 / 25,990 | 13,412 / 13,421 / 13,427 | 13,492 / 13,506 / 13,509 |
| Prefijo abajo | 30 | 30,796 / 30,815 / 30,849 | 14,962 / 14,972 / 14,977 | 15,011 / 15,159 / 15,173 |
| Preparar/publicar conversión km/h | 30 | 7,341 / 7,360 / 7,363 | 7,354 / 7,371 / 7,372 | 7,344 / 7,356 / 7,358 |
| Evento EXE completo en selector | 30 | 42,160 / 42,180 / 42,185 | 8,707 / 8,766 / 8,779 | 8,707 / 8,734 / 8,760 |

La búsqueda física recorre `kilometre` carácter a carácter mediante ALPHA y contactos reales del resolver, cinco veces. No se equipara a las 66 consultas completas del cierre anterior. Sus nueve caracteres explican la distribución y los outliers; todos permanecen en JSON. La ruta de preparación/publicación exacta se mantiene en unos 7,3 ms. El evento EXE completo también mejora: A repetía la búsqueda para localizar la fila seleccionada; B/C ya no enumeran el catálogo al confirmar.

Los objetivos de preparación se cumplen en estas cargas: apertura mediana ≤100 ms/p95 ≤150 ms y búsqueda mediana ≤50 ms/p95 ≤100 ms, incluidas primeras sesiones identificadas. No se han certificado con estas medidas la latencia del primer píxel, todos los arranques fríos físicos ni el percentil de cualquier consulta posible. No hay debounce añadido ni cambio de frecuencias. Los tiempos históricos de cierre (688,841/764,003/862,625 ms) se conservan como referencia sin mezclarlos con esta instrumentación. Se incluye superficie, cuyo caso anterior superó dos segundos.

Ejemplo de ámbitos **exclusivos** de una apertura de longitud A/B: conteo de unidades 281,422/3,389 ms; accesos combinados 134,094/2,031 ms; conteos combinados 136,181/0,209 ms; cálculo dimensional 56,846/5,342 ms; preparación propia nueva de B 8,002 ms. El coste propio de abrir Toolbox apenas cambia (19,420/19,190 ms). Estos ámbitos explican la reducción de recorridos, sin atribuirla a Giac, PSRAM o una captura. `paired-analysis.json` conserva todos los scopes, contadores, transporte y muestras por comando.

La campaña C hizo dos calentamientos y **15 ciclos medidos**, tras 50 errores y 50 escalares para estabilizar el historial de 50 entradas y sus buffers conocidos. Incluyó destinos completos/componentes, denominador elevado, Ans, favoritos, cuatro locales distribuidos, FORMAT, búsqueda/prefijos/cancelación, error/recuperación, historial y HOME. No se reinició Giac dentro de la campaña ni se vació el historial para mejorar las cifras.

Se guardaron mm y km/h desde la operación normal de Favoritos, se comprobó su persistencia tras RESET y se retiraron por esa misma operación. El conjunto inicial y restaurado era vacío. Los cambios de idioma y RAD se guardaron desde Settings; brillo 144 conservado. No se escribió ningún archivo de usuario directamente ni se inyectaron fallos de almacenamiento en la placa. `physical/favorites-preserved.json` y los eventos documentan esas escrituras normales.

## Recursos

El índice de navegación ocupa **1.282 B en flash** (408 B de grupos y 874 B de filas). Admisión y ranking comparten **3.978 B de heap por sesión**; no hay otro buffer de búsqueda por carácter. La vista y sus 64 conteos ocupan 536 B en Xtensa. Session pasa de 1.392 a 1.928 B; la diferencia total de ese modal es 4.514 B antes del overhead del asignador. No existe copia del registro en RAM ni widget/AST por variante. No hay un caché global nuevo.

CalculationApp pasa de 1.000 a 1.008 B. UnitOutputMenu ocupa **652 B** y se asigna solo mientras está abierto, con tres canvases/previews reutilizados. NodeUnit sigue en 52 B e HistoryEntry en 160 B. La muestra host de preparación rápida registró pico propio 1.640 B y 1.376 B vivos, con 28 asignaciones; excluye LVGL/Giac y no se presenta como pico físico.

| Ordinario final | WROOM | CAM |
|---|---:|---:|
| Imagen | 6.266.912 B | 6.179.376 B |
| Flash enlazada | 6.266.541 B | 6.179.017 B |
| RAM estática | 119.672 B | 118.264 B |
| IRAM texto | 60.407 B | 59.039 B |

En WROOM la imagen crece 6.800 B respecto al ordinario recibido y quedan **286.688 B** en la partición de 6.553.600 B, por encima de los 128 KiB presupuestados. RAM estática e IRAM permanecen iguales. Los tamaños/hashes, secciones, tipos y frames proceden del ELF final en `D-final-firmware/resources-firmware.json`, no del binario con sonda.

Frames **propios** Xtensa: View::prepare 64 B, View::count 80 B, preparación de sugerencias 64 B (lambda de admisión 272 B), identidad normalizada 160 B, openFormatMenu 112 B y updateFormatMenu 256 B. No se suman como si fueran la pila total. No cambian los hot paths de layout/draw/cursor ni su política de asignaciones.

El pool conserva **64 KiB**. En 100 ciclos host finales, tras cuatro calentamientos, el mínimo LVGL muestreado fue 6.296 B y el bloque mayor mínimo 5.336 B; HOME se estabilizó en 67 objetos, tres timers y cero handles temporales. La capacidad utilizable del harness (57.696 B) no sustituye la configuración nominal.

Las 15 fronteras HOME físicas después de saturar el historial dieron:

| Métrica física | Rango/mínimo observado |
|---|---:|
| Memoria interna libre / bloque mayor | 142.520 / 98.292 B, constantes |
| PSRAM libre | 8.176.239–8.176.451 B |
| Bloque PSRAM mayor | 8.126.452 B, constante |
| LVGL libre / bloque mayor en HOME | 41.220 / 29.716 B, constantes |
| Mínimo LVGL muestreado / bloque mayor mínimo | 30.408 / 29.716 B |
| Pila sin usar, high-water mínimo | 56.948 B |
| Objetos / pantallas / timers | 71 / 5 / 3, constantes |
| Bloques PSRAM asignados | 873, constantes |

La variación de 212 B de PSRAM es reversible en esta carga; no aparece pérdida sostenida. Los 873 bloques coinciden con la retención acotada conocida. La unidad de high-water se comprobó con `sizeof(StackType_t)==1`; la pila del loop sigue siendo la previa de 65.536 B. No se multiplica por cuatro ni se confunde el frame de contains (1.696 B) con la pila completa. Son muestras, no máximos exhaustivos de heap/pila. Datos y checkpoints en `physical-cycles-summary.json` y `physical/C-cycles/`.

## Publicación y evidencia visual

El emulador se compila fuera de `C:\.piobuild\numOS\emulator_pc`, se valida y se publica con `publish-emulator-windows.ps1`. Se conserva la copia anterior y las DLL fijadas. `run-emulator-windows.ps1` se prueba por la ruta habitual con conversión y Ans. Los hashes exactos están en `published-artifacts.json`; el ejecutable final de 18.136.220 B tiene SHA-256 `11d0a239ce816654a06fffbadab3fc17dfe28f70379808d9f5bf0c42a6836a17`. `D-emulator-final-publication.log` y `D-launcher-final.log` conservan publicación y smoke satisfactorios.

La galería autocontenida está en `out/unit-picker-ux-perf-01/gallery/index.html`, con originales 320×240 EN/ES y recursos relativos comprobados; el ZIP contiene solo galería/manifest, sin fuentes ni backups de placa. El paquete web se prepara localmente y no se despliega. Los logs y manifiestos esenciales se copian fuera de cachés a `essential/`; `prepared-web-final/` conserva el paquete web final y `durable-evidence-manifest.json` sus hashes.

La matriz final `D-web-final/` pasa shell y componente en Chromium, Firefox y WebKit (seis recorridos). `D-webkit-final-accumulated/` pasa ocho recorridos acumulados (dos superficies × cuatro locales), conservando logs de ciclo de vida. `D-web-locales/` conserva además 24 recorridos regionales del candidato anterior al ajuste visual de composición ancha; no se presentan como otra ejecución de ese binario final. Los cierres históricos de WebKit siguen sin causa demostrada. Las pasadas nuevas se registran con sus logs y no se presentan como explicación de esos incidentes. parMixed y Grapher con pool fijo mantienen su alcance histórico. WASM-MATH continúa siendo una API escalar sin Quantity/Toolbox/LVGL; no se atribuye integración dimensional a este trabajo.

## Restauración y revisión humana

La aplicación ordinaria final WROOM tiene SHA-256 **`307341cf2a29dd8c2770d9ea8d242648b1dfb11300dda7f16d1df1a79a18c6cc`**, imagen de 6.266.912 B. Se actualizó exclusivamente `app0` en `0x10000`, se verificó con esptool y se leyó de forma independiente toda la imagen: coincidencia byte a byte y de SHA-256. El prefijo `0x00000–0x0ffff`, incluida la tabla de particiones, permanece idéntico antes/después. No se escribió bootloader, particiones, OTA selector, seguridad ni datos del usuario fuera de las operaciones normales de Settings/Favoritos.

`ordinary-final-switch/operation.json` documenta la identidad redescubierta, aplicación, escritura y lectura. `ordinary-artifact-check.json` comprueba que la configuración es la ordinaria excepto el directorio aislado de compilación y que no existen símbolos ni marcadores de la sonda en ELF/imagen. `ordinary-final-boot.log` confirma el arranque en HOME. Se restauró es-ES/RAD desde Settings, favoritos originales vacíos y brillo 144 antes de retirar la sonda; no se escribieron preferencias durante la actualización.

La campaña semántica física con sonda verificó conversiones y Ans; sus tiempos siguen correspondiendo a esa sonda. La revisión humana siguiente pertenece al ordinario final documentado, sin nueva actualización de aplicación ni operación serie sobre la PCB. La identidad instalada se conserva con el alcance de la lectura completa y el arranque del 5 de octubre; no se inventa una comprobación nueva.

UNIT-PICKER-FINALIZE-01, 6 de octubre de 2026, recoge dos preguntas sucesivas:

1. **APROBADA**: `6 km / (300 s) → 20 m/s → SHIFT → ALPHA → FORMAT → Unidad de salida → km/h → 72 km/h`. Respuesta literal: «Sí, todo el recorrido funciona y se lee bien». La pregunta cubría el tirón anterior, nombres completos y confirmación sin confusión. No se pidió estimar milisegundos ni se impuso un orden de favoritos.
2. **APROBADA**: `2 m → mostrar en cm → Ans/(2 s) → 1 m/s → HOME`. Respuesta literal: «Sí, obtengo 1 m/s y vuelve a HOME». Este es el smoke manual del canónico en el ordinario final; no se sustituye por una captura o aserción de la sonda.

Los registros literales están en `out/unit-picker-finalize-01/review.json`. Se mantienen los ajustes y favoritos del usuario; esta finalización no ha cambiado preferencias, clocks, fuentes, matemáticas, particiones ni el pool.

## Estado final de Git y reproducción

Al finalizar UNIT-PICKER-UX-PERF-01, HEAD era `7fcdced6a5092e7521855354fb8a0bbc6f0b45c5`, en `main`, con el índice vacío. UNIT-PICKER-FINALIZE-01 prepara los 22 archivos del manifiesto, exclusivamente, para el único commit local aceptado. El SHA y el estado final se registran fuera del propio commit en `out/unit-picker-finalize-01/closeout-receipt.json`, evitando una cadena de commits documentales. No hay push ni despliegue. Los worktrees y `.vscode/settings.json` coinciden con A. `final-change-manifest.json` enumera **22 archivos** de código, pruebas y documentación; excluye el PNG preexistente. `git diff --check` pasa. `D-final/` conserva la fuente completa, archivos nuevos, hashes, parches binarios separados y emulador con DLL/copia anterior.

Las fuentes de runtime coinciden byte a byte con `C-context-final` y el árbol del paquete web final. `final-audit.py`, `collect-evidence.py`, `analyze-paired.py`, `analyze-cycles.py` y los scripts de campaña conservados permiten contrastar identidad y recalcular tablas. Los comandos/configuraciones de build permanecen en los JSON/INI/logs de A/B/C, junto con los intentos anteriores; no se exige igualdad de hashes entre builds con distinta instrumentación/metadatos.

El emulador publicado corresponde al native final. El WASM de interfaz preparado, sin desplegar, ocupa 7.722.527 B y tiene SHA-256 `64c3763b16a9abbc9baca4154dbe161acf7631fb87ef6f0546f292a04dba31d6`. Se conserva en `prepared-web-final/`; no es el módulo headless WASM-MATH. La galería reúne **14 originales 320×240** con recursos relativos validados. Su ZIP no incluye fuentes, sondas ni backups físicos.

## Comprobaciones de UNIT-PICKER-FINALIZE-01

Antes de editar el informe, los **22 archivos** y las **1.451 fuentes del snapshot completo D-final** coincidían por SHA-256. El delta ejecutable permanece idéntico al candidato probado; esta finalización cambia únicamente este informe entre los archivos que se guardan. Se reutiliza D-final sin copiar de nuevo el repositorio. Se conservan sus hashes, parches y manifiestos originales en `out/unit-picker-finalize-01/received/`.

**Ejecutado ahora**, con el native publicado/candidato de SHA-256 `11d0a239ce816654a06fffbadab3fc17dfe28f70379808d9f5bf0c42a6836a17`:

- Generador del catálogo `--check`: los siete artefactos generados coinciden, incluido el índice nuevo.
- Selector: 14 recorridos por eventos, incluidos cancelación, prefijos, EXE mantenido, contactos ALPHA y composición larga.
- Sugerencias: 80 alternativas compatibles, identidades estables, favoritos y familias semánticas.
- Receptor obsoleto: el probe privado host final rechaza la selección y conserva la cantidad; vuelta a 2+2. No se instala en la PCB.
- Puerta rápida de Calculation: 113 casos preservados, con sus códigos de rechazo esperados.
- Conversión/Ans: un recorrido con 16 aserciones, `2 m → cm → Ans/(2 s) → 1 m/s`, y el mismo smoke mediante `run-emulator-windows.ps1` en un filesystem host aislado.
- `git diff --check`; comprobación de índice, configuración protegida, frontera de archivos e includes/artefactos necesarios.

**Reutilizado**, sin volver a ejecutar ni sumar como muestras nuevas: oráculo diferencial de B, 900 trazas, 100 ciclos host, 15 ciclos físicos, matriz final de navegadores y todas las medidas A/B/C. `received/identity.json` registra los hashes exactos de sus logs/JSON, y las fronteras de código correspondientes continúan iguales. Las excepciones históricas de WebKit, parMixed y Grapher conservan su alcance.

La única corrección adicional es la leyenda HTML **«Composici?n» → «Composición»**, en UTF-8. Se corrigió el generador local `out/unit-picker-ux-perf-01/package-gallery.py` y se regeneró el ZIP. No era una observación del LCD. Se verificaron los recursos relativos, 14 PNG originales de 320×240 idénticos por bytes/hashes a las capturas del candidato final, y ausencia de fuentes, sondas/backups. HTML anterior y ZIP recibido se conservan en `received/`; `gallery-validation.json` contiene el hash del ZIP corregido. Esta corrección de galería no requirió compilación ni pruebas matemáticas adicionales.

Runtime guardado: las fuentes ejecutables son las de D-final. Runtime ordinario documentado en PCB: SHA-256 `307341cf2a29dd8c2770d9ea8d242648b1dfb11300dda7f16d1df1a79a18c6cc`, 6.266.912 B, con lectura completa y boot previos verificados. Native publicado: SHA-256 `11d0a239ce816654a06fffbadab3fc17dfe28f70379808d9f5bf0c42a6836a17`, con DLL y copia anterior conservadas. Un HEAD nuevo en Git no equivale a un binario reinstalado: no se recompiló para cambiar metadata ni se atribuye retrospectivamente aceptación a otro hash.

## Archivos e integración

Producto: `src/apps/CalculationApp.cpp/.h`, `src/ui/Toolbox.cpp/.h`, `src/ui/UnitQuickChoices.h`, `src/math/ToolboxCatalog.cpp/.h`, `src/math/ToolboxView.h`, `src/math/units/UnitToolboxProvider.inc`, `src/math/units/UnitNavigationIndex.inc`.

Generación/pruebas: `scripts/generate-unit-catalog.py`, `scripts/build-toolbox-allocation-probe.py`, `scripts/test-calculation-quantities.py`, `scripts/test-quantity-faults.py`, `scripts/test-quantity-lifecycle.py`, `scripts/test-unit-picker.py`, `scripts/test-unit-picker-faults.py`, `tests/host/unit_picker_view_checks.cpp`, `tests/host/unit_quick_choices_checks.cpp`, `tests/wasm/calculation-quantities.mjs`, `tests/wasm/calculation-quantities-focused.mjs`, y este informe. `final-change-manifest.json` registra hashes y delta exacto; las sondas, binarios, backups y galerías permanecen en rutas ignoradas.

No cambia el contrato matemático ni el esquema de favoritos. La selección rápida termina en el mismo receptor `selectOutput/publishOutput`; solo una confirmación cambia la vista. Navegar no cambia canónico, Ans, PreAns, memorias ni Recientes de inserción. SCI/ENG/FIX parten de la representación elegida por la ruta anterior, sin aplicar un prefijo dos veces.

Asunto del único commit local autorizado en UNIT-PICKER-FINALIZE-01: `perf(units): accelerate output selection and streamline conversion`. Su SHA final se registra en el recibo externo; no se modifica este informe solo para insertar el hash del propio commit.
