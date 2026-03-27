#pragma once

#include <cstddef>
#include <elden-x/paramdef/MOVE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_MOVE_PARAM_ST_FIELDS(X) \\
    X(stayId, offsetof(::er::paramdef::move_param_st, stayId)) \\
    X(walkF, offsetof(::er::paramdef::move_param_st, walkF)) \\
    X(walkB, offsetof(::er::paramdef::move_param_st, walkB)) \\
    X(walkL, offsetof(::er::paramdef::move_param_st, walkL)) \\
    X(walkR, offsetof(::er::paramdef::move_param_st, walkR)) \\
    X(dashF, offsetof(::er::paramdef::move_param_st, dashF)) \\
    X(dashB, offsetof(::er::paramdef::move_param_st, dashB)) \\
    X(dashL, offsetof(::er::paramdef::move_param_st, dashL)) \\
    X(dashR, offsetof(::er::paramdef::move_param_st, dashR)) \\
    X(superDash, offsetof(::er::paramdef::move_param_st, superDash)) \\
    X(escapeF, offsetof(::er::paramdef::move_param_st, escapeF)) \\
    X(escapeB, offsetof(::er::paramdef::move_param_st, escapeB)) \\
    X(escapeL, offsetof(::er::paramdef::move_param_st, escapeL)) \\
    X(escapeR, offsetof(::er::paramdef::move_param_st, escapeR)) \\
    X(turnL, offsetof(::er::paramdef::move_param_st, turnL)) \\
    X(trunR, offsetof(::er::paramdef::move_param_st, trunR)) \\
    X(largeTurnL, offsetof(::er::paramdef::move_param_st, largeTurnL)) \\
    X(largeTurnR, offsetof(::er::paramdef::move_param_st, largeTurnR)) \\
    X(stepMove, offsetof(::er::paramdef::move_param_st, stepMove)) \\
    X(flyStay, offsetof(::er::paramdef::move_param_st, flyStay)) \\
    X(flyWalkF, offsetof(::er::paramdef::move_param_st, flyWalkF)) \\
    X(flyWalkFL, offsetof(::er::paramdef::move_param_st, flyWalkFL)) \\
    X(flyWalkFR, offsetof(::er::paramdef::move_param_st, flyWalkFR)) \\
    X(flyWalkFL2, offsetof(::er::paramdef::move_param_st, flyWalkFL2)) \\
    X(flyWalkFR2, offsetof(::er::paramdef::move_param_st, flyWalkFR2)) \\
    X(flyDashF, offsetof(::er::paramdef::move_param_st, flyDashF)) \\
    X(flyDashFL, offsetof(::er::paramdef::move_param_st, flyDashFL)) \\
    X(flyDashFR, offsetof(::er::paramdef::move_param_st, flyDashFR)) \\
    X(flyDashFL2, offsetof(::er::paramdef::move_param_st, flyDashFL2)) \\
    X(flyDashFR2, offsetof(::er::paramdef::move_param_st, flyDashFR2)) \\
    X(dashEscapeF, offsetof(::er::paramdef::move_param_st, dashEscapeF)) \\
    X(dashEscapeB, offsetof(::er::paramdef::move_param_st, dashEscapeB)) \\
    X(dashEscapeL, offsetof(::er::paramdef::move_param_st, dashEscapeL)) \\
    X(dashEscapeR, offsetof(::er::paramdef::move_param_st, dashEscapeR)) \\
    X(analogMoveParamId, offsetof(::er::paramdef::move_param_st, analogMoveParamId)) \\
    X(turnNoAnimAngle, offsetof(::er::paramdef::move_param_st, turnNoAnimAngle)) \\
    X(turn45Angle, offsetof(::er::paramdef::move_param_st, turn45Angle)) \\
    X(turn90Angle, offsetof(::er::paramdef::move_param_st, turn90Angle)) \\
    X(turnWaitNoAnimAngle, offsetof(::er::paramdef::move_param_st, turnWaitNoAnimAngle))

