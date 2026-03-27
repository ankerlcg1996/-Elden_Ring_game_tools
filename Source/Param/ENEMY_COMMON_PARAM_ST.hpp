#pragma once

#include <cstddef>
#include <elden-x/paramdef/ENEMY_COMMON_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_ENEMY_COMMON_PARAM_ST_FIELDS(X) \\
    X(reserved0, offsetof(::er::paramdef::enemy_common_param_st, reserved0)) \\
    X(soundTargetTryApproachTime, offsetof(::er::paramdef::enemy_common_param_st, soundTargetTryApproachTime)) \\
    X(searchTargetTryApproachTime, offsetof(::er::paramdef::enemy_common_param_st, searchTargetTryApproachTime)) \\
    X(memoryTargetTryApproachTime, offsetof(::er::paramdef::enemy_common_param_st, memoryTargetTryApproachTime)) \\
    X(reserved5, offsetof(::er::paramdef::enemy_common_param_st, reserved5)) \\
    X(activateChrByTime_PhantomId, offsetof(::er::paramdef::enemy_common_param_st, activateChrByTime_PhantomId)) \\
    X(findUnfavorableFailedPointDist, offsetof(::er::paramdef::enemy_common_param_st, findUnfavorableFailedPointDist)) \\
    X(findUnfavorableFailedPointHeight, offsetof(::er::paramdef::enemy_common_param_st, findUnfavorableFailedPointHeight)) \\
    X(reserved18, offsetof(::er::paramdef::enemy_common_param_st, reserved18))

#define ERD_OFFSET_ENEMY_COMMON_PARAM_ST_reserved0 offsetof(::er::paramdef::enemy_common_param_st, reserved0)
#define ERD_OFFSET_ENEMY_COMMON_PARAM_ST_soundTargetTryApproachTime offsetof(::er::paramdef::enemy_common_param_st, soundTargetTryApproachTime)
#define ERD_OFFSET_ENEMY_COMMON_PARAM_ST_searchTargetTryApproachTime offsetof(::er::paramdef::enemy_common_param_st, searchTargetTryApproachTime)
#define ERD_OFFSET_ENEMY_COMMON_PARAM_ST_memoryTargetTryApproachTime offsetof(::er::paramdef::enemy_common_param_st, memoryTargetTryApproachTime)
#define ERD_OFFSET_ENEMY_COMMON_PARAM_ST_reserved5 offsetof(::er::paramdef::enemy_common_param_st, reserved5)
#define ERD_OFFSET_ENEMY_COMMON_PARAM_ST_activateChrByTime_PhantomId offsetof(::er::paramdef::enemy_common_param_st, activateChrByTime_PhantomId)
#define ERD_OFFSET_ENEMY_COMMON_PARAM_ST_findUnfavorableFailedPointDist offsetof(::er::paramdef::enemy_common_param_st, findUnfavorableFailedPointDist)
#define ERD_OFFSET_ENEMY_COMMON_PARAM_ST_findUnfavorableFailedPointHeight offsetof(::er::paramdef::enemy_common_param_st, findUnfavorableFailedPointHeight)
#define ERD_OFFSET_ENEMY_COMMON_PARAM_ST_reserved18 offsetof(::er::paramdef::enemy_common_param_st, reserved18)
