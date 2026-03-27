#pragma once

#include <cstddef>
#include <elden-x/paramdef/LOAD_BALANCER_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_LOAD_BALANCER_PARAM_ST_FIELDS(X) \\
    X(lowerFpsThreshold, offsetof(::er::paramdef::load_balancer_param_st, lowerFpsThreshold)) \\
    X(upperFpsThreshold, offsetof(::er::paramdef::load_balancer_param_st, upperFpsThreshold)) \\
    X(lowerFpsContinousCount, offsetof(::er::paramdef::load_balancer_param_st, lowerFpsContinousCount)) \\
    X(upperFpsContinousCount, offsetof(::er::paramdef::load_balancer_param_st, upperFpsContinousCount)) \\
    X(downAfterChangeSleep, offsetof(::er::paramdef::load_balancer_param_st, downAfterChangeSleep)) \\
    X(upAfterChangeSleep, offsetof(::er::paramdef::load_balancer_param_st, upAfterChangeSleep)) \\
    X(postProcessLightShaft, offsetof(::er::paramdef::load_balancer_param_st, postProcessLightShaft)) \\
    X(postProcessBloom, offsetof(::er::paramdef::load_balancer_param_st, postProcessBloom)) \\
    X(postProcessGlow, offsetof(::er::paramdef::load_balancer_param_st, postProcessGlow)) \\
    X(postProcessAA, offsetof(::er::paramdef::load_balancer_param_st, postProcessAA)) \\
    X(postProcessSSAO, offsetof(::er::paramdef::load_balancer_param_st, postProcessSSAO)) \\
    X(postProcessDOF, offsetof(::er::paramdef::load_balancer_param_st, postProcessDOF)) \\
    X(postProcessMotionBlur, offsetof(::er::paramdef::load_balancer_param_st, postProcessMotionBlur)) \\
    X(postProcessMotionBlurIteration, offsetof(::er::paramdef::load_balancer_param_st, postProcessMotionBlurIteration)) \\
    X(reserve0, offsetof(::er::paramdef::load_balancer_param_st, reserve0)) \\
    X(shadowBlur, offsetof(::er::paramdef::load_balancer_param_st, shadowBlur)) \\
    X(sfxParticleHalf, offsetof(::er::paramdef::load_balancer_param_st, sfxParticleHalf)) \\
    X(sfxReflection, offsetof(::er::paramdef::load_balancer_param_st, sfxReflection)) \\
    X(sfxWaterInteraction, offsetof(::er::paramdef::load_balancer_param_st, sfxWaterInteraction)) \\
    X(sfxGlow, offsetof(::er::paramdef::load_balancer_param_st, sfxGlow)) \\
    X(sfxDistortion, offsetof(::er::paramdef::load_balancer_param_st, sfxDistortion)) \\
    X(sftSoftSprite, offsetof(::er::paramdef::load_balancer_param_st, sftSoftSprite)) \\
    X(sfxLightShaft, offsetof(::er::paramdef::load_balancer_param_st, sfxLightShaft)) \\
    X(sfxScaleRenderDistanceScale, offsetof(::er::paramdef::load_balancer_param_st, sfxScaleRenderDistanceScale)) \\
    X(dynamicResolution, offsetof(::er::paramdef::load_balancer_param_st, dynamicResolution)) \\
    X(shadowCascade0ResolutionHalf, offsetof(::er::paramdef::load_balancer_param_st, shadowCascade0ResolutionHalf)) \\
    X(shadowCascade1ResolutionHalf, offsetof(::er::paramdef::load_balancer_param_st, shadowCascade1ResolutionHalf)) \\
    X(chrWetDisablePlayer, offsetof(::er::paramdef::load_balancer_param_st, chrWetDisablePlayer)) \\
    X(chrWetDisableRemotePlayer, offsetof(::er::paramdef::load_balancer_param_st, chrWetDisableRemotePlayer)) \\
    X(chrWetDisableEnemy, offsetof(::er::paramdef::load_balancer_param_st, chrWetDisableEnemy)) \\
    X(dynamicResolutionPercentageMin, offsetof(::er::paramdef::load_balancer_param_st, dynamicResolutionPercentageMin)) \\
    X(dynamicResolutionPercentageMax, offsetof(::er::paramdef::load_balancer_param_st, dynamicResolutionPercentageMax)) \\
    X(reserve1, offsetof(::er::paramdef::load_balancer_param_st, reserve1))

