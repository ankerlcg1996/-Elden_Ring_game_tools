#pragma once

#include <cstddef>
#include <elden-x/paramdef/AI_ATTACK_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_AI_ATTACK_PARAM_ST_FIELDS(X) \\
    X(attackTableId, offsetof(::er::paramdef::ai_attack_param_st, attackTableId)) \\
    X(attackId, offsetof(::er::paramdef::ai_attack_param_st, attackId)) \\
    X(successDistance, offsetof(::er::paramdef::ai_attack_param_st, successDistance)) \\
    X(turnTimeBeforeAttack, offsetof(::er::paramdef::ai_attack_param_st, turnTimeBeforeAttack)) \\
    X(frontAngleRange, offsetof(::er::paramdef::ai_attack_param_st, frontAngleRange)) \\
    X(upAngleThreshold, offsetof(::er::paramdef::ai_attack_param_st, upAngleThreshold)) \\
    X(downAngleThershold, offsetof(::er::paramdef::ai_attack_param_st, downAngleThershold)) \\
    X(isFirstAttack, offsetof(::er::paramdef::ai_attack_param_st, isFirstAttack)) \\
    X(doesSelectOnOutRange, offsetof(::er::paramdef::ai_attack_param_st, doesSelectOnOutRange)) \\
    X(minOptimalDistance, offsetof(::er::paramdef::ai_attack_param_st, minOptimalDistance)) \\
    X(maxOptimalDistance, offsetof(::er::paramdef::ai_attack_param_st, maxOptimalDistance)) \\
    X(baseDirectionForOptimalAngle1, offsetof(::er::paramdef::ai_attack_param_st, baseDirectionForOptimalAngle1)) \\
    X(optimalAttackAngleRange1, offsetof(::er::paramdef::ai_attack_param_st, optimalAttackAngleRange1)) \\
    X(baseDirectionForOptimalAngle2, offsetof(::er::paramdef::ai_attack_param_st, baseDirectionForOptimalAngle2)) \\
    X(optimalAttackAngleRange2, offsetof(::er::paramdef::ai_attack_param_st, optimalAttackAngleRange2)) \\
    X(intervalForExec, offsetof(::er::paramdef::ai_attack_param_st, intervalForExec)) \\
    X(selectionTendency, offsetof(::er::paramdef::ai_attack_param_st, selectionTendency)) \\
    X(shortRangeTendency, offsetof(::er::paramdef::ai_attack_param_st, shortRangeTendency)) \\
    X(middleRangeTendency, offsetof(::er::paramdef::ai_attack_param_st, middleRangeTendency)) \\
    X(farRangeTendency, offsetof(::er::paramdef::ai_attack_param_st, farRangeTendency)) \\
    X(outRangeTendency, offsetof(::er::paramdef::ai_attack_param_st, outRangeTendency)) \\
    X(deriveAttackId1, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId1)) \\
    X(deriveAttackId2, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId2)) \\
    X(deriveAttackId3, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId3)) \\
    X(deriveAttackId4, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId4)) \\
    X(deriveAttackId5, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId5)) \\
    X(deriveAttackId6, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId6)) \\
    X(deriveAttackId7, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId7)) \\
    X(deriveAttackId8, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId8)) \\
    X(deriveAttackId9, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId9)) \\
    X(deriveAttackId10, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId10)) \\
    X(deriveAttackId11, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId11)) \\
    X(deriveAttackId12, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId12)) \\
    X(deriveAttackId13, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId13)) \\
    X(deriveAttackId14, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId14)) \\
    X(deriveAttackId15, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId15)) \\
    X(deriveAttackId16, offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId16)) \\
    X(goalLifeMin, offsetof(::er::paramdef::ai_attack_param_st, goalLifeMin)) \\
    X(goalLifeMax, offsetof(::er::paramdef::ai_attack_param_st, goalLifeMax)) \\
    X(doesSelectOnInnerRange, offsetof(::er::paramdef::ai_attack_param_st, doesSelectOnInnerRange)) \\
    X(enableAttackOnBattleStart, offsetof(::er::paramdef::ai_attack_param_st, enableAttackOnBattleStart)) \\
    X(doesSelectOnTargetDown, offsetof(::er::paramdef::ai_attack_param_st, doesSelectOnTargetDown)) \\
    X(pad1, offsetof(::er::paramdef::ai_attack_param_st, pad1)) \\
    X(minArriveDistance, offsetof(::er::paramdef::ai_attack_param_st, minArriveDistance)) \\
    X(maxArriveDistance, offsetof(::er::paramdef::ai_attack_param_st, maxArriveDistance)) \\
    X(comboExecDistance, offsetof(::er::paramdef::ai_attack_param_st, comboExecDistance)) \\
    X(comboExecRange, offsetof(::er::paramdef::ai_attack_param_st, comboExecRange))

