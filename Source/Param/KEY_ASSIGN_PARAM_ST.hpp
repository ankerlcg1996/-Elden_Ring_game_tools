#pragma once

#include <cstddef>
#include <elden-x/paramdef/KEY_ASSIGN_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_KEY_ASSIGN_PARAM_ST_FIELDS(X) \\
    X(padKeyId, offsetof(::er::paramdef::key_assign_param_st, padKeyId)) \\
    X(keyboardModifyKey, offsetof(::er::paramdef::key_assign_param_st, keyboardModifyKey)) \\
    X(keyboardKeyId, offsetof(::er::paramdef::key_assign_param_st, keyboardKeyId)) \\
    X(mouseModifyKey, offsetof(::er::paramdef::key_assign_param_st, mouseModifyKey)) \\
    X(mouseKeyId, offsetof(::er::paramdef::key_assign_param_st, mouseKeyId)) \\
    X(reserved, offsetof(::er::paramdef::key_assign_param_st, reserved))

#define ERD_OFFSET_KEY_ASSIGN_PARAM_ST_padKeyId offsetof(::er::paramdef::key_assign_param_st, padKeyId)
#define ERD_OFFSET_KEY_ASSIGN_PARAM_ST_keyboardModifyKey offsetof(::er::paramdef::key_assign_param_st, keyboardModifyKey)
#define ERD_OFFSET_KEY_ASSIGN_PARAM_ST_keyboardKeyId offsetof(::er::paramdef::key_assign_param_st, keyboardKeyId)
#define ERD_OFFSET_KEY_ASSIGN_PARAM_ST_mouseModifyKey offsetof(::er::paramdef::key_assign_param_st, mouseModifyKey)
#define ERD_OFFSET_KEY_ASSIGN_PARAM_ST_mouseKeyId offsetof(::er::paramdef::key_assign_param_st, mouseKeyId)
#define ERD_OFFSET_KEY_ASSIGN_PARAM_ST_reserved offsetof(::er::paramdef::key_assign_param_st, reserved)
