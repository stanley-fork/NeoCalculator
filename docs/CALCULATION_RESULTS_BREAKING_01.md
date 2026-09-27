# Calculation Results Breaking 01

Actualización posterior: las cuatro familias y once ejemplos de este informe histórico se reparan y verifican en [Calculation FORMAT 01](CALCULATION_FORMAT_01.md). Las observaciones siguientes conservan el estado anterior para la comparación detectora.

**67 expresiones pequeñas en cuatro rondas**, introducidas por eventos reales de Calculation. Se investigan capacidades y respuestas matemáticas, sin usar overflow de pantalla, números gigantes, agotamiento de recursos ni límites físicos como fallos. Los problemas descritos **no se corrigen**: el código de esta sesión repara el renderer, según [Renderer Recovery 02](RENDERER_RECOVERY_02.md).

Corpus: `tests/fixtures/calculation-results-breaking.json`. Runner: `scripts/test-calculation-results-breaking.py`. Evidencia: `out/renderer-recovery-02/results-final-r1`…`results-final-r4`; resumen normalizado `results-corpus.json`. Cada caso conserva teclas, AST de entrada, serialización, AST realmente mostrado, estado, texto exacto/aproximado, `.numos` y capturas 320×240/4×.

Completar una sesión del runner significa que se obtuvo evidencia; **no significa que su matemática haya pasado**. El observador privado solo lee campos. No introduce expresiones directamente en el AST ni añade entradas alternativas al producto.

## Rondas

| Ronda | Casos | Investigación |
|---|---:|---|
| 1 | 24 | Aritmética, fracciones, potencias racionales, raíces, logaritmos, trigonometría e identidades numéricas |
| 2 | 20 | Álgebra simbólica, complejos, ramas principales, RAD/DEG y dominios |
| 3 | 17 | Ans/PreAns, historial, S⇔D, factorial/factorización y precisión |
| 4 | 6 | Refinamiento de bases de logaritmo inválidas y controles de Ans real/complejo directo |

Los intentos iniciales con un alias FACT inexistente y SHIFT+log —que introduce otra plantilla— se conservan como errores del runner, no del producto. El corpus final usa `logbase` y la matriz real SHIFT `(4,0)` + multiplicación `(1,7)` para la tecla publicada como factorial.

## Fallos confirmados, pendientes

### RES-01 — La tecla factorial no introduce factorial

Tres reproducciones: `5`, factorial, EXE devuelve **5**; `0`, factorial, EXE devuelve **0**; `3`, factorial, `+2`, EXE devuelve **5**. Se esperan respectivamente **120, 1 y 8**.

La matriz identifica esa combinación como `SemanticId::factorial`, con leyenda `!`. Calculation recibe `KeyCode::FACT`; su handler solo factoriza en primos si ya existe un resultado. Durante la edición no añade ningún operador: el primer AST sigue siendo `Number "5"` y se serializa exactamente `5`. Es un conflicto de entrada/acción que termina dando una respuesta a otra expresión, no una incapacidad aritmética de Giac. El Giac incluido evalúa `5!` como 120 y `0!` como 1 en la prueba host separada.

Después de EXE la misma tecla sí muestra factorización de enteros, por ejemplo 360. Ese comportamiento histórico se registra como control, sin confundir factorización prima y factorial. No se cambia ninguno en esta entrega.

### RES-02 — Ans reutiliza un número anterior después de un complejo

Reproducción: `5 EXE`, AC, `√(-1) EXE`, AC, `Ans² EXE`. La pantalla intermedia muestra **i**, pero la final muestra **25**, en lugar de **−1**. La última serialización es `((numos_Ans)^(2))`.

El flujo actual solo actualiza Ans cuando obtiene su representación numérica admitida; el complejo no la tiene y conserva el 5 anterior. El control directo `[√(-1)]²` da −1, y el control `5 EXE`, `3 EXE`, `Ans² EXE` da 9. Por tanto no es un problema de potencia, pantalla o tamaño. No se cambia la política de variables en esta entrega.

`Ans` después de una expresión simbólica también conserva el valor numérico previo. El código documenta una política de Ans numérico: se registra como limitación funcional relacionada, sin inflar el número de fallos ni afirmar que existía soporte simbólico prometido.

### RES-03 — Simplificaciones exactas disponibles no llegan al resultado visible

Cuatro reproducciones confirmadas por AST de resultado y captura:

| Entrada | Resultado visible actual | Respuesta exacta disponible |
|---|---|---|
| `27^(1/3)` | raíz cúbica de 27 | 3 |
| `2x+3x` | `2×x+3×x` | `5x` |
| `sin(1)^2+cos(1)^2` | la misma suma | 1 |
| `sin(x)^2+cos(x)^2` | la misma suma | 1 |

