#pragma once

#include <cstddef>
#include <elden-x/paramdef/CUTSCENE_GPARAM_WEATHER_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_CUTSCENE_GPARAM_WEATHER_PARAM_ST_FIELDS(X) \\
    X(disableParamReserve2, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, disableParamReserve2)) \\
    X(DstWeather_Sunny, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Sunny)) \\
    X(DstWeather_ClearSky, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_ClearSky)) \\
    X(DstWeather_WeakCloudy, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_WeakCloudy)) \\
    X(DstWeather_Cloud, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Cloud)) \\
    X(DstWeather_Rain, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Rain)) \\
    X(DstWeather_HeavyRain, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_HeavyRain)) \\
    X(DstWeather_Storm, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Storm)) \\
    X(DstWeather_StormForBattle, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_StormForBattle)) \\
    X(DstWeather_Snow, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Snow)) \\
    X(DstWeather_HeavySnow, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_HeavySnow)) \\
    X(DstWeather_Fog, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Fog)) \\
    X(DstWeather_HeavyFog, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_HeavyFog)) \\
    X(DstWeather_SandStorm, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_SandStorm)) \\
    X(DstWeather_HeavyFogRain, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_HeavyFogRain)) \\
    X(PostPlayIngameWeather, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, PostPlayIngameWeather)) \\
    X(IndoorOutdoorType, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, IndoorOutdoorType)) \\
    X(TakeOverDstWeather_Sunny, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Sunny)) \\
    X(TakeOverDstWeather_ClearSky, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_ClearSky)) \\
    X(TakeOverDstWeather_WeakCloudy, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_WeakCloudy)) \\
    X(TakeOverDstWeather_Cloud, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Cloud)) \\
    X(TakeOverDstWeather_Rain, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Rain)) \\
    X(TakeOverDstWeather_HeavyRain, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_HeavyRain)) \\
    X(TakeOverDstWeather_Storm, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Storm)) \\
    X(TakeOverDstWeather_StormForBattle, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_StormForBattle)) \\
    X(TakeOverDstWeather_Snow, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Snow)) \\
    X(TakeOverDstWeather_HeavySnow, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_HeavySnow)) \\
    X(TakeOverDstWeather_Fog, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Fog)) \\
    X(TakeOverDstWeather_HeavyFog, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_HeavyFog)) \\
    X(TakeOverDstWeather_SandStorm, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_SandStorm)) \\
    X(TakeOverDstWeather_HeavyFogRain, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_HeavyFogRain)) \\
    X(reserved, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, reserved)) \\
    X(DstWeather_Snowstorm, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Snowstorm)) \\
    X(DstWeather_LightningStorm, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_LightningStorm)) \\
    X(DstWeather_Reserved3, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved3)) \\
    X(DstWeather_Reserved4, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved4)) \\
    X(DstWeather_Reserved5, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved5)) \\
    X(DstWeather_Reserved6, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved6)) \\
    X(DstWeather_Reserved7, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved7)) \\
    X(DstWeather_Reserved8, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved8)) \\
    X(TakeOverDstWeather_Snowstorm, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Snowstorm)) \\
    X(TakeOverDstWeather_LightningStorm, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_LightningStorm)) \\
    X(TakeOverDstWeather_Reserved3, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved3)) \\
    X(TakeOverDstWeather_Reserved4, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved4)) \\
    X(TakeOverDstWeather_Reserved5, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved5)) \\
    X(TakeOverDstWeather_Reserved6, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved6)) \\
    X(TakeOverDstWeather_Reserved7, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved7)) \\
    X(TakeOverDstWeather_Reserved8, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved8)) \\
    X(IsEnableApplyMapGdRegionIdForGparam, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, IsEnableApplyMapGdRegionIdForGparam)) \\
    X(reserved2, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, reserved2)) \\
    X(OverrideMapGdRegionId, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, OverrideMapGdRegionId)) \\
    X(reserved1, offsetof(::er::paramdef::cutscene_gparam_weather_param_st, reserved1))

#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_disableParamReserve2 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, disableParamReserve2)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Sunny offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Sunny)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_ClearSky offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_ClearSky)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_WeakCloudy offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_WeakCloudy)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Cloud offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Cloud)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Rain offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Rain)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_HeavyRain offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_HeavyRain)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Storm offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Storm)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_StormForBattle offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_StormForBattle)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Snow offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Snow)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_HeavySnow offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_HeavySnow)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Fog offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Fog)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_HeavyFog offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_HeavyFog)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_SandStorm offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_SandStorm)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_HeavyFogRain offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_HeavyFogRain)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_PostPlayIngameWeather offsetof(::er::paramdef::cutscene_gparam_weather_param_st, PostPlayIngameWeather)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_IndoorOutdoorType offsetof(::er::paramdef::cutscene_gparam_weather_param_st, IndoorOutdoorType)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Sunny offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Sunny)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_ClearSky offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_ClearSky)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_WeakCloudy offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_WeakCloudy)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Cloud offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Cloud)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Rain offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Rain)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_HeavyRain offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_HeavyRain)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Storm offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Storm)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_StormForBattle offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_StormForBattle)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Snow offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Snow)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_HeavySnow offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_HeavySnow)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Fog offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Fog)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_HeavyFog offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_HeavyFog)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_SandStorm offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_SandStorm)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_HeavyFogRain offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_HeavyFogRain)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_reserved offsetof(::er::paramdef::cutscene_gparam_weather_param_st, reserved)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Snowstorm offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Snowstorm)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_LightningStorm offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_LightningStorm)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Reserved3 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved3)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Reserved4 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved4)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Reserved5 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved5)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Reserved6 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved6)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Reserved7 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved7)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_DstWeather_Reserved8 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, DstWeather_Reserved8)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Snowstorm offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Snowstorm)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_LightningStorm offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_LightningStorm)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Reserved3 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved3)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Reserved4 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved4)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Reserved5 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved5)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Reserved6 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved6)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Reserved7 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved7)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_TakeOverDstWeather_Reserved8 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, TakeOverDstWeather_Reserved8)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_IsEnableApplyMapGdRegionIdForGparam offsetof(::er::paramdef::cutscene_gparam_weather_param_st, IsEnableApplyMapGdRegionIdForGparam)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_reserved2 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, reserved2)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_OverrideMapGdRegionId offsetof(::er::paramdef::cutscene_gparam_weather_param_st, OverrideMapGdRegionId)
#define ERD_OFFSET_CUTSCENE_GPARAM_WEATHER_PARAM_ST_reserved1 offsetof(::er::paramdef::cutscene_gparam_weather_param_st, reserved1)
