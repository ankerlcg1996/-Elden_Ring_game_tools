#pragma once

#include <cstddef>
#include <elden-x/paramdef/MENU_VALUE_TABLE_SPEC.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_MENU_VALUE_TABLE_SPEC_FIELDS(X) \\
    X(value, offsetof(::er::paramdef::menu_value_table_spec, value)) \\
    X(textId, offsetof(::er::paramdef::menu_value_table_spec, textId)) \\
    X(compareType, offsetof(::er::paramdef::menu_value_table_spec, compareType)) \\
    X(padding, offsetof(::er::paramdef::menu_value_table_spec, padding))

#define ERD_OFFSET_MENU_VALUE_TABLE_SPEC_value offsetof(::er::paramdef::menu_value_table_spec, value)
#define ERD_OFFSET_MENU_VALUE_TABLE_SPEC_textId offsetof(::er::paramdef::menu_value_table_spec, textId)
#define ERD_OFFSET_MENU_VALUE_TABLE_SPEC_compareType offsetof(::er::paramdef::menu_value_table_spec, compareType)
#define ERD_OFFSET_MENU_VALUE_TABLE_SPEC_padding offsetof(::er::paramdef::menu_value_table_spec, padding)
