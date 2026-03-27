#pragma once

#include <cstddef>
#include <elden-x/paramdef/GRASS_TYPE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_GRASS_TYPE_PARAM_ST_FIELDS(X) \\
    X(lodRange, offsetof(::er::paramdef::grass_type_param_st, lodRange)) \\
    X(lod0ClusterType, offsetof(::er::paramdef::grass_type_param_st, lod0ClusterType)) \\
    X(lod1ClusterType, offsetof(::er::paramdef::grass_type_param_st, lod1ClusterType)) \\
    X(lod2ClusterType, offsetof(::er::paramdef::grass_type_param_st, lod2ClusterType)) \\
    X(pad0, offsetof(::er::paramdef::grass_type_param_st, pad0)) \\
    X(distributionType, offsetof(::er::paramdef::grass_type_param_st, distributionType)) \\
    X(baseDensity, offsetof(::er::paramdef::grass_type_param_st, baseDensity)) \\
    X(model0Name, offsetof(::er::paramdef::grass_type_param_st, model0Name)) \\
    X(flatTextureName, offsetof(::er::paramdef::grass_type_param_st, flatTextureName)) \\
    X(billboardTextureName, offsetof(::er::paramdef::grass_type_param_st, billboardTextureName)) \\
    X(normalInfluence, offsetof(::er::paramdef::grass_type_param_st, normalInfluence)) \\
    X(inclinationMax, offsetof(::er::paramdef::grass_type_param_st, inclinationMax)) \\
    X(inclinationJitter, offsetof(::er::paramdef::grass_type_param_st, inclinationJitter)) \\
    X(scaleBaseMin, offsetof(::er::paramdef::grass_type_param_st, scaleBaseMin)) \\
    X(scaleBaseMax, offsetof(::er::paramdef::grass_type_param_st, scaleBaseMax)) \\
    X(scaleHeightMin, offsetof(::er::paramdef::grass_type_param_st, scaleHeightMin)) \\
    X(scaleHeightMax, offsetof(::er::paramdef::grass_type_param_st, scaleHeightMax)) \\
    X(colorShade1_r, offsetof(::er::paramdef::grass_type_param_st, colorShade1_r)) \\
    X(colorShade1_g, offsetof(::er::paramdef::grass_type_param_st, colorShade1_g)) \\
    X(colorShade1_b, offsetof(::er::paramdef::grass_type_param_st, colorShade1_b)) \\
    X(colorShade2_r, offsetof(::er::paramdef::grass_type_param_st, colorShade2_r)) \\
    X(colorShade2_g, offsetof(::er::paramdef::grass_type_param_st, colorShade2_g)) \\
    X(colorShade2_b, offsetof(::er::paramdef::grass_type_param_st, colorShade2_b)) \\
    X(flatSplitType, offsetof(::er::paramdef::grass_type_param_st, flatSplitType)) \\
    X(flatBladeCount, offsetof(::er::paramdef::grass_type_param_st, flatBladeCount)) \\
    X(flatSlant, offsetof(::er::paramdef::grass_type_param_st, flatSlant)) \\
    X(flatRadius, offsetof(::er::paramdef::grass_type_param_st, flatRadius)) \\
    X(castShadow, offsetof(::er::paramdef::grass_type_param_st, castShadow)) \\
    X(windAmplitude, offsetof(::er::paramdef::grass_type_param_st, windAmplitude)) \\
    X(pad1, offsetof(::er::paramdef::grass_type_param_st, pad1)) \\
    X(windCycle, offsetof(::er::paramdef::grass_type_param_st, windCycle)) \\
    X(orientationAngle, offsetof(::er::paramdef::grass_type_param_st, orientationAngle)) \\
    X(orientationRange, offsetof(::er::paramdef::grass_type_param_st, orientationRange)) \\
    X(spacing, offsetof(::er::paramdef::grass_type_param_st, spacing)) \\
    X(dithering, offsetof(::er::paramdef::grass_type_param_st, dithering)) \\
    X(pad, offsetof(::er::paramdef::grass_type_param_st, pad)) \\
    X(simpleModelName, offsetof(::er::paramdef::grass_type_param_st, simpleModelName)) \\
    X(model1Name, offsetof(::er::paramdef::grass_type_param_st, model1Name))

