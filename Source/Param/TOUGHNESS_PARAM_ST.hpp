#pragma once

#include <cstddef>
#include <elden-x/paramdef/TOUGHNESS_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_TOUGHNESS_PARAM_ST_FIELDS(X) \\
    X(correctionRate, offsetof(::er::paramdef::toughness_param_st, correctionRate)) \\
    X(minToughness, offsetof(::er::paramdef::toughness_param_st, minToughness)) \\
    X(isNonEffectiveCorrectionForMin, offsetof(::er::paramdef::toughness_param_st, isNonEffectiveCorrectionForMin)) \\
    X(pad2, offsetof(::er::paramdef::toughness_param_st, pad2)) \\
    X(spEffectId, offsetof(::er::paramdef::toughness_param_st, spEffectId)) \\
    X(proCorrectionRate, offsetof(::er::paramdef::toughness_param_st, proCorrectionRate)) \\
    X(pad1, offsetof(::er::paramdef::toughness_param_st, pad1))

#define ERD_OFFSET_TOUGHNESS_PARAM_ST_correctionRate offsetof(::er::paramdef::toughness_param_st, correctionRate)
#define ERD_OFFSET_TOUGHNESS_PARAM_ST_minToughness offsetof(::er::paramdef::toughness_param_st, minToughness)
#define ERD_OFFSET_TOUGHNESS_PARAM_ST_isNonEffectiveCorrectionForMin offsetof(::er::paramdef::toughness_param_st, isNonEffectiveCorrectionForMin)
#define ERD_OFFSET_TOUGHNESS_PARAM_ST_pad2 offsetof(::er::paramdef::toughness_param_st, pad2)
#define ERD_OFFSET_TOUGHNESS_PARAM_ST_spEffectId offsetof(::er::paramdef::toughness_param_st, spEffectId)
#define ERD_OFFSET_TOUGHNESS_PARAM_ST_proCorrectionRate offsetof(::er::paramdef::toughness_param_st, proCorrectionRate)
#define ERD_OFFSET_TOUGHNESS_PARAM_ST_pad1 offsetof(::er::paramdef::toughness_param_st, pad1)
