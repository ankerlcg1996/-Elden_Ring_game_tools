#pragma once

#include <cstddef>
#include <elden-x/paramdef/LOCK_CAM_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_LOCK_CAM_PARAM_ST_FIELDS(X) \\
    X(camDistTarget, offsetof(::er::paramdef::lock_cam_param_st, camDistTarget)) \\
    X(rotRangeMinX, offsetof(::er::paramdef::lock_cam_param_st, rotRangeMinX)) \\
    X(lockRotXShiftRatio, offsetof(::er::paramdef::lock_cam_param_st, lockRotXShiftRatio)) \\
    X(chrOrgOffset_Y, offsetof(::er::paramdef::lock_cam_param_st, chrOrgOffset_Y)) \\
    X(chrLockRangeMaxRadius, offsetof(::er::paramdef::lock_cam_param_st, chrLockRangeMaxRadius)) \\
    X(camFovY, offsetof(::er::paramdef::lock_cam_param_st, camFovY)) \\
    X(chrLockRangeMaxRadius_forD, offsetof(::er::paramdef::lock_cam_param_st, chrLockRangeMaxRadius_forD)) \\
    X(chrLockRangeMaxRadius_forPD, offsetof(::er::paramdef::lock_cam_param_st, chrLockRangeMaxRadius_forPD)) \\
    X(closeMaxHeight, offsetof(::er::paramdef::lock_cam_param_st, closeMaxHeight)) \\
    X(closeMinHeight, offsetof(::er::paramdef::lock_cam_param_st, closeMinHeight)) \\
    X(closeAngRange, offsetof(::er::paramdef::lock_cam_param_st, closeAngRange)) \\
    X(closeMaxRadius, offsetof(::er::paramdef::lock_cam_param_st, closeMaxRadius)) \\
    X(closeMaxRadius_forD, offsetof(::er::paramdef::lock_cam_param_st, closeMaxRadius_forD)) \\
    X(closeMaxRadius_forPD, offsetof(::er::paramdef::lock_cam_param_st, closeMaxRadius_forPD)) \\
    X(bulletMaxRadius, offsetof(::er::paramdef::lock_cam_param_st, bulletMaxRadius)) \\
    X(bulletMaxRadius_forD, offsetof(::er::paramdef::lock_cam_param_st, bulletMaxRadius_forD)) \\
    X(bulletMaxRadius_forPD, offsetof(::er::paramdef::lock_cam_param_st, bulletMaxRadius_forPD)) \\
    X(bulletAngRange, offsetof(::er::paramdef::lock_cam_param_st, bulletAngRange)) \\
    X(lockTgtKeepTime, offsetof(::er::paramdef::lock_cam_param_st, lockTgtKeepTime)) \\
    X(chrTransChaseRateForNormal, offsetof(::er::paramdef::lock_cam_param_st, chrTransChaseRateForNormal)) \\
    X(pad, offsetof(::er::paramdef::lock_cam_param_st, pad))

#define ERD_OFFSET_LOCK_CAM_PARAM_ST_camDistTarget offsetof(::er::paramdef::lock_cam_param_st, camDistTarget)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_rotRangeMinX offsetof(::er::paramdef::lock_cam_param_st, rotRangeMinX)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_lockRotXShiftRatio offsetof(::er::paramdef::lock_cam_param_st, lockRotXShiftRatio)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_chrOrgOffset_Y offsetof(::er::paramdef::lock_cam_param_st, chrOrgOffset_Y)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_chrLockRangeMaxRadius offsetof(::er::paramdef::lock_cam_param_st, chrLockRangeMaxRadius)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_camFovY offsetof(::er::paramdef::lock_cam_param_st, camFovY)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_chrLockRangeMaxRadius_forD offsetof(::er::paramdef::lock_cam_param_st, chrLockRangeMaxRadius_forD)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_chrLockRangeMaxRadius_forPD offsetof(::er::paramdef::lock_cam_param_st, chrLockRangeMaxRadius_forPD)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_closeMaxHeight offsetof(::er::paramdef::lock_cam_param_st, closeMaxHeight)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_closeMinHeight offsetof(::er::paramdef::lock_cam_param_st, closeMinHeight)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_closeAngRange offsetof(::er::paramdef::lock_cam_param_st, closeAngRange)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_closeMaxRadius offsetof(::er::paramdef::lock_cam_param_st, closeMaxRadius)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_closeMaxRadius_forD offsetof(::er::paramdef::lock_cam_param_st, closeMaxRadius_forD)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_closeMaxRadius_forPD offsetof(::er::paramdef::lock_cam_param_st, closeMaxRadius_forPD)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_bulletMaxRadius offsetof(::er::paramdef::lock_cam_param_st, bulletMaxRadius)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_bulletMaxRadius_forD offsetof(::er::paramdef::lock_cam_param_st, bulletMaxRadius_forD)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_bulletMaxRadius_forPD offsetof(::er::paramdef::lock_cam_param_st, bulletMaxRadius_forPD)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_bulletAngRange offsetof(::er::paramdef::lock_cam_param_st, bulletAngRange)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_lockTgtKeepTime offsetof(::er::paramdef::lock_cam_param_st, lockTgtKeepTime)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_chrTransChaseRateForNormal offsetof(::er::paramdef::lock_cam_param_st, chrTransChaseRateForNormal)
#define ERD_OFFSET_LOCK_CAM_PARAM_ST_pad offsetof(::er::paramdef::lock_cam_param_st, pad)