#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_lodRange offsetof(::er::paramdef::grass_type_param_st, lodRange)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_lod0ClusterType offsetof(::er::paramdef::grass_type_param_st, lod0ClusterType)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_lod1ClusterType offsetof(::er::paramdef::grass_type_param_st, lod1ClusterType)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_lod2ClusterType offsetof(::er::paramdef::grass_type_param_st, lod2ClusterType)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_pad0 offsetof(::er::paramdef::grass_type_param_st, pad0)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_distributionType offsetof(::er::paramdef::grass_type_param_st, distributionType)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_baseDensity offsetof(::er::paramdef::grass_type_param_st, baseDensity)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_model0Name offsetof(::er::paramdef::grass_type_param_st, model0Name)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_flatTextureName offsetof(::er::paramdef::grass_type_param_st, flatTextureName)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_billboardTextureName offsetof(::er::paramdef::grass_type_param_st, billboardTextureName)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_normalInfluence offsetof(::er::paramdef::grass_type_param_st, normalInfluence)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_inclinationMax offsetof(::er::paramdef::grass_type_param_st, inclinationMax)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_inclinationJitter offsetof(::er::paramdef::grass_type_param_st, inclinationJitter)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_scaleBaseMin offsetof(::er::paramdef::grass_type_param_st, scaleBaseMin)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_scaleBaseMax offsetof(::er::paramdef::grass_type_param_st, scaleBaseMax)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_scaleHeightMin offsetof(::er::paramdef::grass_type_param_st, scaleHeightMin)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_scaleHeightMax offsetof(::er::paramdef::grass_type_param_st, scaleHeightMax)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_colorShade1_r offsetof(::er::paramdef::grass_type_param_st, colorShade1_r)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_colorShade1_g offsetof(::er::paramdef::grass_type_param_st, colorShade1_g)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_colorShade1_b offsetof(::er::paramdef::grass_type_param_st, colorShade1_b)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_colorShade2_r offsetof(::er::paramdef::grass_type_param_st, colorShade2_r)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_colorShade2_g offsetof(::er::paramdef::grass_type_param_st, colorShade2_g)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_colorShade2_b offsetof(::er::paramdef::grass_type_param_st, colorShade2_b)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_flatSplitType offsetof(::er::paramdef::grass_type_param_st, flatSplitType)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_flatBladeCount offsetof(::er::paramdef::grass_type_param_st, flatBladeCount)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_flatSlant offsetof(::er::paramdef::grass_type_param_st, flatSlant)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_flatRadius offsetof(::er::paramdef::grass_type_param_st, flatRadius)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_castShadow offsetof(::er::paramdef::grass_type_param_st, castShadow)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_windAmplitude offsetof(::er::paramdef::grass_type_param_st, windAmplitude)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_pad1 offsetof(::er::paramdef::grass_type_param_st, pad1)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_windCycle offsetof(::er::paramdef::grass_type_param_st, windCycle)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_orientationAngle offsetof(::er::paramdef::grass_type_param_st, orientationAngle)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_orientationRange offsetof(::er::paramdef::grass_type_param_st, orientationRange)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_spacing offsetof(::er::paramdef::grass_type_param_st, spacing)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_dithering offsetof(::er::paramdef::grass_type_param_st, dithering)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_pad offsetof(::er::paramdef::grass_type_param_st, pad)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_simpleModelName offsetof(::er::paramdef::grass_type_param_st, simpleModelName)
#define ERD_OFFSET_GRASS_TYPE_PARAM_ST_model1Name offsetof(::er::paramdef::grass_type_param_st, model1Name)
