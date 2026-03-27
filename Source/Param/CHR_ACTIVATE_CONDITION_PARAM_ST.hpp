#pragma once

#include <cstddef>
#include <elden-x/paramdef/CHR_ACTIVATE_CONDITION_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CHR_ACTIVATE_CONDITION_PARAM_ST_FIELDS(X) \\
    X(timeStartHour, offsetof(::er::paramdef::chr_activate_condition_param_st, timeStartHour)) \\
    X(timeStartMin, offsetof(::er::paramdef::chr_activate_condition_param_st, timeStartMin)) \\
    X(timeEndHour, offsetof(::er::paramdef::chr_activate_condition_param_st, timeEndHour)) \\
    X(timeEndMin, offsetof(::er::paramdef::chr_activate_condition_param_st, timeEndMin)) \\
    X(pad2, offsetof(::er::paramdef::chr_activate_condition_param_st, pad2))

#define ERD_OFFSET_CHR_ACTIVATE_CONDITION_PARAM_ST_timeStartHour offsetof(::er::paramdef::chr_activate_condition_param_st, timeStartHour)
#define ERD_OFFSET_CHR_ACTIVATE_CONDITION_PARAM_ST_timeStartMin offsetof(::er::paramdef::chr_activate_condition_param_st, timeStartMin)
#define ERD_OFFSET_CHR_ACTIVATE_CONDITION_PARAM_ST_timeEndHour offsetof(::er::paramdef::chr_activate_condition_param_st, timeEndHour)
#define ERD_OFFSET_CHR_ACTIVATE_CONDITION_PARAM_ST_timeEndMin offsetof(::er::paramdef::chr_activate_condition_param_st, timeEndMin)
#define ERD_OFFSET_CHR_ACTIVATE_CONDITION_PARAM_ST_pad2 offsetof(::er::paramdef::chr_activate_condition_param_st, pad2)
