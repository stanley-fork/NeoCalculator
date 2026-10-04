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
 * CalculationEngine.h — GIAC-B01: CalculationApp's Giac evaluation adapter.
 *
 * The ONE boundary between the Calculation UI and numos::GiacEngine:
 *   VPAM MathAST  --serialize-->  Giac text  --GiacEngine-->  structured
 *   result tree   --bridge-->     NumOS MathAST (Natural Display) and/or an
 *                                 exact ExactVal mirror.
 *
 * Contract:
 *  - Giac is the mathematical authority. There is NO fallback re-evaluation
 *    with MathEvaluator/OmniSolver/custom CAS on any path in this adapter.
 *  - Calculation ENTER evaluates exact decimal literals and applies Giac's
 *    simplification when it does not expand the answer. Symbolic denominators
 *    keep their domain restrictions; other apps retain their own policies.
 *  - Serialization walks the authored VPAM tree (never display text) with
 *    explicit parentheses; unsupported node shapes are rejected with a typed
 *    error (ParseError + diagnostic), never emitted ambiguously.
 *  - Presentation tiers:
 *      1. exactValValid — a lossless ExactVal mirror supports legacy storage,
 *         rational presentation and numeric probes alongside the typed AST.
 *      2. exactAST       — structured MathAST built from the engine result
 *         tree (big integers, exact complex values, collections, matrices,
 *         intervals, piecewise, infinities, undefined and unevaluated calls).
 *         Exact structure is never rebuilt from doubles or printed output.
 *      3. TextFallback   — Giac's own printed exactText shown as plain text
 *         only for bounded or genuinely unsupported typed shapes, carrying a
 *         stable fallback reason.
 *  - Variables: NumOS VariableManager stays the persistent source of truth.
 *    A-F, Ans and PreAns are mirrored into the Giac context (exactly) before
 *    every evaluation; x, y, z deliberately remain FREE symbols so symbolic
 *    results and future solve/diff/factor keep their Giac meaning. Ans/PreAns
 *    map to the reserved identifiers numos_Ans / numos_PreAns to avoid Giac
 *    built-in collisions. Values whose exact form exceeds ExactVal keep a
 *    session-exact Giac text (validated against the current VariableManager
 *    snapshot and revision so the two stores cannot silently drift). Complex
 *    and symbolic Ans/A-F are session-only; the persistent format is unchanged.
 *
 * No LVGL, no Arduino: host-harness testable (tests/host).
 */

#pragma once

#include <string>

#include "MathAST.h"
#include "GeneratedMathNotation.h"
#include "ExactVal.h"
#include "giac/GiacEngine.h"

namespace numos {

enum class CalcResultKind : uint8_t {
    None,          // no successful result (error status)
    Structured,    // ExactVal tier or structured MathAST tier
    TextFallback   // presentation falls back to Giac's printed text
};

enum class ResultReusePolicy : uint8_t {
    FullyRoundTrippable,
    DisplayOnly,
    NonReusable
};

enum class ResultSToDPolicy : uint8_t {
    Scalar,
    ElementWise,
    Unavailable
};

// A display intent with provenance, never a replacement for the canonical value.
enum class ResultPresentation : uint8_t { Canonical, BothInfinities };

struct CalculationEvaluation {
    MathEngineStatus status = MathEngineStatus::Unsupported;
    CalcResultKind kind = CalcResultKind::None;

    bool exactValValid = false;      // lossless legacy scalar mirror usable
    vpam::ExactVal exactVal;         // valid only when exactValValid

    vpam::NodePtr exactAST;          // canonical typed display, also for scalars
                                     // (null on TextFallback)
    vpam::NodePtr approximateAST;    // typed evalf tree; never parsed text

    std::string serialized;          // what was sent to Giac (diagnostics)
    std::string exactText;           // Giac exact print (always set when Ok)
    std::string approximateText;     // numeric companion ("" when none)
    std::string diagnostic;          // engine/serializer messages
    EngineFallbackReason fallbackReason = EngineFallbackReason::None;
    ResultReusePolicy reusePolicy = ResultReusePolicy::NonReusable;
    ResultSToDPolicy sToDPolicy = ResultSToDPolicy::Unavailable;
    ResultPresentation presentation = ResultPresentation::Canonical;

    bool ok() const { return status == MathEngineStatus::Ok; }
    bool displayable() const {
        return ok() || (status == MathEngineStatus::Undefined && exactAST);
    }
};

class CalculationEngine {
public:
    static CalculationEngine& instance();

    /// Full ENTER pipeline: serialize -> mirror variables -> Giac evaluate
    /// -> presentation tiers. Never re-evaluates through the legacy engine.
    CalculationEvaluation evaluate(const vpam::MathNode* root);

    /// Call AFTER VariableManager::updateAns(): rotates the session-exact
    /// Ans text (Ans -> PreAns) so exact chains (sqrt(2), 2^100) survive
    /// the int64 ExactVal store.
    void noteAnsRotated(const std::string& exactGiacText, bool mirrorRotated = true,
                       ResultPresentation presentation = ResultPresentation::Canonical);

    /// Call AFTER an STO wrote VariableManager: keeps the stored variable's
    /// session-exact text coherent (STO copies Ans).
    void noteVariableStored(char varName);
    /// Store the canonical Ans. Non-scalar values are session-only: this does
    /// not encode a stale scalar into the persistent ExactVal file.
    bool storeAns(char varName);

    // ── Pure helpers, exposed for the host harness ───────────────────────
    /// VPAM tree -> Giac input text. False + error on unsupported shapes.
    static bool serializeForGiac(const vpam::MathNode* root, std::string& out,
                                 std::string& error,
                                 bool exactDecimals = false);
    /// Exact Giac text for an ExactVal ((num/den)*outer*sqrt(inner)*pi^k*e^m).
    static std::string exactValToGiacText(const vpam::ExactVal& v);
    /// Engine result tree -> exact ExactVal mirror (strict: false unless the
    /// conversion is lossless).
    static bool resultTreeToExactVal(const EngineResultNode& tree,
                                     vpam::ExactVal& out);
    /// Engine result tree -> MathAST row for Natural Display (strict).
    static vpam::NodePtr resultTreeToAST(const EngineResultNode& tree,
                                       ProductNotation notation = ProductNotation::Explicit);
    static ResultReusePolicy reusePolicyForResult(
        const EngineResultNode& tree);
    static ResultSToDPolicy sToDPolicyForResult(
        const EngineResultNode& tree, bool hasApproximateTree);

private:
    CalculationEngine() = default;

    struct SessionExact {
        bool valid = false;
        bool sessionOnly = false;
        ResultPresentation presentation = ResultPresentation::Canonical;
        uint32_t revision = 0;
        vpam::ExactVal snapshot;   // VariableManager value the text mirrors
        std::string text;          // exact Giac expression for that value
    };

    static int sessionIndex(char varName);           // A-F,'#','$' -> 0..7
    static const char* giacNameFor(char varName);    // 'A'..'F', numos_Ans...
    void syncVariablesToGiac(std::string& diagnostic);
    ResultPresentation inputPresentation(const vpam::MathNode* root) const;

    SessionExact _session[8];
};

} // namespace numos
