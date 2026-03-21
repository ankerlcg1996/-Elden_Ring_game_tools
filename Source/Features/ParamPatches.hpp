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
    void SyncNoMagicRequirements(const Game::SingletonRegistry& singletons);
    void SyncAllMagicOneSlot(const Game::SingletonRegistry& singletons);
    void SyncWeightlessEquipment(const Game::SingletonRegistry& singletons);
    void SyncMountAnywhere(const Game::SingletonRegistry& singletons);
    void SyncSpiritAshesAnywhere(const Game::SingletonRegistry& singletons);
    void SyncItemDiscovery(const Game::SingletonRegistry& singletons);
    void SyncPermanentLantern(const Game::SingletonRegistry& singletons);
    void SyncInvisibleHelmets(const Game::SingletonRegistry& singletons);

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

    struct HelmetState {
        std::uint8_t* row = nullptr;
        std::array<std::uint8_t, 7> head_and_invisible_flags{};
    };

    struct MagicRequirementState {
        std::uint8_t* row = nullptr;
        std::uint8_t arcane_requirement = 0;
        std::uint8_t intelligence_requirement = 0;
        std::uint8_t faith_requirement = 0;
    };

    struct MagicSlotState {
        std::uint8_t* row = nullptr;
        std::uint8_t slot_length = 0;
    };

    struct EquipmentWeightState {
        std::uint8_t* row = nullptr;
        std::size_t offset = 0;
        float weight = 0.0f;
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

    CodePatchState mount_anywhere_patch_primary_;
    CodePatchState mount_anywhere_patch_secondary_;
    CodePatchState spirit_ashes_anywhere_patch_primary_;
    CodePatchState spirit_ashes_anywhere_patch_secondary_;
    CodePatchState crafting_material_cost_patch_primary_;
    CodePatchState crafting_material_cost_patch_secondary_;

    ItemDiscoveryState item_discovery_;
    PermanentLanternState permanent_lantern_;

    std::vector<HelmetState> helmet_rows_;
    bool helmet_rows_captured_ = false;
    bool invisible_helmets_active_ = false;
};

}  // namespace ERD::Features
