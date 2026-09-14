// SPDX-License-Identifier: GPL-3.0-or-later
// Compiled inside the existing math boundary TU for identical native/WASM linkage.
namespace tutor {
namespace {
struct CatalogEntry {
    const char *key;
    const char *english;
    const char *schema;
    const char *spanish;
    const char *french;
};
constexpr CatalogEntry catalog[] = {
    {"domain.nonzero", "A denominator cannot be zero. Keep this restriction throughout the solution.", "",
     "Un denominador no puede ser cero. Conserva esta restriccion durante la solucion.",
     "Un denominateur ne peut pas etre nul. Conserver cette condition pendant la resolution."},
    {"values.substitute", "Replace stored variables with their saved values for this solve.", "",
     nullptr, nullptr},
    {"algebra.expand", "Expand the products and powers so we can collect like terms.", "", nullptr,
     nullptr},
    {"algebra.collect", "Collect like terms by combining the coefficients of each power.",
     "", nullptr, nullptr},
    {"equation.swap", "Swap the two sides to put {0} on the left.", "v",
     "Intercambia los lados para poner {0} a la izquierda.",
     "Echanger les membres pour placer {0} a gauche."},
    {"equation.add", "Add {0} to both sides to isolate the term containing {1}.", "ev",
     "Suma {0} a ambos lados para aislar el termino que contiene {1}.",
     "Ajouter {0} aux deux membres pour isoler le terme contenant {1}."},
    {"equation.subtract", "Subtract {0} from both sides to isolate the term containing {1}.", "ev",
     "Resta {0} de ambos lados para aislar el termino que contiene {1}.",
     "Soustraire {0} des deux membres pour isoler le terme contenant {1}."},
    {"equation.divide", "Divide both sides by {0} to isolate {1}.", "ev",
     "Divide ambos lados entre {0} para aislar {1}.",
     "Diviser les deux membres par {0} pour isoler {1}."},
    {"rational.clear",
     "Multiply both sides by the common denominator to remove the fractions. Keep the original restrictions.", "",
     nullptr, nullptr},
    {"polynomial.factor", "Factor the left side so we can use the zero-product rule.", "",
     nullptr, nullptr},
    {"square.branches",
     "Take the square root with both signs to include every solution.",
     "", nullptr, nullptr},
    {"quadratic.formula",
     "Use the quadratic formula to find every value of {0} that satisfies the equation.",
     "v", nullptr, nullptr},
    {"product.zero",
     "A product is zero when at least one factor is zero. Set each factor to zero and solve each case.",
     "", nullptr, nullptr},
    {"candidate.excluded", "Reject this candidate: it makes an original denominator zero.", "",
     nullptr, nullptr},
    {"terminal.isolated", "{0} is already isolated. This equation gives its solution directly.",
     "v", "{0} ya esta aislada. La ecuacion da directamente su solucion.",
     "{0} est deja isolee. Cette equation donne directement sa solution."},
    {"terminal.identity", "Both sides are equal for every value in the selected domain.", "",
     nullptr, nullptr},
    {"terminal.conditional",
     "Both sides are equal for every value satisfying the displayed denominator restrictions.", "",
     nullptr, nullptr},
    {"terminal.contradiction", "The equality is false: no value can satisfy it.", "", nullptr,
     nullptr},
    {"terminal.no_real",
     "The discriminant is negative. No real square root exists, so there are no real "
     "solutions.",
     "", nullptr, nullptr},
    {"terminal.complete",
     "These are all solutions under the displayed restrictions. Each also satisfies the original "
     "equation.",
     "", nullptr, nullptr},
    {"system.swap", "Swap equations {0} and {1} to use a nonzero coefficient for elimination.", "rr", nullptr, nullptr},
    {"system.scale", "Divide equation {0} by {1} to make the coefficient of {2} equal to 1.", "rev",
     nullptr, nullptr},
    {"system.add", "Add {0} times equation {1} to equation {2} to eliminate {3}.", "errv", nullptr,
     nullptr},
    {"system.subtract", "Subtract {0} times equation {1} from equation {2} to eliminate {3}.",
     "errv", nullptr, nullptr},
    {"system.unique",
     "Each variable is isolated. These values form one simultaneous solution tuple.", "", nullptr,
     nullptr},
    {"system.family",
     "A zero row adds no constraint. Free variables may vary, but must satisfy the remaining "
     "equations together.",
     "", nullptr, nullptr},
    {"system.inconsistent",
     "A row requires zero to equal a nonzero value. The equations have no simultaneous solution.",
     "", nullptr, nullptr},
    {"status.unsupported",
     "A checked method is not available for this equation family. The ordinary Giac result remains "
     "available.",
     "", nullptr, nullptr},
    {"status.partial",
     "The explanation reached its resource limit. The verified steps shown are partial; "
     "completeness is not established.",
     "", nullptr, nullptr},
    {"status.check_failed",
     "The tutor could not verify this derivation. Its instructions are withheld; the ordinary "
     "result remains available.",
     "", nullptr, nullptr},
    {"status.reconciliation",
     "The checked derivation and the Giac answer adapter disagree. The ordinary result has not "
     "been changed.",
     "", nullptr, nullptr},
    {"product.repeated",
     "A square is zero only when its base is zero. Solve this one case; the repeated root is counted once.",
     "", nullptr, nullptr},
    {"terminal.excluded_all",
     "Every candidate violates an original denominator restriction, so there are no solutions.", "",
     nullptr, nullptr},
    {"system.add_unit", "Add equation {0} to equation {1} to eliminate {2}.",
     "rrv", nullptr, nullptr},
    {"system.subtract_unit",
     "Subtract equation {0} from equation {1} to eliminate {2}.", "rrv", nullptr,
     nullptr},
    {"view.alternative", "Case {0}", "i", "Caso {0}", "Cas {0}"},
    {"view.active_alternative", "Case {0}: current step", "i", nullptr, nullptr},
    {"view.rejected_alternative", "Case {0}: excluded", "i", nullptr, nullptr},
    {"view.equation", "Equation {0}", "i", "Ecuacion {0}", "Equation {0}"},
    {"view.formula_unavailable", "This formula cannot be displayed in the current view.", "", nullptr, nullptr},
    {"equation.add_collect", "Add the displayed term to both sides to collect the {0} terms on the left.", "v", nullptr, nullptr},
    {"equation.subtract_collect", "Subtract the displayed term from both sides to collect the {0} terms on the left.", "v", nullptr, nullptr},
    {"equation.add_zero_right", "Add {0} to both sides to put zero on the right.", "e", nullptr, nullptr},
    {"equation.subtract_zero_right", "Subtract {0} from both sides to put zero on the right.", "e", nullptr, nullptr},
    {"equation.add_displayed_isolate", "Add the displayed amount to both sides to isolate the term containing {0}.", "v", nullptr, nullptr},
    {"equation.subtract_displayed_isolate", "Subtract the displayed amount from both sides to isolate the term containing {0}.", "v", nullptr, nullptr},
    {"equation.add_displayed_zero_right", "Add the displayed expression to both sides to put zero on the right.", "", nullptr, nullptr},
    {"equation.subtract_displayed_zero_right", "Subtract the displayed expression from both sides to put zero on the right.", "", nullptr, nullptr},
    {"equation.add_system", "Add {0} to both sides to collect the variable terms on the left.", "e", nullptr, nullptr},
    {"equation.subtract_system", "Subtract {0} from both sides to collect the variable terms on the left.", "e", nullptr, nullptr},
    {"equation.add_displayed_system", "Add the displayed expression to both sides to collect the variable terms on the left.", "", nullptr, nullptr},
    {"equation.subtract_displayed_system", "Subtract the displayed expression from both sides to collect the variable terms on the left.", "", nullptr, nullptr},
    {"equation.divide_square", "Divide both sides by {0} to isolate the square of {1}.", "ev", nullptr, nullptr},
    {"equation.divide_displayed", "Divide both sides by the displayed coefficient to isolate {0}.", "v", nullptr, nullptr},
    {"equation.divide_displayed_square", "Divide both sides by the displayed coefficient to isolate the square of {0}.", "v", nullptr, nullptr},
    {"square.zero", "Zero has just one square root: zero.", "", nullptr, nullptr},
    {"system.scale_displayed", "Divide equation {0} by the displayed coefficient to make the coefficient of {1} equal to 1.", "rv", nullptr, nullptr},
    {"system.add_scaled", "Add the displayed multiple of equation {0} to equation {1} to eliminate {2}.", "rrv", nullptr, nullptr},
    {"system.subtract_scaled", "Subtract the displayed multiple of equation {0} from equation {1} to eliminate {2}.", "rrv", nullptr, nullptr},
    {"view.step", "Step {0} of {1}", "ii", "Paso {0} de {1}", "Etape {0} sur {1}"},
    {"view.summary", "Summary {0} of {1}", "ii", "Resumen {0} de {1}", "Resume {0} sur {1}"},
    {"view.start", "Starting equations", "", "Ecuaciones iniciales", "Equations initiales"},
    {"view.solutions", "Solutions", "", "Soluciones", "Solutions"},
    {"view.no_solution", "No solution", "", "Sin solucion", "Aucune solution"},
    {"view.family", "Solution family", "", "Familia de soluciones", "Famille de solutions"},
    {"view.already", "Already solved", "", "Ya esta resuelta", "Deja resolue"},
    {"view.identity", "An identity", "", "Una identidad", "Une identite"},
    {"view.guided_hint", "<> Step  ^v Scroll  EXE Summary  VAR Pan  BACK", "", nullptr, nullptr},
    {"view.summary_hint", "<> Step  ^v Scroll  EXE Guided  VAR Pan  BACK", "", nullptr, nullptr},
    {"view.before", "Before", "", "Antes", "Avant"},
    {"view.after", "After", "", "Despues", "Apres"},
    {"view.restrictions", "Restrictions ({0})", "i", "Restricciones ({0})", "Restrictions ({0})"},
    {"view.operand", "Amount used on both sides", "", nullptr, nullptr},
    {"view.unchanged_case", "Case {0}: unchanged", "i", "Caso {0}: sin cambios", "Cas {0} : inchange"},
    {"view.solution_number", "Solution {0}", "i", "Solucion {0}", "Solution {0}"},
    {"view.system_together", "One simultaneous solution", "", nullptr, nullptr},
    {"view.verified", "Verified", "", "Verificado", "Verifie"},
    {"view.partial_short", "Partial", "", "Parcial", "Partiel"},
    {"view.coefficients", "Identify the coefficients", "", nullptr, nullptr},
    {"view.discriminant", "Compute the discriminant", "", nullptr, nullptr},
    {"view.quadratic_formula", "Use the quadratic formula", "", nullptr, nullptr},
    {"view.substitution", "Substitute the checked values", "", nullptr, nullptr},
    {"view.match_coefficients", "Write the equation with zero on the right, then match its coefficients with the standard form.", "",
     "Escribe la ecuacion con cero a la derecha y compara sus coeficientes con la forma estandar.",
     "Ecrire l'equation avec zero a droite, puis identifier les coefficients dans la forme standard."},
    {"view.compute_discriminant", "The discriminant determines which square root the formula uses.", "", nullptr, nullptr},
    {"view.use_formula", "Insert the coefficients and discriminant. The two signs give the possible solutions.", "", nullptr, nullptr},
    {"view.read_roots", "Evaluate the minus and plus choices to obtain the solutions shown.", "", nullptr, nullptr},
    {"view.standard_form", "Standard form", "", nullptr, nullptr},
    {"view.coefficient_values", "Checked coefficients", "", nullptr, nullptr},
    {"view.definition", "Definition", "", nullptr, nullptr},
    {"view.evaluated", "With these values", "", nullptr, nullptr},
    {"view.starting_system", "Find values that satisfy all these equations together.", "", nullptr, nullptr},
    {"view.start_equation", "Starting equation", "", nullptr, nullptr},
    {"view.original", "Original equation", "", nullptr, nullptr},
    {"view.current", "Current equation", "", nullptr, nullptr},
    {"view.unavailable", "This teaching page could not be prepared. The ordinary result is available with BACK.", "", nullptr, nullptr},
    {"view.expand", "Expand the expression", "", nullptr, nullptr},
    {"view.collect", "Collect like terms", "", nullptr, nullptr},
    {"view.balance", "Balance both sides", "", nullptr, nullptr},
    {"view.divide", "Divide both sides", "", "Divide ambos lados", "Diviser les deux membres"},
    {"view.clear", "Clear the denominators", "", nullptr, nullptr},
    {"view.factor", "Factor the polynomial", "", nullptr, nullptr},
    {"view.square_roots", "Consider both square roots", "", nullptr, nullptr},
    {"view.cases", "Use the zero-product property", "", nullptr, nullptr},
    {"view.check_candidate", "Check the original domain", "", nullptr, nullptr},
    {"view.row_swap", "Swap the equations", "", nullptr, nullptr},
    {"view.row_scale", "Scale one equation", "", nullptr, nullptr},
    {"view.row_add", "Eliminate a variable", "", nullptr, nullptr},
    {"view.saved_values", "Use the saved values", "", nullptr, nullptr},
    {"view.domain", "Preserve the domain", "", nullptr, nullptr},
    {"view.solve_first", "Solve the current equations first.", "", nullptr, nullptr},
    {"view.zero_right", "Equivalent form with zero on the right", "",
     "Forma equivalente con cero a la derecha", "Forme equivalente avec zero a droite"},
    {"abs.range", "An absolute value cannot be negative. The other side must satisfy this condition.", "", "Un valor absoluto no puede ser negativo. El otro lado debe cumplir esta condicion.", "Une valeur absolue ne peut pas etre negative. L autre membre doit respecter cette condition."},
    {"radical.range", "The principal square root is nonnegative, so the other side must also be nonnegative.", "", nullptr, nullptr},
    {"radical.domain", "For a real square root, the expression under the root must be nonnegative.", "", nullptr, nullptr},
    {"abs.cases", "An absolute value equals the other side when its inside expression equals that value or its opposite. Solve both cases under the sign condition.", "", nullptr, nullptr},
    {"abs.zero", "An absolute value is zero exactly when the expression inside it is zero. Only one case is needed.", "", nullptr, nullptr},
    {"radical.square", "Square both sides to remove the root. Squaring can introduce extra candidates; check each one in the original equation.", "", "Eleva ambos lados al cuadrado para eliminar la raiz. Comprueba cada candidato en la ecuacion original.", "Elever les deux membres au carre pour eliminer la racine. Verifier chaque candidat dans l equation initiale."},
    {"abs.impossible", "An absolute value is nonnegative, so it cannot equal this negative value. There is no real solution.", "", nullptr, nullptr},
    {"radical.impossible", "The principal square root is nonnegative, so it cannot equal this negative value. There is no real solution.", "", nullptr, nullptr},
    {"candidate.original_valid", "This candidate satisfies every sign and domain condition and the original equation.", "", nullptr, nullptr},
    {"candidate.original_invalid", "Reject this candidate: substituting it into the original equation gives unequal sides.", "", nullptr, nullptr},
    {"candidate.sign_invalid", "Reject this candidate: the isolated other side is negative.", "", nullptr, nullptr},
    {"candidate.radicand_invalid", "Reject this candidate: the original radicand is negative.", "", nullptr, nullptr},
    {"candidate.duplicate", "This value is already an accepted solution from another case. Count it only once.", "", nullptr, nullptr},
    {"abs.add", "Add the displayed term to both sides to isolate the absolute-value term.", "", nullptr, nullptr},
    {"abs.subtract", "Subtract the displayed term from both sides to isolate the absolute-value term.", "", nullptr, nullptr},
    {"abs.divide", "Divide both sides by the displayed nonzero coefficient to isolate the absolute value.", "", nullptr, nullptr},
    {"radical.add", "Add the displayed term to both sides to isolate the square-root term.", "", nullptr, nullptr},
    {"radical.subtract", "Subtract the displayed term from both sides to isolate the square-root term.", "", nullptr, nullptr},
    {"radical.divide", "Divide both sides by the displayed nonzero coefficient to isolate the square root.", "", nullptr, nullptr},
    {"view.sign", "Preserve the sign condition", "", nullptr, nullptr},
    {"view.abs_cases", "Split into cases", "", nullptr, nullptr},
    {"view.square_both", "Square both sides", "", nullptr, nullptr},
    {"view.original_check", "Check the original equation", "", nullptr, nullptr},
    {"view.empty_case", "This case has no real solution", "", nullptr, nullptr},
    {"view.subcase", "Case {0}.{1}", "ii", "Caso {0}.{1}", "Cas {0}.{1}"},
    {"view.active_subcase", "Case {0}.{1}: current step", "ii", nullptr, nullptr},
    {"view.rejected_subcase", "Case {0}.{1}: rejected candidate", "ii", nullptr, nullptr},
    {"view.candidates", "Candidates", "", "Candidatos", "Candidats"},
    {"view.read_candidates", "These values solve this case equation. Check each candidate in the original equation and its conditions.", "", nullptr, nullptr},
};
static_assert(sizeof(catalog) / sizeof(catalog[0]) == unsigned(Message::Count), "catalog coverage");
char schemaType(ParameterKind p) {
    return p == ParameterKind::Expression ? 'e'
           : p == ParameterKind::Variable ? 'v'
           : p == ParameterKind::Row      ? 'r'
                                          : 'i';
}
void hashBytes(uint64_t &hash, const std::string &text) {
    for (unsigned char c : text) {
        hash ^= c;
        hash *= 1099511628211ull;
    }
    hash ^= 255;
    hash *= 1099511628211ull;
}
std::string quote(const std::string &s) {
    std::string out = "\"";
    for (unsigned char c : s) {
        if (c == '"' || c == '\\')
            out += '\\';
        if (c == '\n')
            out += "\\n";
        else if (c >= 32)
            out += char(c);
    }
    return out + '"';
}
} // namespace
const char *ruleId(Rule r) {
    static constexpr const char *ids[] = {
        "domain.nonzero",     "values.substitute", "algebra.expand",    "algebra.collect",
        "equation.swap",      "equation.add",      "equation.divide",   "rational.clear",
        "polynomial.factor",  "square.branches",   "quadratic.formula", "product.zero",
        "candidate.excluded", "terminal.isolated", "terminal.identity", "terminal.contradiction",
        "terminal.finish",    "system.swap",       "system.scale",      "system.add",
        "system.finish", "condition.range", "abs.cases", "radical.candidates", "terminal.nonnegative",
        "candidate.original_valid", "candidate.original_invalid", "candidate.duplicate", "domain.radicand_nonnegative"};
    return unsigned(r) < sizeof(ids) / sizeof(ids[0]) ? ids[unsigned(r)] : "unknown";
}
const char *messageKey(Message k) {
    return unsigned(k) < unsigned(Message::Count) ? catalog[unsigned(k)].key : "unknown";
}
const char *messageFallback(Message k) {
    return unsigned(k)<unsigned(Message::Count)&&!catalog[unsigned(k)].schema[0]
        ? catalog[unsigned(k)].english : "";
}
bool validateMessage(Message k, const Vector<Parameter> &p) {
    if (unsigned(k) >= unsigned(Message::Count))
        return false;
    const auto *schema = catalog[unsigned(k)].schema;
    if (std::strlen(schema) != p.size())
        return false;
    for (size_t i = 0; i < p.size(); ++i)
        if (schema[i] != schemaType(p[i].kind) || p[i].value.empty() ||
            p[i].value.size() > Limits::expressionBytes)
            return false;
    return true;
}
bool validateCatalogs() {
    for (const auto &entry : catalog)
        for (const char *value : {entry.english, entry.spanish, entry.french}) {
            if (!value)
                continue;
            std::array<unsigned, 8> count{};
            for (size_t i = 0; value[i]; ++i)
                if (value[i] == '{') {
                    if (value[i + 1] < '0' || value[i + 1] > '7' || value[i + 2] != '}')
                        return false;
                    ++count[value[i + 1] - '0'];
                    i += 2;
                }
            for (size_t i = 0; i < count.size(); ++i)
                if (count[i] != (i < std::strlen(entry.schema) ? 1u : 0u))
                    return false;
        }
    return true;
}
std::string explain(Message k, const Vector<Parameter> &p, Locale locale) {
    if (!validateMessage(k, p))
        return "[invalid message parameters]";
    const auto &entry = catalog[unsigned(k)];
    const char *value = entry.english;
    if (locale == Locale::Spanish && entry.spanish)
        value = entry.spanish;
    if (locale == Locale::French && entry.french)
        value = entry.french;
    // Explicit per-message English fallback. Parameter values are never localized.
    std::string out;
    for (size_t i = 0; value[i]; ++i) {
        if (value[i] == '{') {
            out += p[value[i + 1] - '0'].value;
            i += 2;
        } else {
            out += value[i];
            if (locale == Locale::Pseudo && std::strchr("aeiou", value[i]))
                out += value[i];
        }
    }
    return locale == Locale::Pseudo ? "[!! " + out + " !!]" : out;
}
namespace {
bool equalTree(const EngineResultNode& a, const EngineResultNode& b, unsigned depth=0) {
    if(depth>Limits::depth || a.kind!=b.kind || a.text!=b.text || a.children.size()!=b.children.size() ||
       a.rows!=b.rows || a.columns!=b.columns || a.leftClosed!=b.leftClosed || a.rightClosed!=b.rightClosed || a.fallbackReason!=b.fallbackReason) return false;
    for(size_t i=0;i<a.children.size();++i) if(!equalTree(a.children[i],b.children[i],depth+1))return false;
    return true;
}
void hashTree(uint64_t& h,const EngineResultNode& n,unsigned depth=0) {
    hashBytes(h,std::to_string(unsigned(n.kind))); hashBytes(h,n.text);
    if(depth>Limits::depth)return;
    for(const auto& c:n.children)hashTree(h,c,depth+1);
}
size_t treeBytes(const EngineResultNode& n,unsigned depth=0) {
    if(depth>Limits::depth)return Limits::retainedBytes;
    size_t bytes=n.text.capacity()+1+n.children.capacity()*sizeof(EngineResultNode);
    for(const auto& c:n.children)bytes+=treeBytes(c,depth+1);
    return bytes;
}
}
bool sameCondition(const Condition& a,const Condition& b) {
    return a.nonzero==b.nonzero && a.source.equation==b.source.equation && a.source.side==b.source.side && a.source.children==b.source.children &&
        a.kind==b.kind && a.role==b.role && a.sourceState==b.sourceState && a.verification==b.verification &&
        bool(a.expression)==bool(b.expression) && (!a.expression || equalTree(*a.expression,*b.expression));
}
uint64_t fingerprint(const State &s) {
    uint64_t h = 1469598103934665603ull;
    hashBytes(h, std::to_string(unsigned(s.conclusion)));
    for (const auto &b : s.branches) {
        hashBytes(h, std::to_string(b.id));
        hashBytes(h, std::to_string(unsigned(b.status)));
        if (b.origin || b.originalCheck != Verdict::Unknown) {
            hashBytes(h, std::to_string(b.origin)); hashBytes(h, std::to_string(unsigned(b.originalCheck)));
        }
        for (const auto &e : b.equations) {
            hashBytes(h, e.lhs);
            hashBytes(h, e.rhs);
        }
    }
    for (const auto &c : s.conditions) {
        hashBytes(h, c.nonzero);
        if (c.kind != ConditionKind::Nonzero || c.role != ConditionRole::Denominator || c.sourceState || c.verification != Verdict::Unknown || c.expression) {
            hashBytes(h, std::to_string(unsigned(c.kind))); hashBytes(h, std::to_string(unsigned(c.role)));
            hashBytes(h, std::to_string(c.sourceState)); hashBytes(h, std::to_string(unsigned(c.verification)));
            if (c.expression) hashTree(h, *c.expression);
        }
        hashBytes(h, std::to_string(c.source.equation));
        hashBytes(h, std::to_string(c.source.side));
        for (auto i : c.source.children)
            hashBytes(h, std::to_string(i));
    }
    return h;
}
uint64_t fingerprint(const Snapshot &s) {
    uint64_t h = 1469598103934665603ull;
    for (const auto &e : s.authored) {
        hashBytes(h, e.lhs);
        hashBytes(h, e.rhs);
    }
    for (const auto &v : s.variables)
        hashBytes(h, v);
    for (const auto &b : s.bindings) {
        hashBytes(h, b.variable);
        hashBytes(h, b.value);
    }
    for (const auto &b : s.contextValues) {
        hashBytes(h, b.variable);
        hashBytes(h, b.value);
    }
    hashBytes(h, std::to_string(s.inputEpoch));
    hashBytes(h, std::to_string(s.engineGeneration));
    hashBytes(h, s.complex ? "complex" : "real");
    hashBytes(h, s.degrees ? "deg" : "rad");
    return h;
}
size_t retainedBytes(const Derivation &d) {
    size_t n = sizeof(d) + d.diagnostic.capacity() + d.states.capacity() * sizeof(State) +
               d.steps.capacity() * sizeof(Step);
    auto eqs = [&](const Vector<Equation> &e) {
        n += e.capacity() * sizeof(Equation);
        for (const auto &x : e)
            n += x.lhs.capacity() + x.rhs.capacity() + 2;
    };
    eqs(d.input.authored);
    n += d.input.variables.capacity() * sizeof(std::string) +
         (d.input.bindings.capacity() + d.input.contextValues.capacity()) * sizeof(Binding);
    for (const auto &v : d.input.variables)
        n += v.capacity() + 1;
    for (const auto &b : d.input.bindings)
        n += b.variable.capacity() + b.value.capacity() + 2;
    for (const auto &b : d.input.contextValues)
        n += b.variable.capacity() + b.value.capacity() + 2;
    for (const auto &s : d.states) {
        n += s.branches.capacity() * sizeof(Branch) + s.conditions.capacity() * sizeof(Condition);
        for (const auto &b : s.branches)
            eqs(b.equations);
        for (const auto &c : s.conditions) {
            n += c.nonzero.capacity() + 1 + c.source.children.capacity();
            // Conservative accounting includes shared tree payload per state.
            if(c.expression)n+=sizeof(EngineResultNode)+32+treeBytes(*c.expression);
        }
    }
    for (const auto &s : d.steps) {
        n += s.operand.capacity() + 1 + s.auxiliaries.capacity() * sizeof(std::string) +
             s.affected.capacity() * sizeof(Path) + s.parameters.capacity() * sizeof(Parameter) +
             s.prerequisites.capacity() + s.introduced.capacity() + s.discharged.capacity() +
             s.substeps.capacity() * sizeof(uint16_t);
        for (const auto &a : s.auxiliaries)
            n += a.capacity() + 1;
        for (const auto &p : s.parameters)
            n += p.value.capacity() + 1;
        for (const auto &p : s.affected)
            n += p.children.capacity();
    }
    return n; // Conservative retained C++ capacity accounting; allocator overhead is measured
              // separately.
}
std::string replayJson(const Derivation &d, Locale locale) {
    std::ostringstream o;
    auto strings = [&](const auto &values) {
        o << '[';
        bool first = true;
        for (const auto &v : values) {
            if (!first)
                o << ',';
            first = false;
            o << quote(v);
        }
        o << ']';
    };
    auto numbers = [&](const auto &values) {
        o << '[';
        bool first = true;
        for (auto v : values) {
            if (!first)
                o << ',';
            first = false;
            o << unsigned(v);
        }
        o << ']';
    };
    auto equations = [&](const auto &values) {
        o << '[';
        bool first = true;
        for (const auto &e : values) {
            if (!first)
                o << ',';
            first = false;
            o << '[' << quote(e.lhs) << ',' << quote(e.rhs) << ']';
        }
        o << ']';
    };
    auto bindings = [&](const auto &values) {
        o << '[';
        bool first = true;
        for (const auto &b : values) {
            if (!first)
                o << ',';
            first = false;
            o << '[' << quote(b.variable) << ',' << quote(b.value) << ']';
        }
        o << ']';
    };
    auto path = [&](const Path &p) {
        o << "{\"equation\":" << unsigned(p.equation) << ",\"side\":" << unsigned(p.side)
          << ",\"children\":";
        numbers(p.children);
        o << '}';
    };
    o << "{\"version\":1,\"snapshot\":" << fingerprint(d.input)
      << ",\"epoch\":" << d.input.inputEpoch << ",\"generation\":" << d.input.engineGeneration
      << ",\"complex\":" << d.input.complex << ",\"degrees\":" << d.input.degrees
      << ",\"authored\":";
    equations(d.input.authored);
    o << ",\"variables\":";
    strings(d.input.variables);
    o << ",\"bindings\":";
    bindings(d.input.bindings);
    o << ",\"contextValues\":";
    bindings(d.input.contextValues);
    o << ",\"status\":" << unsigned(d.status) << ",\"validity\":" << unsigned(d.validity)
      << ",\"completeness\":" << unsigned(d.completeness)
      << ",\"candidates\":" << unsigned(d.candidates)
      << ",\"reconciliation\":" << unsigned(d.reconciliation)
      << ",\"bytes\":" << d.metrics.retainedBytes
      << ",\"vectorHeapBytes\":" << d.metrics.vectorHeapBytes
      << ",\"peakVectorHeapBytes\":" << d.metrics.peakVectorHeapBytes
      << ",\"calls\":" << d.metrics.symbolicCalls << ",\"micros\":" << d.metrics.elapsedMicros
      << ",\"diagnostic\":" << quote(d.diagnostic) << ",\"states\":[";
    for (size_t i = 0; i < d.states.size(); ++i) {
        if (i)
            o << ',';
        const auto &s = d.states[i];
        o << "{\"id\":" << i << ",\"fingerprint\":" << s.fingerprint
          << ",\"conclusion\":" << unsigned(s.conclusion) << ",\"conditions\":[";
        for (size_t j = 0; j < s.conditions.size(); ++j) {
            if (j)
                o << ',';
            o << quote(s.conditions[j].nonzero);
        }
        o << "],\"conditionPaths\":[";
        for (size_t j = 0; j < s.conditions.size(); ++j) {
            if (j)
                o << ',';
            path(s.conditions[j].source);
        }
        o << "]";
        bool typed=false;for(const auto& c:s.conditions)typed=typed || bool(c.expression);
        if(typed) {
            o << ",\"typedConditions\":[";
            for(size_t j=0;j<s.conditions.size();++j) {
                if(j)o<<',';const auto& c=s.conditions[j];
                o<<"{\"kind\":"<<unsigned(c.kind)<<",\"role\":"<<unsigned(c.role)<<",\"sourceState\":"<<c.sourceState
                 <<",\"verification\":"<<unsigned(c.verification)<<",\"expression\":"<<quote(c.nonzero)<<'}';
            }
            o<<']';
        }
        o << ",\"branches\":[";
        for (size_t j = 0; j < s.branches.size(); ++j) {
            if (j)
                o << ',';
            const auto &b = s.branches[j];
            o << "{\"id\":" << unsigned(b.id)
              << ",\"rejected\":" << (b.status == BranchStatus::Rejected) << ",\"equations\":";
            equations(b.equations);
            if(b.origin)o<<",\"origin\":"<<unsigned(b.origin)<<",\"status\":"<<unsigned(b.status)<<",\"originalCheck\":"<<unsigned(b.originalCheck);
            o << '}';
        }
        o << "]}";
    }
    o << "],\"steps\":[";
    for (size_t i = 0; i < d.steps.size(); ++i) {
        if (i)
            o << ',';
        const auto &s = d.steps[i];
        o << "{\"rule\":" << quote(ruleId(s.rule)) << ",\"before\":" << s.before
          << ",\"after\":" << s.after << ",\"relation\":" << unsigned(s.relation)
          << ",\"verdict\":" << unsigned(s.verification) << ",\"operand\":" << quote(s.operand)
          << ",\"key\":" << quote(messageKey(s.explanation))
          << ",\"text\":" << quote(explain(s.explanation, s.parameters, locale))
          << ",\"group\":" << s.group << ",\"branch\":" << unsigned(s.branch)
          << ",\"row\":" << unsigned(s.row) << ",\"otherRow\":" << unsigned(s.otherRow)
          << ",\"parameters\":[";
        for (size_t j = 0; j < s.parameters.size(); ++j) {
            if (j)
                o << ',';
            o << quote(s.parameters[j].value);
        }
        o << "],\"parameterKinds\":[";
        for (size_t j = 0; j < s.parameters.size(); ++j) {
            if (j)
                o << ',';
            o << unsigned(s.parameters[j].kind);
        }
        o << "],\"auxiliaries\":";
        strings(s.auxiliaries);
        o << ",\"prerequisites\":";
        numbers(s.prerequisites);
        o << ",\"introduced\":";
        numbers(s.introduced);
        o << ",\"discharged\":";
        numbers(s.discharged);
        if(s.caseStart!=UINT16_MAX)o<<",\"caseStart\":"<<s.caseStart<<",\"caseBranch\":"<<unsigned(s.caseBranch);
        o << ",\"substeps\":";
        numbers(s.substeps);
        o << ",\"affected\":[";
        for (size_t j = 0; j < s.affected.size(); ++j) {
            if (j)
                o << ',';
            path(s.affected[j]);
        }
        o << "]}";
    }
    o << "]}";
    return o.str();
}

} // namespace tutor
