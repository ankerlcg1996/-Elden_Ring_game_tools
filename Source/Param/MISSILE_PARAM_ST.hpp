#pragma once

#include <cstddef>
#include <elden-x/paramdef/MISSILE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_MISSILE_PARAM_ST_FIELDS(X) \\
    X(FFXID, offsetof(::er::paramdef::missile_param_st, FFXID)) \\
    X(LifeTime, offsetof(::er::paramdef::missile_param_st, LifeTime)) \\
    X(HitSphereRadius, offsetof(::er::paramdef::missile_param_st, HitSphereRadius)) \\
    X(HitDamage, offsetof(::er::paramdef::missile_param_st, HitDamage)) \\
    X(reserve0, offsetof(::er::paramdef::missile_param_st, reserve0)) \\
    X(InitVelocity, offsetof(::er::paramdef::missile_param_st, InitVelocity)) \\
    X(distance, offsetof(::er::paramdef::missile_param_st, distance)) \\
    X(gravityInRange, offsetof(::er::paramdef::missile_param_st, gravityInRange)) \\
    X(gravityOutRange, offsetof(::er::paramdef::missile_param_st, gravityOutRange)) \\
    X(mp, offsetof(::er::paramdef::missile_param_st, mp)) \\
    X(accelInRange, offsetof(::er::paramdef::missile_param_st, accelInRange)) \\
    X(accelOutRange, offsetof(::er::paramdef::missile_param_st, accelOutRange)) \\
    X(reserve1, offsetof(::er::paramdef::missile_param_st, reserve1)) \\
    X(HitMissileID, offsetof(::er::paramdef::missile_param_st, HitMissileID)) \\
    X(DiedNaturaly, offsetof(::er::paramdef::missile_param_st, DiedNaturaly)) \\
    X(ExplosionDie, offsetof(::er::paramdef::missile_param_st, ExplosionDie)) \\
    X(behaviorId, offsetof(::er::paramdef::missile_param_st, behaviorId)) \\
    X(reserve_last, offsetof(::er::paramdef::missile_param_st, reserve_last))

#define ERD_OFFSET_MISSILE_PARAM_ST_FFXID offsetof(::er::paramdef::missile_param_st, FFXID)
#define ERD_OFFSET_MISSILE_PARAM_ST_LifeTime offsetof(::er::paramdef::missile_param_st, LifeTime)
#define ERD_OFFSET_MISSILE_PARAM_ST_HitSphereRadius offsetof(::er::paramdef::missile_param_st, HitSphereRadius)
#define ERD_OFFSET_MISSILE_PARAM_ST_HitDamage offsetof(::er::paramdef::missile_param_st, HitDamage)
#define ERD_OFFSET_MISSILE_PARAM_ST_reserve0 offsetof(::er::paramdef::missile_param_st, reserve0)
#define ERD_OFFSET_MISSILE_PARAM_ST_InitVelocity offsetof(::er::paramdef::missile_param_st, InitVelocity)
#define ERD_OFFSET_MISSILE_PARAM_ST_distance offsetof(::er::paramdef::missile_param_st, distance)
#define ERD_OFFSET_MISSILE_PARAM_ST_gravityInRange offsetof(::er::paramdef::missile_param_st, gravityInRange)
#define ERD_OFFSET_MISSILE_PARAM_ST_gravityOutRange offsetof(::er::paramdef::missile_param_st, gravityOutRange)
#define ERD_OFFSET_MISSILE_PARAM_ST_mp offsetof(::er::paramdef::missile_param_st, mp)
#define ERD_OFFSET_MISSILE_PARAM_ST_accelInRange offsetof(::er::paramdef::missile_param_st, accelInRange)
#define ERD_OFFSET_MISSILE_PARAM_ST_accelOutRange offsetof(::er::paramdef::missile_param_st, accelOutRange)
#define ERD_OFFSET_MISSILE_PARAM_ST_reserve1 offsetof(::er::paramdef::missile_param_st, reserve1)
#define ERD_OFFSET_MISSILE_PARAM_ST_HitMissileID offsetof(::er::paramdef::missile_param_st, HitMissileID)
#define ERD_OFFSET_MISSILE_PARAM_ST_DiedNaturaly offsetof(::er::paramdef::missile_param_st, DiedNaturaly)
#define ERD_OFFSET_MISSILE_PARAM_ST_ExplosionDie offsetof(::er::paramdef::missile_param_st, ExplosionDie)
#define ERD_OFFSET_MISSILE_PARAM_ST_behaviorId offsetof(::er::paramdef::missile_param_st, behaviorId)
#define ERD_OFFSET_MISSILE_PARAM_ST_reserve_last offsetof(::er::paramdef::missile_param_st, reserve_last)
