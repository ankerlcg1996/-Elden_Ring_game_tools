#pragma once

#include <cstddef>
#include <elden-x/paramdef/EVENT_FLAG_USAGE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_EVENT_FLAG_USAGE_PARAM_ST_FIELDS(X) \\
    X(usageType, offsetof(::er::paramdef::event_flag_usage_param_st, usageType)) \\
    X(playlogCategory, offsetof(::er::paramdef::event_flag_usage_param_st, playlogCategory)) \\
    X(padding1, offsetof(::er::paramdef::event_flag_usage_param_st, padding1)) \\
    X(flagNum, offsetof(::er::paramdef::event_flag_usage_param_st, flagNum)) \\
    X(padding2, offsetof(::er::paramdef::event_flag_usage_param_st, padding2))

#define ERD_OFFSET_EVENT_FLAG_USAGE_PARAM_ST_usageType offsetof(::er::paramdef::event_flag_usage_param_st, usageType)
#define ERD_OFFSET_EVENT_FLAG_USAGE_PARAM_ST_playlogCategory offsetof(::er::paramdef::event_flag_usage_param_st, playlogCategory)
#define ERD_OFFSET_EVENT_FLAG_USAGE_PARAM_ST_padding1 offsetof(::er::paramdef::event_flag_usage_param_st, padding1)
#define ERD_OFFSET_EVENT_FLAG_USAGE_PARAM_ST_flagNum offsetof(::er::paramdef::event_flag_usage_param_st, flagNum)
#define ERD_OFFSET_EVENT_FLAG_USAGE_PARAM_ST_padding2 offsetof(::er::paramdef::event_flag_usage_param_st, padding2)
