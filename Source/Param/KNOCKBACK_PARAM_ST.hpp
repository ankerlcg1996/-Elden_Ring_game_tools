#pragma once

#include <cstddef>
#include <elden-x/paramdef/KNOCKBACK_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_KNOCKBACK_PARAM_ST_FIELDS(X) \\
    X(damage_Min_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_Min_ContTime)) \\
    X(damage_S_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_S_ContTime)) \\
    X(damage_M_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_M_ContTime)) \\
    X(damage_L_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_L_ContTime)) \\
    X(damage_BlowS_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_BlowS_ContTime)) \\
    X(damage_BlowM_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_BlowM_ContTime)) \\
    X(damage_Strike_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_Strike_ContTime)) \\
    X(damage_Uppercut_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_Uppercut_ContTime)) \\
    X(damage_Push_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_Push_ContTime)) \\
    X(damage_Breath_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_Breath_ContTime)) \\
    X(damage_HeadShot_ContTime, offsetof(::er::paramdef::knockback_param_st, damage_HeadShot_ContTime)) \\
    X(guard_S_ContTime, offsetof(::er::paramdef::knockback_param_st, guard_S_ContTime)) \\
    X(guard_L_ContTime, offsetof(::er::paramdef::knockback_param_st, guard_L_ContTime)) \\
    X(guard_LL_ContTime, offsetof(::er::paramdef::knockback_param_st, guard_LL_ContTime)) \\
    X(guardBrake_ContTime, offsetof(::er::paramdef::knockback_param_st, guardBrake_ContTime)) \\
    X(damage_Min_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_Min_DecTime)) \\
    X(damage_S_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_S_DecTime)) \\
    X(damage_M_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_M_DecTime)) \\
    X(damage_L_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_L_DecTime)) \\
    X(damage_BlowS_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_BlowS_DecTime)) \\
    X(damage_BlowM_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_BlowM_DecTime)) \\
    X(damage_Strike_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_Strike_DecTime)) \\
    X(damage_Uppercut_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_Uppercut_DecTime)) \\
    X(damage_Push_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_Push_DecTime)) \\
    X(damage_Breath_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_Breath_DecTime)) \\
    X(damage_HeadShot_DecTime, offsetof(::er::paramdef::knockback_param_st, damage_HeadShot_DecTime)) \\
    X(guard_S_DecTime, offsetof(::er::paramdef::knockback_param_st, guard_S_DecTime)) \\
    X(guard_L_DecTime, offsetof(::er::paramdef::knockback_param_st, guard_L_DecTime)) \\
    X(guard_LL_DecTime, offsetof(::er::paramdef::knockback_param_st, guard_LL_DecTime)) \\
    X(guardBrake_DecTime, offsetof(::er::paramdef::knockback_param_st, guardBrake_DecTime)) \\
    X(pad, offsetof(::er::paramdef::knockback_param_st, pad))

#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Min_ContTime offsetof(::er::paramdef::knockback_param_st, damage_Min_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_S_ContTime offsetof(::er::paramdef::knockback_param_st, damage_S_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_M_ContTime offsetof(::er::paramdef::knockback_param_st, damage_M_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_L_ContTime offsetof(::er::paramdef::knockback_param_st, damage_L_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_BlowS_ContTime offsetof(::er::paramdef::knockback_param_st, damage_BlowS_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_BlowM_ContTime offsetof(::er::paramdef::knockback_param_st, damage_BlowM_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Strike_ContTime offsetof(::er::paramdef::knockback_param_st, damage_Strike_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Uppercut_ContTime offsetof(::er::paramdef::knockback_param_st, damage_Uppercut_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Push_ContTime offsetof(::er::paramdef::knockback_param_st, damage_Push_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Breath_ContTime offsetof(::er::paramdef::knockback_param_st, damage_Breath_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_HeadShot_ContTime offsetof(::er::paramdef::knockback_param_st, damage_HeadShot_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_guard_S_ContTime offsetof(::er::paramdef::knockback_param_st, guard_S_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_guard_L_ContTime offsetof(::er::paramdef::knockback_param_st, guard_L_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_guard_LL_ContTime offsetof(::er::paramdef::knockback_param_st, guard_LL_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_guardBrake_ContTime offsetof(::er::paramdef::knockback_param_st, guardBrake_ContTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Min_DecTime offsetof(::er::paramdef::knockback_param_st, damage_Min_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_S_DecTime offsetof(::er::paramdef::knockback_param_st, damage_S_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_M_DecTime offsetof(::er::paramdef::knockback_param_st, damage_M_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_L_DecTime offsetof(::er::paramdef::knockback_param_st, damage_L_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_BlowS_DecTime offsetof(::er::paramdef::knockback_param_st, damage_BlowS_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_BlowM_DecTime offsetof(::er::paramdef::knockback_param_st, damage_BlowM_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Strike_DecTime offsetof(::er::paramdef::knockback_param_st, damage_Strike_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Uppercut_DecTime offsetof(::er::paramdef::knockback_param_st, damage_Uppercut_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Push_DecTime offsetof(::er::paramdef::knockback_param_st, damage_Push_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_Breath_DecTime offsetof(::er::paramdef::knockback_param_st, damage_Breath_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_damage_HeadShot_DecTime offsetof(::er::paramdef::knockback_param_st, damage_HeadShot_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_guard_S_DecTime offsetof(::er::paramdef::knockback_param_st, guard_S_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_guard_L_DecTime offsetof(::er::paramdef::knockback_param_st, guard_L_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_guard_LL_DecTime offsetof(::er::paramdef::knockback_param_st, guard_LL_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_guardBrake_DecTime offsetof(::er::paramdef::knockback_param_st, guardBrake_DecTime)
#define ERD_OFFSET_KNOCKBACK_PARAM_ST_pad offsetof(::er::paramdef::knockback_param_st, pad)
