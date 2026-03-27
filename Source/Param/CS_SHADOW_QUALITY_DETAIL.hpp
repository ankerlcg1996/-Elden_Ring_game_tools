#pragma once

#include <cstddef>
#include <elden-x/paramdef/CS_SHADOW_QUALITY_DETAIL.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CS_SHADOW_QUALITY_DETAIL_FIELDS(X) \\
    X(enabled, offsetof(::er::paramdef::cs_shadow_quality_detail, enabled)) \\
    X(maxFilterLevel, offsetof(::er::paramdef::cs_shadow_quality_detail, maxFilterLevel)) \\
    X(dmy, offsetof(::er::paramdef::cs_shadow_quality_detail, dmy)) \\
    X(textureSizeScaler, offsetof(::er::paramdef::cs_shadow_quality_detail, textureSizeScaler)) \\
    X(textureSizeDivider, offsetof(::er::paramdef::cs_shadow_quality_detail, textureSizeDivider)) \\
    X(textureMinSize, offsetof(::er::paramdef::cs_shadow_quality_detail, textureMinSize)) \\
    X(textureMaxSize, offsetof(::er::paramdef::cs_shadow_quality_detail, textureMaxSize)) \\
    X(blurCountBias, offsetof(::er::paramdef::cs_shadow_quality_detail, blurCountBias))

#define ERD_OFFSET_CS_SHADOW_QUALITY_DETAIL_enabled offsetof(::er::paramdef::cs_shadow_quality_detail, enabled)
#define ERD_OFFSET_CS_SHADOW_QUALITY_DETAIL_maxFilterLevel offsetof(::er::paramdef::cs_shadow_quality_detail, maxFilterLevel)
#define ERD_OFFSET_CS_SHADOW_QUALITY_DETAIL_dmy offsetof(::er::paramdef::cs_shadow_quality_detail, dmy)
#define ERD_OFFSET_CS_SHADOW_QUALITY_DETAIL_textureSizeScaler offsetof(::er::paramdef::cs_shadow_quality_detail, textureSizeScaler)
#define ERD_OFFSET_CS_SHADOW_QUALITY_DETAIL_textureSizeDivider offsetof(::er::paramdef::cs_shadow_quality_detail, textureSizeDivider)
#define ERD_OFFSET_CS_SHADOW_QUALITY_DETAIL_textureMinSize offsetof(::er::paramdef::cs_shadow_quality_detail, textureMinSize)
#define ERD_OFFSET_CS_SHADOW_QUALITY_DETAIL_textureMaxSize offsetof(::er::paramdef::cs_shadow_quality_detail, textureMaxSize)
#define ERD_OFFSET_CS_SHADOW_QUALITY_DETAIL_blurCountBias offsetof(::er::paramdef::cs_shadow_quality_detail, blurCountBias)
