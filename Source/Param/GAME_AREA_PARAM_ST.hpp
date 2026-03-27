#pragma once

#include <cstddef>
#include <elden-x/paramdef/GAME_AREA_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_GAME_AREA_PARAM_ST_FIELDS(X) \\
    X(disableParamReserve2, offsetof(::er::paramdef::game_area_param_st, disableParamReserve2)) \\
    X(bonusSoul_single, offsetof(::er::paramdef::game_area_param_st, bonusSoul_single)) \\
    X(bonusSoul_multi, offsetof(::er::paramdef::game_area_param_st, bonusSoul_multi)) \\
    X(humanityPointCountFlagIdTop, offsetof(::er::paramdef::game_area_param_st, humanityPointCountFlagIdTop)) \\
    X(humanityDropPoint1, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint1)) \\
    X(humanityDropPoint2, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint2)) \\
    X(humanityDropPoint3, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint3)) \\
    X(humanityDropPoint4, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint4)) \\
    X(humanityDropPoint5, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint5)) \\
    X(humanityDropPoint6, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint6)) \\
    X(humanityDropPoint7, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint7)) \\
    X(humanityDropPoint8, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint8)) \\
    X(humanityDropPoint9, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint9)) \\
    X(humanityDropPoint10, offsetof(::er::paramdef::game_area_param_st, humanityDropPoint10)) \\
    X(soloBreakInPoint_Min, offsetof(::er::paramdef::game_area_param_st, soloBreakInPoint_Min)) \\
    X(soloBreakInPoint_Max, offsetof(::er::paramdef::game_area_param_st, soloBreakInPoint_Max)) \\
    X(defeatBossFlagId_forSignAimList, offsetof(::er::paramdef::game_area_param_st, defeatBossFlagId_forSignAimList)) \\
    X(displayAimFlagId, offsetof(::er::paramdef::game_area_param_st, displayAimFlagId)) \\
    X(foundBossFlagId, offsetof(::er::paramdef::game_area_param_st, foundBossFlagId)) \\
    X(foundBossTextId, offsetof(::er::paramdef::game_area_param_st, foundBossTextId)) \\
    X(notFindBossTextId, offsetof(::er::paramdef::game_area_param_st, notFindBossTextId)) \\
    X(bossChallengeFlagId, offsetof(::er::paramdef::game_area_param_st, bossChallengeFlagId)) \\
    X(defeatBossFlagId, offsetof(::er::paramdef::game_area_param_st, defeatBossFlagId)) \\
    X(bossPosX, offsetof(::er::paramdef::game_area_param_st, bossPosX)) \\
    X(bossPosY, offsetof(::er::paramdef::game_area_param_st, bossPosY)) \\
    X(bossPosZ, offsetof(::er::paramdef::game_area_param_st, bossPosZ)) \\
    X(bossMapAreaNo, offsetof(::er::paramdef::game_area_param_st, bossMapAreaNo)) \\
    X(bossMapBlockNo, offsetof(::er::paramdef::game_area_param_st, bossMapBlockNo)) \\
    X(bossMapMapNo, offsetof(::er::paramdef::game_area_param_st, bossMapMapNo)) \\
    X(reserve, offsetof(::er::paramdef::game_area_param_st, reserve))

#define ERD_OFFSET_GAME_AREA_PARAM_ST_disableParamReserve2 offsetof(::er::paramdef::game_area_param_st, disableParamReserve2)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_bonusSoul_single offsetof(::er::paramdef::game_area_param_st, bonusSoul_single)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_bonusSoul_multi offsetof(::er::paramdef::game_area_param_st, bonusSoul_multi)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityPointCountFlagIdTop offsetof(::er::paramdef::game_area_param_st, humanityPointCountFlagIdTop)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint1 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint1)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint2 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint2)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint3 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint3)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint4 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint4)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint5 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint5)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint6 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint6)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint7 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint7)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint8 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint8)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint9 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint9)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_humanityDropPoint10 offsetof(::er::paramdef::game_area_param_st, humanityDropPoint10)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_soloBreakInPoint_Min offsetof(::er::paramdef::game_area_param_st, soloBreakInPoint_Min)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_soloBreakInPoint_Max offsetof(::er::paramdef::game_area_param_st, soloBreakInPoint_Max)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_defeatBossFlagId_forSignAimList offsetof(::er::paramdef::game_area_param_st, defeatBossFlagId_forSignAimList)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_displayAimFlagId offsetof(::er::paramdef::game_area_param_st, displayAimFlagId)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_foundBossFlagId offsetof(::er::paramdef::game_area_param_st, foundBossFlagId)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_foundBossTextId offsetof(::er::paramdef::game_area_param_st, foundBossTextId)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_notFindBossTextId offsetof(::er::paramdef::game_area_param_st, notFindBossTextId)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_bossChallengeFlagId offsetof(::er::paramdef::game_area_param_st, bossChallengeFlagId)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_defeatBossFlagId offsetof(::er::paramdef::game_area_param_st, defeatBossFlagId)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_bossPosX offsetof(::er::paramdef::game_area_param_st, bossPosX)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_bossPosY offsetof(::er::paramdef::game_area_param_st, bossPosY)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_bossPosZ offsetof(::er::paramdef::game_area_param_st, bossPosZ)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_bossMapAreaNo offsetof(::er::paramdef::game_area_param_st, bossMapAreaNo)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_bossMapBlockNo offsetof(::er::paramdef::game_area_param_st, bossMapBlockNo)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_bossMapMapNo offsetof(::er::paramdef::game_area_param_st, bossMapMapNo)
#define ERD_OFFSET_GAME_AREA_PARAM_ST_reserve offsetof(::er::paramdef::game_area_param_st, reserve)