#define ERD_OFFSET_AI_ATTACK_PARAM_ST_attackTableId offsetof(::er::paramdef::ai_attack_param_st, attackTableId)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_attackId offsetof(::er::paramdef::ai_attack_param_st, attackId)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_successDistance offsetof(::er::paramdef::ai_attack_param_st, successDistance)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_turnTimeBeforeAttack offsetof(::er::paramdef::ai_attack_param_st, turnTimeBeforeAttack)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_frontAngleRange offsetof(::er::paramdef::ai_attack_param_st, frontAngleRange)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_upAngleThreshold offsetof(::er::paramdef::ai_attack_param_st, upAngleThreshold)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_downAngleThershold offsetof(::er::paramdef::ai_attack_param_st, downAngleThershold)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_isFirstAttack offsetof(::er::paramdef::ai_attack_param_st, isFirstAttack)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_doesSelectOnOutRange offsetof(::er::paramdef::ai_attack_param_st, doesSelectOnOutRange)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_minOptimalDistance offsetof(::er::paramdef::ai_attack_param_st, minOptimalDistance)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_maxOptimalDistance offsetof(::er::paramdef::ai_attack_param_st, maxOptimalDistance)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_baseDirectionForOptimalAngle1 offsetof(::er::paramdef::ai_attack_param_st, baseDirectionForOptimalAngle1)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_optimalAttackAngleRange1 offsetof(::er::paramdef::ai_attack_param_st, optimalAttackAngleRange1)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_baseDirectionForOptimalAngle2 offsetof(::er::paramdef::ai_attack_param_st, baseDirectionForOptimalAngle2)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_optimalAttackAngleRange2 offsetof(::er::paramdef::ai_attack_param_st, optimalAttackAngleRange2)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_intervalForExec offsetof(::er::paramdef::ai_attack_param_st, intervalForExec)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_selectionTendency offsetof(::er::paramdef::ai_attack_param_st, selectionTendency)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_shortRangeTendency offsetof(::er::paramdef::ai_attack_param_st, shortRangeTendency)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_middleRangeTendency offsetof(::er::paramdef::ai_attack_param_st, middleRangeTendency)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_farRangeTendency offsetof(::er::paramdef::ai_attack_param_st, farRangeTendency)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_outRangeTendency offsetof(::er::paramdef::ai_attack_param_st, outRangeTendency)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId1 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId1)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId2 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId2)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId3 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId3)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId4 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId4)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId5 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId5)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId6 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId6)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId7 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId7)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId8 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId8)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId9 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId9)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId10 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId10)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId11 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId11)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId12 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId12)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId13 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId13)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId14 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId14)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId15 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId15)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_deriveAttackId16 offsetof(::er::paramdef::ai_attack_param_st, deriveAttackId16)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_goalLifeMin offsetof(::er::paramdef::ai_attack_param_st, goalLifeMin)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_goalLifeMax offsetof(::er::paramdef::ai_attack_param_st, goalLifeMax)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_doesSelectOnInnerRange offsetof(::er::paramdef::ai_attack_param_st, doesSelectOnInnerRange)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_enableAttackOnBattleStart offsetof(::er::paramdef::ai_attack_param_st, enableAttackOnBattleStart)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_doesSelectOnTargetDown offsetof(::er::paramdef::ai_attack_param_st, doesSelectOnTargetDown)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_pad1 offsetof(::er::paramdef::ai_attack_param_st, pad1)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_minArriveDistance offsetof(::er::paramdef::ai_attack_param_st, minArriveDistance)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_maxArriveDistance offsetof(::er::paramdef::ai_attack_param_st, maxArriveDistance)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_comboExecDistance offsetof(::er::paramdef::ai_attack_param_st, comboExecDistance)
#define ERD_OFFSET_AI_ATTACK_PARAM_ST_comboExecRange offsetof(::er::paramdef::ai_attack_param_st, comboExecRange)
