#pragma once

#include <cstddef>
#include <elden-x/paramdef/PARTS_DRAW_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_PARTS_DRAW_PARAM_ST_FIELDS(X) \\
    X(lv01_BorderDist, offsetof(::er::paramdef::parts_draw_param_st, lv01_BorderDist)) \\
    X(lv01_PlayDist, offsetof(::er::paramdef::parts_draw_param_st, lv01_PlayDist)) \\
    X(lv12_BorderDist, offsetof(::er::paramdef::parts_draw_param_st, lv12_BorderDist)) \\
    X(lv12_PlayDist, offsetof(::er::paramdef::parts_draw_param_st, lv12_PlayDist)) \\
    X(lv23_BorderDist, offsetof(::er::paramdef::parts_draw_param_st, lv23_BorderDist)) \\
    X(lv23_PlayDist, offsetof(::er::paramdef::parts_draw_param_st, lv23_PlayDist)) \\
    X(lv34_BorderDist, offsetof(::er::paramdef::parts_draw_param_st, lv34_BorderDist)) \\
    X(lv34_PlayDist, offsetof(::er::paramdef::parts_draw_param_st, lv34_PlayDist)) \\
    X(lv45_BorderDist, offsetof(::er::paramdef::parts_draw_param_st, lv45_BorderDist)) \\
    X(lv45_PlayDist, offsetof(::er::paramdef::parts_draw_param_st, lv45_PlayDist)) \\
    X(tex_lv01_BorderDist, offsetof(::er::paramdef::parts_draw_param_st, tex_lv01_BorderDist)) \\
    X(tex_lv01_PlayDist, offsetof(::er::paramdef::parts_draw_param_st, tex_lv01_PlayDist)) \\
    X(drawDist, offsetof(::er::paramdef::parts_draw_param_st, drawDist)) \\
    X(drawFadeRange, offsetof(::er::paramdef::parts_draw_param_st, drawFadeRange)) \\
    X(shadowDrawDist, offsetof(::er::paramdef::parts_draw_param_st, shadowDrawDist)) \\
    X(shadowFadeRange, offsetof(::er::paramdef::parts_draw_param_st, shadowFadeRange)) \\
    X(motionBlur_BorderDist, offsetof(::er::paramdef::parts_draw_param_st, motionBlur_BorderDist)) \\
    X(isPointLightShadowSrc, offsetof(::er::paramdef::parts_draw_param_st, isPointLightShadowSrc)) \\
    X(isDirLightShadowSrc, offsetof(::er::paramdef::parts_draw_param_st, isDirLightShadowSrc)) \\
    X(isShadowDst, offsetof(::er::paramdef::parts_draw_param_st, isShadowDst)) \\
    X(isShadowOnly, offsetof(::er::paramdef::parts_draw_param_st, isShadowOnly)) \\
    X(drawByReflectCam, offsetof(::er::paramdef::parts_draw_param_st, drawByReflectCam)) \\
    X(drawOnlyReflectCam, offsetof(::er::paramdef::parts_draw_param_st, drawOnlyReflectCam)) \\
    X(IncludeLodMapLv, offsetof(::er::paramdef::parts_draw_param_st, IncludeLodMapLv)) \\
    X(isNoFarClipDraw, offsetof(::er::paramdef::parts_draw_param_st, isNoFarClipDraw)) \\
    X(lodType, offsetof(::er::paramdef::parts_draw_param_st, lodType)) \\
    X(shadowDrawLodOffset, offsetof(::er::paramdef::parts_draw_param_st, shadowDrawLodOffset)) \\
    X(isTraceCameraXZ, offsetof(::er::paramdef::parts_draw_param_st, isTraceCameraXZ)) \\
    X(isSkydomeDrawPhase, offsetof(::er::paramdef::parts_draw_param_st, isSkydomeDrawPhase)) \\
    X(DistantViewModel_BorderDist, offsetof(::er::paramdef::parts_draw_param_st, DistantViewModel_BorderDist)) \\
    X(DistantViewModel_PlayDist, offsetof(::er::paramdef::parts_draw_param_st, DistantViewModel_PlayDist)) \\
    X(LimtedActivate_BorderDist_forGrid, offsetof(::er::paramdef::parts_draw_param_st, LimtedActivate_BorderDist_forGrid)) \\
    X(LimtedActivate_PlayDist_forGrid, offsetof(::er::paramdef::parts_draw_param_st, LimtedActivate_PlayDist_forGrid)) \\
    X(zSortOffsetForNoFarClipDraw, offsetof(::er::paramdef::parts_draw_param_st, zSortOffsetForNoFarClipDraw)) \\
    X(shadowDrawAlphaTestDist, offsetof(::er::paramdef::parts_draw_param_st, shadowDrawAlphaTestDist)) \\
    X(fowardDrawEnvmapBlendType, offsetof(::er::paramdef::parts_draw_param_st, fowardDrawEnvmapBlendType)) \\
    X(LBDrawDistScaleParamID, offsetof(::er::paramdef::parts_draw_param_st, LBDrawDistScaleParamID)) \\
    X(resereve, offsetof(::er::paramdef::parts_draw_param_st, resereve))

