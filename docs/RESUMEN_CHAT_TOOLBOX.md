# NumOS — resumen de toda la conversación sobre Toolbox

**Estado: 2 de octubre de 2026.** Este documento reúne la implementación inicial
TOOLBOX-01, las revisiones de diseño solicitadas, los alfabetos, las reparaciones
del emulador y el ajuste final de infinito. La imagen que lo acompaña,
[RESUMEN_CHAT_TOOLBOX.png](RESUMEN_CHAT_TOOLBOX.png), contiene capturas reales del
emulador, incluida la comparación entre el menú inicial y el popup actual.

## 1. Toolbox de uso diario

Se implementó una Toolbox compartida por Calculation, Equations, Calculus y
Grapher, filtrada según las funciones que admite cada editor. Se mantuvieron los
recorridos existentes de Steps en los resultados de Equations, las acciones
contextuales de Grapher y los controles VAR/STO, FORMAT, TABLE y ENG.

El catálogo tiene nombres, ayuda y búsqueda en inglés y español. La búsqueda
admite alias y acentos; en Windows se puede empezar a escribir con Toolbox
abierta. En el teclado de la calculadora se usa Buscar y ALPHA. Cada entrada tiene
una identidad estable y una receta que construye nodos matemáticos tipados.
La inserción conserva la expresión y coloca el cursor en el argumento adecuado;
una construcción fallida conserva el estado anterior del editor.

Se añadieron favoritos persistentes, con un límite explícito de 24, y los 12
recientes distintos de la sesión. Se pueden añadir, quitar y ordenar favoritos.
El guardado usa dos registros con generación y comprobación de integridad, para
poder recuperar el anterior si falla una escritura. Se añadieron mensajes de
lista vacía, ayuda y errores recuperables. La estructura admite proveedores
adicionales de catálogo sin ampliar ahora el alcance a unidades o nuevas apps.

## 2. Del menú plano al popup

La primera versión ocupaba demasiado espacio y repetía funciones con tecla
propia. Tras las imágenes y comentarios, se cambió a un popup que deja visible
la expresión en la que se estaba trabajando, con pestañas de Funciones,
Favoritos, Recientes y Buscar. La raíz presenta valor absoluto, raíz de índice y
logaritmo, seguidos de categorías.

Se ocultaron de la navegación y la búsqueda los duplicados con tecla directa,
como fracción y potencia, conservando sus identidades para favoritos antiguos.
Las previews pasaron de casillas vacías a argumentos matemáticos legibles.
Se corrigió el pivote de dibujo que desplazaba las barras de valor absoluto.
La expresión superior sigue la zona de edición cuando una fórmula es alta.

Se estudió la organización de NumWorks como referencia. La implementación usa
el AST, el renderer y las fuentes de NumOS; no incorpora Poincaré ni Escher.

## 3. Errores señalados en las capturas y nuevos alfabetos

Se corrigieron el foco incompleto de las pestañas, los indicadores de submenú
demasiado pequeños y la superposición del aviso de búsqueda sin resultados con
la consulta. El foco usa relleno y subrayado, las flechas se dibujan con trazos
de dos píxeles y las ayudas usan una fuente de 12 píxeles. La consulta tiene
fallback para español y caracteres griegos.

Se añadió **Variables** con tres submenús:

- **Latin Alphabet:** las 26 letras de a a z y sus mayúsculas.
- **Greek Alphabet:** las 24 letras de α a ω y sus mayúsculas.
- **Special Characters:** π, e, i, ϑ, ϕ, ϵ, ϖ, ς e infinito.

La decisión de interacción fue **EXE para insertar la minúscula; DERECHA para
abrir las dos formas, con la mayúscula ya seleccionada; EXE para confirmarla**.
Así la elección queda visible y coincide con las variantes del logaritmo.
Las variantes se pueden buscar y guardar por separado. No depende de SHIFT.

