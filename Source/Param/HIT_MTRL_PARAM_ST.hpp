#pragma once

#include <cstddef>
#include <elden-x/paramdef/HIT_MTRL_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_HIT_MTRL_PARAM_ST_FIELDS(X) \\
    X(aiVolumeRate, offsetof(::er::paramdef::hit_mtrl_param_st, aiVolumeRate)) \\
    X(spEffectIdOnHit0, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0)) \\
    X(spEffectIdOnHit1, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1)) \\
    X(hardnessType, offsetof(::er::paramdef::hit_mtrl_param_st, hardnessType)) \\
    X(pad2, offsetof(::er::paramdef::hit_mtrl_param_st, pad2)) \\
    X(spEffectIdOnHit0_ClearCount_2, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_2)) \\
    X(spEffectIdOnHit0_ClearCount_3, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_3)) \\
    X(spEffectIdOnHit0_ClearCount_4, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_4)) \\
    X(spEffectIdOnHit0_ClearCount_5, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_5)) \\
    X(spEffectIdOnHit0_ClearCount_6, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_6)) \\
    X(spEffectIdOnHit0_ClearCount_7, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_7)) \\
    X(spEffectIdOnHit0_ClearCount_8, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_8)) \\
    X(spEffectIdOnHit1_ClearCount_2, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_2)) \\
    X(spEffectIdOnHit1_ClearCount_3, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_3)) \\
    X(spEffectIdOnHit1_ClearCount_4, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_4)) \\
    X(spEffectIdOnHit1_ClearCount_5, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_5)) \\
    X(spEffectIdOnHit1_ClearCount_6, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_6)) \\
    X(spEffectIdOnHit1_ClearCount_7, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_7)) \\
    X(spEffectIdOnHit1_ClearCount_8, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_8)) \\
    X(replaceMateiralId_Rain, offsetof(::er::paramdef::hit_mtrl_param_st, replaceMateiralId_Rain)) \\
    X(pad4, offsetof(::er::paramdef::hit_mtrl_param_st, pad4)) \\
    X(spEffectId_forWet00, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet00)) \\
    X(spEffectId_forWet01, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet01)) \\
    X(spEffectId_forWet02, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet02)) \\
    X(spEffectId_forWet03, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet03)) \\
    X(spEffectId_forWet04, offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet04))

#define ERD_OFFSET_HIT_MTRL_PARAM_ST_aiVolumeRate offsetof(::er::paramdef::hit_mtrl_param_st, aiVolumeRate)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit0 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit1 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_hardnessType offsetof(::er::paramdef::hit_mtrl_param_st, hardnessType)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_pad2 offsetof(::er::paramdef::hit_mtrl_param_st, pad2)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit0_ClearCount_2 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_2)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit0_ClearCount_3 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_3)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit0_ClearCount_4 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_4)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit0_ClearCount_5 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_5)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit0_ClearCount_6 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_6)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit0_ClearCount_7 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_7)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit0_ClearCount_8 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit0_ClearCount_8)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit1_ClearCount_2 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_2)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit1_ClearCount_3 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_3)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit1_ClearCount_4 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_4)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit1_ClearCount_5 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_5)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit1_ClearCount_6 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_6)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit1_ClearCount_7 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_7)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectIdOnHit1_ClearCount_8 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectIdOnHit1_ClearCount_8)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_replaceMateiralId_Rain offsetof(::er::paramdef::hit_mtrl_param_st, replaceMateiralId_Rain)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_pad4 offsetof(::er::paramdef::hit_mtrl_param_st, pad4)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectId_forWet00 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet00)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectId_forWet01 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet01)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectId_forWet02 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet02)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectId_forWet03 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet03)
#define ERD_OFFSET_HIT_MTRL_PARAM_ST_spEffectId_forWet04 offsetof(::er::paramdef::hit_mtrl_param_st, spEffectId_forWet04)