Son expresiones equivalentes sin reducir, **no igualdades falsas**. En los casos numéricos hay aproximación disponible; el defecto funcional es la ausencia de simplificación exacta. En la identidad simbólica el alumno tampoco recibe el 1 exacto que el backend puede producir.

`tests/host/calculation_result_capabilities.cpp` compara las entradas actuales `evaluateStructured` y `simplify` del mismo Giac enlazado. `capabilities/results.log` conserva los resultados: `simplify` devuelve 3, `5*x`, 1 y 1 con estado OK. El test host demuestra capacidad existente; no sustituye las sesiones de teclas de producto. No se activa `simplify` ni se cambia la evaluación.

**Control negativo importante:** `√2×√8` conserva un texto interno sin reducir, pero su AST de resultado y la pantalla muestran **4**. Se excluye de los fallos visibles. No se deduce el resultado dibujado mirando solo `_exactText`. Tampoco se exige expandir arbitrariamente `(√2+√3)²`, ni cancelar `(x²−1)/(x−1)` ignorando su condición de dominio.

### RES-04 — Bases de logaritmo inválidas aceptadas como resultado correcto

Tres reproducciones:

| Entrada por plantilla | Serialización observada | Resultado/estado |
|---|---|---|
| `log₁(2)` | `logb((2),(1))` | `oo`, OK |
| `log₁(0.5)` | `logb((0.5),(1))` | `infinity`, OK |
| `log₀(2)` | `logb((2),(0))` | `0`, OK |

Las bases 0 y 1 no definen ese logaritmo. La división por log(base) no valida el dominio original. Estos casos necesitan una respuesta de dominio inválido; devolver un número o infinito con estado correcto no representa el problema introducido. Todos son pequeños y finitos como entradas; no es overflow numérico o gráfico.

Controles: `log₂(8)=3`, `log₀.₅(8)=−3`, `log₂(1)=0`; `log₁(1)` sí devuelve undefined. Ese último caso demuestra que el fallo no es una imposibilidad general de expresar un estado indefinido. No se parchean parser, serialización, Giac ni clasificación del resultado.

## Controles y discrepancias que no se cuentan como errores nuevos

- RAD/DEG y trigonometría ordinaria pasan los controles seleccionados. Se preservan ramas principales complejas; `(-8)^(1/3)` no se denuncia por no elegir la raíz real.
- `0^0` devuelve undefined. `1/0` sigue la política existente de infinito; no se convierte automáticamente en un bug distinto.
- La opción usada por el harness se llama **equations complex policy**. Que Calculation devuelva complejos con esa opción no demuestra que viole una política propia de Calculation.
- `0.3−0.2−0.1` deja un residuo de doble precisión. Se registra como precisión numérica, no como falta de capacidad ni como un nuevo problema del renderer.
- La factorización de expresiones simbólicas o complejas muestra su error explícito; no se afirma que convierta esos resultados en cero.
- Historial, PreAns, decimal periódico y radicales tienen controles separados. No se usan capturas con árboles distintos para declarar una mejora o regresión.

Los resultados observados de **60 sesiones comparables** coinciden antes/después del arreglo del renderer. Una sesión con la ruta logbase corregida y las seis nuevas de la cuarta ronda no tienen comparación inicial equivalente; se indican como tales en `results-invariance.json`. Los siete controles de viewport comparan además todos los payloads de nodos, slots, serialización, estados y respuestas, excluyendo únicamente geometría y posición del cursor. Las 900 trazas del tutor también mantienen su matemática.

## Reproducción y revisión

```powershell
python scripts/build-calculation-observer.py --source SOURCE --build BUILD --compile-commands COMPILE_COMMANDS.json --out OBSERVER_ASCII
python scripts/test-calculation-results-breaking.py --bin OBSERVER_ASCII/observer.exe --out out/results-round1 --round 1
# Repetir --round 2, 3 y 4 en directorios distintos.
```

La galería `out/renderer-recovery-02/visual/index.html` y su ZIP contienen los once ejemplos confirmados, controles, originales y ampliaciones enteras. El factorial conserva además la captura editable: permite ver que se perdió la intención antes de evaluar. Ans incluye el estado intermedio i y el resultado posterior 25. No se afirma revisión humana del LCD.

Quedan pendientes **cuatro familias y once ejemplos confirmados**: 3 de factorial, 1 de Ans complejo, 4 de simplificación exacta y 3 de dominio de logaritmos. Ninguna recibe corrección, cambio de expected a su valor erróneo ni promoción de golden en esta sesión.