#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_lowerFpsThreshold offsetof(::er::paramdef::load_balancer_param_st, lowerFpsThreshold)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_upperFpsThreshold offsetof(::er::paramdef::load_balancer_param_st, upperFpsThreshold)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_lowerFpsContinousCount offsetof(::er::paramdef::load_balancer_param_st, lowerFpsContinousCount)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_upperFpsContinousCount offsetof(::er::paramdef::load_balancer_param_st, upperFpsContinousCount)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_downAfterChangeSleep offsetof(::er::paramdef::load_balancer_param_st, downAfterChangeSleep)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_upAfterChangeSleep offsetof(::er::paramdef::load_balancer_param_st, upAfterChangeSleep)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_postProcessLightShaft offsetof(::er::paramdef::load_balancer_param_st, postProcessLightShaft)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_postProcessBloom offsetof(::er::paramdef::load_balancer_param_st, postProcessBloom)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_postProcessGlow offsetof(::er::paramdef::load_balancer_param_st, postProcessGlow)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_postProcessAA offsetof(::er::paramdef::load_balancer_param_st, postProcessAA)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_postProcessSSAO offsetof(::er::paramdef::load_balancer_param_st, postProcessSSAO)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_postProcessDOF offsetof(::er::paramdef::load_balancer_param_st, postProcessDOF)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_postProcessMotionBlur offsetof(::er::paramdef::load_balancer_param_st, postProcessMotionBlur)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_postProcessMotionBlurIteration offsetof(::er::paramdef::load_balancer_param_st, postProcessMotionBlurIteration)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_reserve0 offsetof(::er::paramdef::load_balancer_param_st, reserve0)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_shadowBlur offsetof(::er::paramdef::load_balancer_param_st, shadowBlur)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_sfxParticleHalf offsetof(::er::paramdef::load_balancer_param_st, sfxParticleHalf)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_sfxReflection offsetof(::er::paramdef::load_balancer_param_st, sfxReflection)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_sfxWaterInteraction offsetof(::er::paramdef::load_balancer_param_st, sfxWaterInteraction)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_sfxGlow offsetof(::er::paramdef::load_balancer_param_st, sfxGlow)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_sfxDistortion offsetof(::er::paramdef::load_balancer_param_st, sfxDistortion)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_sftSoftSprite offsetof(::er::paramdef::load_balancer_param_st, sftSoftSprite)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_sfxLightShaft offsetof(::er::paramdef::load_balancer_param_st, sfxLightShaft)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_sfxScaleRenderDistanceScale offsetof(::er::paramdef::load_balancer_param_st, sfxScaleRenderDistanceScale)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_dynamicResolution offsetof(::er::paramdef::load_balancer_param_st, dynamicResolution)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_shadowCascade0ResolutionHalf offsetof(::er::paramdef::load_balancer_param_st, shadowCascade0ResolutionHalf)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_shadowCascade1ResolutionHalf offsetof(::er::paramdef::load_balancer_param_st, shadowCascade1ResolutionHalf)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_chrWetDisablePlayer offsetof(::er::paramdef::load_balancer_param_st, chrWetDisablePlayer)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_chrWetDisableRemotePlayer offsetof(::er::paramdef::load_balancer_param_st, chrWetDisableRemotePlayer)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_chrWetDisableEnemy offsetof(::er::paramdef::load_balancer_param_st, chrWetDisableEnemy)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_dynamicResolutionPercentageMin offsetof(::er::paramdef::load_balancer_param_st, dynamicResolutionPercentageMin)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_dynamicResolutionPercentageMax offsetof(::er::paramdef::load_balancer_param_st, dynamicResolutionPercentageMax)
#define ERD_OFFSET_LOAD_BALANCER_PARAM_ST_reserve1 offsetof(::er::paramdef::load_balancer_param_st, reserve1)
