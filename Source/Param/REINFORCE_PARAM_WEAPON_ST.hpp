#pragma once

#include <cstddef>
#include <elden-x/paramdef/REINFORCE_PARAM_WEAPON_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_REINFORCE_PARAM_WEAPON_ST_FIELDS(X) \\
    X(physicsAtkRate, offsetof(::er::paramdef::reinforce_param_weapon_st, physicsAtkRate)) \\
    X(magicAtkRate, offsetof(::er::paramdef::reinforce_param_weapon_st, magicAtkRate)) \\
    X(fireAtkRate, offsetof(::er::paramdef::reinforce_param_weapon_st, fireAtkRate)) \\
    X(thunderAtkRate, offsetof(::er::paramdef::reinforce_param_weapon_st, thunderAtkRate)) \\
    X(staminaAtkRate, offsetof(::er::paramdef::reinforce_param_weapon_st, staminaAtkRate)) \\
    X(saWeaponAtkRate, offsetof(::er::paramdef::reinforce_param_weapon_st, saWeaponAtkRate)) \\
    X(saDurabilityRate, offsetof(::er::paramdef::reinforce_param_weapon_st, saDurabilityRate)) \\
    X(correctStrengthRate, offsetof(::er::paramdef::reinforce_param_weapon_st, correctStrengthRate)) \\
    X(correctAgilityRate, offsetof(::er::paramdef::reinforce_param_weapon_st, correctAgilityRate)) \\
    X(correctMagicRate, offsetof(::er::paramdef::reinforce_param_weapon_st, correctMagicRate)) \\
    X(correctFaithRate, offsetof(::er::paramdef::reinforce_param_weapon_st, correctFaithRate)) \\
    X(physicsGuardCutRate, offsetof(::er::paramdef::reinforce_param_weapon_st, physicsGuardCutRate)) \\
    X(magicGuardCutRate, offsetof(::er::paramdef::reinforce_param_weapon_st, magicGuardCutRate)) \\
    X(fireGuardCutRate, offsetof(::er::paramdef::reinforce_param_weapon_st, fireGuardCutRate)) \\
    X(thunderGuardCutRate, offsetof(::er::paramdef::reinforce_param_weapon_st, thunderGuardCutRate)) \\
    X(poisonGuardResistRate, offsetof(::er::paramdef::reinforce_param_weapon_st, poisonGuardResistRate)) \\
    X(diseaseGuardResistRate, offsetof(::er::paramdef::reinforce_param_weapon_st, diseaseGuardResistRate)) \\
    X(bloodGuardResistRate, offsetof(::er::paramdef::reinforce_param_weapon_st, bloodGuardResistRate)) \\
    X(curseGuardResistRate, offsetof(::er::paramdef::reinforce_param_weapon_st, curseGuardResistRate)) \\
    X(staminaGuardDefRate, offsetof(::er::paramdef::reinforce_param_weapon_st, staminaGuardDefRate)) \\
    X(spEffectId1, offsetof(::er::paramdef::reinforce_param_weapon_st, spEffectId1)) \\
    X(spEffectId2, offsetof(::er::paramdef::reinforce_param_weapon_st, spEffectId2)) \\
    X(spEffectId3, offsetof(::er::paramdef::reinforce_param_weapon_st, spEffectId3)) \\
    X(residentSpEffectId1, offsetof(::er::paramdef::reinforce_param_weapon_st, residentSpEffectId1)) \\
    X(residentSpEffectId2, offsetof(::er::paramdef::reinforce_param_weapon_st, residentSpEffectId2)) \\
    X(residentSpEffectId3, offsetof(::er::paramdef::reinforce_param_weapon_st, residentSpEffectId3)) \\
    X(materialSetId, offsetof(::er::paramdef::reinforce_param_weapon_st, materialSetId)) \\
    X(maxReinforceLevel, offsetof(::er::paramdef::reinforce_param_weapon_st, maxReinforceLevel)) \\
    X(darkAtkRate, offsetof(::er::paramdef::reinforce_param_weapon_st, darkAtkRate)) \\
    X(darkGuardCutRate, offsetof(::er::paramdef::reinforce_param_weapon_st, darkGuardCutRate)) \\
    X(correctLuckRate, offsetof(::er::paramdef::reinforce_param_weapon_st, correctLuckRate)) \\
    X(freezeGuardDefRate, offsetof(::er::paramdef::reinforce_param_weapon_st, freezeGuardDefRate)) \\
    X(reinforcePriceRate, offsetof(::er::paramdef::reinforce_param_weapon_st, reinforcePriceRate)) \\
    X(baseChangePriceRate, offsetof(::er::paramdef::reinforce_param_weapon_st, baseChangePriceRate)) \\
    X(enableGemRank, offsetof(::er::paramdef::reinforce_param_weapon_st, enableGemRank)) \\
    X(pad2, offsetof(::er::paramdef::reinforce_param_weapon_st, pad2)) \\
    X(sleepGuardDefRate, offsetof(::er::paramdef::reinforce_param_weapon_st, sleepGuardDefRate)) \\
    X(madnessGuardDefRate, offsetof(::er::paramdef::reinforce_param_weapon_st, madnessGuardDefRate)) \\
    X(baseAtkRate, offsetof(::er::paramdef::reinforce_param_weapon_st, baseAtkRate))

