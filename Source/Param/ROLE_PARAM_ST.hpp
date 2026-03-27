#pragma once

#include <cstddef>
#include <elden-x/paramdef/ROLE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_ROLE_PARAM_ST_FIELDS(X) \\
    X(teamType, offsetof(::er::paramdef::role_param_st, teamType)) \\
    X(pad10, offsetof(::er::paramdef::role_param_st, pad10)) \\
    X(phantomParamId, offsetof(::er::paramdef::role_param_st, phantomParamId)) \\
    X(spEffectID0, offsetof(::er::paramdef::role_param_st, spEffectID0)) \\
    X(spEffectID1, offsetof(::er::paramdef::role_param_st, spEffectID1)) \\
    X(spEffectID2, offsetof(::er::paramdef::role_param_st, spEffectID2)) \\
    X(spEffectID3, offsetof(::er::paramdef::role_param_st, spEffectID3)) \\
    X(spEffectID4, offsetof(::er::paramdef::role_param_st, spEffectID4)) \\
    X(spEffectID5, offsetof(::er::paramdef::role_param_st, spEffectID5)) \\
    X(spEffectID6, offsetof(::er::paramdef::role_param_st, spEffectID6)) \\
    X(spEffectID7, offsetof(::er::paramdef::role_param_st, spEffectID7)) \\
    X(spEffectID8, offsetof(::er::paramdef::role_param_st, spEffectID8)) \\
    X(spEffectID9, offsetof(::er::paramdef::role_param_st, spEffectID9)) \\
    X(sosSignSfxId, offsetof(::er::paramdef::role_param_st, sosSignSfxId)) \\
    X(mySosSignSfxId, offsetof(::er::paramdef::role_param_st, mySosSignSfxId)) \\
    X(summonStartAnimId, offsetof(::er::paramdef::role_param_st, summonStartAnimId)) \\
    X(itemlotParamId, offsetof(::er::paramdef::role_param_st, itemlotParamId)) \\
    X(voiceChatGroup, offsetof(::er::paramdef::role_param_st, voiceChatGroup)) \\
    X(roleNameColor, offsetof(::er::paramdef::role_param_st, roleNameColor)) \\
    X(pad1, offsetof(::er::paramdef::role_param_st, pad1)) \\
    X(roleNameId, offsetof(::er::paramdef::role_param_st, roleNameId)) \\
    X(threatLv, offsetof(::er::paramdef::role_param_st, threatLv)) \\
    X(phantomParamId_vowRank1, offsetof(::er::paramdef::role_param_st, phantomParamId_vowRank1)) \\
    X(phantomParamId_vowRank2, offsetof(::er::paramdef::role_param_st, phantomParamId_vowRank2)) \\
    X(phantomParamId_vowRank3, offsetof(::er::paramdef::role_param_st, phantomParamId_vowRank3)) \\
    X(spEffectID_vowRank0, offsetof(::er::paramdef::role_param_st, spEffectID_vowRank0)) \\
    X(spEffectID_vowRank1, offsetof(::er::paramdef::role_param_st, spEffectID_vowRank1)) \\
    X(spEffectID_vowRank2, offsetof(::er::paramdef::role_param_st, spEffectID_vowRank2)) \\
    X(spEffectID_vowRank3, offsetof(::er::paramdef::role_param_st, spEffectID_vowRank3)) \\
    X(signPhantomId, offsetof(::er::paramdef::role_param_st, signPhantomId)) \\
    X(nonPlayerSummonStartAnimId, offsetof(::er::paramdef::role_param_st, nonPlayerSummonStartAnimId)) \\
    X(pad2, offsetof(::er::paramdef::role_param_st, pad2))

#define ERD_OFFSET_ROLE_PARAM_ST_teamType offsetof(::er::paramdef::role_param_st, teamType)
#define ERD_OFFSET_ROLE_PARAM_ST_pad10 offsetof(::er::paramdef::role_param_st, pad10)
#define ERD_OFFSET_ROLE_PARAM_ST_phantomParamId offsetof(::er::paramdef::role_param_st, phantomParamId)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID0 offsetof(::er::paramdef::role_param_st, spEffectID0)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID1 offsetof(::er::paramdef::role_param_st, spEffectID1)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID2 offsetof(::er::paramdef::role_param_st, spEffectID2)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID3 offsetof(::er::paramdef::role_param_st, spEffectID3)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID4 offsetof(::er::paramdef::role_param_st, spEffectID4)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID5 offsetof(::er::paramdef::role_param_st, spEffectID5)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID6 offsetof(::er::paramdef::role_param_st, spEffectID6)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID7 offsetof(::er::paramdef::role_param_st, spEffectID7)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID8 offsetof(::er::paramdef::role_param_st, spEffectID8)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID9 offsetof(::er::paramdef::role_param_st, spEffectID9)
#define ERD_OFFSET_ROLE_PARAM_ST_sosSignSfxId offsetof(::er::paramdef::role_param_st, sosSignSfxId)
#define ERD_OFFSET_ROLE_PARAM_ST_mySosSignSfxId offsetof(::er::paramdef::role_param_st, mySosSignSfxId)
#define ERD_OFFSET_ROLE_PARAM_ST_summonStartAnimId offsetof(::er::paramdef::role_param_st, summonStartAnimId)
#define ERD_OFFSET_ROLE_PARAM_ST_itemlotParamId offsetof(::er::paramdef::role_param_st, itemlotParamId)
#define ERD_OFFSET_ROLE_PARAM_ST_voiceChatGroup offsetof(::er::paramdef::role_param_st, voiceChatGroup)
#define ERD_OFFSET_ROLE_PARAM_ST_roleNameColor offsetof(::er::paramdef::role_param_st, roleNameColor)
#define ERD_OFFSET_ROLE_PARAM_ST_pad1 offsetof(::er::paramdef::role_param_st, pad1)
#define ERD_OFFSET_ROLE_PARAM_ST_roleNameId offsetof(::er::paramdef::role_param_st, roleNameId)
#define ERD_OFFSET_ROLE_PARAM_ST_threatLv offsetof(::er::paramdef::role_param_st, threatLv)
#define ERD_OFFSET_ROLE_PARAM_ST_phantomParamId_vowRank1 offsetof(::er::paramdef::role_param_st, phantomParamId_vowRank1)
#define ERD_OFFSET_ROLE_PARAM_ST_phantomParamId_vowRank2 offsetof(::er::paramdef::role_param_st, phantomParamId_vowRank2)
#define ERD_OFFSET_ROLE_PARAM_ST_phantomParamId_vowRank3 offsetof(::er::paramdef::role_param_st, phantomParamId_vowRank3)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID_vowRank0 offsetof(::er::paramdef::role_param_st, spEffectID_vowRank0)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID_vowRank1 offsetof(::er::paramdef::role_param_st, spEffectID_vowRank1)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID_vowRank2 offsetof(::er::paramdef::role_param_st, spEffectID_vowRank2)
#define ERD_OFFSET_ROLE_PARAM_ST_spEffectID_vowRank3 offsetof(::er::paramdef::role_param_st, spEffectID_vowRank3)
#define ERD_OFFSET_ROLE_PARAM_ST_signPhantomId offsetof(::er::paramdef::role_param_st, signPhantomId)
#define ERD_OFFSET_ROLE_PARAM_ST_nonPlayerSummonStartAnimId offsetof(::er::paramdef::role_param_st, nonPlayerSummonStartAnimId)
#define ERD_OFFSET_ROLE_PARAM_ST_pad2 offsetof(::er::paramdef::role_param_st, pad2)
