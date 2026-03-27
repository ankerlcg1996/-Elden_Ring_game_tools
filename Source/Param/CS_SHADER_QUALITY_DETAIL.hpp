#pragma once

#include <cstddef>
#include <elden-x/paramdef/CS_SHADER_QUALITY_DETAIL.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CS_SHADER_QUALITY_DETAIL_FIELDS(X) \\
    X(sssEnabled, offsetof(::er::paramdef::cs_shader_quality_detail, sssEnabled)) \\
    X(tessellationEnabled, offsetof(::er::paramdef::cs_shader_quality_detail, tessellationEnabled)) \\
    X(highPrecisionNormalEnabled, offsetof(::er::paramdef::cs_shader_quality_detail, highPrecisionNormalEnabled)) \\
    X(dmy, offsetof(::er::paramdef::cs_shader_quality_detail, dmy))

#define ERD_OFFSET_CS_SHADER_QUALITY_DETAIL_sssEnabled offsetof(::er::paramdef::cs_shader_quality_detail, sssEnabled)
#define ERD_OFFSET_CS_SHADER_QUALITY_DETAIL_tessellationEnabled offsetof(::er::paramdef::cs_shader_quality_detail, tessellationEnabled)
#define ERD_OFFSET_CS_SHADER_QUALITY_DETAIL_highPrecisionNormalEnabled offsetof(::er::paramdef::cs_shader_quality_detail, highPrecisionNormalEnabled)
#define ERD_OFFSET_CS_SHADER_QUALITY_DETAIL_dmy offsetof(::er::paramdef::cs_shader_quality_detail, dmy)
