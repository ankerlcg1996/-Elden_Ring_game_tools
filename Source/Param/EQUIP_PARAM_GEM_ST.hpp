#pragma once

#include <cstddef>
#include <elden-x/paramdef/EQUIP_PARAM_GEM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_EQUIP_PARAM_GEM_ST_FIELDS(X) \\
    X(disableParamReserve2, offsetof(::er::paramdef::equip_param_gem_st, disableParamReserve2)) \\
    X(iconId, offsetof(::er::paramdef::equip_param_gem_st, iconId)) \\
    X(rank, offsetof(::er::paramdef::equip_param_gem_st, rank)) \\
    X(sortGroupId, offsetof(::er::paramdef::equip_param_gem_st, sortGroupId)) \\
    X(spEffectId0, offsetof(::er::paramdef::equip_param_gem_st, spEffectId0)) \\
    X(spEffectId1, offsetof(::er::paramdef::equip_param_gem_st, spEffectId1)) \\
    X(spEffectId2, offsetof(::er::paramdef::equip_param_gem_st, spEffectId2)) \\
    X(itemGetTutorialFlagId, offsetof(::er::paramdef::equip_param_gem_st, itemGetTutorialFlagId)) \\
    X(swordArtsParamId, offsetof(::er::paramdef::equip_param_gem_st, swordArtsParamId)) \\
    X(mountValue, offsetof(::er::paramdef::equip_param_gem_st, mountValue)) \\
    X(sellValue, offsetof(::er::paramdef::equip_param_gem_st, sellValue)) \\
    X(saleValue, offsetof(::er::paramdef::equip_param_gem_st, saleValue)) \\
    X(sortId, offsetof(::er::paramdef::equip_param_gem_st, sortId)) \\
    X(compTrophySedId, offsetof(::er::paramdef::equip_param_gem_st, compTrophySedId)) \\
    X(trophySeqId, offsetof(::er::paramdef::equip_param_gem_st, trophySeqId)) \\
    X(rarity, offsetof(::er::paramdef::equip_param_gem_st, rarity)) \\
    X(defaultWepAttr, offsetof(::er::paramdef::equip_param_gem_st, defaultWepAttr)) \\
    X(isSpecialSwordArt, offsetof(::er::paramdef::equip_param_gem_st, isSpecialSwordArt)) \\
    X(pad2, offsetof(::er::paramdef::equip_param_gem_st, pad2)) \\
    X(reserved2_canMountWep, offsetof(::er::paramdef::equip_param_gem_st, reserved2_canMountWep)) \\
    X(spEffectMsgId0, offsetof(::er::paramdef::equip_param_gem_st, spEffectMsgId0)) \\
    X(spEffectMsgId1, offsetof(::er::paramdef::equip_param_gem_st, spEffectMsgId1)) \\
    X(spEffectId_forAtk0, offsetof(::er::paramdef::equip_param_gem_st, spEffectId_forAtk0)) \\
    X(spEffectId_forAtk1, offsetof(::er::paramdef::equip_param_gem_st, spEffectId_forAtk1)) \\
    X(spEffectId_forAtk2, offsetof(::er::paramdef::equip_param_gem_st, spEffectId_forAtk2)) \\
    X(mountWepTextId, offsetof(::er::paramdef::equip_param_gem_st, mountWepTextId)) \\
    X(pad6, offsetof(::er::paramdef::equip_param_gem_st, pad6))

#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_disableParamReserve2 offsetof(::er::paramdef::equip_param_gem_st, disableParamReserve2)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_iconId offsetof(::er::paramdef::equip_param_gem_st, iconId)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_rank offsetof(::er::paramdef::equip_param_gem_st, rank)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_sortGroupId offsetof(::er::paramdef::equip_param_gem_st, sortGroupId)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_spEffectId0 offsetof(::er::paramdef::equip_param_gem_st, spEffectId0)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_spEffectId1 offsetof(::er::paramdef::equip_param_gem_st, spEffectId1)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_spEffectId2 offsetof(::er::paramdef::equip_param_gem_st, spEffectId2)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_itemGetTutorialFlagId offsetof(::er::paramdef::equip_param_gem_st, itemGetTutorialFlagId)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_swordArtsParamId offsetof(::er::paramdef::equip_param_gem_st, swordArtsParamId)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_mountValue offsetof(::er::paramdef::equip_param_gem_st, mountValue)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_sellValue offsetof(::er::paramdef::equip_param_gem_st, sellValue)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_saleValue offsetof(::er::paramdef::equip_param_gem_st, saleValue)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_sortId offsetof(::er::paramdef::equip_param_gem_st, sortId)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_compTrophySedId offsetof(::er::paramdef::equip_param_gem_st, compTrophySedId)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_trophySeqId offsetof(::er::paramdef::equip_param_gem_st, trophySeqId)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_rarity offsetof(::er::paramdef::equip_param_gem_st, rarity)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_defaultWepAttr offsetof(::er::paramdef::equip_param_gem_st, defaultWepAttr)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_isSpecialSwordArt offsetof(::er::paramdef::equip_param_gem_st, isSpecialSwordArt)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_pad2 offsetof(::er::paramdef::equip_param_gem_st, pad2)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_reserved2_canMountWep offsetof(::er::paramdef::equip_param_gem_st, reserved2_canMountWep)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_spEffectMsgId0 offsetof(::er::paramdef::equip_param_gem_st, spEffectMsgId0)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_spEffectMsgId1 offsetof(::er::paramdef::equip_param_gem_st, spEffectMsgId1)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_spEffectId_forAtk0 offsetof(::er::paramdef::equip_param_gem_st, spEffectId_forAtk0)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_spEffectId_forAtk1 offsetof(::er::paramdef::equip_param_gem_st, spEffectId_forAtk1)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_spEffectId_forAtk2 offsetof(::er::paramdef::equip_param_gem_st, spEffectId_forAtk2)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_mountWepTextId offsetof(::er::paramdef::equip_param_gem_st, mountWepTextId)
#define ERD_OFFSET_EQUIP_PARAM_GEM_ST_pad6 offsetof(::er::paramdef::equip_param_gem_st, pad6)
