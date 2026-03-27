#pragma once

#include <cstddef>
#include <elden-x/paramdef/CS_MOTION_BLUR_QUALITY_DETAIL.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CS_MOTION_BLUR_QUALITY_DETAIL_FIELDS(X) \\
    X(enabled, offsetof(::er::paramdef::cs_motion_blur_quality_detail, enabled)) \\
    X(ombEnabled, offsetof(::er::paramdef::cs_motion_blur_quality_detail, ombEnabled)) \\
    X(forceScaleVelocityBuffer, offsetof(::er::paramdef::cs_motion_blur_quality_detail, forceScaleVelocityBuffer)) \\
    X(cheapFilterMode, offsetof(::er::paramdef::cs_motion_blur_quality_detail, cheapFilterMode)) \\
    X(sampleCountBias, offsetof(::er::paramdef::cs_motion_blur_quality_detail, sampleCountBias)) \\
    X(recurrenceCountBias, offsetof(::er::paramdef::cs_motion_blur_quality_detail, recurrenceCountBias)) \\
    X(blurMaxLengthScale, offsetof(::er::paramdef::cs_motion_blur_quality_detail, blurMaxLengthScale))

#define ERD_OFFSET_CS_MOTION_BLUR_QUALITY_DETAIL_enabled offsetof(::er::paramdef::cs_motion_blur_quality_detail, enabled)
#define ERD_OFFSET_CS_MOTION_BLUR_QUALITY_DETAIL_ombEnabled offsetof(::er::paramdef::cs_motion_blur_quality_detail, ombEnabled)
#define ERD_OFFSET_CS_MOTION_BLUR_QUALITY_DETAIL_forceScaleVelocityBuffer offsetof(::er::paramdef::cs_motion_blur_quality_detail, forceScaleVelocityBuffer)
#define ERD_OFFSET_CS_MOTION_BLUR_QUALITY_DETAIL_cheapFilterMode offsetof(::er::paramdef::cs_motion_blur_quality_detail, cheapFilterMode)
#define ERD_OFFSET_CS_MOTION_BLUR_QUALITY_DETAIL_sampleCountBias offsetof(::er::paramdef::cs_motion_blur_quality_detail, sampleCountBias)
#define ERD_OFFSET_CS_MOTION_BLUR_QUALITY_DETAIL_recurrenceCountBias offsetof(::er::paramdef::cs_motion_blur_quality_detail, recurrenceCountBias)
#define ERD_OFFSET_CS_MOTION_BLUR_QUALITY_DETAIL_blurMaxLengthScale offsetof(::er::paramdef::cs_motion_blur_quality_detail, blurMaxLengthScale)
