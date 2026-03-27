#pragma once

#include <cstddef>
#include <elden-x/paramdef/WEATHER_LOT_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_WEATHER_LOT_PARAM_ST_FIELDS(X) \\
    X(disableParamReserve2, offsetof(::er::paramdef::weather_lot_param_st, disableParamReserve2)) \\
    X(weatherType0, offsetof(::er::paramdef::weather_lot_param_st, weatherType0)) \\
    X(weatherType1, offsetof(::er::paramdef::weather_lot_param_st, weatherType1)) \\
    X(weatherType2, offsetof(::er::paramdef::weather_lot_param_st, weatherType2)) \\
    X(weatherType3, offsetof(::er::paramdef::weather_lot_param_st, weatherType3)) \\
    X(weatherType4, offsetof(::er::paramdef::weather_lot_param_st, weatherType4)) \\
    X(weatherType5, offsetof(::er::paramdef::weather_lot_param_st, weatherType5)) \\
    X(weatherType6, offsetof(::er::paramdef::weather_lot_param_st, weatherType6)) \\
    X(weatherType7, offsetof(::er::paramdef::weather_lot_param_st, weatherType7)) \\
    X(weatherType8, offsetof(::er::paramdef::weather_lot_param_st, weatherType8)) \\
    X(weatherType9, offsetof(::er::paramdef::weather_lot_param_st, weatherType9)) \\
    X(weatherType10, offsetof(::er::paramdef::weather_lot_param_st, weatherType10)) \\
    X(weatherType11, offsetof(::er::paramdef::weather_lot_param_st, weatherType11)) \\
    X(weatherType12, offsetof(::er::paramdef::weather_lot_param_st, weatherType12)) \\
    X(weatherType13, offsetof(::er::paramdef::weather_lot_param_st, weatherType13)) \\
    X(weatherType14, offsetof(::er::paramdef::weather_lot_param_st, weatherType14)) \\
    X(weatherType15, offsetof(::er::paramdef::weather_lot_param_st, weatherType15)) \\
    X(lotteryWeight0, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight0)) \\
    X(lotteryWeight1, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight1)) \\
    X(lotteryWeight2, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight2)) \\
    X(lotteryWeight3, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight3)) \\
    X(lotteryWeight4, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight4)) \\
    X(lotteryWeight5, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight5)) \\
    X(lotteryWeight6, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight6)) \\
    X(lotteryWeight7, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight7)) \\
    X(lotteryWeight8, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight8)) \\
    X(lotteryWeight9, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight9)) \\
    X(lotteryWeight10, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight10)) \\
    X(lotteryWeight11, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight11)) \\
    X(lotteryWeight12, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight12)) \\
    X(lotteryWeight13, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight13)) \\
    X(lotteryWeight14, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight14)) \\
    X(lotteryWeight15, offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight15)) \\
    X(timezoneLimit, offsetof(::er::paramdef::weather_lot_param_st, timezoneLimit)) \\
    X(timezoneStartHour, offsetof(::er::paramdef::weather_lot_param_st, timezoneStartHour)) \\
    X(timezoneStartMinute, offsetof(::er::paramdef::weather_lot_param_st, timezoneStartMinute)) \\
    X(timezoneEndHour, offsetof(::er::paramdef::weather_lot_param_st, timezoneEndHour)) \\
    X(timezoneEndMinute, offsetof(::er::paramdef::weather_lot_param_st, timezoneEndMinute)) \\
    X(reserve, offsetof(::er::paramdef::weather_lot_param_st, reserve))

#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_disableParamReserve2 offsetof(::er::paramdef::weather_lot_param_st, disableParamReserve2)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType0 offsetof(::er::paramdef::weather_lot_param_st, weatherType0)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType1 offsetof(::er::paramdef::weather_lot_param_st, weatherType1)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType2 offsetof(::er::paramdef::weather_lot_param_st, weatherType2)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType3 offsetof(::er::paramdef::weather_lot_param_st, weatherType3)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType4 offsetof(::er::paramdef::weather_lot_param_st, weatherType4)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType5 offsetof(::er::paramdef::weather_lot_param_st, weatherType5)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType6 offsetof(::er::paramdef::weather_lot_param_st, weatherType6)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType7 offsetof(::er::paramdef::weather_lot_param_st, weatherType7)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType8 offsetof(::er::paramdef::weather_lot_param_st, weatherType8)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType9 offsetof(::er::paramdef::weather_lot_param_st, weatherType9)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType10 offsetof(::er::paramdef::weather_lot_param_st, weatherType10)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType11 offsetof(::er::paramdef::weather_lot_param_st, weatherType11)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType12 offsetof(::er::paramdef::weather_lot_param_st, weatherType12)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType13 offsetof(::er::paramdef::weather_lot_param_st, weatherType13)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType14 offsetof(::er::paramdef::weather_lot_param_st, weatherType14)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_weatherType15 offsetof(::er::paramdef::weather_lot_param_st, weatherType15)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight0 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight0)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight1 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight1)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight2 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight2)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight3 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight3)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight4 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight4)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight5 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight5)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight6 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight6)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight7 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight7)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight8 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight8)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight9 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight9)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight10 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight10)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight11 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight11)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight12 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight12)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight13 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight13)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight14 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight14)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_lotteryWeight15 offsetof(::er::paramdef::weather_lot_param_st, lotteryWeight15)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_timezoneLimit offsetof(::er::paramdef::weather_lot_param_st, timezoneLimit)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_timezoneStartHour offsetof(::er::paramdef::weather_lot_param_st, timezoneStartHour)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_timezoneStartMinute offsetof(::er::paramdef::weather_lot_param_st, timezoneStartMinute)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_timezoneEndHour offsetof(::er::paramdef::weather_lot_param_st, timezoneEndHour)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_timezoneEndMinute offsetof(::er::paramdef::weather_lot_param_st, timezoneEndMinute)
#define ERD_OFFSET_WEATHER_LOT_PARAM_ST_reserve offsetof(::er::paramdef::weather_lot_param_st, reserve)
