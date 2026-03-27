#pragma once

#include <cstddef>
#include <elden-x/paramdef/PHANTOM_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_PHANTOM_PARAM_ST_FIELDS(X) \\
    X(edgeColorA, offsetof(::er::paramdef::phantom_param_st, edgeColorA)) \\
    X(frontColorA, offsetof(::er::paramdef::phantom_param_st, frontColorA)) \\
    X(diffMulColorA, offsetof(::er::paramdef::phantom_param_st, diffMulColorA)) \\
    X(specMulColorA, offsetof(::er::paramdef::phantom_param_st, specMulColorA)) \\
    X(lightColorA, offsetof(::er::paramdef::phantom_param_st, lightColorA)) \\
    X(edgeColorR, offsetof(::er::paramdef::phantom_param_st, edgeColorR)) \\
    X(edgeColorG, offsetof(::er::paramdef::phantom_param_st, edgeColorG)) \\
    X(edgeColorB, offsetof(::er::paramdef::phantom_param_st, edgeColorB)) \\
    X(frontColorR, offsetof(::er::paramdef::phantom_param_st, frontColorR)) \\
    X(frontColorG, offsetof(::er::paramdef::phantom_param_st, frontColorG)) \\
    X(frontColorB, offsetof(::er::paramdef::phantom_param_st, frontColorB)) \\
    X(diffMulColorR, offsetof(::er::paramdef::phantom_param_st, diffMulColorR)) \\
    X(diffMulColorG, offsetof(::er::paramdef::phantom_param_st, diffMulColorG)) \\
    X(diffMulColorB, offsetof(::er::paramdef::phantom_param_st, diffMulColorB)) \\
    X(specMulColorR, offsetof(::er::paramdef::phantom_param_st, specMulColorR)) \\
    X(specMulColorG, offsetof(::er::paramdef::phantom_param_st, specMulColorG)) \\
    X(specMulColorB, offsetof(::er::paramdef::phantom_param_st, specMulColorB)) \\
    X(lightColorR, offsetof(::er::paramdef::phantom_param_st, lightColorR)) \\
    X(lightColorG, offsetof(::er::paramdef::phantom_param_st, lightColorG)) \\
    X(lightColorB, offsetof(::er::paramdef::phantom_param_st, lightColorB)) \\
    X(reserve, offsetof(::er::paramdef::phantom_param_st, reserve)) \\
    X(alpha, offsetof(::er::paramdef::phantom_param_st, alpha)) \\
    X(blendRate, offsetof(::er::paramdef::phantom_param_st, blendRate)) \\
    X(blendType, offsetof(::er::paramdef::phantom_param_st, blendType)) \\
    X(isEdgeSubtract, offsetof(::er::paramdef::phantom_param_st, isEdgeSubtract)) \\
    X(isFrontSubtract, offsetof(::er::paramdef::phantom_param_st, isFrontSubtract)) \\
    X(isNo2Pass, offsetof(::er::paramdef::phantom_param_st, isNo2Pass)) \\
    X(edgePower, offsetof(::er::paramdef::phantom_param_st, edgePower)) \\
    X(glowScale, offsetof(::er::paramdef::phantom_param_st, glowScale))

#define ERD_OFFSET_PHANTOM_PARAM_ST_edgeColorA offsetof(::er::paramdef::phantom_param_st, edgeColorA)
#define ERD_OFFSET_PHANTOM_PARAM_ST_frontColorA offsetof(::er::paramdef::phantom_param_st, frontColorA)
#define ERD_OFFSET_PHANTOM_PARAM_ST_diffMulColorA offsetof(::er::paramdef::phantom_param_st, diffMulColorA)
#define ERD_OFFSET_PHANTOM_PARAM_ST_specMulColorA offsetof(::er::paramdef::phantom_param_st, specMulColorA)
#define ERD_OFFSET_PHANTOM_PARAM_ST_lightColorA offsetof(::er::paramdef::phantom_param_st, lightColorA)
#define ERD_OFFSET_PHANTOM_PARAM_ST_edgeColorR offsetof(::er::paramdef::phantom_param_st, edgeColorR)
#define ERD_OFFSET_PHANTOM_PARAM_ST_edgeColorG offsetof(::er::paramdef::phantom_param_st, edgeColorG)
#define ERD_OFFSET_PHANTOM_PARAM_ST_edgeColorB offsetof(::er::paramdef::phantom_param_st, edgeColorB)
#define ERD_OFFSET_PHANTOM_PARAM_ST_frontColorR offsetof(::er::paramdef::phantom_param_st, frontColorR)
#define ERD_OFFSET_PHANTOM_PARAM_ST_frontColorG offsetof(::er::paramdef::phantom_param_st, frontColorG)
#define ERD_OFFSET_PHANTOM_PARAM_ST_frontColorB offsetof(::er::paramdef::phantom_param_st, frontColorB)
#define ERD_OFFSET_PHANTOM_PARAM_ST_diffMulColorR offsetof(::er::paramdef::phantom_param_st, diffMulColorR)
#define ERD_OFFSET_PHANTOM_PARAM_ST_diffMulColorG offsetof(::er::paramdef::phantom_param_st, diffMulColorG)
#define ERD_OFFSET_PHANTOM_PARAM_ST_diffMulColorB offsetof(::er::paramdef::phantom_param_st, diffMulColorB)
#define ERD_OFFSET_PHANTOM_PARAM_ST_specMulColorR offsetof(::er::paramdef::phantom_param_st, specMulColorR)
#define ERD_OFFSET_PHANTOM_PARAM_ST_specMulColorG offsetof(::er::paramdef::phantom_param_st, specMulColorG)
#define ERD_OFFSET_PHANTOM_PARAM_ST_specMulColorB offsetof(::er::paramdef::phantom_param_st, specMulColorB)
#define ERD_OFFSET_PHANTOM_PARAM_ST_lightColorR offsetof(::er::paramdef::phantom_param_st, lightColorR)
#define ERD_OFFSET_PHANTOM_PARAM_ST_lightColorG offsetof(::er::paramdef::phantom_param_st, lightColorG)
#define ERD_OFFSET_PHANTOM_PARAM_ST_lightColorB offsetof(::er::paramdef::phantom_param_st, lightColorB)
#define ERD_OFFSET_PHANTOM_PARAM_ST_reserve offsetof(::er::paramdef::phantom_param_st, reserve)
#define ERD_OFFSET_PHANTOM_PARAM_ST_alpha offsetof(::er::paramdef::phantom_param_st, alpha)
#define ERD_OFFSET_PHANTOM_PARAM_ST_blendRate offsetof(::er::paramdef::phantom_param_st, blendRate)
#define ERD_OFFSET_PHANTOM_PARAM_ST_blendType offsetof(::er::paramdef::phantom_param_st, blendType)
#define ERD_OFFSET_PHANTOM_PARAM_ST_isEdgeSubtract offsetof(::er::paramdef::phantom_param_st, isEdgeSubtract)
#define ERD_OFFSET_PHANTOM_PARAM_ST_isFrontSubtract offsetof(::er::paramdef::phantom_param_st, isFrontSubtract)
#define ERD_OFFSET_PHANTOM_PARAM_ST_isNo2Pass offsetof(::er::paramdef::phantom_param_st, isNo2Pass)
#define ERD_OFFSET_PHANTOM_PARAM_ST_edgePower offsetof(::er::paramdef::phantom_param_st, edgePower)
#define ERD_OFFSET_PHANTOM_PARAM_ST_glowScale offsetof(::er::paramdef::phantom_param_st, glowScale)
