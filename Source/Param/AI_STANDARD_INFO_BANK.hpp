#pragma once

#include <cstddef>
#include <elden-x/paramdef/AI_STANDARD_INFO_BANK.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_AI_STANDARD_INFO_BANK_FIELDS(X) \\
    X(RadarRange, offsetof(::er::paramdef::ai_standard_info_bank, RadarRange)) \\
    X(RadarAngleX, offsetof(::er::paramdef::ai_standard_info_bank, RadarAngleX)) \\
    X(RadarAngleY, offsetof(::er::paramdef::ai_standard_info_bank, RadarAngleY)) \\
    X(TerritorySize, offsetof(::er::paramdef::ai_standard_info_bank, TerritorySize)) \\
    X(ThreatBeforeAttackRate, offsetof(::er::paramdef::ai_standard_info_bank, ThreatBeforeAttackRate)) \\
    X(ForceThreatOnFirstLocked, offsetof(::er::paramdef::ai_standard_info_bank, ForceThreatOnFirstLocked)) \\
    X(reserve0, offsetof(::er::paramdef::ai_standard_info_bank, reserve0)) \\
    X(Attack1_Distance, offsetof(::er::paramdef::ai_standard_info_bank, Attack1_Distance)) \\
    X(Attack1_Margin, offsetof(::er::paramdef::ai_standard_info_bank, Attack1_Margin)) \\
    X(Attack1_Rate, offsetof(::er::paramdef::ai_standard_info_bank, Attack1_Rate)) \\
    X(Attack1_ActionID, offsetof(::er::paramdef::ai_standard_info_bank, Attack1_ActionID)) \\
    X(Attack1_DelayMin, offsetof(::er::paramdef::ai_standard_info_bank, Attack1_DelayMin)) \\
    X(Attack1_DelayMax, offsetof(::er::paramdef::ai_standard_info_bank, Attack1_DelayMax)) \\
    X(Attack1_ConeAngle, offsetof(::er::paramdef::ai_standard_info_bank, Attack1_ConeAngle)) \\
    X(reserve10, offsetof(::er::paramdef::ai_standard_info_bank, reserve10)) \\
    X(Attack2_Distance, offsetof(::er::paramdef::ai_standard_info_bank, Attack2_Distance)) \\
    X(Attack2_Margin, offsetof(::er::paramdef::ai_standard_info_bank, Attack2_Margin)) \\
    X(Attack2_Rate, offsetof(::er::paramdef::ai_standard_info_bank, Attack2_Rate)) \\
    X(Attack2_ActionID, offsetof(::er::paramdef::ai_standard_info_bank, Attack2_ActionID)) \\
    X(Attack2_DelayMin, offsetof(::er::paramdef::ai_standard_info_bank, Attack2_DelayMin)) \\
    X(Attack2_DelayMax, offsetof(::er::paramdef::ai_standard_info_bank, Attack2_DelayMax)) \\
    X(Attack2_ConeAngle, offsetof(::er::paramdef::ai_standard_info_bank, Attack2_ConeAngle)) \\
    X(reserve11, offsetof(::er::paramdef::ai_standard_info_bank, reserve11)) \\
    X(Attack3_Distance, offsetof(::er::paramdef::ai_standard_info_bank, Attack3_Distance)) \\
    X(Attack3_Margin, offsetof(::er::paramdef::ai_standard_info_bank, Attack3_Margin)) \\
    X(Attack3_Rate, offsetof(::er::paramdef::ai_standard_info_bank, Attack3_Rate)) \\
    X(Attack3_ActionID, offsetof(::er::paramdef::ai_standard_info_bank, Attack3_ActionID)) \\
    X(Attack3_DelayMin, offsetof(::er::paramdef::ai_standard_info_bank, Attack3_DelayMin)) \\
    X(Attack3_DelayMax, offsetof(::er::paramdef::ai_standard_info_bank, Attack3_DelayMax)) \\
    X(Attack3_ConeAngle, offsetof(::er::paramdef::ai_standard_info_bank, Attack3_ConeAngle)) \\
    X(reserve12, offsetof(::er::paramdef::ai_standard_info_bank, reserve12)) \\
    X(Attack4_Distance, offsetof(::er::paramdef::ai_standard_info_bank, Attack4_Distance)) \\
    X(Attack4_Margin, offsetof(::er::paramdef::ai_standard_info_bank, Attack4_Margin)) \\
    X(Attack4_Rate, offsetof(::er::paramdef::ai_standard_info_bank, Attack4_Rate)) \\
    X(Attack4_ActionID, offsetof(::er::paramdef::ai_standard_info_bank, Attack4_ActionID)) \\
    X(Attack4_DelayMin, offsetof(::er::paramdef::ai_standard_info_bank, Attack4_DelayMin)) \\
    X(Attack4_DelayMax, offsetof(::er::paramdef::ai_standard_info_bank, Attack4_DelayMax)) \\
    X(Attack4_ConeAngle, offsetof(::er::paramdef::ai_standard_info_bank, Attack4_ConeAngle)) \\
    X(reserve13, offsetof(::er::paramdef::ai_standard_info_bank, reserve13)) \\
    X(reserve_last, offsetof(::er::paramdef::ai_standard_info_bank, reserve_last))

