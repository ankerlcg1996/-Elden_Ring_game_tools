#pragma once

#include <cstddef>
#include <elden-x/paramdef/CS_REFLECTION_QUALITY_DETAIL.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CS_REFLECTION_QUALITY_DETAIL_FIELDS(X) \\
    X(enabled, offsetof(::er::paramdef::cs_reflection_quality_detail, enabled)) \\
    X(localLightEnabled, offsetof(::er::paramdef::cs_reflection_quality_detail, localLightEnabled)) \\
    X(localLightForceEnabled, offsetof(::er::paramdef::cs_reflection_quality_detail, localLightForceEnabled)) \\
    X(dmy, offsetof(::er::paramdef::cs_reflection_quality_detail, dmy)) \\
    X(resolutionDivider, offsetof(::er::paramdef::cs_reflection_quality_detail, resolutionDivider)) \\
    X(ssrEnabled, offsetof(::er::paramdef::cs_reflection_quality_detail, ssrEnabled)) \\
    X(ssrGaussianBlurEnabled, offsetof(::er::paramdef::cs_reflection_quality_detail, ssrGaussianBlurEnabled)) \\
    X(dmy2, offsetof(::er::paramdef::cs_reflection_quality_detail, dmy2)) \\
    X(ssrDepthRejectThresholdScale, offsetof(::er::paramdef::cs_reflection_quality_detail, ssrDepthRejectThresholdScale)) \\
    X(ssrRayTraceStepScale, offsetof(::er::paramdef::cs_reflection_quality_detail, ssrRayTraceStepScale)) \\
    X(ssrFadeToViewerBias, offsetof(::er::paramdef::cs_reflection_quality_detail, ssrFadeToViewerBias)) \\
    X(ssrFresnelRejectBias, offsetof(::er::paramdef::cs_reflection_quality_detail, ssrFresnelRejectBias))

#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_enabled offsetof(::er::paramdef::cs_reflection_quality_detail, enabled)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_localLightEnabled offsetof(::er::paramdef::cs_reflection_quality_detail, localLightEnabled)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_localLightForceEnabled offsetof(::er::paramdef::cs_reflection_quality_detail, localLightForceEnabled)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_dmy offsetof(::er::paramdef::cs_reflection_quality_detail, dmy)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_resolutionDivider offsetof(::er::paramdef::cs_reflection_quality_detail, resolutionDivider)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_ssrEnabled offsetof(::er::paramdef::cs_reflection_quality_detail, ssrEnabled)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_ssrGaussianBlurEnabled offsetof(::er::paramdef::cs_reflection_quality_detail, ssrGaussianBlurEnabled)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_dmy2 offsetof(::er::paramdef::cs_reflection_quality_detail, dmy2)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_ssrDepthRejectThresholdScale offsetof(::er::paramdef::cs_reflection_quality_detail, ssrDepthRejectThresholdScale)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_ssrRayTraceStepScale offsetof(::er::paramdef::cs_reflection_quality_detail, ssrRayTraceStepScale)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_ssrFadeToViewerBias offsetof(::er::paramdef::cs_reflection_quality_detail, ssrFadeToViewerBias)
#define ERD_OFFSET_CS_REFLECTION_QUALITY_DETAIL_ssrFresnelRejectBias offsetof(::er::paramdef::cs_reflection_quality_detail, ssrFresnelRejectBias)
