#pragma once

#include <cstddef>
#include <elden-x/paramdef/LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_FIELDS(X) \\
    X(TargetMapId, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, TargetMapId)) \\
    X(TargetEventId, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, TargetEventId)) \\
    X(SrcAssetId, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, SrcAssetId)) \\
    X(SrcAssetPartsNo, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, SrcAssetPartsNo)) \\
    X(DstAssetId, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, DstAssetId)) \\
    X(DstAssetPartsNo, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, DstAssetPartsNo)) \\
    X(SrcAssetIdRangeMin, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, SrcAssetIdRangeMin)) \\
    X(SrcAssetIdRangeMax, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, SrcAssetIdRangeMax)) \\
    X(DstAssetIdRangeMin, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, DstAssetIdRangeMin)) \\
    X(DstAssetIdRangeMax, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, DstAssetIdRangeMax)) \\
    X(LimitedMapRegionId0, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionId0)) \\
    X(LimitedMapRegionId1, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionId1)) \\
    X(LimitedMapRegionId2, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionId2)) \\
    X(LimitedMapRegionId3, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionId3)) \\
    X(reserve, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, reserve)) \\
    X(LimitedMapRegionAssetId, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionAssetId)) \\
    X(LimitedMapRegioAssetPartsNo, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegioAssetPartsNo)) \\
    X(LimitedMapRegioAssetIdRangeMin, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegioAssetIdRangeMin)) \\
    X(LimitedMapRegioAssetIdRangeMax, offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegioAssetIdRangeMax))

#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_TargetMapId offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, TargetMapId)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_TargetEventId offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, TargetEventId)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_SrcAssetId offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, SrcAssetId)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_SrcAssetPartsNo offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, SrcAssetPartsNo)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_DstAssetId offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, DstAssetId)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_DstAssetPartsNo offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, DstAssetPartsNo)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_SrcAssetIdRangeMin offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, SrcAssetIdRangeMin)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_SrcAssetIdRangeMax offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, SrcAssetIdRangeMax)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_DstAssetIdRangeMin offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, DstAssetIdRangeMin)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_DstAssetIdRangeMax offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, DstAssetIdRangeMax)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_LimitedMapRegionId0 offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionId0)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_LimitedMapRegionId1 offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionId1)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_LimitedMapRegionId2 offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionId2)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_LimitedMapRegionId3 offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionId3)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_reserve offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, reserve)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_LimitedMapRegionAssetId offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegionAssetId)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_LimitedMapRegioAssetPartsNo offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegioAssetPartsNo)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_LimitedMapRegioAssetIdRangeMin offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegioAssetIdRangeMin)
#define ERD_OFFSET_LEGACY_DISTANT_VIEW_PARTS_REPLACE_PARAM_LimitedMapRegioAssetIdRangeMax offsetof(::er::paramdef::legacy_distant_view_parts_replace_param, LimitedMapRegioAssetIdRangeMax)
