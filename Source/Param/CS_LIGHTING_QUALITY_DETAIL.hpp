#pragma once

#include <cstddef>
#include <elden-x/paramdef/CS_LIGHTING_QUALITY_DETAIL.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CS_LIGHTING_QUALITY_DETAIL_FIELDS(X) \\
    X(localLightDistFactor, offsetof(::er::paramdef::cs_lighting_quality_detail, localLightDistFactor)) \\
    X(localLightShadowEnabled, offsetof(::er::paramdef::cs_lighting_quality_detail, localLightShadowEnabled)) \\
    X(forwardPassLightingEnabled, offsetof(::er::paramdef::cs_lighting_quality_detail, forwardPassLightingEnabled)) \\
    X(localLightShadowSpecLevelMax, offsetof(::er::paramdef::cs_lighting_quality_detail, localLightShadowSpecLevelMax)) \\
    X(dmy, offsetof(::er::paramdef::cs_lighting_quality_detail, dmy))

#define ERD_OFFSET_CS_LIGHTING_QUALITY_DETAIL_localLightDistFactor offsetof(::er::paramdef::cs_lighting_quality_detail, localLightDistFactor)
#define ERD_OFFSET_CS_LIGHTING_QUALITY_DETAIL_localLightShadowEnabled offsetof(::er::paramdef::cs_lighting_quality_detail, localLightShadowEnabled)
#define ERD_OFFSET_CS_LIGHTING_QUALITY_DETAIL_forwardPassLightingEnabled offsetof(::er::paramdef::cs_lighting_quality_detail, forwardPassLightingEnabled)
#define ERD_OFFSET_CS_LIGHTING_QUALITY_DETAIL_localLightShadowSpecLevelMax offsetof(::er::paramdef::cs_lighting_quality_detail, localLightShadowSpecLevelMax)
#define ERD_OFFSET_CS_LIGHTING_QUALITY_DETAIL_dmy offsetof(::er::paramdef::cs_lighting_quality_detail, dmy)
