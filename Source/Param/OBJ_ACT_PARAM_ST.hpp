#pragma once

#include <cstddef>
#include <elden-x/paramdef/OBJ_ACT_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_OBJ_ACT_PARAM_ST_FIELDS(X) \\
    X(actionEnableMsgId, offsetof(::er::paramdef::obj_act_param_st, actionEnableMsgId)) \\
    X(actionFailedMsgId, offsetof(::er::paramdef::obj_act_param_st, actionFailedMsgId)) \\
    X(spQualifiedPassEventFlag, offsetof(::er::paramdef::obj_act_param_st, spQualifiedPassEventFlag)) \\
    X(playerAnimId, offsetof(::er::paramdef::obj_act_param_st, playerAnimId)) \\
    X(chrAnimId, offsetof(::er::paramdef::obj_act_param_st, chrAnimId)) \\
    X(validDist, offsetof(::er::paramdef::obj_act_param_st, validDist)) \\
    X(spQualifiedId, offsetof(::er::paramdef::obj_act_param_st, spQualifiedId)) \\
    X(spQualifiedId2, offsetof(::er::paramdef::obj_act_param_st, spQualifiedId2)) \\
    X(objDummyId, offsetof(::er::paramdef::obj_act_param_st, objDummyId)) \\
    X(isEventKickSync, offsetof(::er::paramdef::obj_act_param_st, isEventKickSync)) \\
    X(objAnimId, offsetof(::er::paramdef::obj_act_param_st, objAnimId)) \\
    X(validPlayerAngle, offsetof(::er::paramdef::obj_act_param_st, validPlayerAngle)) \\
    X(spQualifiedType, offsetof(::er::paramdef::obj_act_param_st, spQualifiedType)) \\
    X(spQualifiedType2, offsetof(::er::paramdef::obj_act_param_st, spQualifiedType2)) \\
    X(validObjAngle, offsetof(::er::paramdef::obj_act_param_st, validObjAngle)) \\
    X(chrSorbType, offsetof(::er::paramdef::obj_act_param_st, chrSorbType)) \\
    X(eventKickTiming, offsetof(::er::paramdef::obj_act_param_st, eventKickTiming)) \\
    X(pad1, offsetof(::er::paramdef::obj_act_param_st, pad1)) \\
    X(actionButtonParamId, offsetof(::er::paramdef::obj_act_param_st, actionButtonParamId)) \\
    X(enableTreasureDelaySec, offsetof(::er::paramdef::obj_act_param_st, enableTreasureDelaySec)) \\
    X(preActionSfxDmypolyId, offsetof(::er::paramdef::obj_act_param_st, preActionSfxDmypolyId)) \\
    X(preActionSfxId, offsetof(::er::paramdef::obj_act_param_st, preActionSfxId)) \\
    X(unknown_0x38, offsetof(::er::paramdef::obj_act_param_st, unknown_0x38)) \\
    X(unknown_0x3c, offsetof(::er::paramdef::obj_act_param_st, unknown_0x3c)) \\
    X(unknown_0x40, offsetof(::er::paramdef::obj_act_param_st, unknown_0x40)) \\
    X(unknown_0x44, offsetof(::er::paramdef::obj_act_param_st, unknown_0x44)) \\
    X(unknown_0x48, offsetof(::er::paramdef::obj_act_param_st, unknown_0x48)) \\
    X(unknown_0x4c, offsetof(::er::paramdef::obj_act_param_st, unknown_0x4c)) \\
    X(pad2, offsetof(::er::paramdef::obj_act_param_st, pad2))

#define ERD_OFFSET_OBJ_ACT_PARAM_ST_actionEnableMsgId offsetof(::er::paramdef::obj_act_param_st, actionEnableMsgId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_actionFailedMsgId offsetof(::er::paramdef::obj_act_param_st, actionFailedMsgId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_spQualifiedPassEventFlag offsetof(::er::paramdef::obj_act_param_st, spQualifiedPassEventFlag)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_playerAnimId offsetof(::er::paramdef::obj_act_param_st, playerAnimId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_chrAnimId offsetof(::er::paramdef::obj_act_param_st, chrAnimId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_validDist offsetof(::er::paramdef::obj_act_param_st, validDist)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_spQualifiedId offsetof(::er::paramdef::obj_act_param_st, spQualifiedId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_spQualifiedId2 offsetof(::er::paramdef::obj_act_param_st, spQualifiedId2)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_objDummyId offsetof(::er::paramdef::obj_act_param_st, objDummyId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_isEventKickSync offsetof(::er::paramdef::obj_act_param_st, isEventKickSync)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_objAnimId offsetof(::er::paramdef::obj_act_param_st, objAnimId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_validPlayerAngle offsetof(::er::paramdef::obj_act_param_st, validPlayerAngle)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_spQualifiedType offsetof(::er::paramdef::obj_act_param_st, spQualifiedType)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_spQualifiedType2 offsetof(::er::paramdef::obj_act_param_st, spQualifiedType2)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_validObjAngle offsetof(::er::paramdef::obj_act_param_st, validObjAngle)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_chrSorbType offsetof(::er::paramdef::obj_act_param_st, chrSorbType)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_eventKickTiming offsetof(::er::paramdef::obj_act_param_st, eventKickTiming)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_pad1 offsetof(::er::paramdef::obj_act_param_st, pad1)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_actionButtonParamId offsetof(::er::paramdef::obj_act_param_st, actionButtonParamId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_enableTreasureDelaySec offsetof(::er::paramdef::obj_act_param_st, enableTreasureDelaySec)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_preActionSfxDmypolyId offsetof(::er::paramdef::obj_act_param_st, preActionSfxDmypolyId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_preActionSfxId offsetof(::er::paramdef::obj_act_param_st, preActionSfxId)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_unknown_0x38 offsetof(::er::paramdef::obj_act_param_st, unknown_0x38)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_unknown_0x3c offsetof(::er::paramdef::obj_act_param_st, unknown_0x3c)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_unknown_0x40 offsetof(::er::paramdef::obj_act_param_st, unknown_0x40)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_unknown_0x44 offsetof(::er::paramdef::obj_act_param_st, unknown_0x44)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_unknown_0x48 offsetof(::er::paramdef::obj_act_param_st, unknown_0x48)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_unknown_0x4c offsetof(::er::paramdef::obj_act_param_st, unknown_0x4c)
#define ERD_OFFSET_OBJ_ACT_PARAM_ST_pad2 offsetof(::er::paramdef::obj_act_param_st, pad2)
