#pragma once

#include <cstddef>
#include <elden-x/paramdef/SOUND_CHR_PHYSICS_SE_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_SOUND_CHR_PHYSICS_SE_PARAM_ST_FIELDS(X) \\
    X(disableParamReserve2, offsetof(::er::paramdef::sound_chr_physics_se_param_st, disableParamReserve2)) \\
    X(ContactLandSeId, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandSeId)) \\
    X(ContactLandAddSeId, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandAddSeId)) \\
    X(ContactLandPlayNum, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandPlayNum)) \\
    X(IsEnablePlayCountPerRigid, offsetof(::er::paramdef::sound_chr_physics_se_param_st, IsEnablePlayCountPerRigid)) \\
    X(pad, offsetof(::er::paramdef::sound_chr_physics_se_param_st, pad)) \\
    X(ContactLandMinImpuse, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandMinImpuse)) \\
    X(ContactLandMinSpeed, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandMinSpeed)) \\
    X(ContactPlayerSeId, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactPlayerSeId)) \\
    X(ContactPlayerMinImpuse, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactPlayerMinImpuse)) \\
    X(ContactPlayerMinSpeed, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactPlayerMinSpeed)) \\
    X(ContactCheckRigidIdx0, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx0)) \\
    X(ContactCheckRigidIdx1, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx1)) \\
    X(ContactCheckRigidIdx2, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx2)) \\
    X(ContactCheckRigidIdx3, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx3)) \\
    X(ContactCheckRigidIdx4, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx4)) \\
    X(ContactCheckRigidIdx5, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx5)) \\
    X(ContactCheckRigidIdx6, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx6)) \\
    X(ContactCheckRigidIdx7, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx7)) \\
    X(ContactCheckRigidIdx8, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx8)) \\
    X(ContactCheckRigidIdx9, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx9)) \\
    X(ContactCheckRigidIdx10, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx10)) \\
    X(ContactCheckRigidIdx11, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx11)) \\
    X(ContactCheckRigidIdx12, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx12)) \\
    X(ContactCheckRigidIdx13, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx13)) \\
    X(ContactCheckRigidIdx14, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx14)) \\
    X(ContactCheckRigidIdx15, offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx15))

#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_disableParamReserve2 offsetof(::er::paramdef::sound_chr_physics_se_param_st, disableParamReserve2)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactLandSeId offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandSeId)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactLandAddSeId offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandAddSeId)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactLandPlayNum offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandPlayNum)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_IsEnablePlayCountPerRigid offsetof(::er::paramdef::sound_chr_physics_se_param_st, IsEnablePlayCountPerRigid)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_pad offsetof(::er::paramdef::sound_chr_physics_se_param_st, pad)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactLandMinImpuse offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandMinImpuse)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactLandMinSpeed offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactLandMinSpeed)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactPlayerSeId offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactPlayerSeId)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactPlayerMinImpuse offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactPlayerMinImpuse)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactPlayerMinSpeed offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactPlayerMinSpeed)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx0 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx0)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx1 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx1)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx2 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx2)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx3 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx3)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx4 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx4)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx5 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx5)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx6 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx6)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx7 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx7)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx8 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx8)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx9 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx9)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx10 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx10)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx11 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx11)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx12 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx12)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx13 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx13)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx14 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx14)
#define ERD_OFFSET_SOUND_CHR_PHYSICS_SE_PARAM_ST_ContactCheckRigidIdx15 offsetof(::er::paramdef::sound_chr_physics_se_param_st, ContactCheckRigidIdx15)
