#pragma once

#include "../Common.hpp"

#include "../Game/Params.hpp"
#include "../Game/SingletonRegistry.hpp"

namespace ERD::Features {

// 参数表补丁类功能。
// 这类功能本质上是把 CT 里的 ParamPatch / ParamRestore 逻辑翻成 C++。
class ParamPatches {
public:
    struct CodePatchState {
        uintptr_t target = 0;
        void* cave = nullptr;
        std::array<std::uint8_t, 16> original_bytes{};
        std::size_t patch_size = 0;
        bool captured = false;
        bool active = false;
    };

    void Tick(const Game::SingletonRegistry& singletons);

private:
    void SyncFasterRespawn(const Game::SingletonRegistry& singletons);
    void SyncMiniDungeonWarp(const Game::SingletonRegistry& singletons);
    void SyncFreePurchase(const Game::SingletonRegistry& singletons);
    void SyncNoCraftingMaterialCost(const Game::SingletonRegistry& singletons);
    void SyncNoUpgradeMaterialCost(const Game::SingletonRegistry& singletons);
    void SyncAllWeaponsEnchantable(const Game::SingletonRegistry& singletons);
    void SyncAllWeaponsAshOfWarChangeable(const Game::SingletonRegistry& singletons);
    void SyncNoMagicRequirements(const Game::SingletonRegistry& singletons);
    void SyncAllMagicOneSlot(const Game::SingletonRegistry& singletons);
    void SyncWeightlessEquipment(const Game::SingletonRegistry& singletons);
    void SyncWeaponRequirementReduction(const Game::SingletonRegistry& singletons);
    void SyncEasierParry(const Game::SingletonRegistry& singletons);
    void SyncCustomFov(const Game::SingletonRegistry& singletons);
    void SyncCustomCameraDistance(const Game::SingletonRegistry& singletons);
    void SyncSpiritAshesAnywhere(const Game::SingletonRegistry& singletons);
    void SyncTorrentAnywhere(const Game::SingletonRegistry& singletons);
    void SyncOpenMapInCombat(const Game::SingletonRegistry& singletons);
    void SyncInfiniteJump(const Game::SingletonRegistry& singletons);
    void SyncItemDiscovery(const Game::SingletonRegistry& singletons);
    void SyncPlayerDamageMultiplier(const Game::SingletonRegistry& singletons);
    void SyncPlayerDamageCutMultiplier(const Game::SingletonRegistry& singletons);
    void SyncEnemyHpMultiplier(const Game::SingletonRegistry& singletons);
    void SyncPermanentLantern(const Game::SingletonRegistry& singletons);
    void SyncInvisibleHelmets(const Game::SingletonRegistry& singletons);
    void SyncBuffDurationExtend(const Game::SingletonRegistry& singletons);

    struct FasterRespawnState {
        MenuCommonParam* row = nullptr;
        float solo_fade_out_time = 0.0f;
        float party_fade_out_time = 0.0f;
        bool captured = false;
        bool active = false;
    };

    struct MiniDungeonWarpState {
        SpEffectParam* row = nullptr;
        std::uint16_t state_info = 0;
        bool captured = false;
        bool active = false;
    };

    struct UpgradeCostState {
        EquipMtrlSetParam* row = nullptr;
        std::int32_t material_id01 = 0;
        std::int8_t item_num01 = 0;
    };

    struct ShopFreeLineupState {
        std::uint8_t* row = nullptr;
        std::int32_t value = 0;
        std::int32_t value_add = 0;
        float value_magnification = 0.0f;
    };

    struct SellValueState {
        std::uint8_t* row = nullptr;
        std::size_t offset = 0;
        std::int32_t sell_value = 0;
    };

    struct ItemDiscoveryState {
        CalcCorrectGraph* row = nullptr;
        std::array<float, 5> original_values{};
        bool captured = false;
        int applied_multiplier = 1;
    };

    struct PermanentLanternState {
        SpEffectParam* row = nullptr;
        std::int8_t save_category = 0;
        bool captured = false;
        bool active = false;
    };

    struct PlayerDamageMultiplierState {
        std::uint8_t* row = nullptr;
        std::array<float, 5> original_values{};
        bool captured = false;
        int applied_percent = 0;
    };

    struct PlayerDamageCutMultiplierState {
        std::uint8_t* row = nullptr;
        std::array<float, 5> original_values{};
        bool captured = false;
        int applied_percent = 0;
    };

    struct EnemyHpState {
        std::uint8_t* row = nullptr;
        std::int32_t hp = 0;
    };

    struct EnemyHpMultiplierState {
        std::vector<EnemyHpState> rows;
        bool captured = false;
        int applied_percent = 0;
    };

