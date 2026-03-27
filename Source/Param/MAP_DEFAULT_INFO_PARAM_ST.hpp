#pragma once

#include <cstddef>
#include <elden-x/paramdef/MAP_DEFAULT_INFO_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_MAP_DEFAULT_INFO_PARAM_ST_FIELDS(X) \\
    X(disableParamReserve2, offsetof(::er::paramdef::map_default_info_param_st, disableParamReserve2)) \\
    X(EnableFastTravelEventFlagId, offsetof(::er::paramdef::map_default_info_param_st, EnableFastTravelEventFlagId)) \\
    X(WeatherLotTimeOffsetIngameSeconds, offsetof(::er::paramdef::map_default_info_param_st, WeatherLotTimeOffsetIngameSeconds)) \\
    X(WeatherCreateAssetLimitId, offsetof(::er::paramdef::map_default_info_param_st, WeatherCreateAssetLimitId)) \\
    X(MapAiSightType, offsetof(::er::paramdef::map_default_info_param_st, MapAiSightType)) \\
    X(SoundIndoorType, offsetof(::er::paramdef::map_default_info_param_st, SoundIndoorType)) \\
    X(ReverbDefaultType, offsetof(::er::paramdef::map_default_info_param_st, ReverbDefaultType)) \\
    X(BgmPlaceInfo, offsetof(::er::paramdef::map_default_info_param_st, BgmPlaceInfo)) \\
    X(EnvPlaceInfo, offsetof(::er::paramdef::map_default_info_param_st, EnvPlaceInfo)) \\
    X(MapAdditionalSoundBankId, offsetof(::er::paramdef::map_default_info_param_st, MapAdditionalSoundBankId)) \\
    X(MapHeightForSound, offsetof(::er::paramdef::map_default_info_param_st, MapHeightForSound)) \\
    X(IsEnableBlendTimezoneEnvmap, offsetof(::er::paramdef::map_default_info_param_st, IsEnableBlendTimezoneEnvmap)) \\
    X(OverrideGIResolution_XSS, offsetof(::er::paramdef::map_default_info_param_st, OverrideGIResolution_XSS)) \\
    X(MapLoHiChangeBorderDist_XZ, offsetof(::er::paramdef::map_default_info_param_st, MapLoHiChangeBorderDist_XZ)) \\
    X(MapLoHiChangeBorderDist_Y, offsetof(::er::paramdef::map_default_info_param_st, MapLoHiChangeBorderDist_Y)) \\
    X(MapLoHiChangePlayDist, offsetof(::er::paramdef::map_default_info_param_st, MapLoHiChangePlayDist)) \\
    X(MapAutoDrawGroupBackFacePixelNum, offsetof(::er::paramdef::map_default_info_param_st, MapAutoDrawGroupBackFacePixelNum)) \\
    X(PlayerLigntScale, offsetof(::er::paramdef::map_default_info_param_st, PlayerLigntScale)) \\
    X(IsEnableTimezonnePlayerLigntScale, offsetof(::er::paramdef::map_default_info_param_st, IsEnableTimezonnePlayerLigntScale)) \\
    X(isDisableAutoCliffWind, offsetof(::er::paramdef::map_default_info_param_st, isDisableAutoCliffWind)) \\
    X(OpenChrActivateThreshold, offsetof(::er::paramdef::map_default_info_param_st, OpenChrActivateThreshold)) \\
    X(MapMimicryEstablishmentParamId, offsetof(::er::paramdef::map_default_info_param_st, MapMimicryEstablishmentParamId)) \\
    X(OverrideGIResolution_XSX, offsetof(::er::paramdef::map_default_info_param_st, OverrideGIResolution_XSX)) \\
    X(Reserve, offsetof(::er::paramdef::map_default_info_param_st, Reserve))

#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_disableParamReserve2 offsetof(::er::paramdef::map_default_info_param_st, disableParamReserve2)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_EnableFastTravelEventFlagId offsetof(::er::paramdef::map_default_info_param_st, EnableFastTravelEventFlagId)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_WeatherLotTimeOffsetIngameSeconds offsetof(::er::paramdef::map_default_info_param_st, WeatherLotTimeOffsetIngameSeconds)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_WeatherCreateAssetLimitId offsetof(::er::paramdef::map_default_info_param_st, WeatherCreateAssetLimitId)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_MapAiSightType offsetof(::er::paramdef::map_default_info_param_st, MapAiSightType)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_SoundIndoorType offsetof(::er::paramdef::map_default_info_param_st, SoundIndoorType)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_ReverbDefaultType offsetof(::er::paramdef::map_default_info_param_st, ReverbDefaultType)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_BgmPlaceInfo offsetof(::er::paramdef::map_default_info_param_st, BgmPlaceInfo)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_EnvPlaceInfo offsetof(::er::paramdef::map_default_info_param_st, EnvPlaceInfo)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_MapAdditionalSoundBankId offsetof(::er::paramdef::map_default_info_param_st, MapAdditionalSoundBankId)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_MapHeightForSound offsetof(::er::paramdef::map_default_info_param_st, MapHeightForSound)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_IsEnableBlendTimezoneEnvmap offsetof(::er::paramdef::map_default_info_param_st, IsEnableBlendTimezoneEnvmap)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_OverrideGIResolution_XSS offsetof(::er::paramdef::map_default_info_param_st, OverrideGIResolution_XSS)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_MapLoHiChangeBorderDist_XZ offsetof(::er::paramdef::map_default_info_param_st, MapLoHiChangeBorderDist_XZ)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_MapLoHiChangeBorderDist_Y offsetof(::er::paramdef::map_default_info_param_st, MapLoHiChangeBorderDist_Y)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_MapLoHiChangePlayDist offsetof(::er::paramdef::map_default_info_param_st, MapLoHiChangePlayDist)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_MapAutoDrawGroupBackFacePixelNum offsetof(::er::paramdef::map_default_info_param_st, MapAutoDrawGroupBackFacePixelNum)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_PlayerLigntScale offsetof(::er::paramdef::map_default_info_param_st, PlayerLigntScale)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_IsEnableTimezonnePlayerLigntScale offsetof(::er::paramdef::map_default_info_param_st, IsEnableTimezonnePlayerLigntScale)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_isDisableAutoCliffWind offsetof(::er::paramdef::map_default_info_param_st, isDisableAutoCliffWind)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_OpenChrActivateThreshold offsetof(::er::paramdef::map_default_info_param_st, OpenChrActivateThreshold)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_MapMimicryEstablishmentParamId offsetof(::er::paramdef::map_default_info_param_st, MapMimicryEstablishmentParamId)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_OverrideGIResolution_XSX offsetof(::er::paramdef::map_default_info_param_st, OverrideGIResolution_XSX)
#define ERD_OFFSET_MAP_DEFAULT_INFO_PARAM_ST_Reserve offsetof(::er::paramdef::map_default_info_param_st, Reserve)