Se eliminaron las constantes azules: la tinta matemática normal es negra.
Se reutilizaron los glifos STIX y la fuente dedicada de Δ, también para variantes
griegas y tamaños de subíndice/superíndice. Se corrigió la medición de tinta real,
incluidos descendentes y salientes, y la compensación itálica en potencias con
bases griegas o envueltas en una fila. Layout y dibujo comparten las medidas.

Se distinguieron las letras libres e/i/I/π de las constantes reservadas del CAS
mediante una correspondencia cerrada de identificadores Unicode, conservada en
el resultado, Ans e historial. A–F mantienen su comportamiento de memorias STO.
Los símbolos admitidos se validan mediante una lista cerrada; no se inserta texto
CAS arbitrario desde el catálogo.

## 4. Emulador de Windows y disponibilidad durante el trabajo

Se aclaró el mapa de teclas: **F6 abre Toolbox, F5 abre sus opciones y F8 vuelve**.
Las flechas navegan y Enter confirma. El comando habitual sigue siendo:

```powershell
.\scripts\run-emulator-windows.ps1
```

Hubo varias incidencias de ejecutable ausente. Se restauró una versión validada
con sus DLL y se identificó la causa de la repetición: el auto-clean de PlatformIO
podía borrar el directorio de compilación completo al cambiar la configuración
o la estructura de fuentes; el ejecutable publicado estaba dentro de él.

Se separaron definitivamente estas rutas:

- **Publicación estable:** `C:\.piobuild\numOS\emulator_pc\program.exe`, con sus DLL.
- **Compilación predeterminada:** `C:\.piobuild\numOS-build`.

El lanzador prioriza la publicación estable, incluso si otra configuración apunta
a una compilación incompleta. El nuevo `publish-emulator-windows.ps1` comprueba
el candidato y sus runtimes, ejecuta el cálculo 2+3 y sustituye el ejecutable de
forma atómica, conservando `previous-program.exe`. Si falla la validación, las DLL
no coinciden o Windows bloquea el archivo en uso, conserva la versión activa.
No cierra la sesión del usuario para sustituirla.

La obligación de conservar siempre un emulador funcional se escribió tanto en
el **AGENTS.md del repositorio** como en las instrucciones globales para este
workspace. Se verificaron instalación inicial, candidato inválido, bloqueo,
sustitución, copia previa, runtimes incompatibles y rotaciones. También se ejecutó
un clean real en una carpeta aislada: 220 muestras confirmaron que la versión
publicada permaneció accesible y sin cambios.

## 5. Ajuste final de infinito

En **Variables → Special Characters**, la entrada principal es **∞**, sin «+»
escrito. Enter la inserta directamente y DERECHA abre estas tres variantes:

| Forma | Significado al calcular |
|---|---|
| ∞ | Infinito positivo; también admite un signo escrito antes. |
| +∞ | Infinito positivo con signo explícito. |
| −∞ | Infinito negativo. |
| ±∞ | Ambos valores, representados por la lista `[-∞, +∞]`. |

**±∞ no se trata como un único número** ni como el infinito complejo sin dirección
de Giac. Su ayuda explica que produce una lista. Cada variante tiene identidad
propia para búsqueda, recientes y favoritos. El signo negativo permanece unido
al átomo al serializarlo: se verificaron `2·(−∞) = −∞` y `(−∞)² = +∞`.
Al pulsar potencia sobre un infinito con signo, se añaden paréntesis reales al
AST: el editor muestra **(−∞)²**, con la misma agrupación que evalúa el motor.

El catálogo final contiene **152 identidades**, de las cuales **140 son
descubribles** y 12 se conservan por compatibilidad con accesos anteriores.
Special Characters sigue teniendo nueve filas principales; las nuevas variantes
viven dentro del submenú. Infinito mantiene su disponibilidad en Calculation.

## 6. Comprobaciones y límites de lo validado