#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv01_BorderDist offsetof(::er::paramdef::parts_draw_param_st, lv01_BorderDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv01_PlayDist offsetof(::er::paramdef::parts_draw_param_st, lv01_PlayDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv12_BorderDist offsetof(::er::paramdef::parts_draw_param_st, lv12_BorderDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv12_PlayDist offsetof(::er::paramdef::parts_draw_param_st, lv12_PlayDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv23_BorderDist offsetof(::er::paramdef::parts_draw_param_st, lv23_BorderDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv23_PlayDist offsetof(::er::paramdef::parts_draw_param_st, lv23_PlayDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv34_BorderDist offsetof(::er::paramdef::parts_draw_param_st, lv34_BorderDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv34_PlayDist offsetof(::er::paramdef::parts_draw_param_st, lv34_PlayDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv45_BorderDist offsetof(::er::paramdef::parts_draw_param_st, lv45_BorderDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lv45_PlayDist offsetof(::er::paramdef::parts_draw_param_st, lv45_PlayDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_tex_lv01_BorderDist offsetof(::er::paramdef::parts_draw_param_st, tex_lv01_BorderDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_tex_lv01_PlayDist offsetof(::er::paramdef::parts_draw_param_st, tex_lv01_PlayDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_drawDist offsetof(::er::paramdef::parts_draw_param_st, drawDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_drawFadeRange offsetof(::er::paramdef::parts_draw_param_st, drawFadeRange)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_shadowDrawDist offsetof(::er::paramdef::parts_draw_param_st, shadowDrawDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_shadowFadeRange offsetof(::er::paramdef::parts_draw_param_st, shadowFadeRange)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_motionBlur_BorderDist offsetof(::er::paramdef::parts_draw_param_st, motionBlur_BorderDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_isPointLightShadowSrc offsetof(::er::paramdef::parts_draw_param_st, isPointLightShadowSrc)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_isDirLightShadowSrc offsetof(::er::paramdef::parts_draw_param_st, isDirLightShadowSrc)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_isShadowDst offsetof(::er::paramdef::parts_draw_param_st, isShadowDst)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_isShadowOnly offsetof(::er::paramdef::parts_draw_param_st, isShadowOnly)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_drawByReflectCam offsetof(::er::paramdef::parts_draw_param_st, drawByReflectCam)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_drawOnlyReflectCam offsetof(::er::paramdef::parts_draw_param_st, drawOnlyReflectCam)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_IncludeLodMapLv offsetof(::er::paramdef::parts_draw_param_st, IncludeLodMapLv)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_isNoFarClipDraw offsetof(::er::paramdef::parts_draw_param_st, isNoFarClipDraw)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_lodType offsetof(::er::paramdef::parts_draw_param_st, lodType)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_shadowDrawLodOffset offsetof(::er::paramdef::parts_draw_param_st, shadowDrawLodOffset)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_isTraceCameraXZ offsetof(::er::paramdef::parts_draw_param_st, isTraceCameraXZ)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_isSkydomeDrawPhase offsetof(::er::paramdef::parts_draw_param_st, isSkydomeDrawPhase)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_DistantViewModel_BorderDist offsetof(::er::paramdef::parts_draw_param_st, DistantViewModel_BorderDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_DistantViewModel_PlayDist offsetof(::er::paramdef::parts_draw_param_st, DistantViewModel_PlayDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_LimtedActivate_BorderDist_forGrid offsetof(::er::paramdef::parts_draw_param_st, LimtedActivate_BorderDist_forGrid)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_LimtedActivate_PlayDist_forGrid offsetof(::er::paramdef::parts_draw_param_st, LimtedActivate_PlayDist_forGrid)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_zSortOffsetForNoFarClipDraw offsetof(::er::paramdef::parts_draw_param_st, zSortOffsetForNoFarClipDraw)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_shadowDrawAlphaTestDist offsetof(::er::paramdef::parts_draw_param_st, shadowDrawAlphaTestDist)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_fowardDrawEnvmapBlendType offsetof(::er::paramdef::parts_draw_param_st, fowardDrawEnvmapBlendType)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_LBDrawDistScaleParamID offsetof(::er::paramdef::parts_draw_param_st, LBDrawDistScaleParamID)
#define ERD_OFFSET_PARTS_DRAW_PARAM_ST_resereve offsetof(::er::paramdef::parts_draw_param_st, resereve)
