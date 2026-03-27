#pragma once

#include <cstddef>
#include <elden-x/paramdef/ESTUS_FLASK_RECOVERY_PARAM_ST.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_ESTUS_FLASK_RECOVERY_PARAM_ST_FIELDS(X) \\
    X(host, offsetof(::er::paramdef::estus_flask_recovery_param_st, host)) \\
    X(invadeOrb_None, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeOrb_None)) \\
    X(invadeOrb_Umbasa, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeOrb_Umbasa)) \\
    X(invadeOrb_Berserker, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeOrb_Berserker)) \\
    X(invadeOrb_Sinners, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeOrb_Sinners)) \\
    X(invadeSign_None, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeSign_None)) \\
    X(invadeSign_Umbasa, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeSign_Umbasa)) \\
    X(invadeSign_Berserker, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeSign_Berserker)) \\
    X(invadeSign_Sinners, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeSign_Sinners)) \\
    X(invadeRing_Sinners, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeRing_Sinners)) \\
    X(invadeRing_Rosalia, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeRing_Rosalia)) \\
    X(invadeRing_Forest, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeRing_Forest)) \\
    X(coopSign_None, offsetof(::er::paramdef::estus_flask_recovery_param_st, coopSign_None)) \\
    X(coopSign_Umbasa, offsetof(::er::paramdef::estus_flask_recovery_param_st, coopSign_Umbasa)) \\
    X(coopSign_Berserker, offsetof(::er::paramdef::estus_flask_recovery_param_st, coopSign_Berserker)) \\
    X(coopSign_Sinners, offsetof(::er::paramdef::estus_flask_recovery_param_st, coopSign_Sinners)) \\
    X(coopRing_RedHunter, offsetof(::er::paramdef::estus_flask_recovery_param_st, coopRing_RedHunter)) \\
    X(invadeRing_Anor, offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeRing_Anor)) \\
    X(paramReplaceRate, offsetof(::er::paramdef::estus_flask_recovery_param_st, paramReplaceRate)) \\
    X(paramReplaceId, offsetof(::er::paramdef::estus_flask_recovery_param_st, paramReplaceId)) \\
    X(pad, offsetof(::er::paramdef::estus_flask_recovery_param_st, pad))

#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_host offsetof(::er::paramdef::estus_flask_recovery_param_st, host)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeOrb_None offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeOrb_None)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeOrb_Umbasa offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeOrb_Umbasa)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeOrb_Berserker offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeOrb_Berserker)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeOrb_Sinners offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeOrb_Sinners)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeSign_None offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeSign_None)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeSign_Umbasa offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeSign_Umbasa)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeSign_Berserker offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeSign_Berserker)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeSign_Sinners offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeSign_Sinners)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeRing_Sinners offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeRing_Sinners)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeRing_Rosalia offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeRing_Rosalia)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeRing_Forest offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeRing_Forest)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_coopSign_None offsetof(::er::paramdef::estus_flask_recovery_param_st, coopSign_None)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_coopSign_Umbasa offsetof(::er::paramdef::estus_flask_recovery_param_st, coopSign_Umbasa)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_coopSign_Berserker offsetof(::er::paramdef::estus_flask_recovery_param_st, coopSign_Berserker)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_coopSign_Sinners offsetof(::er::paramdef::estus_flask_recovery_param_st, coopSign_Sinners)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_coopRing_RedHunter offsetof(::er::paramdef::estus_flask_recovery_param_st, coopRing_RedHunter)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_invadeRing_Anor offsetof(::er::paramdef::estus_flask_recovery_param_st, invadeRing_Anor)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_paramReplaceRate offsetof(::er::paramdef::estus_flask_recovery_param_st, paramReplaceRate)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_paramReplaceId offsetof(::er::paramdef::estus_flask_recovery_param_st, paramReplaceId)
#define ERD_OFFSET_ESTUS_FLASK_RECOVERY_PARAM_ST_pad offsetof(::er::paramdef::estus_flask_recovery_param_st, pad)