#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_physicsAtkRate offsetof(::er::paramdef::reinforce_param_weapon_st, physicsAtkRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_magicAtkRate offsetof(::er::paramdef::reinforce_param_weapon_st, magicAtkRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_fireAtkRate offsetof(::er::paramdef::reinforce_param_weapon_st, fireAtkRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_thunderAtkRate offsetof(::er::paramdef::reinforce_param_weapon_st, thunderAtkRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_staminaAtkRate offsetof(::er::paramdef::reinforce_param_weapon_st, staminaAtkRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_saWeaponAtkRate offsetof(::er::paramdef::reinforce_param_weapon_st, saWeaponAtkRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_saDurabilityRate offsetof(::er::paramdef::reinforce_param_weapon_st, saDurabilityRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_correctStrengthRate offsetof(::er::paramdef::reinforce_param_weapon_st, correctStrengthRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_correctAgilityRate offsetof(::er::paramdef::reinforce_param_weapon_st, correctAgilityRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_correctMagicRate offsetof(::er::paramdef::reinforce_param_weapon_st, correctMagicRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_correctFaithRate offsetof(::er::paramdef::reinforce_param_weapon_st, correctFaithRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_physicsGuardCutRate offsetof(::er::paramdef::reinforce_param_weapon_st, physicsGuardCutRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_magicGuardCutRate offsetof(::er::paramdef::reinforce_param_weapon_st, magicGuardCutRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_fireGuardCutRate offsetof(::er::paramdef::reinforce_param_weapon_st, fireGuardCutRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_thunderGuardCutRate offsetof(::er::paramdef::reinforce_param_weapon_st, thunderGuardCutRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_poisonGuardResistRate offsetof(::er::paramdef::reinforce_param_weapon_st, poisonGuardResistRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_diseaseGuardResistRate offsetof(::er::paramdef::reinforce_param_weapon_st, diseaseGuardResistRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_bloodGuardResistRate offsetof(::er::paramdef::reinforce_param_weapon_st, bloodGuardResistRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_curseGuardResistRate offsetof(::er::paramdef::reinforce_param_weapon_st, curseGuardResistRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_staminaGuardDefRate offsetof(::er::paramdef::reinforce_param_weapon_st, staminaGuardDefRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_spEffectId1 offsetof(::er::paramdef::reinforce_param_weapon_st, spEffectId1)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_spEffectId2 offsetof(::er::paramdef::reinforce_param_weapon_st, spEffectId2)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_spEffectId3 offsetof(::er::paramdef::reinforce_param_weapon_st, spEffectId3)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_residentSpEffectId1 offsetof(::er::paramdef::reinforce_param_weapon_st, residentSpEffectId1)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_residentSpEffectId2 offsetof(::er::paramdef::reinforce_param_weapon_st, residentSpEffectId2)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_residentSpEffectId3 offsetof(::er::paramdef::reinforce_param_weapon_st, residentSpEffectId3)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_materialSetId offsetof(::er::paramdef::reinforce_param_weapon_st, materialSetId)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_maxReinforceLevel offsetof(::er::paramdef::reinforce_param_weapon_st, maxReinforceLevel)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_darkAtkRate offsetof(::er::paramdef::reinforce_param_weapon_st, darkAtkRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_darkGuardCutRate offsetof(::er::paramdef::reinforce_param_weapon_st, darkGuardCutRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_correctLuckRate offsetof(::er::paramdef::reinforce_param_weapon_st, correctLuckRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_freezeGuardDefRate offsetof(::er::paramdef::reinforce_param_weapon_st, freezeGuardDefRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_reinforcePriceRate offsetof(::er::paramdef::reinforce_param_weapon_st, reinforcePriceRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_baseChangePriceRate offsetof(::er::paramdef::reinforce_param_weapon_st, baseChangePriceRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_enableGemRank offsetof(::er::paramdef::reinforce_param_weapon_st, enableGemRank)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_pad2 offsetof(::er::paramdef::reinforce_param_weapon_st, pad2)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_sleepGuardDefRate offsetof(::er::paramdef::reinforce_param_weapon_st, sleepGuardDefRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_madnessGuardDefRate offsetof(::er::paramdef::reinforce_param_weapon_st, madnessGuardDefRate)
#define ERD_OFFSET_REINFORCE_PARAM_WEAPON_ST_baseAtkRate offsetof(::er::paramdef::reinforce_param_weapon_st, baseAtkRate)
