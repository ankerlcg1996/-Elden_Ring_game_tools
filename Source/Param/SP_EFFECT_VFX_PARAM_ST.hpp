#pragma once

#include <cstddef>
#include <elden-x/paramdef/SP_EFFECT_VFX_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_SP_EFFECT_VFX_PARAM_ST_FIELDS(X) \\
    X(midstSfxId, offsetof(::er::paramdef::sp_effect_vfx_param_st, midstSfxId)) \\
    X(midstSeId, offsetof(::er::paramdef::sp_effect_vfx_param_st, midstSeId)) \\
    X(initSfxId, offsetof(::er::paramdef::sp_effect_vfx_param_st, initSfxId)) \\
    X(initSeId, offsetof(::er::paramdef::sp_effect_vfx_param_st, initSeId)) \\
    X(finishSfxId, offsetof(::er::paramdef::sp_effect_vfx_param_st, finishSfxId)) \\
    X(finishSeId, offsetof(::er::paramdef::sp_effect_vfx_param_st, finishSeId)) \\
    X(camouflageBeginDist, offsetof(::er::paramdef::sp_effect_vfx_param_st, camouflageBeginDist)) \\
    X(camouflageEndDist, offsetof(::er::paramdef::sp_effect_vfx_param_st, camouflageEndDist)) \\
    X(transformProtectorId, offsetof(::er::paramdef::sp_effect_vfx_param_st, transformProtectorId)) \\
    X(midstDmyId, offsetof(::er::paramdef::sp_effect_vfx_param_st, midstDmyId)) \\
    X(initDmyId, offsetof(::er::paramdef::sp_effect_vfx_param_st, initDmyId)) \\
    X(finishDmyId, offsetof(::er::paramdef::sp_effect_vfx_param_st, finishDmyId)) \\
    X(effectType, offsetof(::er::paramdef::sp_effect_vfx_param_st, effectType)) \\
    X(soulParamIdForWepEnchant, offsetof(::er::paramdef::sp_effect_vfx_param_st, soulParamIdForWepEnchant)) \\
    X(playCategory, offsetof(::er::paramdef::sp_effect_vfx_param_st, playCategory)) \\
    X(playPriority, offsetof(::er::paramdef::sp_effect_vfx_param_st, playPriority)) \\
    X(decalId1, offsetof(::er::paramdef::sp_effect_vfx_param_st, decalId1)) \\
    X(decalId2, offsetof(::er::paramdef::sp_effect_vfx_param_st, decalId2)) \\
    X(footEffectPriority, offsetof(::er::paramdef::sp_effect_vfx_param_st, footEffectPriority)) \\
    X(footEffectOffset, offsetof(::er::paramdef::sp_effect_vfx_param_st, footEffectOffset)) \\
    X(traceSfxIdOffsetType, offsetof(::er::paramdef::sp_effect_vfx_param_st, traceSfxIdOffsetType)) \\
    X(forceDeceasedType, offsetof(::er::paramdef::sp_effect_vfx_param_st, forceDeceasedType)) \\
    X(enchantStartDmyId_0, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_0)) \\
    X(enchantEndDmyId_0, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_0)) \\
    X(enchantStartDmyId_1, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_1)) \\
    X(enchantEndDmyId_1, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_1)) \\
    X(enchantStartDmyId_2, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_2)) \\
    X(enchantEndDmyId_2, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_2)) \\
    X(enchantStartDmyId_3, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_3)) \\
    X(enchantEndDmyId_3, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_3)) \\
    X(enchantStartDmyId_4, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_4)) \\
    X(enchantEndDmyId_4, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_4)) \\
    X(enchantStartDmyId_5, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_5)) \\
    X(enchantEndDmyId_5, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_5)) \\
    X(enchantStartDmyId_6, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_6)) \\
    X(enchantEndDmyId_6, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_6)) \\
    X(enchantStartDmyId_7, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_7)) \\
    X(enchantEndDmyId_7, offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_7)) \\
    X(SfxIdOffsetType, offsetof(::er::paramdef::sp_effect_vfx_param_st, SfxIdOffsetType)) \\
    X(phantomParamOverwriteType, offsetof(::er::paramdef::sp_effect_vfx_param_st, phantomParamOverwriteType)) \\
    X(camouflageMinAlpha, offsetof(::er::paramdef::sp_effect_vfx_param_st, camouflageMinAlpha)) \\
    X(wetAspectType, offsetof(::er::paramdef::sp_effect_vfx_param_st, wetAspectType)) \\
    X(phantomParamOverwriteId, offsetof(::er::paramdef::sp_effect_vfx_param_st, phantomParamOverwriteId)) \\
    X(materialParamId, offsetof(::er::paramdef::sp_effect_vfx_param_st, materialParamId)) \\
    X(materialParamInitValue, offsetof(::er::paramdef::sp_effect_vfx_param_st, materialParamInitValue)) \\
    X(materialParamTargetValue, offsetof(::er::paramdef::sp_effect_vfx_param_st, materialParamTargetValue)) \\
    X(materialParamFadeTime, offsetof(::er::paramdef::sp_effect_vfx_param_st, materialParamFadeTime)) \\
    X(footDecalMaterialOffsetOverwriteId, offsetof(::er::paramdef::sp_effect_vfx_param_st, footDecalMaterialOffsetOverwriteId)) \\
    X(pad1, offsetof(::er::paramdef::sp_effect_vfx_param_st, pad1)) \\
    X(unknown_0x98, offsetof(::er::paramdef::sp_effect_vfx_param_st, unknown_0x98)) \\
    X(pad2, offsetof(::er::paramdef::sp_effect_vfx_param_st, pad2))

