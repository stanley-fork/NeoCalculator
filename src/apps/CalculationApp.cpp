/*
 * NeoCalculator - NumOS
 * Copyright (C) 2026 Juan Ramon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * CalculationApp.cpp — Calculadora V.P.A.M. con LVGL 9.5
 *
 * Integración completa de las cuatro fases del motor matemático:
 *   Fase 1: AST dinámico (MathAST) ✓
 *   Fase 2: Cursor + Inserción VPAM (CursorController) ✓
 *   Fase 3: Renderizado LVGL pixel-perfect (MathCanvas) ✓
 *   Fase 4: Autoridad Giac + presentación exacta/decimal ✓
 *
 * La pantalla es 100% LVGL (no TFT directo). Dos widgets MathCanvas:
 *   · Expresión (arriba): editable, con cursor parpadeante
 *   · Resultado (abajo):  solo lectura, muestra tras ENTER
 *
 * S⇔D (FREE_EQ): alterna entre resultado exacto (fracción/radical)
 * y decimal (double formateado).
 */

#include "CalculationApp.h"
#include "../math/tutor/Locale.h"
#include "../math/AngleModeRuntime.h"
#include "../input/KeyCodes.h"
#include "../input/generated/ProductionKeypadMap.generated.h"
#include "../Config.h"
#include <cmath>
#include <cstdlib>
#include "../math/cas/ASTFlattener.h"
#include "../math/cas/SymSimplify.h"
#include "../math/cas/SymExprToAST.h"
#include "../ui/MathTypography.h"
#include "../ui/TutorFonts.h"
#include "../utils/HwUxProbe.h"
#ifdef NATIVE_SIM
  #include <cstdio>
#endif

// ── Colores ──
static constexpr uint32_t COL_BG_HEX     = 0xFFFFFF;   // Blanco puro
static constexpr uint32_t COL_SEP_HEX    = 0x333333;   // Gris separador

// ── Dimensiones ──
static constexpr int SCREEN_W      = 320;
static constexpr int SCREEN_H      = 240;
static constexpr int BAR_H         = ui::StatusBar::HEIGHT + 1;  // 24 + 1 separator
static constexpr int PAD           = 6;     // Safety margin on all edges
static constexpr int CONTENT_TOP   = BAR_H;                     // y = 25: top of content area
static constexpr int CONTENT_BOT   = SCREEN_H - PAD;            // y = 234: bottom of content area
static constexpr int CONTENT_FULL_H = CONTENT_BOT - CONTENT_TOP; // 209 px: full edit-mode height
static constexpr int CONTENT_W     = SCREEN_W - 2 * PAD;        // 308 px: usable content width
static constexpr int SEP_THICK     = 1;   // Separator line height (px)
static constexpr int SEP_GAP       = 4;   // Gap on each side of the separator (band→sep and sep→band)
static constexpr int BAND_MIN_H    = 8;   // Absolute minimum height for each result-mode band

// ════════════════════════════════════════════════════════════════════════════
// Constructor / Destructor
// ════════════════════════════════════════════════════════════════════════════

CalculationApp::CalculationApp()
    : _screen(nullptr)
    , _resultSep(nullptr)
    , _rootRow(nullptr)
    , _hasResult(false)
    , _showDecimal(false)
    , _resultMode(numos::CalculationFormat::Standard)
    , _resultRow(nullptr)
    , _historyIndex(-1)
    , _hasEduSteps(false)
    , _stepViewerActive(false)
    , _stepsContainer(nullptr)
{
}

CalculationApp::~CalculationApp() {
    end();
}

// ════════════════════════════════════════════════════════════════════════════
// begin() — Crea pantalla LVGL, widgets y AST inicial
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::begin() {
    if (_screen) return;   // Ya creada

    createUI();
    resetExpression();
}

// ════════════════════════════════════════════════════════════════════════════
// end() — Destruye la pantalla LVGL y limpia
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::end() {
    closeFormatMenu();
    closeStepViewer();

    _mathCanvas.stopCursorBlink();
    _mathCanvas.destroy();
    _resultCanvas.destroy();
    _statusBar.destroy();

    if (_screen) {
        lv_obj_delete(_screen);
        _screen      = nullptr;
        _resultSep   = nullptr;
        _stepsContainer = nullptr;
        _resultTextLabel = nullptr;   // child of _screen, deleted with it
    }
    _structuredResult.reset();

    _rootNode.reset();
    _rootRow = nullptr;
    _resultNode.reset();
    _resultRow = nullptr;
    _hasResult = false;

    _eduStepLogger.clear();
    _eduArena.reset();
    _hasEduSteps = false;
}

// ════════════════════════════════════════════════════════════════════════════
// load() — Hace visible la pantalla de la calculadora
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::load() {
    if (!_screen) begin();
    lv_screen_load_anim(_screen, LV_SCREEN_LOAD_ANIM_FADE_IN, 200, 0, false);
    _mathCanvas.startCursorBlink();
    _statusBar.update();
    refreshExpression();
}

