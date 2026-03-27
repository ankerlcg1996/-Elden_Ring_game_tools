#pragma once

#include <cstddef>
#include <elden-x/paramdef/BUDDY_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_BUDDY_PARAM_ST_FIELDS(X) \\
    X(disableParamReserve2, offsetof(::er::paramdef::buddy_param_st, disableParamReserve2)) \\
    X(triggerSpEffectId, offsetof(::er::paramdef::buddy_param_st, triggerSpEffectId)) \\
    X(npcParamId, offsetof(::er::paramdef::buddy_param_st, npcParamId)) \\
    X(npcThinkParamId, offsetof(::er::paramdef::buddy_param_st, npcThinkParamId)) \\
    X(npcParamId_ridden, offsetof(::er::paramdef::buddy_param_st, npcParamId_ridden)) \\
    X(npcThinkParamId_ridden, offsetof(::er::paramdef::buddy_param_st, npcThinkParamId_ridden)) \\
    X(x_offset, offsetof(::er::paramdef::buddy_param_st, x_offset)) \\
    X(z_offset, offsetof(::er::paramdef::buddy_param_st, z_offset)) \\
    X(y_angle, offsetof(::er::paramdef::buddy_param_st, y_angle)) \\
    X(appearOnAroundSekihi, offsetof(::er::paramdef::buddy_param_st, appearOnAroundSekihi)) \\
    X(disablePCTargetShare, offsetof(::er::paramdef::buddy_param_st, disablePCTargetShare)) \\
    X(pcFollowType, offsetof(::er::paramdef::buddy_param_st, pcFollowType)) \\
    X(Reserve, offsetof(::er::paramdef::buddy_param_st, Reserve)) \\
    X(dopingSpEffect_lv0, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv0)) \\
    X(dopingSpEffect_lv1, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv1)) \\
    X(dopingSpEffect_lv2, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv2)) \\
    X(dopingSpEffect_lv3, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv3)) \\
    X(dopingSpEffect_lv4, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv4)) \\
    X(dopingSpEffect_lv5, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv5)) \\
    X(dopingSpEffect_lv6, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv6)) \\
    X(dopingSpEffect_lv7, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv7)) \\
    X(dopingSpEffect_lv8, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv8)) \\
    X(dopingSpEffect_lv9, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv9)) \\
    X(dopingSpEffect_lv10, offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv10)) \\
    X(npcPlayerInitParamId, offsetof(::er::paramdef::buddy_param_st, npcPlayerInitParamId)) \\
    X(generateAnimId, offsetof(::er::paramdef::buddy_param_st, generateAnimId)) \\
    X(unknown_0x5c, offsetof(::er::paramdef::buddy_param_st, unknown_0x5c)) \\
    X(unknown_0x60, offsetof(::er::paramdef::buddy_param_st, unknown_0x60)) \\
    X(unknown_0x64, offsetof(::er::paramdef::buddy_param_st, unknown_0x64)) \\
    X(unknown_0x68, offsetof(::er::paramdef::buddy_param_st, unknown_0x68)) \\
    X(unknown_0x6c, offsetof(::er::paramdef::buddy_param_st, unknown_0x6c)) \\
    X(unknown_0x70, offsetof(::er::paramdef::buddy_param_st, unknown_0x70)) \\
    X(unknown_0x74, offsetof(::er::paramdef::buddy_param_st, unknown_0x74)) \\
    X(unknown_0x78, offsetof(::er::paramdef::buddy_param_st, unknown_0x78)) \\
    X(unknown_0x7c, offsetof(::er::paramdef::buddy_param_st, unknown_0x7c)) \\
    X(unknown_0x80, offsetof(::er::paramdef::buddy_param_st, unknown_0x80)) \\
    X(unknown_0x84, offsetof(::er::paramdef::buddy_param_st, unknown_0x84)) \\
    X(unknown_0x88, offsetof(::er::paramdef::buddy_param_st, unknown_0x88)) \\
    X(unknown_0x8c, offsetof(::er::paramdef::buddy_param_st, unknown_0x8c)) \\
    X(unknown_0x90, offsetof(::er::paramdef::buddy_param_st, unknown_0x90)) \\
    X(unknown_0x94, offsetof(::er::paramdef::buddy_param_st, unknown_0x94)) \\
    X(unknown_0x98, offsetof(::er::paramdef::buddy_param_st, unknown_0x98)) \\
    X(unknown_0x9c, offsetof(::er::paramdef::buddy_param_st, unknown_0x9c))

