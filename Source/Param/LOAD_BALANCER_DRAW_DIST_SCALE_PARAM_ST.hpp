#pragma once

#include <cstddef>
#include <elden-x/paramdef/LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_FIELDS(X) \\
    X(Lv00, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv00)) \\
    X(Lv01, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv01)) \\
    X(Lv02, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv02)) \\
    X(Lv03, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv03)) \\
    X(Lv04, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv04)) \\
    X(Lv05, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv05)) \\
    X(Lv06, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv06)) \\
    X(Lv07, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv07)) \\
    X(Lv08, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv08)) \\
    X(Lv09, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv09)) \\
    X(Lv10, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv10)) \\
    X(Lv11, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv11)) \\
    X(Lv12, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv12)) \\
    X(Lv13, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv13)) \\
    X(Lv14, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv14)) \\
    X(Lv15, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv15)) \\
    X(Lv16, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv16)) \\
    X(Lv17, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv17)) \\
    X(Lv18, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv18)) \\
    X(Lv19, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv19)) \\
    X(Lv20, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv20)) \\
    X(reserve, offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, reserve))

#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv00 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv00)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv01 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv01)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv02 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv02)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv03 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv03)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv04 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv04)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv05 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv05)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv06 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv06)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv07 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv07)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv08 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv08)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv09 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv09)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv10 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv10)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv11 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv11)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv12 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv12)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv13 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv13)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv14 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv14)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv15 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv15)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv16 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv16)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv17 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv17)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv18 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv18)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv19 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv19)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_Lv20 offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, Lv20)
#define ERD_OFFSET_LOAD_BALANCER_DRAW_DIST_SCALE_PARAM_ST_reserve offsetof(::er::paramdef::load_balancer_draw_dist_scale_param_st, reserve)
