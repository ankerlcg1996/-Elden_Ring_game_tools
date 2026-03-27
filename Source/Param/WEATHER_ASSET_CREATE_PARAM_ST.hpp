#pragma once

#include <cstddef>
#include <elden-x/paramdef/WEATHER_ASSET_CREATE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_WEATHER_ASSET_CREATE_PARAM_ST_FIELDS(X) \\
    X(AssetId, offsetof(::er::paramdef::weather_asset_create_param_st, AssetId)) \\
    X(SlotNo, offsetof(::er::paramdef::weather_asset_create_param_st, SlotNo)) \\
    X(CreateConditionType, offsetof(::er::paramdef::weather_asset_create_param_st, CreateConditionType)) \\
    X(padding0, offsetof(::er::paramdef::weather_asset_create_param_st, padding0)) \\
    X(TransitionSrcWeather, offsetof(::er::paramdef::weather_asset_create_param_st, TransitionSrcWeather)) \\
    X(TransitionDstWeather, offsetof(::er::paramdef::weather_asset_create_param_st, TransitionDstWeather)) \\
    X(ElapsedTimeCheckweather, offsetof(::er::paramdef::weather_asset_create_param_st, ElapsedTimeCheckweather)) \\
    X(padding1, offsetof(::er::paramdef::weather_asset_create_param_st, padding1)) \\
    X(ElapsedTime, offsetof(::er::paramdef::weather_asset_create_param_st, ElapsedTime)) \\
    X(CreateDelayTime, offsetof(::er::paramdef::weather_asset_create_param_st, CreateDelayTime)) \\
    X(CreateProbability, offsetof(::er::paramdef::weather_asset_create_param_st, CreateProbability)) \\
    X(LifeTime, offsetof(::er::paramdef::weather_asset_create_param_st, LifeTime)) \\
    X(FadeTime, offsetof(::er::paramdef::weather_asset_create_param_st, FadeTime)) \\
    X(EnableCreateTimeMin, offsetof(::er::paramdef::weather_asset_create_param_st, EnableCreateTimeMin)) \\
    X(EnableCreateTimeMax, offsetof(::er::paramdef::weather_asset_create_param_st, EnableCreateTimeMax)) \\
    X(CreatePoint0, offsetof(::er::paramdef::weather_asset_create_param_st, CreatePoint0)) \\
    X(CreatePoint1, offsetof(::er::paramdef::weather_asset_create_param_st, CreatePoint1)) \\
    X(CreatePoint2, offsetof(::er::paramdef::weather_asset_create_param_st, CreatePoint2)) \\
    X(CreatePoint3, offsetof(::er::paramdef::weather_asset_create_param_st, CreatePoint3)) \\
    X(CreateAssetLimitId0, offsetof(::er::paramdef::weather_asset_create_param_st, CreateAssetLimitId0)) \\
    X(CreateAssetLimitId1, offsetof(::er::paramdef::weather_asset_create_param_st, CreateAssetLimitId1)) \\
    X(CreateAssetLimitId2, offsetof(::er::paramdef::weather_asset_create_param_st, CreateAssetLimitId2)) \\
    X(CreateAssetLimitId3, offsetof(::er::paramdef::weather_asset_create_param_st, CreateAssetLimitId3)) \\
    X(Reserved2, offsetof(::er::paramdef::weather_asset_create_param_st, Reserved2))

#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_AssetId offsetof(::er::paramdef::weather_asset_create_param_st, AssetId)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_SlotNo offsetof(::er::paramdef::weather_asset_create_param_st, SlotNo)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreateConditionType offsetof(::er::paramdef::weather_asset_create_param_st, CreateConditionType)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_padding0 offsetof(::er::paramdef::weather_asset_create_param_st, padding0)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_TransitionSrcWeather offsetof(::er::paramdef::weather_asset_create_param_st, TransitionSrcWeather)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_TransitionDstWeather offsetof(::er::paramdef::weather_asset_create_param_st, TransitionDstWeather)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_ElapsedTimeCheckweather offsetof(::er::paramdef::weather_asset_create_param_st, ElapsedTimeCheckweather)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_padding1 offsetof(::er::paramdef::weather_asset_create_param_st, padding1)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_ElapsedTime offsetof(::er::paramdef::weather_asset_create_param_st, ElapsedTime)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreateDelayTime offsetof(::er::paramdef::weather_asset_create_param_st, CreateDelayTime)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreateProbability offsetof(::er::paramdef::weather_asset_create_param_st, CreateProbability)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_LifeTime offsetof(::er::paramdef::weather_asset_create_param_st, LifeTime)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_FadeTime offsetof(::er::paramdef::weather_asset_create_param_st, FadeTime)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_EnableCreateTimeMin offsetof(::er::paramdef::weather_asset_create_param_st, EnableCreateTimeMin)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_EnableCreateTimeMax offsetof(::er::paramdef::weather_asset_create_param_st, EnableCreateTimeMax)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreatePoint0 offsetof(::er::paramdef::weather_asset_create_param_st, CreatePoint0)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreatePoint1 offsetof(::er::paramdef::weather_asset_create_param_st, CreatePoint1)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreatePoint2 offsetof(::er::paramdef::weather_asset_create_param_st, CreatePoint2)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreatePoint3 offsetof(::er::paramdef::weather_asset_create_param_st, CreatePoint3)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreateAssetLimitId0 offsetof(::er::paramdef::weather_asset_create_param_st, CreateAssetLimitId0)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreateAssetLimitId1 offsetof(::er::paramdef::weather_asset_create_param_st, CreateAssetLimitId1)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreateAssetLimitId2 offsetof(::er::paramdef::weather_asset_create_param_st, CreateAssetLimitId2)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_CreateAssetLimitId3 offsetof(::er::paramdef::weather_asset_create_param_st, CreateAssetLimitId3)
#define ERD_OFFSET_WEATHER_ASSET_CREATE_PARAM_ST_Reserved2 offsetof(::er::paramdef::weather_asset_create_param_st, Reserved2)