Las revisiones anteriores incluyeron todos los elementos visibles del catálogo,
279 recorridos de capacidades entre las otras apps, fallos de asignación y de
persistencia, edición e historial, 151.538 controles de espaciado, 2.553 de viewport
y pruebas de corchetes. Se probaron Chromium, Firefox y WebKit, tanto la página
completa como el componente, incluido guardado real de favoritos tras recargar.

Se midieron 100 ciclos tras calentamiento con un pool LVGL de 64 KiB, sin crecimiento
acumulado. La revisión de alfabetos registró un mínimo de 5.872 B libres y un mayor
bloque de 5.160 B. Son mediciones del emulador, no del tiempo ni de la pila en placa.

La revisión de infinito pasa **207 recorridos nativos**, que comprueban navegación,
cálculo, serialización, historial, favoritos y alias. Se verifica además el
rasterizado en los tres tamaños STIX.
Los controles host finales verifican 21.422 condiciones de catálogo y persistencia,
629 puntos de fallo de asignación y 1.645 condiciones de corchetes. Los píxeles de
los nuevos símbolos caben en sus cajas; el layout de átomos no reserva heap.
Tras añadir la agrupación de potencias, pasan de nuevo los 16 recorridos de
infinito y los controles host de editor, resultados, catálogo y corchetes.
La versión final compila en nativo, WebAssembly y los perfiles ESP32 WROOM y CAM;
la prueba web final vuelve a pasar en los tres navegadores y las dos superficies.
El ejecutable nuevo queda publicado y probado mediante el lanzador habitual.

La ampliación de infinito no añade campos a los nodos ni widgets a la UI. El
catálogo crece en flash. Agrupar una base con signo añade un NodeParen y una fila
al crear la potencia; no añade reservas durante su trazado. La creación del AST
y el cálculo de la lista de dos
infinitos ocurren fuera de layout/draw/cursor. No se afirma haber medido el máximo
real de pila de tarea ni la latencia de PSRAM en el dispositivo.

Perfil final obtenido de los ELF, con imágenes locales y sin flash:

| Perfil | Imagen | RAM estática | Texto IRAM |
|---|---:|---:|---:|
| WROOM | 5.751.328 B | 119.544 B | 60.407 B |
| CAM | 5.663.600 B | 118.136 B | 59.039 B |

RAM estática e IRAM no crecen frente a la revisión de alfabetos. En Xtensa,
`Session` sigue ocupando 1.380 B. Agrupar el infinito con signo añade 112 B de
contenido de AST y capacidad de vector, más la contabilidad del allocator, solo
al crear esa potencia: NodeRow 56 B + NodeParen 52 B + un puntero de 4 B.
Los marcos propios de insertPower, layout de NodeRow y layout de NodeParen son
48 B cada uno; no equivalen al máximo de pila de toda la tarea. Las cajas y la
tinta se comprobaron en el emulador, incluidos los paréntesis de la base negativa.

No se ha hecho flash, validación física del LCD, commit ni publicación remota.
Se conserva el trabajo previo del repositorio y la configuración de VS Code.

## 7. Dónde queda cada entrega

| Evidencia | Ubicación |
|---|---|
| Implementación y validación inicial | `out/toolbox-01/` |
| Revisión del popup | `out/toolbox-popup-01/` |
| Alfabetos, glifos y correcciones visuales | `out/toolbox-variables-01/` |
| Disponibilidad permanente del emulador | `out/emulator-availability-01/` |
| Infinito, pruebas y capturas finales | `out/toolbox-infinity-01/` |
| Detalle técnico y catálogo generado | `docs/TOOLBOX_01.md`, `docs/TOOLBOX_01_CATALOG.md` |

Las cifras de los documentos históricos pertenecen a cada revisión; no deben
confundirse con el número de entradas de la versión final. Los dos archivos de
resumen solicitados son este texto y **RESUMEN_CHAT_TOOLBOX.png**.
