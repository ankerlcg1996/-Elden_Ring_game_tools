#pragma once

#include <cstddef>
#include <elden-x/paramdef/GESTURE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_GESTURE_PARAM_ST_FIELDS(X) \\
    X(disableParamReserve2, offsetof(::er::paramdef::gesture_param_st, disableParamReserve2)) \\
    X(itemId, offsetof(::er::paramdef::gesture_param_st, itemId)) \\
    X(msgAnimId, offsetof(::er::paramdef::gesture_param_st, msgAnimId)) \\
    X(pad1, offsetof(::er::paramdef::gesture_param_st, pad1))

#define ERD_OFFSET_GESTURE_PARAM_ST_disableParamReserve2 offsetof(::er::paramdef::gesture_param_st, disableParamReserve2)
#define ERD_OFFSET_GESTURE_PARAM_ST_itemId offsetof(::er::paramdef::gesture_param_st, itemId)
#define ERD_OFFSET_GESTURE_PARAM_ST_msgAnimId offsetof(::er::paramdef::gesture_param_st, msgAnimId)
#define ERD_OFFSET_GESTURE_PARAM_ST_pad1 offsetof(::er::paramdef::gesture_param_st, pad1)