#define ERD_OFFSET_BUDDY_PARAM_ST_disableParamReserve2 offsetof(::er::paramdef::buddy_param_st, disableParamReserve2)
#define ERD_OFFSET_BUDDY_PARAM_ST_triggerSpEffectId offsetof(::er::paramdef::buddy_param_st, triggerSpEffectId)
#define ERD_OFFSET_BUDDY_PARAM_ST_npcParamId offsetof(::er::paramdef::buddy_param_st, npcParamId)
#define ERD_OFFSET_BUDDY_PARAM_ST_npcThinkParamId offsetof(::er::paramdef::buddy_param_st, npcThinkParamId)
#define ERD_OFFSET_BUDDY_PARAM_ST_npcParamId_ridden offsetof(::er::paramdef::buddy_param_st, npcParamId_ridden)
#define ERD_OFFSET_BUDDY_PARAM_ST_npcThinkParamId_ridden offsetof(::er::paramdef::buddy_param_st, npcThinkParamId_ridden)
#define ERD_OFFSET_BUDDY_PARAM_ST_x_offset offsetof(::er::paramdef::buddy_param_st, x_offset)
#define ERD_OFFSET_BUDDY_PARAM_ST_z_offset offsetof(::er::paramdef::buddy_param_st, z_offset)
#define ERD_OFFSET_BUDDY_PARAM_ST_y_angle offsetof(::er::paramdef::buddy_param_st, y_angle)
#define ERD_OFFSET_BUDDY_PARAM_ST_appearOnAroundSekihi offsetof(::er::paramdef::buddy_param_st, appearOnAroundSekihi)
#define ERD_OFFSET_BUDDY_PARAM_ST_disablePCTargetShare offsetof(::er::paramdef::buddy_param_st, disablePCTargetShare)
#define ERD_OFFSET_BUDDY_PARAM_ST_pcFollowType offsetof(::er::paramdef::buddy_param_st, pcFollowType)
#define ERD_OFFSET_BUDDY_PARAM_ST_Reserve offsetof(::er::paramdef::buddy_param_st, Reserve)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv0 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv0)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv1 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv1)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv2 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv2)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv3 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv3)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv4 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv4)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv5 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv5)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv6 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv6)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv7 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv7)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv8 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv8)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv9 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv9)
#define ERD_OFFSET_BUDDY_PARAM_ST_dopingSpEffect_lv10 offsetof(::er::paramdef::buddy_param_st, dopingSpEffect_lv10)
#define ERD_OFFSET_BUDDY_PARAM_ST_npcPlayerInitParamId offsetof(::er::paramdef::buddy_param_st, npcPlayerInitParamId)
#define ERD_OFFSET_BUDDY_PARAM_ST_generateAnimId offsetof(::er::paramdef::buddy_param_st, generateAnimId)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x5c offsetof(::er::paramdef::buddy_param_st, unknown_0x5c)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x60 offsetof(::er::paramdef::buddy_param_st, unknown_0x60)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x64 offsetof(::er::paramdef::buddy_param_st, unknown_0x64)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x68 offsetof(::er::paramdef::buddy_param_st, unknown_0x68)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x6c offsetof(::er::paramdef::buddy_param_st, unknown_0x6c)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x70 offsetof(::er::paramdef::buddy_param_st, unknown_0x70)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x74 offsetof(::er::paramdef::buddy_param_st, unknown_0x74)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x78 offsetof(::er::paramdef::buddy_param_st, unknown_0x78)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x7c offsetof(::er::paramdef::buddy_param_st, unknown_0x7c)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x80 offsetof(::er::paramdef::buddy_param_st, unknown_0x80)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x84 offsetof(::er::paramdef::buddy_param_st, unknown_0x84)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x88 offsetof(::er::paramdef::buddy_param_st, unknown_0x88)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x8c offsetof(::er::paramdef::buddy_param_st, unknown_0x8c)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x90 offsetof(::er::paramdef::buddy_param_st, unknown_0x90)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x94 offsetof(::er::paramdef::buddy_param_st, unknown_0x94)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x98 offsetof(::er::paramdef::buddy_param_st, unknown_0x98)
#define ERD_OFFSET_BUDDY_PARAM_ST_unknown_0x9c offsetof(::er::paramdef::buddy_param_st, unknown_0x9c)
