# Tutor EN / ES — catálogo generado

Fuente: `src/math/tutor/Messages.inc`. No editar esta tabla manualmente.

Clave | Contexto y parámetros | English | Español
--- | --- | --- | ---
domain.nonzero | src\math\giac\GiacTutor.inc:486;  | A denominator cannot be zero. Keep this restriction throughout the solution. | Un denominador no puede ser cero. Conserva esta restricción durante toda la resolución.
values.substitute | src\math\giac\GiacTutor.inc:489;  | Replace stored variables with their saved values for this solve. | Sustituye las variables almacenadas por los valores guardados para esta resolución.
algebra.expand | src\math\giac\GiacTutor.inc:492;  | Expand the products and powers so we can collect like terms. | Desarrolla los productos y las potencias para poder agrupar los términos semejantes.
algebra.collect | src\math\giac\GiacTutor.inc:495;  | Collect like terms by combining the coefficients of each power. | Reduce los términos semejantes sumando los coeficientes de cada potencia.
equation.swap | src\math\giac\GiacTutor.inc:498; variable | Swap the two sides to put {0} on the left. | Intercambia los miembros para situar {0} a la izquierda.
equation.add | src\math\giac\GiacTutor.inc:527; expression,variable | Add {0} to both sides to isolate the term containing {1}. | Suma {0} a ambos miembros para dejar aislado el término que contiene {1}.
equation.subtract | src\math\giac\GiacTutor.inc:527; expression,variable | Subtract {0} from both sides to isolate the term containing {1}. | Resta {0} a ambos miembros para dejar aislado el término que contiene {1}.
equation.divide | src\math\giac\GiacTutor.inc:557; expression,variable | Divide both sides by {0} to isolate {1}. | Divide ambos miembros entre {0} para despejar {1}.
rational.clear | src\math\giac\GiacTutor.inc:564;  | Multiply both sides by the common denominator to remove the fractions. Keep the original restrictions. | Multiplica ambos miembros por el denominador común para eliminar las fracciones. Conserva las restricciones originales.
polynomial.factor | src\math\giac\GiacTutor.inc:567;  | Factor the left side so we can use the zero-product rule. | Factoriza el miembro izquierdo para aplicar la propiedad del producto nulo.
square.branches | src\math\giac\GiacTutor.inc:570;  | Take the square root with both signs to include every solution. | Considera la raíz cuadrada con ambos signos para incluir todas las soluciones.
quadratic.formula | src\math\giac\GiacTutor.inc:574; variable | Use the quadratic formula to find every value of {0} that satisfies the equation. | Aplica la fórmula general de la ecuación de segundo grado para hallar todos los valores de {0} que satisfacen la ecuación.
product.zero | src\math\giac\GiacTutor.inc:583;  | A product is zero when at least one factor is zero. Set each factor to zero and solve each case. | Un producto es cero si al menos uno de sus factores es cero. Iguala cada factor a cero y resuelve cada caso.
candidate.excluded | src\math\giac\GiacTutor.inc:587;  | Reject this candidate: it makes an original denominator zero. | Descarta este candidato: anula un denominador de la ecuación original.
terminal.isolated | src\math\giac\GiacTutor.inc:590; variable | {0} is already isolated. This equation gives its solution directly. | La incógnita {0} ya está despejada. Esta ecuación da su solución directamente.
terminal.identity | src\math\giac\GiacTutor.inc:594;  | Both sides are equal for every value in the selected domain. | Ambos miembros son iguales para cualquier valor del dominio elegido.
terminal.conditional | src\math\giac\GiacTutor.inc:594;  | Both sides are equal for every value satisfying the displayed denominator restrictions. | Ambos miembros son iguales para cualquier valor que cumpla las restricciones indicadas sobre los denominadores.
terminal.contradiction | src\math\giac\GiacTutor.inc:597;  | The equality is false: no value can satisfy it. | La igualdad es falsa: ningún valor puede satisfacerla.
terminal.no_real | src\math\giac\GiacTutor.inc:573;  | The discriminant is negative. No real square root exists, so there are no real solutions. | El discriminante es negativo y no tiene raíz cuadrada real. Por tanto, no hay soluciones reales.
terminal.complete | src\math\giac\GiacTutor.inc:601; src\math\tutor\TeachingPlan.h:55;  | These are all solutions under the displayed restrictions. Each also satisfies the original equation. | Estas son todas las soluciones que cumplen las restricciones indicadas. Todas satisfacen también la ecuación original.
system.swap | src\math\giac\GiacTutor.inc:604; equation row,equation row | Swap equations {0} and {1} to use a nonzero coefficient for elimination. | Intercambia las ecuaciones {0} y {1} para usar un coeficiente distinto de cero en la eliminación.
system.scale | src\math\giac\GiacTutor.inc:609; equation row,expression,variable | Divide equation {0} by {1} to make the coefficient of {2} equal to 1. | Divide la ecuación {0} entre {1} para que el coeficiente de {2} sea 1.
system.add | src\math\giac\GiacTutor.inc:625; expression,equation row,equation row,variable | Add {0} times equation {1} to equation {2} to eliminate {3}. | Suma a la ecuación {2} la ecuación {1} multiplicada por {0} para eliminar {3}.
system.subtract | src\math\giac\GiacTutor.inc:625; expression,equation row,equation row,variable | Subtract {0} times equation {1} from equation {2} to eliminate {3}. | Resta a la ecuación {2} la ecuación {1} multiplicada por {0} para eliminar {3}.
system.unique | src\math\giac\GiacTutor.inc:647;  | Each variable is isolated. These values form one simultaneous solution tuple. | Cada incógnita está despejada. Estos valores forman una única solución conjunta del sistema.
system.family | src\math\giac\GiacTutor.inc:646;  | A zero row adds no constraint. Free variables may vary, but must satisfy the remaining equations together. | Una fila nula no añade restricciones. Las variables libres pueden variar, pero deben satisfacer conjuntamente las ecuaciones restantes.
system.inconsistent | src\math\giac\GiacTutor.inc:645;  | A row requires zero to equal a nonzero value. The equations have no simultaneous solution. | Una fila exige que cero sea igual a un valor distinto de cero. El sistema es incompatible: no tiene solución conjunta.
status.unsupported | src\apps\TutorStepsView.inc:17;  | A checked method is not available for this equation family. The ordinary Giac result remains available. | El tutor aún no dispone de un método comprobado para esta familia de ecuaciones. El resultado de Giac sigue disponible.
status.partial | src\apps\TutorStepsView.inc:17;  | The explanation reached its resource limit. The verified steps shown are partial; completeness is not established. | La explicación ha alcanzado su límite de recursos. Los pasos mostrados están verificados, pero la explicación es parcial y no se ha demostrado que incluya todas las soluciones.
status.check_failed | src\apps\TutorStepsView.inc:15;  | The tutor could not verify this derivation. Its instructions are withheld; the ordinary result remains available. | El tutor no ha podido verificar esta derivación y no muestra sus instrucciones. El resultado de Giac sigue disponible.
status.reconciliation | src\apps\TutorStepsView.inc:16;  | The checked derivation and the Giac answer adapter disagree. The ordinary result has not been changed. | La derivación comprobada y el resultado de Giac no coinciden. El resultado de Giac no se ha modificado.
product.repeated | src\math\giac\GiacTutor.inc:581;  | A square is zero only when its base is zero. Solve this one case; the repeated root is counted once. | Un cuadrado solo es cero cuando su base es cero. Resuelve este único caso; la raíz repetida se cuenta una sola vez.
terminal.excluded_all | src\math\giac\GiacTutor.inc:601;  | Every candidate violates an original denominator restriction, so there are no solutions. | Todos los candidatos incumplen alguna restricción de los denominadores originales. No hay soluciones.
system.add_unit | src\apps\TutorPresentation.inc:280; src\math\giac\GiacTutor.inc:620; equation row,equation row,variable | Add equation {0} to equation {1} to eliminate {2}. | Suma la ecuación {0} a la ecuación {1} para eliminar {2}.
system.subtract_unit | src\apps\TutorPresentation.inc:280; src\math\giac\GiacTutor.inc:620; equation row,equation row,variable | Subtract equation {0} from equation {1} to eliminate {2}. | Resta la ecuación {0} a la ecuación {1} para eliminar {2}.
view.alternative | src\apps\TutorStepsView.inc:104; integer | Case {0} | Caso {0}
view.active_alternative | src\apps\TutorStepsView.inc:129; src\apps\TutorStepsView.inc:88; integer | Case {0}: current step | Caso {0}: paso actual
view.rejected_alternative | src\apps\TutorStepsView.inc:125; src\apps\TutorStepsView.inc:87; integer | Case {0}: excluded | Caso {0}: excluido
view.equation | src\apps\TutorStepsView.inc:102; src\apps\TutorStepsView.inc:178; src\apps\TutorStepsView.inc:256; src\apps\TutorStepsView.inc:262; src\apps\TutorStepsView.inc:279; src\apps\TutorStepsView.inc:280; src\apps\TutorStepsView.inc:283; integer | Equation {0} | Ecuación {0}
view.formula_unavailable | checked rule/message API;  | This formula cannot be displayed in the current view. | Esta fórmula no se puede mostrar en la vista actual.
equation.add_collect | src\math\giac\GiacTutor.inc:536; variable | Add the displayed term to both sides to collect the {0} terms on the left. | Suma el término indicado a ambos miembros para agrupar los términos con {0} a la izquierda.
equation.subtract_collect | src\math\giac\GiacTutor.inc:536; variable | Subtract the displayed term from both sides to collect the {0} terms on the left. | Resta el término indicado a ambos miembros para agrupar los términos con {0} a la izquierda.
equation.add_zero_right | src\math\giac\GiacTutor.inc:532; expression | Add {0} to both sides to put zero on the right. | Suma {0} a ambos miembros para dejar cero a la derecha.
equation.subtract_zero_right | src\math\giac\GiacTutor.inc:532; expression | Subtract {0} from both sides to put zero on the right. | Resta {0} a ambos miembros para dejar cero a la derecha.
equation.add_displayed_isolate | src\math\giac\GiacTutor.inc:528; variable | Add the displayed amount to both sides to isolate the term containing {0}. | Suma la cantidad indicada a ambos miembros para dejar aislado el término que contiene {0}.
equation.subtract_displayed_isolate | src\math\giac\GiacTutor.inc:528; variable | Subtract the displayed amount from both sides to isolate the term containing {0}. | Resta la cantidad indicada a ambos miembros para dejar aislado el término que contiene {0}.
equation.add_displayed_zero_right | src\math\giac\GiacTutor.inc:533;  | Add the displayed expression to both sides to put zero on the right. | Suma la expresión indicada a ambos miembros para dejar cero a la derecha.
equation.subtract_displayed_zero_right | src\math\giac\GiacTutor.inc:533;  | Subtract the displayed expression from both sides to put zero on the right. | Resta la expresión indicada a ambos miembros para dejar cero a la derecha.
equation.add_system | src\math\giac\GiacTutor.inc:539; expression | Add {0} to both sides to collect the variable terms on the left. | Suma {0} a ambos miembros para agrupar los términos con incógnitas a la izquierda.
equation.subtract_system | src\math\giac\GiacTutor.inc:539; expression | Subtract {0} from both sides to collect the variable terms on the left. | Resta {0} a ambos miembros para agrupar los términos con incógnitas a la izquierda.
equation.add_displayed_system | src\math\giac\GiacTutor.inc:540;  | Add the displayed expression to both sides to collect the variable terms on the left. | Suma la expresión indicada a ambos miembros para agrupar los términos con incógnitas a la izquierda.
equation.subtract_displayed_system | src\math\giac\GiacTutor.inc:540;  | Subtract the displayed expression from both sides to collect the variable terms on the left. | Resta la expresión indicada a ambos miembros para agrupar los términos con incógnitas a la izquierda.
equation.divide_square | src\math\giac\GiacTutor.inc:557; expression,variable | Divide both sides by {0} to isolate the square of {1}. | Divide ambos miembros entre {0} para aislar el cuadrado de {1}.
equation.divide_displayed | src\math\giac\GiacTutor.inc:558; variable | Divide both sides by the displayed coefficient to isolate {0}. | Divide ambos miembros entre el coeficiente indicado para despejar {0}.
equation.divide_displayed_square | src\math\giac\GiacTutor.inc:558; variable | Divide both sides by the displayed coefficient to isolate the square of {0}. | Divide ambos miembros entre el coeficiente indicado para aislar el cuadrado de {0}.
square.zero | src\math\giac\GiacTutor.inc:570;  | Zero has just one square root: zero. | Cero tiene una sola raíz cuadrada: cero.
system.scale_displayed | src\math\giac\GiacTutor.inc:609; equation row,variable | Divide equation {0} by the displayed coefficient to make the coefficient of {1} equal to 1. | Divide la ecuación {0} entre el coeficiente indicado para que el coeficiente de {1} sea 1.
system.add_scaled | src\math\giac\GiacTutor.inc:626; equation row,equation row,variable | Add the displayed multiple of equation {0} to equation {1} to eliminate {2}. | Suma el múltiplo indicado de la ecuación {0} a la ecuación {1} para eliminar {2}.
system.subtract_scaled | src\math\giac\GiacTutor.inc:626; equation row,equation row,variable | Subtract the displayed multiple of equation {0} from equation {1} to eliminate {2}. | Resta el múltiplo indicado de la ecuación {0} a la ecuación {1} para eliminar {2}.
view.step | src\apps\TutorStepsView.inc:333; integer,integer | Step {0} of {1} | Paso {0} de {1}
view.summary | src\apps\TutorStepsView.inc:218; src\apps\TutorStepsView.inc:333; integer,integer | Summary {0} of {1} | Resumen {0} de {1}
view.start | src\apps\TutorStepsView.inc:176;  | Starting equations | Ecuaciones iniciales
view.solutions | src\apps\TutorStepsView.inc:140; src\apps\TutorStepsView.inc:197; src\apps\TutorStepsView.inc:209; src\apps\TutorStepsView.inc:93;  | Solutions | Soluciones
view.no_solution | src\apps\EquationsApp.cpp:587; src\apps\TutorStepsView.inc:140; src\apps\TutorStepsView.inc:154; src\apps\TutorStepsView.inc:206;  | No solution | Sin solución
view.family | src\apps\TutorStepsView.inc:207;  | Solution family | Familia de soluciones
view.already | src\apps\TutorStepsView.inc:208;  | Already solved | Ya está resuelta
view.identity | src\apps\TutorStepsView.inc:209;  | An identity | Una identidad
view.guided_hint | src\apps\TutorStepsView.inc:336; src\apps\TutorStepsView.inc:392; src\apps\TutorStepsView.inc:6;  | <> Step  ^v Scroll  EXE Summary  VAR Pan  BACK | <> Paso ^v Despl. EXE Resumen VAR Horiz. BACK
view.summary_hint | src\apps\TutorStepsView.inc:336; src\apps\TutorStepsView.inc:392;  | <> Step  ^v Scroll  EXE Guided  VAR Pan  BACK | <> Paso ^v Despl. EXE Guiada VAR Horiz. BACK
view.before | src\apps\TutorStepsView.inc:219;  | Before | Antes
view.after | checked rule/message API;  | After | Después
view.restrictions | src\apps\TutorStepsView.inc:119; src\apps\TutorStepsView.inc:136; src\apps\TutorStepsView.inc:143; src\apps\TutorStepsView.inc:160; src\apps\TutorStepsView.inc:171; src\apps\TutorStepsView.inc:295; integer | Restrictions ({0}) | Restricciones ({0})
view.operand | src\apps\TutorStepsView.inc:268;  | Amount used on both sides | Cantidad aplicada a ambos miembros
view.unchanged_case | src\apps\TutorStepsView.inc:88; integer | Case {0}: unchanged | Caso {0}: sin cambios
view.solution_number | src\apps\EquationsApp.cpp:638; src\apps\TutorStepsView.inc:103; src\apps\TutorStepsView.inc:81; integer | Solution {0} | Solución {0}
view.system_together | checked rule/message API;  | One simultaneous solution | Una solución conjunta
view.verified | src\apps\TutorStepsView.inc:338; src\apps\TutorStepsView.inc:393;  | Verified | Verificado
view.partial_short | src\apps\TutorStepsView.inc:338; src\apps\TutorStepsView.inc:393;  | Partial | Parcial
view.coefficients | src\apps\TutorStepsView.inc:181;  | Identify the coefficients | Identifica los coeficientes
view.discriminant | src\apps\TutorStepsView.inc:187;  | Compute the discriminant | Calcula el discriminante
view.quadratic_formula | src\apps\TutorStepsView.inc:192; src\apps\TutorStepsView.inc:193;  | Use the quadratic formula | Aplica la fórmula general
view.substitution | src\apps\TutorStepsView.inc:194;  | Substitute the checked values | Sustituye los valores comprobados
view.match_coefficients | src\apps\TutorStepsView.inc:181;  | Write the equation with zero on the right, then match its coefficients with the standard form. | Escribe la ecuación con cero a la derecha e identifica sus coeficientes comparándola con la forma general.
view.compute_discriminant | src\apps\TutorStepsView.inc:187;  | The discriminant determines which square root the formula uses. | El discriminante determina la raíz cuadrada que aparece en la fórmula.
view.use_formula | src\apps\TutorStepsView.inc:192;  | Insert the coefficients and discriminant. The two signs give the possible solutions. | Sustituye los coeficientes y el discriminante. Los dos signos dan las posibles soluciones.
view.read_roots | src\apps\TutorStepsView.inc:198; src\apps\TutorStepsView.inc:204;  | Evaluate the minus and plus choices to obtain the solutions shown. | Calcula las dos opciones, con el signo menos y con el signo más, para obtener las soluciones indicadas.
view.standard_form | src\apps\TutorStepsView.inc:183;  | Standard form | Forma general
view.coefficient_values | src\apps\TutorStepsView.inc:184;  | Checked coefficients | Coeficientes comprobados
view.definition | src\apps\TutorStepsView.inc:188;  | Definition | Definición
view.evaluated | src\apps\TutorStepsView.inc:189;  | With these values | Con estos valores
view.starting_system | src\apps\TutorStepsView.inc:176;  | Find values that satisfy all these equations together. | Busca valores que satisfagan todas estas ecuaciones a la vez.
view.start_equation | checked rule/message API;  | Starting equation | Ecuación inicial
view.original | src\apps\TutorStepsView.inc:116; src\apps\TutorStepsView.inc:141; src\apps\TutorStepsView.inc:158; src\apps\TutorStepsView.inc:162;  | Original equation | Ecuación original
view.current | src\apps\TutorStepsView.inc:68;  | Current equation | Ecuación actual
view.unavailable | src\apps\EquationsApp.cpp:582; src\apps\TutorStepsView.inc:394;  | This teaching page could not be prepared. The ordinary result is available with BACK. | No se ha podido preparar esta página. Pulsa BACK para volver al resultado de Giac.
view.expand | src\apps\TutorStepsView.inc:239;  | Expand the expression | Desarrolla la expresión
view.collect | src\apps\TutorStepsView.inc:240;  | Collect like terms | Reduce los términos semejantes
view.balance | src\apps\TutorStepsView.inc:241; src\apps\TutorStepsView.inc:242;  | Balance both sides | Opera en ambos miembros
view.divide | src\apps\TutorStepsView.inc:243;  | Divide both sides | Divide ambos miembros
view.clear | src\apps\TutorStepsView.inc:244;  | Clear the denominators | Elimina los denominadores
view.factor | src\apps\TutorStepsView.inc:245;  | Factor the polynomial | Factoriza el polinomio
view.square_roots | src\apps\TutorStepsView.inc:131; src\apps\TutorStepsView.inc:246;  | Consider both square roots | Considera ambas raíces cuadradas
view.cases | src\apps\TutorStepsView.inc:247;  | Use the zero-product property | Aplica la propiedad del producto nulo
view.check_candidate | src\apps\TutorStepsView.inc:248;  | Check the original domain | Comprueba el dominio original
view.row_swap | src\apps\TutorStepsView.inc:249;  | Swap the equations | Intercambia las ecuaciones
view.row_scale | src\apps\TutorStepsView.inc:250;  | Scale one equation | Multiplica o divide una ecuación
view.row_add | src\apps\TutorStepsView.inc:251;  | Eliminate a variable | Elimina una incógnita
view.saved_values | src\apps\TutorStepsView.inc:238;  | Use the saved values | Usa los valores guardados
view.domain | src\apps\TutorStepsView.inc:230; src\apps\TutorStepsView.inc:237;  | Preserve the domain | Conserva el dominio
view.solve_first | src\apps\EquationsApp.cpp:549; src\apps\TutorStepsView.inc:10;  | Solve the current equations first. | Resuelve primero las ecuaciones actuales.
view.zero_right | src\apps\TutorStepsView.inc:182;  | Equivalent form with zero on the right | Forma equivalente con cero a la derecha
abs.range | src\math\giac\GiacTutor.inc:635;  | An absolute value cannot be negative. The other side must satisfy this condition. | Un valor absoluto no puede ser negativo. El otro miembro debe cumplir esta condición.
radical.range | src\math\giac\GiacTutor.inc:635;  | The principal square root is nonnegative, so the other side must also be nonnegative. | La raíz cuadrada principal es mayor o igual que cero. El otro miembro también debe serlo.
radical.domain | src\math\giac\GiacTutor.inc:486;  | For a real square root, the expression under the root must be nonnegative. | Para que exista una raíz cuadrada real, su radicando debe ser mayor o igual que cero.
abs.cases | src\math\giac\GiacTutor.inc:636;  | An absolute value equals the other side when its inside expression equals that value or its opposite. Solve both cases under the sign condition. | El valor absoluto coincide con el otro miembro cuando la expresión interior es igual a ese valor o a su opuesto. Resuelve ambos casos conservando la condición de signo.
abs.zero | src\math\giac\GiacTutor.inc:636;  | An absolute value is zero exactly when the expression inside it is zero. Only one case is needed. | Un valor absoluto es cero exactamente cuando la expresión interior es cero. Basta con un caso.
radical.square | src\math\giac\GiacTutor.inc:637;  | Square both sides to remove the root. Squaring can introduce extra candidates; check each one in the original equation. | Eleva ambos miembros al cuadrado para eliminar la raíz. Pueden aparecer candidatos que no satisfagan la ecuación original; comprueba cada uno en ella.
abs.impossible | src\math\giac\GiacTutor.inc:638;  | An absolute value is nonnegative, so it cannot equal this negative value. There is no real solution. | Un valor absoluto es mayor o igual que cero y no puede ser igual a este valor negativo. No hay solución real.
radical.impossible | src\math\giac\GiacTutor.inc:638;  | The principal square root is nonnegative, so it cannot equal this negative value. There is no real solution. | La raíz cuadrada principal es mayor o igual que cero y no puede ser igual a este valor negativo. No hay solución real.
candidate.original_valid | src\math\giac\GiacTutor.inc:639;  | This candidate satisfies every sign and domain condition and the original equation. | Este candidato satisface todas las condiciones de signo y dominio, y también la ecuación original.
candidate.original_invalid | src\math\giac\GiacTutor.inc:642;  | Reject this candidate: substituting it into the original equation gives unequal sides. | Descarta este candidato: al sustituirlo en la ecuación original, los miembros no son iguales.
candidate.sign_invalid | src\math\giac\GiacTutor.inc:641;  | Reject this candidate: the isolated other side is negative. | Descarta este candidato: al sustituirlo, el otro miembro resulta negativo, pero debe ser mayor o igual que cero.
candidate.radicand_invalid | src\math\giac\GiacTutor.inc:642;  | Reject this candidate: the original radicand is negative. | Descarta este candidato: el radicando original es negativo.
candidate.duplicate | src\math\giac\GiacTutor.inc:643;  | This value is already an accepted solution from another case. Count it only once. | Este valor ya es una solución aceptada de otro caso. Cuéntalo una sola vez.
abs.add | src\math\giac\GiacTutor.inc:509;  | Add the displayed term to both sides to isolate the absolute-value term. | Suma el término indicado a ambos miembros para aislar el término con valor absoluto.
abs.subtract | src\apps\TutorPresentation.inc:238; src\apps\TutorPresentation.inc:251; src\math\giac\GiacTutor.inc:509;  | Subtract the displayed term from both sides to isolate the absolute-value term. | Resta el término indicado a ambos miembros para aislar el término con valor absoluto.
abs.divide | src\math\giac\GiacTutor.inc:549;  | Divide both sides by the displayed nonzero coefficient to isolate the absolute value. | Divide ambos miembros entre el coeficiente indicado, distinto de cero, para aislar el valor absoluto.
radical.add | src\math\giac\GiacTutor.inc:509;  | Add the displayed term to both sides to isolate the square-root term. | Suma el término indicado a ambos miembros para aislar el término con raíz cuadrada.
radical.subtract | src\apps\TutorPresentation.inc:238; src\apps\TutorPresentation.inc:251; src\math\giac\GiacTutor.inc:509;  | Subtract the displayed term from both sides to isolate the square-root term. | Resta el término indicado a ambos miembros para aislar el término con raíz cuadrada.
radical.divide | src\math\giac\GiacTutor.inc:549;  | Divide both sides by the displayed nonzero coefficient to isolate the square root. | Divide ambos miembros entre el coeficiente indicado, distinto de cero, para aislar la raíz cuadrada.
view.sign | src\apps\TutorStepsView.inc:231;  | Preserve the sign condition | Conserva la condición de signo
view.abs_cases | src\apps\TutorStepsView.inc:232;  | Split into cases | Separa en casos
view.square_both | src\apps\TutorStepsView.inc:233;  | Square both sides | Eleva ambos miembros al cuadrado
view.original_check | src\apps\TutorStepsView.inc:234;  | Check the original equation | Comprueba la ecuación original
view.empty_case | src\apps\TutorStepsView.inc:235;  | This case has no real solution | Este caso no tiene solución real
view.subcase | src\apps\TutorStepsView.inc:84; integer,integer | Case {0}.{1} | Caso {0}.{1}
view.active_subcase | src\apps\TutorStepsView.inc:84; integer,integer | Case {0}.{1}: current step | Caso {0}.{1}: paso actual
view.rejected_subcase | src\apps\TutorStepsView.inc:84; integer,integer | Case {0}.{1}: rejected candidate | Caso {0}.{1}: candidato descartado
view.candidates | src\apps\TutorStepsView.inc:197;  | Candidates | Candidatos
view.read_candidates | src\apps\TutorStepsView.inc:198;  | These values solve this case equation. Check each candidate in the original equation and its conditions. | Estos valores satisfacen la ecuación de este caso. Comprueba cada candidato en la ecuación original y verifica que cumpla sus condiciones.
log.domain | src\math\giac\GiacTutor.inc:475;  | A real logarithm is defined only when its argument is positive. | El logaritmo real solo está definido cuando su argumento es positivo.
base.valid | src\math\giac\GiacTutor.inc:476;  | The base must be positive and different from one. | La base debe ser positiva y distinta de uno.
exponential.range | src\math\giac\GiacTutor.inc:477;  | A real exponential value is always positive. The other side must be positive too. | Una función exponencial real siempre toma valores positivos. El otro miembro también debe ser positivo.
exponential.injective | src\math\giac\GiacTutor.inc:478;  | With the same valid base, equal exponential values have equal exponents. | Con una misma base válida, dos valores exponenciales iguales tienen exponentes iguales.
exponential.inverse | src\math\giac\GiacTutor.inc:479;  | Take the natural logarithm of both sides. It is the inverse of the real exponential function. | Aplica el logaritmo natural a ambos miembros. Es la función inversa de la exponencial real.
exponential.base_inverse | src\math\giac\GiacTutor.inc:479;  | Take natural logarithms of both sides, then divide by the logarithm of the base. | Aplica el logaritmo natural a ambos miembros y divide después entre el logaritmo de la base.
exponential.exact_power | src\math\giac\GiacTutor.inc:480;  | Express the other side as an exact power of the same base. | Expresa el otro miembro como una potencia exacta de la misma base.
log.inverse | src\math\giac\GiacTutor.inc:481;  | Exponentiate both sides. The natural logarithm and exponential are inverse functions on the valid real domain. | Aplica la función exponencial a ambos miembros. El logaritmo natural y la exponencial son funciones inversas en el dominio real válido.
log.base_inverse | src\math\giac\GiacTutor.inc:481;  | Use each side as the exponent of the valid base. This inverts the logarithm on its positive domain. | Usa cada miembro como exponente de la base válida. Esta operación invierte el logaritmo en su dominio positivo.
log.injective | src\math\giac\GiacTutor.inc:482;  | Logarithms with the same valid base are one-to-one on positive arguments, so their arguments must be equal. | Los logaritmos con la misma base válida son inyectivos para argumentos positivos: si sus valores son iguales, sus argumentos también lo son.
terminal.positive_range | src\math\giac\GiacTutor.inc:483;  | A real exponential value is always positive. It cannot equal zero or a negative value, so there is no real solution. | Una función exponencial real siempre toma valores positivos. No puede valer cero ni un número negativo, por lo que no hay solución real.
transcendental.add | src\math\giac\GiacTutor.inc:502; src\math\giac\GiacTutor.inc:504;  | Add the displayed term to both sides to isolate the function. | Suma el término indicado a ambos miembros para aislar la función.
transcendental.subtract | src\apps\TutorPresentation.inc:238; src\apps\TutorPresentation.inc:252; src\math\giac\GiacTutor.inc:502; src\math\giac\GiacTutor.inc:504;  | Subtract the displayed term from both sides to isolate the function. | Resta el término indicado a ambos miembros para aislar la función.
transcendental.divide | src\math\giac\GiacTutor.inc:546; src\math\giac\GiacTutor.inc:547;  | Divide both sides by the displayed nonzero coefficient to isolate the function. | Divide ambos miembros entre el coeficiente indicado, distinto de cero, para aislar la función.
candidate.log_domain | src\math\giac\GiacTutor.inc:642;  | Reject this candidate: a logarithm argument is not positive. | Descarta este candidato: el argumento de un logaritmo no es positivo.
view.injective | src\apps\TutorStepsView.inc:227;  | Equate the arguments or exponents | Iguala los argumentos o exponentes
view.inverse | src\apps\TutorStepsView.inc:228;  | Use the inverse function | Usa la función inversa
view.exact_power | src\apps\TutorStepsView.inc:229;  | Recognize a common base | Busca una base común
view.add_amount | src\apps\TutorStepsView.inc:150; src\math\tutor\TeachingPlan.h:35; expression | Add {0} to both sides. | Suma {0} a ambos miembros.
view.subtract_amount | src\apps\TutorStepsView.inc:150; src\math\tutor\TeachingPlan.h:35; expression | Subtract {0} from both sides. | Resta {0} a ambos miembros.
view.divide_amount | src\math\tutor\TeachingPlan.h:34; expression | Divide both sides by {0}. | Divide ambos miembros entre {0}.
trig.range | src\math\giac\GiacTutorTrig.inc:119;  | Sine and cosine values lie between -1 and 1. The target is inside this range. | El seno y el coseno toman valores entre -1 y 1. El valor buscado está dentro de ese intervalo.
trig.principal | src\math\giac\GiacTutorTrig.inc:120;  | Find the principal inverse angle in the standard real inverse range. | Usa la función trigonométrica inversa para obtener el ángulo principal.
trig.sine_families | src\math\giac\GiacTutorTrig.inc:121;  | Sine has the same value at supplementary angles and repeats every full turn. Include both families, with any integer k. | El seno tiene el mismo valor en ángulos suplementarios y se repite cada vuelta completa. Incluye ambas familias, con cualquier entero k.
trig.cosine_families | src\math\giac\GiacTutorTrig.inc:121;  | Cosine has the same value at opposite angles and repeats every full turn. Include both families, with any integer k. | El coseno tiene el mismo valor en ángulos opuestos y se repite cada vuelta completa. Incluye ambas familias, con cualquier entero k.
trig.tangent_families | src\math\giac\GiacTutorTrig.inc:121;  | Tangent repeats every half turn. Its principal inverse lies strictly between the poles, so this family is defined for every integer k. | La tangente se repite cada media vuelta. Su ángulo principal está estrictamente entre los polos, así que esta familia está definida para cualquier entero k.
family.subtract | src\math\giac\GiacTutorTrig.inc:122;  | Balance every family relation to remove the constant. The period is unchanged. | Opera en ambos miembros de cada relación para eliminar el término constante. El periodo no cambia.
family.divide | src\math\giac\GiacTutorTrig.inc:123;  | Divide the whole relation by the nonzero coefficient, including both the offset and the period. | Divide toda la relación entre el coeficiente distinto de cero, incluidos el término independiente y el periodo.
family.normalize | src\math\giac\GiacTutorTrig.inc:124;  | A negative period can be reversed because k ranges over all integers. Merge families only when their offsets differ by an exact integer number of periods. | Se puede invertir el signo del periodo porque k recorre todos los enteros. Une dos familias solo si sus términos independientes difieren en un número entero exacto de periodos.
family.finish | src\math\giac\GiacTutorTrig.inc:125;  | These periodic families contain all real solutions. Every integer k is allowed; the complete families satisfy the original equation. | Estas familias periódicas contienen todas las soluciones reales. El parámetro k puede tomar cualquier valor entero y todas las familias satisfacen la ecuación original.
trig.impossible | src\math\giac\GiacTutorTrig.inc:126;  | Sine and cosine always lie between -1 and 1 for real angles. This target is outside that range, so there is no real solution. | Para ángulos reales, el seno y el coseno siempre están entre -1 y 1. El valor buscado queda fuera de ese intervalo, por lo que no hay solución real.
view.periodic | src\apps\TutorStepsView.inc:133; src\apps\TutorStepsView.inc:153; src\apps\TutorStepsView.inc:156;  | Periodic solution families | Soluciones periódicas
view.integer | src\apps\TutorStepsView.inc:135; src\apps\TutorStepsView.inc:167;  | k is any integer. | El parámetro k puede tomar cualquier valor entero.
view.family_number | src\apps\TutorStepsView.inc:134; src\apps\TutorStepsView.inc:165; src\apps\TutorStepsView.inc:166; integer | Family {0} | Familia {0}
view.principal | src\apps\TutorStepsView.inc:155;  | Principal angle | Ángulo principal
view.range | src\apps\TutorStepsView.inc:156;  | Check the range | Comprueba los valores posibles
view.representatives | src\apps\TutorStepsView.inc:138; src\apps\TutorStepsView.inc:170;  | The ordinary result lists representatives. These checked families include every period. | El resultado de Giac muestra representantes. Estas familias comprobadas abarcan todos los periodos.
trig.tangent_domain | src\math\giac\GiacTutorTrig.inc:119;  | Tangent accepts every finite real target. Its argument must avoid the poles where cosine is zero. | La tangente puede tomar cualquier valor real finito. Su argumento debe evitar los polos, donde el coseno es cero.
view.degree_convention | src\apps\TutorStepsView.inc:152;  | Angles are in degrees. An inverse marked rad is evaluated in radians before the exact degree conversion. | Los ángulos están en grados. Una inversa marcada con rad se evalúa en radianes antes de la conversión exacta a grados.
view.representatives_title | src\apps\EquationsApp.cpp:624;  | Giac representatives | Representantes de Giac
view.periodic_agreement | src\apps\TutorStepsView.inc:138; src\apps\TutorStepsView.inc:170;  | Giac independently returned the same complete solution set. | Giac ha obtenido de forma independiente el mismo conjunto completo de soluciones.
view.periodic_independent | src\apps\TutorStepsView.inc:138; src\apps\TutorStepsView.inc:170;  | Results contains Giac's independent periodic answer. Exact set comparison is not established. | Resultados contiene la respuesta periódica independiente de Giac. No se ha establecido la igualdad exacta de los conjuntos.
view.periodic_results | src\apps\EquationsApp.cpp:573;  | Periodic solutions | Soluciones periódicas
view.or | src\apps\EquationsApp.cpp:575;  | or | o
view.steps | src\apps\TutorStepsView.inc:391; src\apps\TutorStepsView.inc:7;  | Steps | Pasos
view.result | src\apps\EquationsApp.cpp:534; src\apps\EquationsApp.cpp:578;  | Result | Resultado
view.result_hint | src\apps\EquationsApp.cpp:534; src\apps\EquationsApp.cpp:628;  | Arrows Scroll   EXE Equations   TOOLBOX Steps | ^v Despl. <> Horiz. EXE Ecuac. TOOLBOX Pasos
view.periodic_hint | src\apps\EquationsApp.cpp:574;  | Arrows Scroll/Pan   BACK Equations   TOOLBOX Steps | ^v Despl. <> Horiz. BACK Ecuac. TOOLBOX Pasos
view.result_pages_hint | src\apps\EquationsApp.cpp:628;  | VAR Page   Arrows Scroll/Pan   BACK Equations | VAR Pág. ^v Despl. <> Horiz. BACK TOOLBOX Pasos
view.recovery_hint | src\apps\EquationsApp.cpp:578;  | BACK Equations   HOME | BACK Ecuaciones   HOME Inicio
view.solve_again | src\apps\EquationsApp.cpp:549;  | Solve again | Resuelve de nuevo
view.invalid_equation | src\apps\EquationsApp.cpp:552;  | Invalid equation | Ecuación no válida
view.unresolved | src\apps\EquationsApp.cpp:553;  | Unresolved / unsupported | Sin resolver / no admitido
view.no_memory | src\apps\EquationsApp.cpp:554;  | Not enough memory | Memoria insuficiente
view.solve_failed | src\apps\EquationsApp.cpp:554;  | Solve failed | No se ha podido resolver
view.solve_error | src\apps\EquationsApp.cpp:554;  | The equation could not be solved. Return to the equations to review the input or try again. | No se ha podido resolver la ecuación. Vuelve a las ecuaciones para revisar los datos o intentarlo de nuevo.
view.presentation_unavailable | src\apps\EquationsApp.cpp:579;  | Presentation unavailable | Vista no disponible
view.no_complex | src\apps\EquationsApp.cpp:587;  | No solutions in the complex domain. | No hay soluciones en el dominio complejo.
view.no_real | src\apps\EquationsApp.cpp:587;  | No solutions in the real domain. | No hay soluciones en el dominio real.
view.conditional_family | src\apps\EquationsApp.cpp:595;  | Conditional family | Familia con restricciones
view.dependent_system | src\apps\EquationsApp.cpp:595;  | Dependent system / family | Sistema dependiente / familia
view.tuple2 | src\apps\EquationsApp.cpp:596;  | Tuple order: x, y | Orden de los valores: x, y
view.tuple3 | src\apps\EquationsApp.cpp:596;  | Tuple order: x, y, z | Orden de los valores: x, y, z
view.family_relations | src\apps\EquationsApp.cpp:597;  | Parameters must satisfy these relations. | Los parámetros deben satisfacer estas relaciones.
view.unresolved_exclusions | src\apps\EquationsApp.cpp:598;  | Only where the original equations are defined; exclusions remain unresolved. | Solo donde estén definidas las ecuaciones originales; las exclusiones siguen sin determinarse.
view.conditional_identity | src\apps\EquationsApp.cpp:601;  | Conditional identity | Identidad con restricciones
view.conditional_explanation | src\apps\EquationsApp.cpp:601;  | Equality holds where the original expression is defined. Domain exclusions remain unresolved; this is not an unrestricted solution set. | La igualdad se cumple donde está definida la expresión original. Las exclusiones del dominio siguen sin determinarse; no es un conjunto de soluciones sin restricciones.
view.all_values | src\apps\EquationsApp.cpp:602;  | Identity / all values | Identidad / todos los valores
view.every_complex | src\apps\EquationsApp.cpp:602;  | Every complex x satisfies the equation. | Cualquier valor complejo de x satisface la ecuación.
view.every_real | src\apps\EquationsApp.cpp:602;  | Every real x satisfies the equation. | Cualquier valor real de x satisface la ecuación.
view.unresolved_result | src\apps\EquationsApp.cpp:605;  | Unresolved result | Resultado sin resolver
view.numerical_candidates | src\apps\EquationsApp.cpp:616;  | Numerical candidates | Candidatos numéricos
view.numerical_note | src\apps\EquationsApp.cpp:611;  | Numerical candidates from Giac. Completeness is not established. | Candidatos numéricos de Giac. No se ha demostrado que incluyan todas las soluciones.
view.candidate_number | src\apps\EquationsApp.cpp:613; integer | Candidate {0} | Candidato {0}
view.exact_text | src\apps\EquationsApp.cpp:641;  | Exact results (text) | Resultados exactos (texto)
view.display_limit | src\apps\EquationsApp.cpp:543;  | Display limit: remaining technical text is not shown. | Límite de visualización: no se muestra el resto del texto técnico.
view.page_real | src\apps\EquationsApp.cpp:625; integer,integer | Solution {0}/{1} \| Real | Solución {0}/{1} \| Real
view.page_complex | src\apps\EquationsApp.cpp:625; integer,integer | Solution {0}/{1} \| Complex | Solución {0}/{1} \| Complejo
view.count_real_one | src\apps\EquationsApp.cpp:626;  | 1 solution \| x \| Real | 1 solución \| x \| Real
view.count_real_many | src\apps\EquationsApp.cpp:627; integer | {0} solutions \| x \| Real | {0} soluciones \| x \| Real
view.count_complex_one | src\apps\EquationsApp.cpp:626;  | 1 solution \| x \| Complex | 1 solución \| x \| Complejo
view.count_complex_many | src\apps\EquationsApp.cpp:627; integer | {0} solutions \| x \| Complex | {0} soluciones \| x \| Complejo
view.language | src\apps\SettingsApp.cpp:416;  | Tutor language | Idioma del tutor
view.english | src\apps\SettingsApp.cpp:417;  | English | English
view.spanish | src\apps\SettingsApp.cpp:417;  | Español | Español
view.solving | src\apps\EquationsApp.cpp:491;  | Solving with Giac... | Resolviendo con Giac...
view.wait | src\apps\EquationsApp.cpp:492;  | Please wait | Espera un momento
view.settings_hint | src\apps\SettingsApp.cpp:415;  | Navigate   Left/Right Adjust   MODE Back | ^v Elige  </> Cambia  MODE Vuelve
substitution.define | src\math\giac\GiacTutorComposition.inc:136; src\math\giac\GiacTutorComposition.inc:258;  | Use an auxiliary variable for the repeated expression. Its definition preserves the original domain. | Sustituye la expresión que se repite por una variable auxiliar. Su definición conserva el dominio original.
substitution.rewrite | src\math\giac\GiacTutorComposition.inc:194; src\math\giac\GiacTutorComposition.inc:259;  | Rewrite the equation in the auxiliary variable using the checked power identities. | Reescribe la ecuación en la variable auxiliar mediante las identidades de potencias comprobadas.
substitution.auxiliary | src\math\giac\GiacTutorComposition.inc:197; src\math\giac\GiacTutorComposition.inc:262;  | Solve the auxiliary equation completely. These are values of the auxiliary variable, not yet solutions for the original unknown. | Resuelve por completo la ecuación auxiliar. Estos son valores de la variable auxiliar, no las soluciones de la incógnita original.
substitution.reject_square | src\math\giac\GiacTutorComposition.inc:77;  | Discard this auxiliary value: a real square cannot be negative. | Descarta este valor auxiliar: un cuadrado real no puede ser negativo.
substitution.reject_exp | src\math\giac\GiacTutorComposition.inc:78;  | Discard this auxiliary value: a real exponential is strictly positive, so zero and negative values are impossible. | Descarta este valor auxiliar: una exponencial real es positiva y no puede valer cero ni tomar valores negativos.
substitution.reject_trig | src\math\giac\GiacTutorComposition.inc:78;  | Discard this auxiliary value: sine and cosine take values only between -1 and 1, including both endpoints. | Descarta este valor auxiliar: el seno y el coseno solo toman valores entre −1 y 1, incluidos ambos extremos.
substitution.pullback | src\math\giac\GiacTutorComposition.inc:210; src\math\giac\GiacTutorComposition.inc:268;  | Return to the original unknown. Solve the displayed equation for this auxiliary value, keeping every preimage. | Vuelve a la incógnita original. Resuelve la ecuación indicada para este valor auxiliar sin perder ninguna preimagen.
substitution.finish | src\math\giac\GiacTutorComposition.inc:223; src\math\giac\GiacTutorComposition.inc:271;  | Every auxiliary value has been accounted for. Combine the checked preimages, counting each solution only once and keeping the original restrictions. | Se han considerado todos los valores auxiliares. Reúne sus preimágenes comprobadas, cuenta cada solución una sola vez y conserva las restricciones originales.
substitution.square | src\math\giac\GiacTutorComposition.inc:159; src\math\giac\GiacTutorComposition.inc:238;  | Consider both signs of the square root. When the square is zero, both signs give the same case. | Considera ambos signos de la raíz cuadrada. Si el cuadrado es cero, ambos signos dan el mismo caso.
view.auxiliary | src\apps\TutorStepsView.inc:123; src\apps\TutorStepsView.inc:125; src\apps\TutorStepsView.inc:128; src\apps\TutorStepsView.inc:322;  | Auxiliary values | Valores de la variable auxiliar
view.pullback | src\apps\TutorStepsView.inc:128;  | Return to the original unknown | Vuelve a la incógnita original
view.substitution_range | src\apps\TutorStepsView.inc:118; src\apps\TutorStepsView.inc:126;  | Possible auxiliary values | Valores posibles de la auxiliar
view.define_substitution | src\apps\TutorStepsView.inc:114;  | Define an auxiliary variable | Define una variable auxiliar
view.auxiliary_equation | src\apps\TutorStepsView.inc:121;  | Auxiliary equation | Ecuación auxiliar
view.auxiliary_number | src\apps\TutorStepsView.inc:326; integer | Auxiliary value {0} | Valor auxiliar {0}
view.return_equation | src\apps\TutorStepsView.inc:116; src\apps\TutorStepsView.inc:141; src\apps\TutorStepsView.inc:158; src\apps\TutorStepsView.inc:162;  | Equation for this auxiliary value | Ecuación para este valor auxiliar
substitution.finish_empty | src\math\giac\GiacTutorComposition.inc:223; src\math\giac\GiacTutorComposition.inc:271;  | Every auxiliary value has been considered. None has a valid real preimage, so the original equation has no real solution. | Se han considerado todos los valores auxiliares. Ninguno tiene una preimagen real válida, por lo que la ecuación original no tiene solución real.
view.periodic_groups_hint | src\apps\EquationsApp.cpp:574;  | Arrows Scroll/Pan  VAR Page  BACK  TOOLBOX Steps | ^v Despl. <> Horiz. VAR Página BACK TOOLBOX Pasos
view.periodic_page | src\apps\EquationsApp.cpp:571; src\apps\TutorStepsView.inc:330; integer,integer | Periodic solutions {0}/{1} | Soluciones periódicas {0}/{1}