    struct BuffDurationExtendState {
        std::uint8_t* row = nullptr;
        float original_extend_life_rate = 1.0f;
        bool captured = false;
        int applied_mode = 0;
    };

    struct HelmetState {
        std::uint8_t* row = nullptr;
        std::array<std::uint8_t, 7> head_and_invisible_flags{};
    };

    struct MagicRequirementState {
        std::uint64_t row_id = 0;
        std::uint8_t arcane_requirement = 0;
        std::uint8_t intelligence_requirement = 0;
        std::uint8_t faith_requirement = 0;
    };

    struct MagicSlotState {
        std::uint64_t row_id = 0;
        std::uint8_t slot_length = 0;
    };

    struct EquipmentWeightState {
        std::uint8_t* row = nullptr;
        std::size_t offset = 0;
        float weight = 0.0f;
    };

    struct WeaponRequirementState {
        std::uint8_t* row = nullptr;
        std::array<std::uint8_t, 10> requirements{};
    };

    struct WeaponEnhanceState {
        std::uint8_t* row = nullptr;
        std::uint8_t is_enhance = 0;
    };

    struct LockCamState {
        LockCamParam* row = nullptr;
        float cam_dist_target = 0.0f;
        float cam_fov_y = 0.0f;
    };

    FasterRespawnState faster_respawn_;
    MiniDungeonWarpState mini_dungeon_warp_;

    std::vector<ShopFreeLineupState> shop_free_lineup_rows_;
    std::vector<SellValueState> shop_free_sell_rows_;
    bool shop_free_captured_ = false;
    bool shop_free_active_ = false;

    std::vector<UpgradeCostState> upgrade_cost_rows_;
    bool upgrade_cost_captured_ = false;
    bool upgrade_cost_active_ = false;

    std::vector<MagicRequirementState> magic_requirement_rows_;
    bool magic_requirement_rows_captured_ = false;
    bool magic_requirement_active_ = false;

    std::vector<MagicSlotState> magic_slot_rows_;
    bool magic_slot_rows_captured_ = false;
    bool magic_slot_active_ = false;

    std::vector<EquipmentWeightState> equipment_weight_rows_;
    bool equipment_weight_rows_captured_ = false;
    bool equipment_weight_active_ = false;
    int equipment_weight_applied_percent_ = 0;

    std::vector<WeaponRequirementState> weapon_requirement_rows_;
    bool weapon_requirement_rows_captured_ = false;
    bool weapon_requirement_active_ = false;
    int weapon_requirement_applied_percent_ = 0;

    std::vector<WeaponEnhanceState> weapon_enhance_rows_;
    bool weapon_enhance_rows_captured_ = false;
    bool weapon_enhance_active_ = false;

    std::vector<WeaponEnhanceState> weapon_ash_of_war_rows_;
    bool weapon_ash_of_war_rows_captured_ = false;
    bool weapon_ash_of_war_active_ = false;

    CodePatchState easier_parry_judge_patch_;
    CodePatchState easier_parry_branch_a_patch_;
    CodePatchState easier_parry_branch_b_patch_;
    int easier_parry_applied_stage_ = 0;

    std::vector<LockCamState> lock_cam_rows_;
    bool lock_cam_rows_captured_ = false;
    bool custom_fov_active_ = false;
    bool custom_camera_distance_active_ = false;

    CodePatchState spirit_ashes_anywhere_patch_primary_;
    CodePatchState spirit_ashes_anywhere_patch_secondary_;
    CodePatchState torrent_anywhere_patch_underworld_;
    CodePatchState torrent_anywhere_patch_whistle_;
    CodePatchState open_map_in_combat_patch_open_map_;
    CodePatchState open_map_in_combat_patch_close_map_;
    CodePatchState infinite_jump_horse_primary_patch_;
    CodePatchState infinite_jump_horse_secondary_patch_;
    CodePatchState infinite_jump_cmp_patch_;
    CodePatchState crafting_material_cost_patch_primary_;
    CodePatchState crafting_material_cost_patch_secondary_;

    ItemDiscoveryState item_discovery_;
    PlayerDamageMultiplierState player_damage_multiplier_;
    PlayerDamageCutMultiplierState player_damage_cut_multiplier_;
    EnemyHpMultiplierState enemy_hp_multiplier_;
    PermanentLanternState permanent_lantern_;
    BuffDurationExtendState buff_duration_extend_;

    std::vector<HelmetState> helmet_rows_;
    bool helmet_rows_captured_ = false;
    bool invisible_helmets_active_ = false;
};

}  // namespace ERD::Features
