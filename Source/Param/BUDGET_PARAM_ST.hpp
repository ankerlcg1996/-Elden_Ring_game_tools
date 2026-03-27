#pragma once

#include <cstddef>
#include <elden-x/paramdef/BUDGET_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_BUDGET_PARAM_ST_FIELDS(X) \\
    X(vram_all, offsetof(::er::paramdef::budget_param_st, vram_all)) \\
    X(vram_mapobj_tex, offsetof(::er::paramdef::budget_param_st, vram_mapobj_tex)) \\
    X(vram_mapobj_mdl, offsetof(::er::paramdef::budget_param_st, vram_mapobj_mdl)) \\
    X(vram_map, offsetof(::er::paramdef::budget_param_st, vram_map)) \\
    X(vram_chr, offsetof(::er::paramdef::budget_param_st, vram_chr)) \\
    X(vram_parts, offsetof(::er::paramdef::budget_param_st, vram_parts)) \\
    X(vram_sfx, offsetof(::er::paramdef::budget_param_st, vram_sfx)) \\
    X(vram_chr_tex, offsetof(::er::paramdef::budget_param_st, vram_chr_tex)) \\
    X(vram_chr_mdl, offsetof(::er::paramdef::budget_param_st, vram_chr_mdl)) \\
    X(vram_parts_tex, offsetof(::er::paramdef::budget_param_st, vram_parts_tex)) \\
    X(vram_parts_mdl, offsetof(::er::paramdef::budget_param_st, vram_parts_mdl)) \\
    X(vram_sfx_tex, offsetof(::er::paramdef::budget_param_st, vram_sfx_tex)) \\
    X(vram_sfx_mdl, offsetof(::er::paramdef::budget_param_st, vram_sfx_mdl)) \\
    X(vram_gi, offsetof(::er::paramdef::budget_param_st, vram_gi)) \\
    X(vram_menu_tex, offsetof(::er::paramdef::budget_param_st, vram_menu_tex)) \\
    X(vram_decal_rt, offsetof(::er::paramdef::budget_param_st, vram_decal_rt)) \\
    X(vram_decal, offsetof(::er::paramdef::budget_param_st, vram_decal)) \\
    X(reserve_0, offsetof(::er::paramdef::budget_param_st, reserve_0)) \\
    X(vram_other_tex, offsetof(::er::paramdef::budget_param_st, vram_other_tex)) \\
    X(vram_other_mdl, offsetof(::er::paramdef::budget_param_st, vram_other_mdl)) \\
    X(havok_anim, offsetof(::er::paramdef::budget_param_st, havok_anim)) \\
    X(havok_ins, offsetof(::er::paramdef::budget_param_st, havok_ins)) \\
    X(havok_hit, offsetof(::er::paramdef::budget_param_st, havok_hit)) \\
    X(vram_other, offsetof(::er::paramdef::budget_param_st, vram_other)) \\
    X(vram_detail_all, offsetof(::er::paramdef::budget_param_st, vram_detail_all)) \\
    X(vram_chr_and_parts, offsetof(::er::paramdef::budget_param_st, vram_chr_and_parts)) \\
    X(havok_navimesh, offsetof(::er::paramdef::budget_param_st, havok_navimesh)) \\
    X(reserve_1, offsetof(::er::paramdef::budget_param_st, reserve_1))

#define ERD_OFFSET_BUDGET_PARAM_ST_vram_all offsetof(::er::paramdef::budget_param_st, vram_all)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_mapobj_tex offsetof(::er::paramdef::budget_param_st, vram_mapobj_tex)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_mapobj_mdl offsetof(::er::paramdef::budget_param_st, vram_mapobj_mdl)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_map offsetof(::er::paramdef::budget_param_st, vram_map)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_chr offsetof(::er::paramdef::budget_param_st, vram_chr)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_parts offsetof(::er::paramdef::budget_param_st, vram_parts)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_sfx offsetof(::er::paramdef::budget_param_st, vram_sfx)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_chr_tex offsetof(::er::paramdef::budget_param_st, vram_chr_tex)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_chr_mdl offsetof(::er::paramdef::budget_param_st, vram_chr_mdl)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_parts_tex offsetof(::er::paramdef::budget_param_st, vram_parts_tex)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_parts_mdl offsetof(::er::paramdef::budget_param_st, vram_parts_mdl)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_sfx_tex offsetof(::er::paramdef::budget_param_st, vram_sfx_tex)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_sfx_mdl offsetof(::er::paramdef::budget_param_st, vram_sfx_mdl)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_gi offsetof(::er::paramdef::budget_param_st, vram_gi)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_menu_tex offsetof(::er::paramdef::budget_param_st, vram_menu_tex)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_decal_rt offsetof(::er::paramdef::budget_param_st, vram_decal_rt)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_decal offsetof(::er::paramdef::budget_param_st, vram_decal)
#define ERD_OFFSET_BUDGET_PARAM_ST_reserve_0 offsetof(::er::paramdef::budget_param_st, reserve_0)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_other_tex offsetof(::er::paramdef::budget_param_st, vram_other_tex)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_other_mdl offsetof(::er::paramdef::budget_param_st, vram_other_mdl)
#define ERD_OFFSET_BUDGET_PARAM_ST_havok_anim offsetof(::er::paramdef::budget_param_st, havok_anim)
#define ERD_OFFSET_BUDGET_PARAM_ST_havok_ins offsetof(::er::paramdef::budget_param_st, havok_ins)
#define ERD_OFFSET_BUDGET_PARAM_ST_havok_hit offsetof(::er::paramdef::budget_param_st, havok_hit)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_other offsetof(::er::paramdef::budget_param_st, vram_other)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_detail_all offsetof(::er::paramdef::budget_param_st, vram_detail_all)
#define ERD_OFFSET_BUDGET_PARAM_ST_vram_chr_and_parts offsetof(::er::paramdef::budget_param_st, vram_chr_and_parts)
#define ERD_OFFSET_BUDGET_PARAM_ST_havok_navimesh offsetof(::er::paramdef::budget_param_st, havok_navimesh)
#define ERD_OFFSET_BUDGET_PARAM_ST_reserve_1 offsetof(::er::paramdef::budget_param_st, reserve_1)
