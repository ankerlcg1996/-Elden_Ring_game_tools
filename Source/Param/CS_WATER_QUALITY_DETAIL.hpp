#pragma once

#include <cstddef>
#include <elden-x/paramdef/CS_WATER_QUALITY_DETAIL.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CS_WATER_QUALITY_DETAIL_FIELDS(X) \\
    X(interactionEnabled, offsetof(::er::paramdef::cs_water_quality_detail, interactionEnabled)) \\
    X(dmy, offsetof(::er::paramdef::cs_water_quality_detail, dmy))

#define ERD_OFFSET_CS_WATER_QUALITY_DETAIL_interactionEnabled offsetof(::er::paramdef::cs_water_quality_detail, interactionEnabled)
#define ERD_OFFSET_CS_WATER_QUALITY_DETAIL_dmy offsetof(::er::paramdef::cs_water_quality_detail, dmy)
