#pragma once

#include <cstddef>
#include <elden-x/paramdef/CS_DOF_QUALITY_DETAIL.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CS_DOF_QUALITY_DETAIL_FIELDS(X) \\
    X(enabled, offsetof(::er::paramdef::cs_dof_quality_detail, enabled)) \\
    X(dmy, offsetof(::er::paramdef::cs_dof_quality_detail, dmy)) \\
    X(forceHiResoBlur, offsetof(::er::paramdef::cs_dof_quality_detail, forceHiResoBlur)) \\
    X(maxBlurLevel, offsetof(::er::paramdef::cs_dof_quality_detail, maxBlurLevel))

#define ERD_OFFSET_CS_DOF_QUALITY_DETAIL_enabled offsetof(::er::paramdef::cs_dof_quality_detail, enabled)
#define ERD_OFFSET_CS_DOF_QUALITY_DETAIL_dmy offsetof(::er::paramdef::cs_dof_quality_detail, dmy)
#define ERD_OFFSET_CS_DOF_QUALITY_DETAIL_forceHiResoBlur offsetof(::er::paramdef::cs_dof_quality_detail, forceHiResoBlur)
#define ERD_OFFSET_CS_DOF_QUALITY_DETAIL_maxBlurLevel offsetof(::er::paramdef::cs_dof_quality_detail, maxBlurLevel)
