#pragma once

#include <cstddef>
#include <elden-x/paramdef/CAMERA_FADE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CAMERA_FADE_PARAM_ST_FIELDS(X) \\
    X(NearMinDist, offsetof(::er::paramdef::camera_fade_param_st, NearMinDist)) \\
    X(NearMaxDist, offsetof(::er::paramdef::camera_fade_param_st, NearMaxDist)) \\
    X(FarMinDist, offsetof(::er::paramdef::camera_fade_param_st, FarMinDist)) \\
    X(FarMaxDist, offsetof(::er::paramdef::camera_fade_param_st, FarMaxDist)) \\
    X(MiddleAlpha, offsetof(::er::paramdef::camera_fade_param_st, MiddleAlpha)) \\
    X(dummy, offsetof(::er::paramdef::camera_fade_param_st, dummy))

#define ERD_OFFSET_CAMERA_FADE_PARAM_ST_NearMinDist offsetof(::er::paramdef::camera_fade_param_st, NearMinDist)
#define ERD_OFFSET_CAMERA_FADE_PARAM_ST_NearMaxDist offsetof(::er::paramdef::camera_fade_param_st, NearMaxDist)
#define ERD_OFFSET_CAMERA_FADE_PARAM_ST_FarMinDist offsetof(::er::paramdef::camera_fade_param_st, FarMinDist)
#define ERD_OFFSET_CAMERA_FADE_PARAM_ST_FarMaxDist offsetof(::er::paramdef::camera_fade_param_st, FarMaxDist)
#define ERD_OFFSET_CAMERA_FADE_PARAM_ST_MiddleAlpha offsetof(::er::paramdef::camera_fade_param_st, MiddleAlpha)
#define ERD_OFFSET_CAMERA_FADE_PARAM_ST_dummy offsetof(::er::paramdef::camera_fade_param_st, dummy)