#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_midstSfxId offsetof(::er::paramdef::sp_effect_vfx_param_st, midstSfxId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_midstSeId offsetof(::er::paramdef::sp_effect_vfx_param_st, midstSeId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_initSfxId offsetof(::er::paramdef::sp_effect_vfx_param_st, initSfxId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_initSeId offsetof(::er::paramdef::sp_effect_vfx_param_st, initSeId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_finishSfxId offsetof(::er::paramdef::sp_effect_vfx_param_st, finishSfxId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_finishSeId offsetof(::er::paramdef::sp_effect_vfx_param_st, finishSeId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_camouflageBeginDist offsetof(::er::paramdef::sp_effect_vfx_param_st, camouflageBeginDist)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_camouflageEndDist offsetof(::er::paramdef::sp_effect_vfx_param_st, camouflageEndDist)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_transformProtectorId offsetof(::er::paramdef::sp_effect_vfx_param_st, transformProtectorId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_midstDmyId offsetof(::er::paramdef::sp_effect_vfx_param_st, midstDmyId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_initDmyId offsetof(::er::paramdef::sp_effect_vfx_param_st, initDmyId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_finishDmyId offsetof(::er::paramdef::sp_effect_vfx_param_st, finishDmyId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_effectType offsetof(::er::paramdef::sp_effect_vfx_param_st, effectType)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_soulParamIdForWepEnchant offsetof(::er::paramdef::sp_effect_vfx_param_st, soulParamIdForWepEnchant)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_playCategory offsetof(::er::paramdef::sp_effect_vfx_param_st, playCategory)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_playPriority offsetof(::er::paramdef::sp_effect_vfx_param_st, playPriority)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_decalId1 offsetof(::er::paramdef::sp_effect_vfx_param_st, decalId1)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_decalId2 offsetof(::er::paramdef::sp_effect_vfx_param_st, decalId2)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_footEffectPriority offsetof(::er::paramdef::sp_effect_vfx_param_st, footEffectPriority)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_footEffectOffset offsetof(::er::paramdef::sp_effect_vfx_param_st, footEffectOffset)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_traceSfxIdOffsetType offsetof(::er::paramdef::sp_effect_vfx_param_st, traceSfxIdOffsetType)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_forceDeceasedType offsetof(::er::paramdef::sp_effect_vfx_param_st, forceDeceasedType)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantStartDmyId_0 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_0)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantEndDmyId_0 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_0)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantStartDmyId_1 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_1)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantEndDmyId_1 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_1)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantStartDmyId_2 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_2)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantEndDmyId_2 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_2)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantStartDmyId_3 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_3)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantEndDmyId_3 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_3)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantStartDmyId_4 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_4)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantEndDmyId_4 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_4)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantStartDmyId_5 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_5)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantEndDmyId_5 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_5)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantStartDmyId_6 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_6)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantEndDmyId_6 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_6)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantStartDmyId_7 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantStartDmyId_7)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_enchantEndDmyId_7 offsetof(::er::paramdef::sp_effect_vfx_param_st, enchantEndDmyId_7)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_SfxIdOffsetType offsetof(::er::paramdef::sp_effect_vfx_param_st, SfxIdOffsetType)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_phantomParamOverwriteType offsetof(::er::paramdef::sp_effect_vfx_param_st, phantomParamOverwriteType)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_camouflageMinAlpha offsetof(::er::paramdef::sp_effect_vfx_param_st, camouflageMinAlpha)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_wetAspectType offsetof(::er::paramdef::sp_effect_vfx_param_st, wetAspectType)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_phantomParamOverwriteId offsetof(::er::paramdef::sp_effect_vfx_param_st, phantomParamOverwriteId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_materialParamId offsetof(::er::paramdef::sp_effect_vfx_param_st, materialParamId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_materialParamInitValue offsetof(::er::paramdef::sp_effect_vfx_param_st, materialParamInitValue)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_materialParamTargetValue offsetof(::er::paramdef::sp_effect_vfx_param_st, materialParamTargetValue)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_materialParamFadeTime offsetof(::er::paramdef::sp_effect_vfx_param_st, materialParamFadeTime)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_footDecalMaterialOffsetOverwriteId offsetof(::er::paramdef::sp_effect_vfx_param_st, footDecalMaterialOffsetOverwriteId)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_pad1 offsetof(::er::paramdef::sp_effect_vfx_param_st, pad1)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_unknown_0x98 offsetof(::er::paramdef::sp_effect_vfx_param_st, unknown_0x98)
#define ERD_OFFSET_SP_EFFECT_VFX_PARAM_ST_pad2 offsetof(::er::paramdef::sp_effect_vfx_param_st, pad2)
