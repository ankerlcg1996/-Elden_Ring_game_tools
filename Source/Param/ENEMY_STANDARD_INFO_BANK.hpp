#pragma once

#include <cstddef>
#include <elden-x/paramdef/ENEMY_STANDARD_INFO_BANK.hpp>

// Auto-generated field offset macros for this param table.
// Regenerate from elden-x paramdef headers when upstream changes.

#define ERD_PARAM_ENEMY_STANDARD_INFO_BANK_FIELDS(X) \\
    X(EnemyBehaviorID, offsetof(::er::paramdef::enemy_standard_info_bank, EnemyBehaviorID)) \\
    X(HP, offsetof(::er::paramdef::enemy_standard_info_bank, HP)) \\
    X(AttackPower, offsetof(::er::paramdef::enemy_standard_info_bank, AttackPower)) \\
    X(ChrType, offsetof(::er::paramdef::enemy_standard_info_bank, ChrType)) \\
    X(HitHeight, offsetof(::er::paramdef::enemy_standard_info_bank, HitHeight)) \\
    X(HitRadius, offsetof(::er::paramdef::enemy_standard_info_bank, HitRadius)) \\
    X(Weight, offsetof(::er::paramdef::enemy_standard_info_bank, Weight)) \\
    X(DynamicFriction, offsetof(::er::paramdef::enemy_standard_info_bank, DynamicFriction)) \\
    X(StaticFriction, offsetof(::er::paramdef::enemy_standard_info_bank, StaticFriction)) \\
    X(UpperDefState, offsetof(::er::paramdef::enemy_standard_info_bank, UpperDefState)) \\
    X(ActionDefState, offsetof(::er::paramdef::enemy_standard_info_bank, ActionDefState)) \\
    X(RotY_per_Second, offsetof(::er::paramdef::enemy_standard_info_bank, RotY_per_Second)) \\
    X(reserve0, offsetof(::er::paramdef::enemy_standard_info_bank, reserve0)) \\
    X(RotY_per_Second_old, offsetof(::er::paramdef::enemy_standard_info_bank, RotY_per_Second_old)) \\
    X(EnableSideStep, offsetof(::er::paramdef::enemy_standard_info_bank, EnableSideStep)) \\
    X(UseRagdollHit, offsetof(::er::paramdef::enemy_standard_info_bank, UseRagdollHit)) \\
    X(reserve_last, offsetof(::er::paramdef::enemy_standard_info_bank, reserve_last)) \\
    X(stamina, offsetof(::er::paramdef::enemy_standard_info_bank, stamina)) \\
    X(staminaRecover, offsetof(::er::paramdef::enemy_standard_info_bank, staminaRecover)) \\
    X(staminaConsumption, offsetof(::er::paramdef::enemy_standard_info_bank, staminaConsumption)) \\
    X(deffenct_Phys, offsetof(::er::paramdef::enemy_standard_info_bank, deffenct_Phys)) \\
    X(reserve_last2, offsetof(::er::paramdef::enemy_standard_info_bank, reserve_last2))

#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_EnemyBehaviorID offsetof(::er::paramdef::enemy_standard_info_bank, EnemyBehaviorID)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_HP offsetof(::er::paramdef::enemy_standard_info_bank, HP)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_AttackPower offsetof(::er::paramdef::enemy_standard_info_bank, AttackPower)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_ChrType offsetof(::er::paramdef::enemy_standard_info_bank, ChrType)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_HitHeight offsetof(::er::paramdef::enemy_standard_info_bank, HitHeight)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_HitRadius offsetof(::er::paramdef::enemy_standard_info_bank, HitRadius)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_Weight offsetof(::er::paramdef::enemy_standard_info_bank, Weight)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_DynamicFriction offsetof(::er::paramdef::enemy_standard_info_bank, DynamicFriction)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_StaticFriction offsetof(::er::paramdef::enemy_standard_info_bank, StaticFriction)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_UpperDefState offsetof(::er::paramdef::enemy_standard_info_bank, UpperDefState)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_ActionDefState offsetof(::er::paramdef::enemy_standard_info_bank, ActionDefState)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_RotY_per_Second offsetof(::er::paramdef::enemy_standard_info_bank, RotY_per_Second)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_reserve0 offsetof(::er::paramdef::enemy_standard_info_bank, reserve0)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_RotY_per_Second_old offsetof(::er::paramdef::enemy_standard_info_bank, RotY_per_Second_old)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_EnableSideStep offsetof(::er::paramdef::enemy_standard_info_bank, EnableSideStep)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_UseRagdollHit offsetof(::er::paramdef::enemy_standard_info_bank, UseRagdollHit)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_reserve_last offsetof(::er::paramdef::enemy_standard_info_bank, reserve_last)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_stamina offsetof(::er::paramdef::enemy_standard_info_bank, stamina)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_staminaRecover offsetof(::er::paramdef::enemy_standard_info_bank, staminaRecover)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_staminaConsumption offsetof(::er::paramdef::enemy_standard_info_bank, staminaConsumption)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_deffenct_Phys offsetof(::er::paramdef::enemy_standard_info_bank, deffenct_Phys)
#define ERD_OFFSET_ENEMY_STANDARD_INFO_BANK_reserve_last2 offsetof(::er::paramdef::enemy_standard_info_bank, reserve_last2)
