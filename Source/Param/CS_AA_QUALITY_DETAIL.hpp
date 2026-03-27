#pragma once

#include <cstddef>
#include <elden-x/paramdef/CS_AA_QUALITY_DETAIL.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CS_AA_QUALITY_DETAIL_FIELDS(X) \\
    X(enabled, offsetof(::er::paramdef::cs_aa_quality_detail, enabled)) \\
    X(forceFXAA2, offsetof(::er::paramdef::cs_aa_quality_detail, forceFXAA2)) \\
    X(dmy, offsetof(::er::paramdef::cs_aa_quality_detail, dmy))

#define ERD_OFFSET_CS_AA_QUALITY_DETAIL_enabled offsetof(::er::paramdef::cs_aa_quality_detail, enabled)
#define ERD_OFFSET_CS_AA_QUALITY_DETAIL_forceFXAA2 offsetof(::er::paramdef::cs_aa_quality_detail, forceFXAA2)
#define ERD_OFFSET_CS_AA_QUALITY_DETAIL_dmy offsetof(::er::paramdef::cs_aa_quality_detail, dmy)
