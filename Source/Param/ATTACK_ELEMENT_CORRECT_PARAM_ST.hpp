#pragma once

#include <cstddef>
#include <elden-x/paramdef/ATTACK_ELEMENT_CORRECT_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_ATTACK_ELEMENT_CORRECT_PARAM_ST_FIELDS(X) \\
    X(overwriteStrengthCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byPhysics)) \\
    X(overwriteDexterityCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byPhysics)) \\
    X(overwriteMagicCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byPhysics)) \\
    X(overwriteFaithCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byPhysics)) \\
    X(overwriteLuckCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byPhysics)) \\
    X(overwriteStrengthCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byMagic)) \\
    X(overwriteDexterityCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byMagic)) \\
    X(overwriteMagicCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byMagic)) \\
    X(overwriteFaithCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byMagic)) \\
    X(overwriteLuckCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byMagic)) \\
    X(overwriteStrengthCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byFire)) \\
    X(overwriteDexterityCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byFire)) \\
    X(overwriteMagicCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byFire)) \\
    X(overwriteFaithCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byFire)) \\
    X(overwriteLuckCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byFire)) \\
    X(overwriteStrengthCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byThunder)) \\
    X(overwriteDexterityCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byThunder)) \\
    X(overwriteMagicCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byThunder)) \\
    X(overwriteFaithCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byThunder)) \\
    X(overwriteLuckCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byThunder)) \\
    X(overwriteStrengthCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byDark)) \\
    X(overwriteDexterityCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byDark)) \\
    X(overwriteMagicCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byDark)) \\
    X(overwriteFaithCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byDark)) \\
    X(overwriteLuckCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byDark)) \\
    X(InfluenceStrengthCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byPhysics)) \\
    X(InfluenceDexterityCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byPhysics)) \\
    X(InfluenceMagicCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byPhysics)) \\
    X(InfluenceFaithCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byPhysics)) \\
    X(InfluenceLuckCorrectRate_byPhysics, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byPhysics)) \\
    X(InfluenceStrengthCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byMagic)) \\
    X(InfluenceDexterityCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byMagic)) \\
    X(InfluenceMagicCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byMagic)) \\
    X(InfluenceFaithCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byMagic)) \\
    X(InfluenceLuckCorrectRate_byMagic, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byMagic)) \\
    X(InfluenceStrengthCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byFire)) \\
    X(InfluenceDexterityCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byFire)) \\
    X(InfluenceMagicCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byFire)) \\
    X(InfluenceFaithCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byFire)) \\
    X(InfluenceLuckCorrectRate_byFire, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byFire)) \\
    X(InfluenceStrengthCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byThunder)) \\
    X(InfluenceDexterityCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byThunder)) \\
    X(InfluenceMagicCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byThunder)) \\
    X(InfluenceFaithCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byThunder)) \\
    X(InfluenceLuckCorrectRate_byThunder, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byThunder)) \\
    X(InfluenceStrengthCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byDark)) \\
    X(InfluenceDexterityCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byDark)) \\
    X(InfluenceMagicCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byDark)) \\
    X(InfluenceFaithCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byDark)) \\
    X(InfluenceLuckCorrectRate_byDark, offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byDark)) \\
    X(pad2, offsetof(::er::paramdef::attack_element_correct_param_st, pad2))

#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteStrengthCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteDexterityCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteMagicCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteFaithCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteLuckCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteStrengthCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteDexterityCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteMagicCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteFaithCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteLuckCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteStrengthCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteDexterityCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteMagicCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteFaithCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteLuckCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteStrengthCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteDexterityCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteMagicCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteFaithCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteLuckCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteStrengthCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, overwriteStrengthCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteDexterityCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, overwriteDexterityCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteMagicCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, overwriteMagicCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteFaithCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, overwriteFaithCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_overwriteLuckCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, overwriteLuckCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceStrengthCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceDexterityCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceMagicCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceFaithCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceLuckCorrectRate_byPhysics offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byPhysics)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceStrengthCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceDexterityCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceMagicCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceFaithCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceLuckCorrectRate_byMagic offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byMagic)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceStrengthCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceDexterityCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceMagicCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceFaithCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceLuckCorrectRate_byFire offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byFire)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceStrengthCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceDexterityCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceMagicCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceFaithCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceLuckCorrectRate_byThunder offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byThunder)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceStrengthCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceStrengthCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceDexterityCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceDexterityCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceMagicCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceMagicCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceFaithCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceFaithCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_InfluenceLuckCorrectRate_byDark offsetof(::er::paramdef::attack_element_correct_param_st, InfluenceLuckCorrectRate_byDark)
#define ERD_OFFSET_ATTACK_ELEMENT_CORRECT_PARAM_ST_pad2 offsetof(::er::paramdef::attack_element_correct_param_st, pad2)