#define ERD_OFFSET_MOVE_PARAM_ST_stayId offsetof(::er::paramdef::move_param_st, stayId)
#define ERD_OFFSET_MOVE_PARAM_ST_walkF offsetof(::er::paramdef::move_param_st, walkF)
#define ERD_OFFSET_MOVE_PARAM_ST_walkB offsetof(::er::paramdef::move_param_st, walkB)
#define ERD_OFFSET_MOVE_PARAM_ST_walkL offsetof(::er::paramdef::move_param_st, walkL)
#define ERD_OFFSET_MOVE_PARAM_ST_walkR offsetof(::er::paramdef::move_param_st, walkR)
#define ERD_OFFSET_MOVE_PARAM_ST_dashF offsetof(::er::paramdef::move_param_st, dashF)
#define ERD_OFFSET_MOVE_PARAM_ST_dashB offsetof(::er::paramdef::move_param_st, dashB)
#define ERD_OFFSET_MOVE_PARAM_ST_dashL offsetof(::er::paramdef::move_param_st, dashL)
#define ERD_OFFSET_MOVE_PARAM_ST_dashR offsetof(::er::paramdef::move_param_st, dashR)
#define ERD_OFFSET_MOVE_PARAM_ST_superDash offsetof(::er::paramdef::move_param_st, superDash)
#define ERD_OFFSET_MOVE_PARAM_ST_escapeF offsetof(::er::paramdef::move_param_st, escapeF)
#define ERD_OFFSET_MOVE_PARAM_ST_escapeB offsetof(::er::paramdef::move_param_st, escapeB)
#define ERD_OFFSET_MOVE_PARAM_ST_escapeL offsetof(::er::paramdef::move_param_st, escapeL)
#define ERD_OFFSET_MOVE_PARAM_ST_escapeR offsetof(::er::paramdef::move_param_st, escapeR)
#define ERD_OFFSET_MOVE_PARAM_ST_turnL offsetof(::er::paramdef::move_param_st, turnL)
#define ERD_OFFSET_MOVE_PARAM_ST_trunR offsetof(::er::paramdef::move_param_st, trunR)
#define ERD_OFFSET_MOVE_PARAM_ST_largeTurnL offsetof(::er::paramdef::move_param_st, largeTurnL)
#define ERD_OFFSET_MOVE_PARAM_ST_largeTurnR offsetof(::er::paramdef::move_param_st, largeTurnR)
#define ERD_OFFSET_MOVE_PARAM_ST_stepMove offsetof(::er::paramdef::move_param_st, stepMove)
#define ERD_OFFSET_MOVE_PARAM_ST_flyStay offsetof(::er::paramdef::move_param_st, flyStay)
#define ERD_OFFSET_MOVE_PARAM_ST_flyWalkF offsetof(::er::paramdef::move_param_st, flyWalkF)
#define ERD_OFFSET_MOVE_PARAM_ST_flyWalkFL offsetof(::er::paramdef::move_param_st, flyWalkFL)
#define ERD_OFFSET_MOVE_PARAM_ST_flyWalkFR offsetof(::er::paramdef::move_param_st, flyWalkFR)
#define ERD_OFFSET_MOVE_PARAM_ST_flyWalkFL2 offsetof(::er::paramdef::move_param_st, flyWalkFL2)
#define ERD_OFFSET_MOVE_PARAM_ST_flyWalkFR2 offsetof(::er::paramdef::move_param_st, flyWalkFR2)
#define ERD_OFFSET_MOVE_PARAM_ST_flyDashF offsetof(::er::paramdef::move_param_st, flyDashF)
#define ERD_OFFSET_MOVE_PARAM_ST_flyDashFL offsetof(::er::paramdef::move_param_st, flyDashFL)
#define ERD_OFFSET_MOVE_PARAM_ST_flyDashFR offsetof(::er::paramdef::move_param_st, flyDashFR)
#define ERD_OFFSET_MOVE_PARAM_ST_flyDashFL2 offsetof(::er::paramdef::move_param_st, flyDashFL2)
#define ERD_OFFSET_MOVE_PARAM_ST_flyDashFR2 offsetof(::er::paramdef::move_param_st, flyDashFR2)
#define ERD_OFFSET_MOVE_PARAM_ST_dashEscapeF offsetof(::er::paramdef::move_param_st, dashEscapeF)
#define ERD_OFFSET_MOVE_PARAM_ST_dashEscapeB offsetof(::er::paramdef::move_param_st, dashEscapeB)
#define ERD_OFFSET_MOVE_PARAM_ST_dashEscapeL offsetof(::er::paramdef::move_param_st, dashEscapeL)
#define ERD_OFFSET_MOVE_PARAM_ST_dashEscapeR offsetof(::er::paramdef::move_param_st, dashEscapeR)
#define ERD_OFFSET_MOVE_PARAM_ST_analogMoveParamId offsetof(::er::paramdef::move_param_st, analogMoveParamId)
#define ERD_OFFSET_MOVE_PARAM_ST_turnNoAnimAngle offsetof(::er::paramdef::move_param_st, turnNoAnimAngle)
#define ERD_OFFSET_MOVE_PARAM_ST_turn45Angle offsetof(::er::paramdef::move_param_st, turn45Angle)
#define ERD_OFFSET_MOVE_PARAM_ST_turn90Angle offsetof(::er::paramdef::move_param_st, turn90Angle)
#define ERD_OFFSET_MOVE_PARAM_ST_turnWaitNoAnimAngle offsetof(::er::paramdef::move_param_st, turnWaitNoAnimAngle)