#define ERD_OFFSET_AI_STANDARD_INFO_BANK_RadarRange offsetof(::er::paramdef::ai_standard_info_bank, RadarRange)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_RadarAngleX offsetof(::er::paramdef::ai_standard_info_bank, RadarAngleX)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_RadarAngleY offsetof(::er::paramdef::ai_standard_info_bank, RadarAngleY)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_TerritorySize offsetof(::er::paramdef::ai_standard_info_bank, TerritorySize)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_ThreatBeforeAttackRate offsetof(::er::paramdef::ai_standard_info_bank, ThreatBeforeAttackRate)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_ForceThreatOnFirstLocked offsetof(::er::paramdef::ai_standard_info_bank, ForceThreatOnFirstLocked)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_reserve0 offsetof(::er::paramdef::ai_standard_info_bank, reserve0)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack1_Distance offsetof(::er::paramdef::ai_standard_info_bank, Attack1_Distance)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack1_Margin offsetof(::er::paramdef::ai_standard_info_bank, Attack1_Margin)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack1_Rate offsetof(::er::paramdef::ai_standard_info_bank, Attack1_Rate)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack1_ActionID offsetof(::er::paramdef::ai_standard_info_bank, Attack1_ActionID)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack1_DelayMin offsetof(::er::paramdef::ai_standard_info_bank, Attack1_DelayMin)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack1_DelayMax offsetof(::er::paramdef::ai_standard_info_bank, Attack1_DelayMax)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack1_ConeAngle offsetof(::er::paramdef::ai_standard_info_bank, Attack1_ConeAngle)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_reserve10 offsetof(::er::paramdef::ai_standard_info_bank, reserve10)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack2_Distance offsetof(::er::paramdef::ai_standard_info_bank, Attack2_Distance)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack2_Margin offsetof(::er::paramdef::ai_standard_info_bank, Attack2_Margin)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack2_Rate offsetof(::er::paramdef::ai_standard_info_bank, Attack2_Rate)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack2_ActionID offsetof(::er::paramdef::ai_standard_info_bank, Attack2_ActionID)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack2_DelayMin offsetof(::er::paramdef::ai_standard_info_bank, Attack2_DelayMin)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack2_DelayMax offsetof(::er::paramdef::ai_standard_info_bank, Attack2_DelayMax)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack2_ConeAngle offsetof(::er::paramdef::ai_standard_info_bank, Attack2_ConeAngle)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_reserve11 offsetof(::er::paramdef::ai_standard_info_bank, reserve11)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack3_Distance offsetof(::er::paramdef::ai_standard_info_bank, Attack3_Distance)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack3_Margin offsetof(::er::paramdef::ai_standard_info_bank, Attack3_Margin)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack3_Rate offsetof(::er::paramdef::ai_standard_info_bank, Attack3_Rate)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack3_ActionID offsetof(::er::paramdef::ai_standard_info_bank, Attack3_ActionID)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack3_DelayMin offsetof(::er::paramdef::ai_standard_info_bank, Attack3_DelayMin)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack3_DelayMax offsetof(::er::paramdef::ai_standard_info_bank, Attack3_DelayMax)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack3_ConeAngle offsetof(::er::paramdef::ai_standard_info_bank, Attack3_ConeAngle)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_reserve12 offsetof(::er::paramdef::ai_standard_info_bank, reserve12)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack4_Distance offsetof(::er::paramdef::ai_standard_info_bank, Attack4_Distance)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack4_Margin offsetof(::er::paramdef::ai_standard_info_bank, Attack4_Margin)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack4_Rate offsetof(::er::paramdef::ai_standard_info_bank, Attack4_Rate)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack4_ActionID offsetof(::er::paramdef::ai_standard_info_bank, Attack4_ActionID)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack4_DelayMin offsetof(::er::paramdef::ai_standard_info_bank, Attack4_DelayMin)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack4_DelayMax offsetof(::er::paramdef::ai_standard_info_bank, Attack4_DelayMax)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_Attack4_ConeAngle offsetof(::er::paramdef::ai_standard_info_bank, Attack4_ConeAngle)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_reserve13 offsetof(::er::paramdef::ai_standard_info_bank, reserve13)
#define ERD_OFFSET_AI_STANDARD_INFO_BANK_reserve_last offsetof(::er::paramdef::ai_standard_info_bank, reserve_last)