// ════════════════════════════════════════════════════════════════════════════
// createUI() — Construye la jerarquía de widgets LVGL
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::createUI() {
    ui::initMathTypography();

    // ── Pantalla ──
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, lv_color_hex(COL_BG_HEX), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_remove_flag(_screen, LV_OBJ_FLAG_SCROLLABLE);

    // ── StatusBar global (24 px + 1 px separador) ──
    _statusBar.create(_screen);
    _statusBar.setTitle("Calculation");
    _statusBar.setBatteryLevel(100);

    // ── MathCanvas — Expression (full-area, vertically centered in edit mode) ──
    _mathCanvas.create(_screen);
    _mathCanvas.setEmptyRootPlaceholderVisible(false);
    _mathCanvas.setAutoHeightEnabled(false);
    _mathCanvas.setTraceLabel("calc_input_edit");
    // LaTeX inline look: top-level atoms use TEXT style so fractions step their
    // numerator/denominator down to SCRIPT style (compact inline rendering).
    _mathCanvas.setMathStyle(vpam::MathStyle::TEXT);
    lv_obj_set_pos(_mathCanvas.obj(), PAD, CONTENT_TOP);
    lv_obj_set_size(_mathCanvas.obj(), CONTENT_W, CONTENT_FULL_H);
    lv_obj_add_style(_mathCanvas.obj(), &ui::style_math_primary, LV_PART_MAIN);

    // ── Separator line expr↔result (#333) — initially hidden ──
    _resultSep = lv_obj_create(_screen);
    lv_obj_set_size(_resultSep, CONTENT_W, 1);
    lv_obj_set_style_bg_color(_resultSep, lv_color_hex(COL_SEP_HEX), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(_resultSep, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(_resultSep, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(_resultSep, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(_resultSep, 0, LV_PART_MAIN);
    lv_obj_remove_flag(_resultSep, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(_resultSep, LV_OBJ_FLAG_HIDDEN);

    // ── MathCanvas — Result (fills remaining area when visible) ──
    _resultCanvas.create(_screen);
    _resultCanvas.setAutoHeightEnabled(false);
    _resultCanvas.setTraceLabel("calc_result");
    _resultCanvas.setMathStyle(vpam::MathStyle::TEXT);
    lv_obj_set_pos(_resultCanvas.obj(), PAD, CONTENT_TOP);
    lv_obj_set_size(_resultCanvas.obj(), CONTENT_W, CONTENT_FULL_H);
    lv_obj_add_style(_resultCanvas.obj(), &ui::style_math_primary, LV_PART_MAIN);
    lv_obj_add_flag(_resultCanvas.obj(), LV_OBJ_FLAG_HIDDEN);
}

// ════════════════════════════════════════════════════════════════════════════
// resetExpression() — Crea un AST vacío y conecta al MathCanvas
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::resetExpression() {
    _rootNode = vpam::makeRow();
    _rootRow  = static_cast<vpam::NodeRow*>(_rootNode.get());

    // Insertar un NodeEmpty inicial para que el usuario sepa dónde escribir
    _rootRow->appendChild(vpam::makeEmpty());

    // Inicializar el cursor al inicio de la fila raíz
    _cursor.init(_rootRow);

    // Conectar al canvas
    _mathCanvas.setExpression(_rootRow, &_cursor);
    refreshExpression();
}

// ════════════════════════════════════════════════════════════════════════════
// refreshExpression() — Recalcula layout e invalida el canvas
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::refreshExpression() {
    if (!_rootRow) return;
    _rootRow->calculateLayout(_mathCanvas.normalMetrics());
    _mathCanvas.invalidate();
}

// ════════════════════════════════════════════════════════════════════════════
// handleKey() — Traduce KeyCode a operaciones VPAM
//
// Usa KeyboardManager para gestionar SHIFT/ALPHA/LOCK/STO.
// En modo ALPHA, las teclas numéricas insertan variables (A-F).
// En modo STO, las teclas de variable ejecutan un guardado.
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::handleKey(const KeyEvent& ev) {
#ifdef NATIVE_SIM
    std::printf("[CALCAPP] handleKey: code=%d action=%d\n",
                static_cast<int>(ev.code), static_cast<int>(ev.action));
#endif
    if (ev.action != KeyAction::PRESS && ev.action != KeyAction::REPEAT) return;

    // ── Step viewer active: intercept keys for scroll/back ──────────────
    if (_stepViewerActive) {
        switch (ev.code) {
            case KeyCode::UP:
                if (_stepsContainer)
                    lv_obj_scroll_by(_stepsContainer, 0, 30, LV_ANIM_ON);
                break;
            case KeyCode::DOWN:
                if (_stepsContainer)
                    lv_obj_scroll_by(_stepsContainer, 0, -30, LV_ANIM_ON);
                break;
            case KeyCode::AC:
            case KeyCode::DEL:
            case KeyCode::F2:
                closeStepViewer();
                break;
            default:
                break;
        }
        return;
    }

    auto& km = vpam::KeyboardManager::instance();

    const auto semantic =
        static_cast<numos::input::SemanticId>(ev.semanticId);
    if (_formatMenu) {
        if (ev.code == KeyCode::UP && _formatChoice) --_formatChoice;
        else if (ev.code == KeyCode::DOWN && _formatChoice + 1 < _formatChoiceCount) ++_formatChoice;
        else if (ev.code == KeyCode::ENTER) {
            applyFormatChoice();
            return;
        } else if (ev.code == KeyCode::AC || ev.code == KeyCode::DEL ||
                   ev.code == KeyCode::FORMAT || ev.code == KeyCode::FORMAT_MENU) {
            closeFormatMenu();
            return;
        }
        updateFormatMenu();
        return;
    }
    if (ev.code == KeyCode::FORMAT_MENU ||
        ((ev.code == KeyCode::FORMAT || ev.code == KeyCode::FREE_EQ) && km.isShift() && km.isAlpha())) {
        km.consumeModifier();
        openFormatMenu();
        _statusBar.update();
        return;
    }
    if (semantic == numos::input::SemanticId::engineering_notation ||
        (ev.code == KeyCode::EXP && km.isShift() && !km.isAlpha())) {
        km.consumeModifier();
        engineeringFormat();
        _statusBar.update();
        return;
    }
    if (_hasResult && _resultMode == numos::CalculationFormat::Engineering &&
        !km.isShift() && (ev.code == KeyCode::LEFT || ev.code == KeyCode::RIGHT)) {
        engineeringFormat(ev.code == KeyCode::LEFT ? -1 : 1);
        return;
    }

    if (semantic == numos::input::SemanticId::left_bracket ||
        semantic == numos::input::SemanticId::right_bracket) {
        clearResult();
        if (semantic == numos::input::SemanticId::left_bracket)
            _cursor.insertParen(vpam::DelimKind::Bracket);
        else _cursor.closeBracket();
        _statusBar.update();
        _mathCanvas.resetCursorBlink();
        refreshExpression();
        return;
    }
    if (semantic == numos::input::SemanticId::pow10) {
        if (_cursor.insertPowerOfTen()) clearResult();
        _statusBar.update();
        _mathCanvas.resetCursorBlink();
        refreshExpression();
        return;
    }
    if (semantic >= numos::input::SemanticId::alpha_A &&
        semantic <= numos::input::SemanticId::alpha_Z) {
        const char variable = static_cast<char>(
            'A' + static_cast<int>(semantic) -
            static_cast<int>(numos::input::SemanticId::alpha_A));
        clearResult();
        _cursor.insertVariable(variable);
        _statusBar.update();
        _mathCanvas.resetCursorBlink();
        refreshExpression();
        return;
    }
    if (semantic == numos::input::SemanticId::asin ||
        semantic == numos::input::SemanticId::acos ||
        semantic == numos::input::SemanticId::atan) {
        clearResult();
        const auto function =
            semantic == numos::input::SemanticId::asin
                ? vpam::FuncKind::ArcSin
                : semantic == numos::input::SemanticId::acos
                    ? vpam::FuncKind::ArcCos
                    : vpam::FuncKind::ArcTan;
        _cursor.insertFunction(function);
        _statusBar.update();
        _mathCanvas.resetCursorBlink();
        refreshExpression();
        return;
    }

    // ── Modificadores: SHIFT / ALPHA / STO ──────────────────────────────
    if (ev.code == KeyCode::SHIFT) {
        km.pressShift();
        _statusBar.update();
        return;
    }
    if (ev.code == KeyCode::ALPHA) {
        km.pressAlpha();
        _statusBar.update();
        return;
    }
    if (ev.code == KeyCode::STO) {
        km.pressStore();
        _statusBar.update();
        return;
    }

    // ── Modo STO: esperando tecla de variable para guardar ──────────────
    if (km.isStore()) {
        char varName = alphaKeyToVarName(ev.code);
        if (varName != '\0') {
            executeStore(varName);
            km.reset();
            _statusBar.update();
            return;
        }
        // Cualquier otra tecla cancela STO
        km.reset();
        _statusBar.update();
        // Continuar procesando la tecla normalmente
    }

    // ── Modo ALPHA: teclas numéricas → variables ────────────────────────
    if (km.isAlpha()) {
        char varName = alphaKeyToVarName(ev.code);
        if (varName != '\0') {
            clearResult();
            _cursor.insertVariable(varName);
            km.consumeModifier();
            _statusBar.update();
            _mathCanvas.resetCursorBlink();
            refreshExpression();
            return;
        }
        // Si no es una tecla mapeada a variable, consuma el modifier y siga
        km.consumeModifier();
        _statusBar.update();
    }

    // ── SHIFT + combos que generan key codes virtuales ──────────────────
    KeyCode effectiveCode = ev.code;
    if (km.isShift() && ev.code == KeyCode::TABLE) {
        effectiveCode = KeyCode::FACT;
        km.consumeModifier();
    }

    bool changed = true;

    switch (effectiveCode) {
        // ── Step viewer (F2) ──
        case KeyCode::F2:
            if (_stepViewerActive) {
                closeStepViewer();
            } else if (_hasEduSteps && _hasResult) {
                openStepViewer();
            }
            changed = false;
            break;

        // ── Dígitos ──
        case KeyCode::NUM_0: clearResult(); _cursor.insertDigit('0'); break;
        case KeyCode::NUM_1: clearResult(); _cursor.insertDigit('1'); break;
        case KeyCode::NUM_2: clearResult(); _cursor.insertDigit('2'); break;
        case KeyCode::NUM_3: clearResult(); _cursor.insertDigit('3'); break;
        case KeyCode::NUM_4: clearResult(); _cursor.insertDigit('4'); break;
        case KeyCode::NUM_5: clearResult(); _cursor.insertDigit('5'); break;
        case KeyCode::NUM_6: clearResult(); _cursor.insertDigit('6'); break;
        case KeyCode::NUM_7: clearResult(); _cursor.insertDigit('7'); break;
        case KeyCode::NUM_8: clearResult(); _cursor.insertDigit('8'); break;
        case KeyCode::NUM_9: clearResult(); _cursor.insertDigit('9'); break;
        // Punto decimal: la inserción correcta vive ahora en el guard compartido
        // de CursorController::insertDigit (Phase 8E). Se revierte el workaround
        // local de Phase 8D — ya no se necesita.
        case KeyCode::DOT:   clearResult(); _cursor.insertDigit('.'); break;

        // ── Operadores ──
        case KeyCode::ADD: clearResult(); _cursor.insertOperator(vpam::OpKind::Add); break;
        case KeyCode::NEG:
        case KeyCode::NEGATE:
        case KeyCode::SUB: clearResult(); _cursor.insertOperator(vpam::OpKind::Sub); break;
        case KeyCode::MUL: clearResult(); _cursor.insertOperator(vpam::OpKind::Mul); break;
        case KeyCode::DIVIDE: clearResult(); _cursor.insertOperator(vpam::OpKind::Div); break;

        // ── Estructuras VPAM ──
        case KeyCode::DIV:
        case KeyCode::FRAC:   clearResult(); _cursor.insertFraction(); break;
        case KeyCode::POW:    clearResult(); _cursor.insertPower();    break;
        case KeyCode::SQUARE:
            clearResult();
            _cursor.insertPower();
            _cursor.insertDigit('2');
            break;
        case KeyCode::SQRT:   clearResult(); _cursor.insertRoot();     break;
        case KeyCode::LPAREN:
            clearResult();
            _cursor.insertParen(km.isShift() ? vpam::DelimKind::Bracket : vpam::DelimKind::Paren);
            if (km.isShift()) km.consumeModifier();
            break;
        case KeyCode::LBRACKET: clearResult(); _cursor.insertParen(vpam::DelimKind::Bracket); break;
        case KeyCode::RBRACKET: clearResult(); _cursor.closeBracket(); break;
        case KeyCode::RPAREN:
            clearResult();
            if (km.isShift()) { _cursor.closeBracket(); km.consumeModifier(); }
            else _cursor.moveRight();
            break;
        case KeyCode::COMMA:  clearResult(); _cursor.insertVariable(','); break;
        case KeyCode::EQUAL:
            clearResult();
            _cursor.insertOperator(vpam::OpKind::Eq);
            break;
        case KeyCode::EXP:
            if (_cursor.insertPowerOfTen()) clearResult();
            break;

        // ── Funciones trigonométricas / logarítmicas ──
        case KeyCode::SIN:
            clearResult();
            if (km.isShift()) {
                _cursor.insertFunction(vpam::FuncKind::ArcSin);
                km.consumeModifier();
            } else {
                _cursor.insertFunction(vpam::FuncKind::Sin);
            }
            break;
        case KeyCode::COS:
            clearResult();
            if (km.isShift()) {
                _cursor.insertFunction(vpam::FuncKind::ArcCos);
                km.consumeModifier();
            } else {
                _cursor.insertFunction(vpam::FuncKind::Cos);
            }
            break;
        case KeyCode::TAN:
            clearResult();
            if (km.isShift()) {
                _cursor.insertFunction(vpam::FuncKind::ArcTan);
                km.consumeModifier();
            } else {
                _cursor.insertFunction(vpam::FuncKind::Tan);
            }
            break;

        // ── Logaritmos ──
        case KeyCode::LN:
            clearResult();
            _cursor.insertFunction(vpam::FuncKind::Ln);
            break;
        case KeyCode::LOG:
            if (km.isShift()) {
                if (_cursor.insertPowerOfTen()) clearResult();
                km.consumeModifier();
            } else {
                clearResult();
                _cursor.insertFunction(vpam::FuncKind::Log);
            }
            break;
        case KeyCode::LOG_BASE:
            clearResult();
            _cursor.insertLogBase();
            break;

        // ── Constantes algebraicas ──
        case KeyCode::CONST_PI:
            clearResult();
            _cursor.insertConstant(vpam::ConstKind::Pi);
            break;
        case KeyCode::CONST_E:
            clearResult();
            _cursor.insertConstant(vpam::ConstKind::E);
            break;

        // ── Variables directas (teclado físico o serial) ──
        case KeyCode::VAR_X:
            clearResult();
            _cursor.insertVariable('x');
            break;
        case KeyCode::VAR_Y:
            clearResult();
            _cursor.insertVariable('y');
            break;
        case KeyCode::ANS:
            clearResult();
            _cursor.insertVariable(vpam::VAR_ANS);
            break;
        case KeyCode::PREANS:
            clearResult();
            _cursor.insertVariable(vpam::VAR_PREANS);
            break;

        // ── Variables Alpha (via serial o mapeo directo) ──
        case KeyCode::ALPHA_A: clearResult(); _cursor.insertVariable('A'); break;
        case KeyCode::ALPHA_B: clearResult(); _cursor.insertVariable('B'); break;
        case KeyCode::ALPHA_C: clearResult(); _cursor.insertVariable('C'); break;
        case KeyCode::ALPHA_D: clearResult(); _cursor.insertVariable('D'); break;
        case KeyCode::ALPHA_E: clearResult(); _cursor.insertVariable('E'); break;
        case KeyCode::ALPHA_F: clearResult(); _cursor.insertVariable('F'); break;

        // ── Navegación ──
        case KeyCode::LEFT:
            if (_hasResult && (km.isShift() || _resultCanvas.hasHorizontalOverflow())) {
                (km.isShift() ? _mathCanvas : _resultCanvas).scrollBounded(20);
                changed = false;
            } else {
                _cursor.moveLeft();
            }
            break;
        case KeyCode::RIGHT:
            if (_hasResult && (km.isShift() || _resultCanvas.hasHorizontalOverflow())) {
                (km.isShift() ? _mathCanvas : _resultCanvas).scrollBounded(-20);
                changed = false;
            } else {
                _cursor.moveRight();
            }
            break;
        case KeyCode::UP:
            if (_hasResult && km.isShift()) {
                // WHY: keep plain UP/DOWN for history. A tall result takes
                // precedence; otherwise expose the tall authored expression.
                (_resultCanvas.hasVerticalOverflow() ? _resultCanvas : _mathCanvas)
                    .scrollVerticalBounded(20);
                changed = false;
                break;
            }
            if (_hasResult || _historyIndex >= 0) {
                // Navegar historial: ir a la entrada anterior
                navigateHistory(-1);
                changed = false;
            } else {
                _cursor.moveUp();
            }
            break;
        case KeyCode::DOWN:
            if (_hasResult && km.isShift()) {
                (_resultCanvas.hasVerticalOverflow() ? _resultCanvas : _mathCanvas)
                    .scrollVerticalBounded(-20);
                changed = false;
                break;
            }
            if (_historyIndex >= 0) {
                // Navegar historial: ir a la entrada más reciente
                navigateHistory(1);
                changed = false;
            } else {
                _cursor.moveDown();
            }
            break;

        // ── Borrado ──
        case KeyCode::DEL: clearResult(); _cursor.backspace(); break;

        // ── AC: limpiar toda la expresión, resultado y modificador ──
        case KeyCode::AC:
            clearResult();
            resetExpression();
            _historyIndex = -1;
            km.reset();
            break;

        // ── ENTER: evaluar el AST ──
        case KeyCode::ENTER:
            evaluateExpression();
            changed = false;
            break;

        // ── S⇔D: alternar exacto / decimal ──
        case KeyCode::FREE_EQ:
        case KeyCode::FORMAT:
            if (_hasResult) {
                toggleSD();
            }
            changed = false;
            break;

        // ── FACT: factorización en primos ──
        case KeyCode::FACT:
            if (_hasResult) {
                clearResult();
                resetExpression();
                _cursor.insertVariable(vpam::VAR_ANS);
            }
            changed = _cursor.insertFactorial();
            break;

        default:
            changed = false;
            break;
    }

    // Consumir SHIFT simple después de cualquier acción
    km.consumeModifier();
    _statusBar.update();

    if (changed) {
        _mathCanvas.resetCursorBlink();
        refreshExpression();
    }
}

// ════════════════════════════════════════════════════════════════════════════
// evaluateExpression() — Evalúa el AST y muestra el resultado
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::evaluateExpression() {
    if (!_rootRow) return;
    numos::HwUxProbe hwux("calculation", "evaluate");

    // Giac is the mathematical authority for Calculation.
    // Serialize the authored VPAM tree, mirror A-F/Ans/PreAns into the Giac
    // context, evaluate, and receive the presentation tiers. NO legacy
    // re-evaluation happens on any path.
    numos::CalculationEvaluation ev =
        numos::CalculationEngine::instance().evaluate(_rootRow);
#if defined(NATIVE_SIM) && !defined(__EMSCRIPTEN__)
    if (std::getenv("NUMOS_CALC_INPUT_TRACE"))
        std::printf("[CALC-EVAL] serialized=%s diagnostic=%s status=%d\n",
                    ev.serialized.c_str(), ev.diagnostic.c_str(), int(ev.status));
#endif

    _hasResult      = true;
    _lastStatus     = ev.status;
    _lastKind       = ev.kind;
    _exactValValid  = ev.exactValValid;
    _exactText      = ev.exactText;
    _approxText     = ev.approximateText;
    _structuredResult = std::move(ev.exactAST);
    _structuredApproxResult = std::move(ev.approximateAST);
    _reusePolicy = ev.reusePolicy;
    _sToDPolicy = ev.sToDPolicy;
    _fallbackReason = ev.fallbackReason;
    _angleResult = ev.ok() && numos::authoredAngle(_rootRow);
    _resultInDegrees = numos::angleModeIsDeg();
    _phaseUnit = _resultInDegrees ? numos::CalculationFormat::Degrees : numos::CalculationFormat::Radians;

    // ExactVal mirror: exact when tier 1; numeric fallback otherwise (probes,
    // FACT and the persistent Ans store still see a coherent value).
    bool ansUpdated = false;
    if (ev.ok()) {
        bool numericMirror = false;
        if (ev.exactValValid) {
            _lastResult = ev.exactVal;
        } else {
            double d = 0.0;
            char* end = nullptr;
            const bool numeric =
                !_approxText.empty() &&
                (d = std::strtod(_approxText.c_str(), &end), end && *end == '\0') &&
                std::isfinite(d);
            if (numeric) {
                _lastResult = vpam::ExactVal::fromDouble(d);
                numericMirror = true;
            } else {
                // The canonical structured result owns non-real/symbolic values.
                // Do not fabricate a zero for legacy scalar-only consumers.
                _lastResult = vpam::ExactVal::makeError("Math ERROR");
        }
        }
        // Ans policy: successful results with an exact or numeric value
        // update the persistent NumOS store; the exact Giac text rides
        // along in the session so sqrt(2)/2^100 chains stay exact. Pure
        // free-symbol results (no numeric mirror) leave Ans untouched.
        if (ev.exactValValid || numericMirror) {
            vpam::VariableManager::instance().updateAns(_lastResult);
            ansUpdated = true;
        }
        if (_reusePolicy != numos::ResultReusePolicy::NonReusable)
            numos::CalculationEngine::instance().noteAnsRotated(ev.exactText, ansUpdated);
    } else {
        // Typed error statuses keep the legacy error rendering (and the
        // assert_error contract). Failed evaluations never overwrite Ans.
        const char* msg = "Math ERROR";
        if (ev.status == numos::MathEngineStatus::ParseError) msg = "Syntax ERROR";
        if (ev.status == numos::MathEngineStatus::Unsupported)
            msg = numos::tutor::productLocale == numos::tutor::Locale::Spanish
                ? "Limite de expresion" : "Expression limit";
        _lastResult = vpam::ExactVal::makeError(msg);
    }
    (void)ansUpdated;

    // Standard/exact is always the initial view, including decimal literals.
    _resultMode = numos::CalculationFormat::Standard;
    _engineeringShift = 0;
    _showDecimal = false;

    // ── Guardar en historial ─────────────────────────────────────────
    {
        HistoryEntry entry;
        entry.exprAST = vpam::cloneNode(_rootRow);
        entry.result  = _lastResult;
        entry.status  = _lastStatus;
        entry.kind    = _lastKind;
        entry.exactValValid = _exactValValid;
        entry.exactText  = _exactText;
        entry.approxText = _approxText;
        entry.angleResult = _angleResult;
        entry.resultInDegrees = _resultInDegrees;
        if (_structuredResult)
            entry.resultAST = vpam::cloneNode(_structuredResult.get());
        if (_structuredApproxResult)
            entry.approximateAST = vpam::cloneNode(_structuredApproxResult.get());
        entry.reusePolicy = _reusePolicy;
        entry.sToDPolicy = _sToDPolicy;
        entry.fallbackReason = _fallbackReason;
        _history.push_back(std::move(entry));
        if (static_cast<int>(_history.size()) > MAX_HISTORY) {
            _history.erase(_history.begin());
        }
    }
    _historyIndex = -1;

    _mathCanvas.stopCursorBlink();
    _mathCanvas.setExpression(_rootRow, nullptr);

    // Educational steps (tutor mathematics stays on the custom CAS).
    _eduStepLogger.clear();
    _eduArena.reset();
    _hasEduSteps = false;
    if (setting_edu_steps && ev.ok()) {
        generateEduSteps();
    }

    showResult();

    const char* status = "error";
    switch (_lastStatus) {
        case numos::MathEngineStatus::Ok: status = "ok"; break;
        case numos::MathEngineStatus::Undefined: status = "undefined"; break;
        case numos::MathEngineStatus::ParseError: status = "parse_error"; break;
        case numos::MathEngineStatus::EvaluationError: status = "evaluation_error"; break;
        case numos::MathEngineStatus::Unsupported: status = "unsupported"; break;
        case numos::MathEngineStatus::OutOfMemory: status = "out_of_memory"; break;
    }
    const char* kind = _lastKind == numos::CalcResultKind::Structured
        ? "structured" : (_lastKind == numos::CalcResultKind::TextFallback
        ? "text_fallback" : "none");
    hwux.finish(status, kind, "giac", numos::hwUxHash(_exactText.c_str()));
}

bool CalculationApp::navigateBack() {
    if (_formatMenu) { closeFormatMenu(); return true; }
    if (!_stepViewerActive) return false;
    closeStepViewer();
    return true;
}

// ════════════════════════════════════════════════════════════════════════════
// applyResultLayout() — Dynamically trim expression, reveal separator, fill result
//
// Called after ENTER (evaluation) or FACT (factorization) to transition
// from full-screen Edit Mode to the split-screen Result Mode layout.
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::applyResultLayout() {
    // ── Content-proportional result-mode split ──────────────────────────────
    //
    // Available space for the two bands (separating overhead excluded):
    //   avail = CONTENT_FULL_H - SEP_THICK - 2*SEP_GAP
    //         = 209 - 1 - 8 = 200 px
    //
    // Invariants (always satisfied before any LVGL call):
    //   inH  >= BAND_MIN_H
    //   resH >= BAND_MIN_H
    //   inH + resH == avail  (so total == CONTENT_FULL_H)
    //
    // Centering is handled by the renderer: each MathCanvas centers its content
    // vertically using:  baseline = y1 + (widgetH + ascent - descent) / 2

    // Bare content heights (0 pad — centering is done by the renderer, not by padding).
    const int16_t cIn  = vpam::mathObjectHeightPx(
        _rootRow->layout(),   _mathCanvas.normalMetrics(),  0);
    const int16_t cRes = vpam::mathObjectHeightPx(
        _resultRow->layout(), _resultCanvas.normalMetrics(), 0);

    // Fixed overhead = separator + gaps on both sides.
    const int16_t overhead = static_cast<int16_t>(SEP_THICK + 2 * SEP_GAP);
    const int16_t avail    = static_cast<int16_t>(CONTENT_FULL_H - overhead);

    // Proportional split; handle every degenerate case with no branching surprise.
    int16_t inH, resH;
    bool overflowed = false;

    const int16_t combined = static_cast<int16_t>(cIn + cRes);
    if (combined <= 0) {
        // Both layouts are empty/invalid: give each band half.
        inH  = static_cast<int16_t>(avail / 2);
        resH = static_cast<int16_t>(avail - inH);
    } else {
        // Proportional assignment (integer multiply first to avoid precision loss).
        inH  = static_cast<int16_t>(static_cast<int32_t>(avail) * cIn / combined);
        resH = static_cast<int16_t>(avail - inH);

        if (combined > avail) {
            // Overflow: both bands are already shrunk proportionally — accept it.
            overflowed = true;
        } else {
            // Ensure each band is at least as tall as its content.
            // Adjustments are self-canceling: the sum stays == avail.
            if (inH < cIn) {
                inH  = cIn;
                resH = static_cast<int16_t>(avail - inH);
            } else if (resH < cRes) {
                resH = cRes;
                inH  = static_cast<int16_t>(avail - resH);
            }
        }
    }

    // Apply absolute minimums last.  Shrink the other band by the same amount
    // to preserve the invariant  inH + resH == avail.
    if (inH < BAND_MIN_H) {
        inH  = BAND_MIN_H;
        resH = static_cast<int16_t>(avail - inH);
    }
    if (resH < BAND_MIN_H) {
        resH = BAND_MIN_H;
        inH  = static_cast<int16_t>(avail - resH);
    }
    // Final safety: if avail itself is too small (extreme screen/content edge case)
    // force each to BAND_MIN_H and let LVGL clip; total may exceed avail but screen
    // hardware bounds remain valid.
    if (inH < BAND_MIN_H)  inH  = BAND_MIN_H;
    if (resH < BAND_MIN_H) resH = BAND_MIN_H;

    // ── Derive absolute y-coordinates ──────────────────────────────────────
    // Layout (top-to-bottom):
    //   [inY .. inY+inH)   : input  canvas
    //   [inY+inH .. sepY)  : SEP_GAP
    //   [sepY .. sepY+SEP_THICK) : separator
    //   [sepY+SEP_THICK .. resY) : SEP_GAP
    //   [resY .. resY+resH)      : result canvas
    //
    const int16_t inY  = static_cast<int16_t>(CONTENT_TOP);
    const int16_t sepY = static_cast<int16_t>(inY  + inH  + SEP_GAP);
    const int16_t resY = static_cast<int16_t>(sepY + SEP_THICK + SEP_GAP);

#if defined(NUMOS_MATH_RENDER_TRACE_ONCE)
    Serial.printf(
        "[CALC-SPLIT] cIn=%d cRes=%d avail=%d inH=%d resH=%d "
        "inY=%d inBot=%d sepY=%d resY=%d resBot=%d overflow=%s\n",
        (int)cIn, (int)cRes, (int)avail, (int)inH, (int)resH,
        (int)inY,  (int)(inY  + inH),
        (int)sepY,
        (int)resY, (int)(resY + resH),
        overflowed ? "yes" : "no");
#else
    (void)overflowed;
#endif

    // ── Apply to LVGL objects ───────────────────────────────────────────────
    lv_obj_set_pos(_mathCanvas.obj(), PAD, inY);
    lv_obj_set_size(_mathCanvas.obj(), CONTENT_W, inH);

    lv_obj_set_pos(_resultSep, PAD, sepY);
    lv_obj_remove_flag(_resultSep, LV_OBJ_FLAG_HIDDEN);

    lv_obj_set_pos(_resultCanvas.obj(), PAD, resY);
    lv_obj_set_size(_resultCanvas.obj(), CONTENT_W, resH);
    lv_obj_remove_flag(_resultCanvas.obj(), LV_OBJ_FLAG_HIDDEN);

    _mathCanvas.setTraceLabel("calc_input_result_compact");
}

// ════════════════════════════════════════════════════════════════════════════
// GIAC-B01: presentación de texto plano (tier 3 — fallback honesto)
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::showTextResult(const std::string& text) {
    if (!_screen || !_rootRow) return;

    // No structured result AST in this tier.
    _resultNode.reset();
    _resultRow = nullptr;
    if (_resultCanvas.obj())
        lv_obj_add_flag(_resultCanvas.obj(), LV_OBJ_FLAG_HIDDEN);

    if (!_resultTextLabel) {
        _resultTextLabel = lv_label_create(_screen);
        // stix_math fonts have no space glyph — plain text must use the
        // default (montserrat) font.
        lv_obj_set_style_text_color(_resultTextLabel, lv_color_hex(0x1A1A1A),
                                    LV_PART_MAIN);
        lv_label_set_long_mode(_resultTextLabel, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(_resultTextLabel, CONTENT_W);
    }
    lv_label_set_text(_resultTextLabel, text.c_str());
    lv_obj_remove_flag(_resultTextLabel, LV_OBJ_FLAG_HIDDEN);

    // Same band geometry idea as applyResultLayout(), with a fixed-height
    // text band instead of a result canvas.
    const int16_t cIn = vpam::mathObjectHeightPx(
        _rootRow->layout(), _mathCanvas.normalMetrics(), 0);
    const int16_t overhead = static_cast<int16_t>(SEP_THICK + 2 * SEP_GAP);
    const int16_t avail    = static_cast<int16_t>(CONTENT_FULL_H - overhead);
    constexpr int16_t TEXT_BAND_H = 48;   // up to two wrapped lines

    int16_t inH = static_cast<int16_t>(avail - TEXT_BAND_H);
    if (inH > cIn) inH = cIn;
    if (inH < BAND_MIN_H) inH = BAND_MIN_H;

    const int16_t inY  = static_cast<int16_t>(CONTENT_TOP);
    const int16_t sepY = static_cast<int16_t>(inY + inH + SEP_GAP);
    const int16_t resY = static_cast<int16_t>(sepY + SEP_THICK + SEP_GAP);

    lv_obj_set_pos(_mathCanvas.obj(), PAD, inY);
    lv_obj_set_size(_mathCanvas.obj(), CONTENT_W, inH);

    lv_obj_set_pos(_resultSep, PAD, sepY);
    lv_obj_remove_flag(_resultSep, LV_OBJ_FLAG_HIDDEN);

    lv_obj_set_pos(_resultTextLabel, PAD, resY);

    _mathCanvas.setTraceLabel("calc_input_result_compact");
    _mathCanvas.invalidate();

    if (_hasEduSteps) {
        _statusBar.setTitle("F2: View Steps");
    }
}

void CalculationApp::hideTextResult() {
    if (_resultTextLabel)
        lv_obj_add_flag(_resultTextLabel, LV_OBJ_FLAG_HIDDEN);
}

// ════════════════════════════════════════════════════════════════════════════
// showResult() — Genera y muestra el AST del resultado (3 estados)
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::showResult() {
    if (_rootRow) {
        // WHY: failed input is still an editor expression. Keep its pending
        // slots visible after EXE; a read-only canvas would hide that evidence.
        _mathCanvas.setExpression(_rootRow,
            _lastStatus == numos::MathEngineStatus::Ok ? nullptr : &_cursor);
        _mathCanvas.stopCursorBlink();
    }

    hideTextResult();

    if (_lastStatus == numos::MathEngineStatus::Ok &&
        _lastKind == numos::CalcResultKind::TextFallback &&
        _resultMode == numos::CalculationFormat::Standard) {
        // Tier 3: Giac's own printed result as plain text — honest fallback,
        // never a legacy re-evaluation.
        std::string visible = "Giac text fallback (";
        visible += numos::engineFallbackReasonName(_fallbackReason);
        visible += "): ";
        visible += _exactText;
        showTextResult(visible);
        return;
    }

    _resultNode = formattedResult();
    if (!_resultNode) {
        _resultMode = numos::CalculationFormat::Standard;
        _resultNode = _structuredResult ? vpam::cloneNode(_structuredResult.get())
            : vpam::MathEvaluator::resultToAST(_lastResult);
    }
    _resultRow = static_cast<vpam::NodeRow*>(_resultNode.get());
    if (!_resultRow) return;
    // Conectar al canvas de resultado (sin cursor)
    _resultCanvas.setExpression(_resultRow, nullptr);
    _resultCanvas.resetScroll();   // Resetear scroll al cambiar de estado

    // Calcular layout del resultado
    _resultRow->calculateLayout(_resultCanvas.normalMetrics());

    // Dynamic layout: trim expression, position separator, fill result area
    applyResultLayout();

    _resultCanvas.invalidate();
    _mathCanvas.invalidate();

    // Show F2 hint in status bar when edu steps are available
    if (_hasEduSteps) {
        _statusBar.setTitle("F2: View Steps");
    }
}

// ════════════════════════════════════════════════════════════════════════════
// clearResult() — Oculta el resultado y restaura el cursor
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::clearResult() {
    if (!_hasResult) return;

    _hasResult = false;
    _resultNode.reset();
    _resultRow = nullptr;
    _structuredResult.reset();
    _structuredApproxResult.reset();
    _reusePolicy = numos::ResultReusePolicy::NonReusable;
    _sToDPolicy = numos::ResultSToDPolicy::Unavailable;
    _fallbackReason = numos::EngineFallbackReason::None;
    _lastKind = numos::CalcResultKind::None;
    _exactValValid = false;
    _exactText.clear();
    _approxText.clear();

    // Clear educational steps
    _eduStepLogger.clear();
    _eduArena.reset();
    _hasEduSteps = false;

    // Hide separator, result canvas and text-fallback label
    if (_resultSep) lv_obj_add_flag(_resultSep, LV_OBJ_FLAG_HIDDEN);
    if (_resultCanvas.obj()) lv_obj_add_flag(_resultCanvas.obj(), LV_OBJ_FLAG_HIDDEN);
    hideTextResult();

    // Restore expression canvas to full-area vertically-centered Edit Mode.
    // Re-set position too: applyResultLayout() moves the canvas, clearResult() must undo it.
    lv_obj_set_pos(_mathCanvas.obj(), PAD, CONTENT_TOP);
    lv_obj_set_height(_mathCanvas.obj(), CONTENT_FULL_H);
    _mathCanvas.setTraceLabel("calc_input_edit");
    _mathCanvas.setExpression(_rootRow, &_cursor);
    _mathCanvas.invalidate();

    // Restore title
    _statusBar.setTitle("Calculation");

    // Restaurar cursor
    _mathCanvas.startCursorBlink();
}

// ════════════════════════════════════════════════════════════════════════════
// toggleSD() — Rota entre 3 estados: Simbólico → Periódico → Extendido → ...
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::toggleSD() {
    if (!_hasResult || _lastStatus != numos::MathEngineStatus::Ok) return;
    if (_resultMode == numos::CalculationFormat::Standard) {
        if (!formatAvailable(numos::CalculationFormat::Decimal)) return;
        _resultMode = numos::CalculationFormat::Decimal;
    } else _resultMode = numos::CalculationFormat::Standard;
    _engineeringShift = 0;
    _showDecimal = _resultMode == numos::CalculationFormat::Decimal;
    showResult();
}

// ════════════════════════════════════════════════════════════════════════════
// navigateHistory() — Navega por el historial con flechas arriba/abajo
//
// direction: -1 = ir atrás (más antigua), +1 = ir adelante (más reciente)
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::navigateHistory(int direction) {
    if (_history.empty()) return;

    int newIndex;
    if (_historyIndex < 0) {
        // Estamos en "nueva expresión": arriba va al último
        if (direction < 0) {
            newIndex = static_cast<int>(_history.size()) - 1;
        } else {
            return;  // Ya estamos al final
        }
    } else {
        newIndex = _historyIndex + direction;
    }

    // Clamp
    if (newIndex < 0) newIndex = 0;
    if (newIndex >= static_cast<int>(_history.size())) {
        // Pasamos del final → volver a nueva expresión
        _historyIndex = -1;
        clearResult();
        resetExpression();
        return;
    }

    loadHistoryEntry(newIndex);
}

// ════════════════════════════════════════════════════════════════════════════
// loadHistoryEntry() — Carga una entrada del historial en los canvas
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::loadHistoryEntry(int index) {
    if (index < 0 || index >= static_cast<int>(_history.size())) return;

    _historyIndex = index;
    auto& entry = _history[index];

    // Clonar el AST de la expresión (el original se queda en el historial)
    _rootNode = vpam::cloneNode(entry.exprAST.get());
    _rootRow  = static_cast<vpam::NodeRow*>(_rootNode.get());

    // Inicializar cursor al final de la expresión cargada
    _cursor.init(_rootRow);

    // Mostrar en el canvas de expresión
    _mathCanvas.setExpression(_rootRow, nullptr);
    _mathCanvas.resetScroll();
    refreshExpression();
    _mathCanvas.stopCursorBlink();

    // Mostrar el resultado almacenado
    _lastResult  = entry.result;
    _hasResult   = true;
    _showDecimal = false;
    _resultMode  = numos::CalculationFormat::Standard;
    _lastStatus    = entry.status;
    _lastKind      = entry.kind;
    _exactValValid = entry.exactValValid;
    _exactText     = entry.exactText;
    _approxText    = entry.approxText;
    _angleResult = entry.angleResult;
    _resultInDegrees = entry.resultInDegrees;
    _phaseUnit = _resultInDegrees ? numos::CalculationFormat::Degrees : numos::CalculationFormat::Radians;
    _structuredResult = entry.resultAST
                            ? vpam::cloneNode(entry.resultAST.get())
                            : vpam::NodePtr();
    _structuredApproxResult = entry.approximateAST
                            ? vpam::cloneNode(entry.approximateAST.get())
                            : vpam::NodePtr();
    _reusePolicy = entry.reusePolicy;
    _sToDPolicy = entry.sToDPolicy;
    _fallbackReason = entry.fallbackReason;
    showResult();
}

// ════════════════════════════════════════════════════════════════════════════
// alphaKeyToVarName() — Mapea tecla → char de variable en modo ALPHA
//
// Mapeo estilo Casio:
//   NUM_1→A, NUM_2→B, NUM_3→C, NUM_4→D, NUM_5→E, NUM_6→F
//   VAR_X→x, VAR_Y→y, SOLVE(→z)
//   ENTER→Ans (#), FREE_EQ→PreAns ($)
// ════════════════════════════════════════════════════════════════════════════

char CalculationApp::alphaKeyToVarName(KeyCode code) {
    switch (code) {
        case KeyCode::NUM_1:  return 'A';
        case KeyCode::NUM_2:  return 'B';
        case KeyCode::NUM_3:  return 'C';
        case KeyCode::NUM_4:  return 'D';
        case KeyCode::NUM_5:  return 'E';
        case KeyCode::NUM_6:  return 'F';
        case KeyCode::VAR_X:  return 'x';
        case KeyCode::VAR_Y:  return 'y';
        case KeyCode::SOLVE:  return 'z'; // z mapped to SOLVE key position
        default:              return '\0';
    }
}

// ════════════════════════════════════════════════════════════════════════════
// executeStore() — Guarda el Ans actual en la variable indicada
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::executeStore(char varName) {
    numos::CalculationEngine::instance().storeAns(varName);

}

// ════════════════════════════════════════════════════════════════════════════
// generateEduSteps() — Generate step-by-step breakdown of arithmetic
//
// Converts the VPAM AST to a CAS SymExpr, then uses SymSimplify
// in single-pass mode to produce atomic simplification steps.
// Each intermediate expression is logged with a reason string.
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::generateEduSteps() {
    if (!_rootRow) return;

    _eduStepLogger.clear();
    _eduArena.reset();
    _hasEduSteps = false;

    // Convert VPAM AST → CAS SymExpr tree
    cas::ASTFlattener flattener;
    flattener.setVariable('x');
    flattener.setArena(&_eduArena);
    cas::SymExpr* expr = flattener.flattenToExpr(_rootRow);
    if (!expr) return;

    // Log the original expression
    _eduStepLogger.logExpr("Original expression", expr,
                           cas::MethodId::General, "Input");

    // Run simplification passes one at a time (atomic mode).
    // Safety limit prevents infinite loops in edge-case expressions.
    cas::SymExpr* current = expr;
    static constexpr int MAX_EDU_STEPS = 20;

    for (int step = 0; step < MAX_EDU_STEPS; ++step) {
        cas::SymExpr* next = cas::SymSimplify::simplifyPass(current, _eduArena);

        // Fixed point reached — no more simplifications
        if (next == current) break;

        // Determine reason based on the transformation type
        std::string reason;
        if (next->type == cas::SymExprType::Num) {
            reason = "Evaluate result";
        } else if (current->type == cas::SymExprType::Pow ||
                   (current->type == cas::SymExprType::Add && next->type != cas::SymExprType::Add)) {
            reason = "Solve exponent";
        } else if (current->type == cas::SymExprType::Mul ||
                   (current->type == cas::SymExprType::Add &&
                    next->type == cas::SymExprType::Add)) {
            reason = "Simplify terms";
        } else {
            reason = "Simplify";
        }

        // FPU guard: evaluate at x=0 to check for math errors (e.g. 1/0)
        double intermediateVal = next->evaluate(0.0);
        if (!std::isfinite(intermediateVal) && next->type != cas::SymExprType::Num) {
            // Skip logging steps with math errors in intermediate results
            current = next;
            continue;
        }

        _eduStepLogger.logExpr(next->toString(), next,
                               cas::MethodId::General, reason);

        current = next;
    }

    // Log final result
    if (current != expr && _eduStepLogger.count() > 1) {
        _hasEduSteps = true;
    }
}

// ════════════════════════════════════════════════════════════════════════════
// openStepViewer() — Show the step-by-step viewer screen
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::openStepViewer() {
    if (_stepViewerActive || !_hasEduSteps) return;
    _stepViewerActive = true;

    // Hide the main expression/result canvases
    if (_mathCanvas.obj()) lv_obj_add_flag(_mathCanvas.obj(), LV_OBJ_FLAG_HIDDEN);
    if (_resultCanvas.obj()) lv_obj_add_flag(_resultCanvas.obj(), LV_OBJ_FLAG_HIDDEN);
    if (_resultSep) lv_obj_add_flag(_resultSep, LV_OBJ_FLAG_HIDDEN);
    hideTextResult();

    _statusBar.setTitle("Steps");

    // Create scrollable steps container
    int barH = ui::StatusBar::HEIGHT + 1;
    _stepsContainer = lv_obj_create(_screen);
    lv_obj_set_size(_stepsContainer, SCREEN_W, SCREEN_H - barH);
    lv_obj_set_pos(_stepsContainer, 0, barH);
    lv_obj_set_style_bg_color(_stepsContainer, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(_stepsContainer, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(_stepsContainer, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(_stepsContainer, PAD, LV_PART_MAIN);
    lv_obj_set_flex_flow(_stepsContainer, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(_stepsContainer, 4, LV_PART_MAIN);
    lv_obj_add_flag(_stepsContainer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(_stepsContainer, LV_DIR_VER);
    lv_obj_add_style(_stepsContainer, &ui::style_math_primary, LV_PART_MAIN);

    buildStepsDisplay();
    lv_obj_scroll_to_y(_stepsContainer, 0, LV_ANIM_OFF);
    lv_obj_invalidate(_screen);
}

// ════════════════════════════════════════════════════════════════════════════
// closeStepViewer() — Return from step viewer to normal calculator view
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::closeStepViewer() {
    if (!_stepViewerActive) return;
    _stepViewerActive = false;

    // Clean up step renderers
    _stepRenderers.clear();

    // Remove steps container
    if (_stepsContainer) {
        lv_obj_delete(_stepsContainer);
        _stepsContainer = nullptr;
    }

    // Restore main UI
    if (_mathCanvas.obj()) lv_obj_remove_flag(_mathCanvas.obj(), LV_OBJ_FLAG_HIDDEN);
    if (_hasResult) {
        if (_lastStatus == numos::MathEngineStatus::Ok &&
            _lastKind == numos::CalcResultKind::TextFallback) {
            std::string visible = "Giac text fallback (";
            visible += numos::engineFallbackReasonName(_fallbackReason);
            visible += "): ";
            visible += _exactText;
            showTextResult(visible);
        } else {
            if (_resultCanvas.obj()) lv_obj_remove_flag(_resultCanvas.obj(), LV_OBJ_FLAG_HIDDEN);
            if (_resultSep) lv_obj_remove_flag(_resultSep, LV_OBJ_FLAG_HIDDEN);
        }
        _statusBar.setTitle("F2: View Steps");
    } else {
        _statusBar.setTitle("Calculation");
    }

    lv_obj_invalidate(_screen);
}

// ════════════════════════════════════════════════════════════════════════════
// buildStepsDisplay() — Build LVGL widgets for each step
// ════════════════════════════════════════════════════════════════════════════

void CalculationApp::buildStepsDisplay() {
    _stepRenderers.clear();
    if (!_stepsContainer) return;

    lv_obj_clean(_stepsContainer);

    const auto& steps = _eduStepLogger.steps();

    if (steps.empty()) {
        lv_obj_t* lbl = lv_label_create(_stepsContainer);
        lv_label_set_text(lbl, "No steps available.");
        lv_obj_add_style(lbl, &ui::style_math_primary, LV_PART_MAIN);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0x808080), LV_PART_MAIN);
        return;
    }

    static constexpr int CANVAS_W = SCREEN_W - 2 * PAD - 16;

    // Helper: create a MathCanvas from a NodePtr
    auto emitCanvas = [&](vpam::NodePtr node) {
        if (!node) return;
        node = cas::SymExprToAST::ensureRow(std::move(node));
        auto srd = std::make_unique<StepRenderData>();
        srd->nodeData = std::move(node);
        srd->canvas.create(_stepsContainer);
        srd->canvas.setAutoHeightEnabled(false);

        auto* row = static_cast<vpam::NodeRow*>(srd->nodeData.get());
        srd->canvas.setExpression(row, nullptr);
        row->calculateLayout(srd->canvas.normalMetrics());

        int16_t w = row->layout().width + 24;
        int16_t h = vpam::mathObjectHeightPx(row->layout(), srd->canvas.normalMetrics(), 8);
        if (w > CANVAS_W) w = CANVAS_W;
        if (h < 22) h = 22;
        lv_obj_set_size(srd->canvas.obj(), w, h);
        srd->canvas.invalidate();
        _stepRenderers.push_back(std::move(srd));
    };

    // Iterate all steps
    for (size_t i = 0; i < steps.size(); ++i) {
        const auto& step = steps[i];

        // Step number and description
        if (!step.description.empty()) {
            char buf[200];
            snprintf(buf, sizeof(buf), "%d. %s", (int)(i + 1),
                     step.description.c_str());

            lv_obj_t* descLbl = lv_label_create(_stepsContainer);
            lv_label_set_text(descLbl, buf);
            lv_obj_set_width(descLbl, SCREEN_W - 2 * PAD - 8);
            lv_label_set_long_mode(descLbl, LV_LABEL_LONG_WRAP);
            lv_obj_add_style(descLbl, &ui::style_math_primary, LV_PART_MAIN);
            lv_obj_set_style_text_color(descLbl, lv_color_hex(0x1A1A1A), LV_PART_MAIN);
        }

        // Reason label (right side / below)
        if (!step.reason.empty()) {
            lv_obj_t* reasonLbl = lv_label_create(_stepsContainer);
            char reasonBuf[128];
            snprintf(reasonBuf, sizeof(reasonBuf), "  " LV_SYMBOL_RIGHT " %s",
                     step.reason.c_str());
            lv_label_set_text(reasonLbl, reasonBuf);
            lv_obj_add_style(reasonLbl, &ui::style_math_primary, LV_PART_MAIN);
            lv_obj_set_style_text_color(reasonLbl, lv_color_hex(0x4A90D9), LV_PART_MAIN);
        }

        // MathCanvas for CAS expression
        if (step.mathExpr) {
            vpam::NodePtr astNode = cas::SymExprToAST::convert(step.mathExpr);
            emitCanvas(std::move(astNode));
        }
    }

    // Footer hint
    lv_obj_t* hintLbl = lv_label_create(_stepsContainer);
    lv_label_set_text(hintLbl,
                      LV_SYMBOL_UP LV_SYMBOL_DOWN " Scroll    F2/AC: Back");
    lv_obj_add_style(hintLbl, &ui::style_math_primary, LV_PART_MAIN);
    lv_obj_set_style_text_color(hintLbl, lv_color_hex(0x808080), LV_PART_MAIN);
}

#ifdef NATIVE_SIM
// ════════════════════════════════════════════════════════════════════════════
// Calculation emulator probes — read-only accessors for the .numos runner.
// ════════════════════════════════════════════════════════════════════════════

const char* CalculationApp::debugCalcResultKind() const {
    switch (_lastKind) {
        case numos::CalcResultKind::Structured:   return "structured";
        case numos::CalcResultKind::TextFallback: return "text_fallback";
        case numos::CalcResultKind::None:
        default:                                  return "none";
    }
}

const char* CalculationApp::debugCalcStatus() const {
    switch (_lastStatus) {
        case numos::MathEngineStatus::Ok:              return "ok";
        case numos::MathEngineStatus::Undefined:       return "undefined";
        case numos::MathEngineStatus::ParseError:      return "parse_error";
        case numos::MathEngineStatus::EvaluationError: return "evaluation_error";
        case numos::MathEngineStatus::Unsupported:     return "unsupported";
        case numos::MathEngineStatus::OutOfMemory:     return "out_of_memory";
    }
    return "?";
}

const std::string& CalculationApp::debugCalcExactText() const {
    return _exactText;
}
#endif // NATIVE_SIM

#ifdef NATIVE_SIM
namespace {
bool calcInputTreeValid(const vpam::MathNode* node, const vpam::MathNode* parent,
                        const vpam::MathNode* cursor, bool& found, unsigned depth = 0) {
    if (!node || depth > 64 || node->parent() != parent) return false;
    if (node == cursor) found = true;
    if (node->type() == vpam::NodeType::Number &&
        (node->layout().width <= 0 || node->layout().height() <= 0)) return false;
    for (int i = 0; i < node->childCount(); ++i)
        if (!calcInputTreeValid(node->child(i),node,cursor,found,depth+1)) return false;
    return true;
}
}
bool CalculationApp::debugInput(const std::string& expected) const {
    std::string serialized, diagnostic;
    const bool complete = numos::CalculationEngine::serializeForGiac(_rootRow, serialized, diagnostic);
    const auto& cursor = _cursor.cursor();
    if (expected == "dump") {
        std::printf("[CALC-INPUT] cursor=%d slot=%s complete=%d serialized=%s diagnostic=%s\n%s",
                    cursor.index, cursor.row == _rootRow ? "root" : "nested", complete,
                    serialized.c_str(), diagnostic.c_str(), vpam::dumpTree(_rootRow).c_str());
        return true;
    }
    if (expected == "incomplete") return !complete;
    if (expected == "complete") return complete;
    if (expected == "structure") {
        bool found = false;
        return calcInputTreeValid(_rootRow,nullptr,cursor.row,found) && found &&
            cursor.index >= 0 && cursor.index <= cursor.row->childCount();
    }
    if (expected.compare(0,5,"near ") == 0) {
        double value=0, tolerance=0; char extra=0;
        if (std::sscanf(expected.c_str()+5,"%lf %lf %c",&value,&tolerance,&extra)!=2) return false;
        return _hasResult && _lastStatus == numos::MathEngineStatus::Ok &&
            std::isfinite(_lastResult.toDouble()) && tolerance >= 0 &&
            std::fabs(_lastResult.toDouble()-value) <= tolerance;
    }
    if (expected.compare(0,9,"rational ") == 0) {
        long long numerator=0, denominator=0;char extra=0;
        int64_t lhs=0,rhs=0;
        if (std::sscanf(expected.c_str()+9,"%lld %lld %c",&numerator,&denominator,&extra)!=2 ||
            denominator==0 || !_hasResult || !_exactValValid || !_lastResult.ok ||
            _lastResult.approximate || !_lastResult.isRational()) return false;
        // Exact rational comparison, including a sign kept in Giac's denominator.
        return !__builtin_mul_overflow(_lastResult.num, denominator, &lhs) &&
               !__builtin_mul_overflow(numerator, _lastResult.den, &rhs) && lhs==rhs;
    }
    if (expected == "cursor") return cursor.row && cursor.index >= 0 && cursor.index <= cursor.row->childCount();
    if (expected.compare(0,11,"serialized ") == 0) {
        if (complete && serialized == expected.substr(11)) return true;
        std::printf("[CALC-INPUT] actual=%s diagnostic=%s\n",serialized.c_str(),diagnostic.c_str());
    }
    return false;
}
#endif


const std::string& CalculationApp::numericResultText() const {
    numos::DecimalParts scalar;
    return numos::decimalParts(_approxText, scalar) ? _approxText : _exactText;
}

bool CalculationApp::complexResult() const {
    bool imaginary = false;
    unsigned budget = 400;
    return numos::numericComplex(_structuredResult.get(), imaginary, budget) && imaginary;
}

bool CalculationApp::formatAvailable(numos::CalculationFormat format) const {
    using F = numos::CalculationFormat;
    if (!_hasResult || _lastStatus != numos::MathEngineStatus::Ok) return false;
    const bool rational = _exactValValid && _lastResult.isRational();
    switch (format) {
        case F::Standard: return true;
        case F::Decimal: return _structuredApproxResult != nullptr &&
            _sToDPolicy != numos::ResultSToDPolicy::Unavailable;
        case F::Periodic:
        case F::Extended: return rational && _lastResult.den > 1;
        case F::MixedFraction:
            return rational && _lastResult.den > 1 &&
                _lastResult.num != INT64_MIN &&
                (_lastResult.num / _lastResult.den != 0);
        case F::PrimeFactors:
            // Bounded interactive factorization; unlike normal evaluation,
            // factoring a large semiprime can have unpredictable cost.
            return !_angleResult && rational && _lastResult.den == 1 &&
                _lastResult.num >= -1000000000000LL && _lastResult.num <= 1000000000000LL &&
                (_lastResult.num < -1 || _lastResult.num > 1);
        case F::Scientific:
        case F::Engineering: {
            numos::DecimalParts scalar;
            return numos::decimalParts(numericResultText(), scalar);
        }
        case F::Polar:
        case F::Exponential: return complexResult();
        case F::Fixed: {
            numos::DecimalParts scalar;
            return numos::decimalParts(numericResultText(), scalar) && scalar.exponent <= 255;
        }
        case F::Radians: case F::Degrees: case F::Gradians: {
            numos::DecimalParts scalar;
            return (_angleResult && numos::decimalParts(numericResultText(), scalar)) ||
                   (complexResult() && _resultMode == F::Polar);
        }
        default: return false;
    }
}

vpam::NodePtr CalculationApp::formattedResult() {
    using F = numos::CalculationFormat;
    switch (_resultMode) {
        case F::Standard:
            return _structuredResult ? vpam::cloneNode(_structuredResult.get())
                : vpam::MathEvaluator::resultToAST(_lastResult);
        case F::Decimal:
            return _structuredApproxResult ? vpam::cloneNode(_structuredApproxResult.get()) : nullptr;
        case F::Periodic: return vpam::MathEvaluator::resultToPeriodicAST(_lastResult);
        case F::Extended: return vpam::MathEvaluator::resultToExtendedAST(_lastResult, 200);
        case F::Scientific: return numos::powerOfTenFormat(numericResultText(), false);
        case F::Engineering: return numos::powerOfTenFormat(numericResultText(), true, _engineeringShift);
        case F::MixedFraction: return numos::mixedFractionFormat(_lastResult);
        case F::PrimeFactors: {
            auto result = numos::GiacEngine::instance().evaluateStructured(("ifactor(" + _exactText + ")").c_str());
            return result.base.ok() && result.hasTree ? numos::CalculationEngine::resultTreeToAST(result.tree) : nullptr;
        }
        case F::Polar:
        case F::Exponential: {
            auto& engine = numos::GiacEngine::instance();
            auto magnitude = engine.evaluateStructured(("abs(" + _exactText + ")").c_str());
            // WHY: Giac's arg() in DEG can round an exact phase to a double.
            // Evaluate this display transform in a scoped radian context;
            // the engine restores its mode and the input setting is untouched.
            const std::string angle = "arg(" + _exactText + ")";
            std::string radians = angle;
            if (_resultMode == F::Polar) {
                if (_phaseUnit == F::Degrees) radians = "(" + radians + ")*180/pi";
                if (_phaseUnit == F::Gradians) radians = "(" + radians + ")*200/pi";
            }
            auto argument = engine.evaluateStructured(radians.c_str(), true,
                numos::GiacEngine::EvaluationAngle::Radians);
            if (!magnitude.base.ok() || !magnitude.hasTree || !argument.base.ok() || !argument.hasTree) return nullptr;
            auto r = numos::CalculationEngine::resultTreeToAST(magnitude.tree);
            auto theta = numos::CalculationEngine::resultTreeToAST(argument.tree);
            if (!r || !theta) return nullptr;
            if (_resultMode == F::Polar) {
                auto* row = static_cast<vpam::NodeRow*>(r.get());
                row->appendChild(vpam::makeSymbol("\xE2\x80\x89\xE2\x88\xA0\xE2\x80\x89"));
                row->appendChild(std::move(theta));
                row->appendChild(vpam::makeSymbol(_phaseUnit == F::Degrees ? "\xC2\xB0" :
                    _phaseUnit == F::Gradians ? "\xE2\x80\x89gon" : "\xE2\x80\x89rad"));
                return r;
            }
            auto exponent = vpam::makeRow();
            auto* exp = static_cast<vpam::NodeRow*>(exponent.get());
            exp->appendChild(vpam::makeConstant(vpam::ConstKind::Imag));
            exp->appendChild(vpam::makeOperator(vpam::OpKind::Mul));
            exp->appendChild(vpam::makeParen(std::move(theta)));
            auto* row = static_cast<vpam::NodeRow*>(r.get());
            row->appendChild(vpam::makeOperator(vpam::OpKind::Mul));
            row->appendChild(vpam::makePower(vpam::makeConstant(vpam::ConstKind::E), std::move(exponent)));
            return r;
        }
        case F::Fixed: {
            std::string decimal;
            if (!numos::fixedDecimal(numericResultText(), _fixedPlaces, decimal)) return nullptr;
            numos::EngineResultNode number;
            number.kind = numos::EngineNodeKind::Decimal;
            number.text = std::move(decimal);
            return numos::CalculationEngine::resultTreeToAST(number);
        }
        case F::Radians: case F::Degrees: case F::Gradians: {
            std::string expression = "(" + _exactText + ")";
            if (_resultInDegrees) expression += "*pi/180";
            if (_resultMode == F::Degrees) expression += "*180/pi";
            if (_resultMode == F::Gradians) expression += "*200/pi";
            auto angle = numos::GiacEngine::instance().evaluateStructured(expression.c_str(), true);
            if (!angle.base.ok() || !angle.hasTree) return nullptr;
            auto value = numos::CalculationEngine::resultTreeToAST(angle.tree);
            if (value) static_cast<vpam::NodeRow*>(value.get())->appendChild(vpam::makeSymbol(
                _resultMode == F::Degrees ? "\xC2\xB0" :
                _resultMode == F::Gradians ? "\xE2\x80\x89gon" : "\xE2\x80\x89rad"));
            return value;
        }
        default: return nullptr;
    }
}

void CalculationApp::selectFormat(numos::CalculationFormat format) {
    if (!formatAvailable(format)) return;
    if (format == numos::CalculationFormat::Fixed) { openFormatMenu(true); return; }
    if (complexResult() && (format == numos::CalculationFormat::Radians ||
        format == numos::CalculationFormat::Degrees || format == numos::CalculationFormat::Gradians)) {
        _phaseUnit = format;
        _resultMode = numos::CalculationFormat::Polar;
    } else _resultMode = format;
    _engineeringShift = 0;
    _showDecimal = format == numos::CalculationFormat::Decimal;
    showResult();
}

void CalculationApp::engineeringFormat(int direction) {
    if (!formatAvailable(numos::CalculationFormat::Engineering)) return;
    if (_resultMode != numos::CalculationFormat::Engineering) _engineeringShift = 0;
    else _engineeringShift = std::max(-6, std::min(6, _engineeringShift - direction));
    _resultMode = numos::CalculationFormat::Engineering;
    showResult();
}

void CalculationApp::openFormatMenu(bool pickDigits) {
    if (_formatMenu || !_hasResult || _lastStatus != numos::MathEngineStatus::Ok) return;
    _formatPickingDigits = pickDigits;
    _formatChoiceCount = _formatChoice = 0;
    if (pickDigits) { _formatChoiceCount = 10; _formatChoice = _fixedPlaces; }
    else for (unsigned i = 0; i < static_cast<unsigned>(numos::CalculationFormat::Count); ++i) {
        const auto format = static_cast<numos::CalculationFormat>(i);
        if (!formatAvailable(format)) continue;
        if (format == _resultMode) _formatChoice = _formatChoiceCount;
        _formatChoices[_formatChoiceCount++] = format;
    }
    if (!_formatChoiceCount) return;
    const bool spanish = numos::tutor::productLocale == numos::tutor::Locale::Spanish;
    _formatMenu = lv_obj_create(_screen);
    lv_obj_remove_style_all(_formatMenu);
    lv_obj_set_pos(_formatMenu, 0, 24);
    lv_obj_set_size(_formatMenu, 320, 216);
    lv_obj_remove_flag(_formatMenu, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(_formatMenu, lv_color_hex(0x14243B), 0);
    lv_obj_set_style_bg_opa(_formatMenu, LV_OPA_40, 0);

    auto* panel = lv_obj_create(_formatMenu);
    lv_obj_remove_style_all(panel);
    const int visibleRows = std::min<int>(5, _formatChoiceCount);
    const int panelHeight = 82 + visibleRows * 24;
    lv_obj_set_pos(panel, 8, (216 - panelHeight) / 2);
    lv_obj_set_size(panel, 304, panelHeight);
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(panel, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(panel, 10, 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xDCE3EE), 0);
    auto label = [](lv_obj_t* parent, const char* text, int x, int y,
                    const lv_font_t* font, uint32_t color) {
        auto* object = lv_label_create(parent);
        lv_label_set_text(object, text);
        lv_obj_set_pos(object, x, y);
        lv_obj_set_style_text_font(object, font, 0);
        lv_obj_set_style_text_color(object, lv_color_hex(color), 0);
        return object;
    };
    label(panel, spanish ? "MOSTRAR COMO" : "DISPLAY AS", 14, 9,
          &lv_font_montserrat_12, 0x526D91);
    label(panel, pickDigits ? (spanish ? "Decimales fijos" : "Decimal places") :
          (spanish ? "Formato del resultado" : "Result format"), 14, 25,
          &lv_font_montserrat_14, 0x18283F);
    _formatMenuCounter = label(panel, "", 256, 26, &lv_font_montserrat_12, 0x758399);

    for (unsigned i = 0; i < 5; ++i) {
        auto* row = _formatMenuRows[i] = lv_obj_create(panel);
        lv_obj_remove_style_all(row);
        lv_obj_set_pos(row, 8, 51 + i * 24);
        lv_obj_set_size(row, 278, 24);
        lv_obj_set_style_radius(row, 5, 0);
        lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);
        // WHY: reuse the existing Spanish fallback; ASCII geometry, size and
        // kerning stay unchanged, while í/ó and their capitals have real glyphs.
        _formatMenuLabels[i] = label(row, "", 9, 4, ui::tutorFont14(), 0x344155);
        _formatMenuMarks[i] = label(row, "", 254, 5, &lv_font_montserrat_12, 0x526D91);
        lv_obj_add_event_cb(row, [](lv_event_t* event) {
            auto* self = static_cast<CalculationApp*>(lv_event_get_user_data(event));
            auto* target = lv_event_get_target_obj(event);
            for (unsigned slot = 0; slot < 5; ++slot) {
                if (self->_formatMenuRows[slot] != target) continue;
                const unsigned first = self->_formatChoice < 5 ? 0 : self->_formatChoice - 4;
                if (first + slot >= self->_formatChoiceCount) return;
                self->_formatChoice = first + slot;
                self->applyFormatChoice(); return;
            }
        }, LV_EVENT_CLICKED, this);
    }
    auto* track = lv_obj_create(panel);
    lv_obj_remove_style_all(track);
    lv_obj_set_pos(track, 292, 55); lv_obj_set_size(track, 2, 112);
    lv_obj_set_style_bg_color(track, lv_color_hex(0xE6ECF4), 0);
    lv_obj_set_style_bg_opa(track, LV_OPA_COVER, 0);
    if (_formatChoiceCount <= 5) lv_obj_add_flag(track, LV_OBJ_FLAG_HIDDEN);
    _formatMenuThumb = lv_obj_create(track);
    lv_obj_remove_style_all(_formatMenuThumb);
    lv_obj_set_style_bg_color(_formatMenuThumb, lv_color_hex(0x9BACBF), 0);
    lv_obj_set_style_bg_opa(_formatMenuThumb, LV_OPA_COVER, 0);
    lv_obj_set_width(_formatMenuThumb, 2);
    const int footerY = panelHeight - 20;
    label(panel, LV_SYMBOL_UP LV_SYMBOL_DOWN, 15, footerY, &lv_font_montserrat_12, 0x64748B);
    label(panel, spanish ? "Elegir" : "Choose", 41, footerY, &lv_font_montserrat_12, 0x64748B);
    label(panel, spanish ? "EXE  Aplicar" : "EXE  Apply", 113, footerY, &lv_font_montserrat_12, 0x344155);
    label(panel, "BACK", 254, footerY, &lv_font_montserrat_12, 0x64748B);
    updateFormatMenu();
}

void CalculationApp::updateFormatMenu() {
    if (!_formatMenu) return;
    const bool spanish = numos::tutor::productLocale == numos::tutor::Locale::Spanish;
    lv_label_set_text_fmt(_formatMenuCounter, "%u / %u", _formatChoice + 1, _formatChoiceCount);
    const unsigned first = _formatChoice < 5 ? 0 : _formatChoice - 4;
    for (unsigned slot = 0; slot < 5; ++slot) {
        auto* row = _formatMenuRows[slot];
        const unsigned i = first + slot;
        if (i >= _formatChoiceCount) { lv_obj_add_flag(row, LV_OBJ_FLAG_HIDDEN); continue; }
        lv_obj_remove_flag(row, LV_OBJ_FLAG_HIDDEN);
        const bool focused = i == _formatChoice;
        lv_obj_set_style_bg_color(row, lv_color_hex(0x245DB2), 0);
        lv_obj_set_style_bg_opa(row, focused ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
        if (_formatPickingDigits) {
            const char* precisionLabel = spanish
                ? (i == 1 ? "%u decimal" : "%u decimales")
                : (i == 1 ? "%u decimal place" : "%u decimal places");
            lv_label_set_text_fmt(_formatMenuLabels[slot], precisionLabel, i);
        } else {
            const char* name = numos::calculationFormatLabel(_formatChoices[i], spanish);
            if (_formatChoices[i] == numos::CalculationFormat::Standard && complexResult())
                name = spanish ? "Cartesiana (a + bi)" : "Cartesian (a + bi)";
            lv_label_set_text(_formatMenuLabels[slot], name);
        }
        lv_obj_set_style_text_color(_formatMenuLabels[slot], lv_color_hex(focused ? 0xFFFFFF : 0x344155), 0);
        lv_label_set_text(_formatMenuMarks[slot], (_formatPickingDigits ? i == _fixedPlaces : _formatChoices[i] == _resultMode) ? LV_SYMBOL_OK : "");
        lv_obj_set_style_text_color(_formatMenuMarks[slot], lv_color_hex(focused ? 0xFFFFFF : 0x245DB2), 0);
    }
    const unsigned visible = std::min<unsigned>(5, _formatChoiceCount);
    const unsigned height = 112 * visible / _formatChoiceCount;
    lv_obj_set_height(_formatMenuThumb, height);
    lv_obj_set_y(_formatMenuThumb, _formatChoiceCount > 5 ?
                 (112-height)*first/(_formatChoiceCount-5) : 0);
}

void CalculationApp::closeFormatMenu() {
    if (_formatMenu) lv_obj_delete(_formatMenu);
    _formatMenu = _formatMenuCounter = _formatMenuThumb = nullptr;
    for (unsigned i = 0; i < 5; ++i)
        _formatMenuRows[i] = _formatMenuLabels[i] = _formatMenuMarks[i] = nullptr;
}

void CalculationApp::applyFormatChoice() {
    if (_formatPickingDigits) {
        _fixedPlaces = _formatChoice;
        closeFormatMenu();
        _resultMode = numos::CalculationFormat::Fixed;
        showResult();
    } else {
        const auto chosen = _formatChoices[_formatChoice];
        closeFormatMenu();
        selectFormat(chosen);
    }
}
