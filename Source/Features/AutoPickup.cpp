#include "AutoPickup.hpp"

#include "../Game/Params.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Param/ACTIONBUTTON_PARAM_ST.hpp"
#include "../ThirdParty/MinHook/MinHook.h"

#include <elden-x/utils/modutils.hpp>

#include <algorithm>
#include <array>
#include <span>

#include <spdlog/spdlog.h>

namespace ERD::Features {
namespace {

AutoPickup* g_auto_pickup_instance = nullptr;

extern "C" int ExecActionButtonParamProxyWrapper();
extern "C" uintptr_t ExecuteActionButtonParamProxyReturn = 0;

constexpr std::array<std::uint32_t, 78> kMaterialActionButtonIds{{
    7800, 7810, 7811, 7812, 7813, 7814, 7815, 7816, 7817, 7818, 7819, 7820, 7821, 7822,
    7823, 7824, 7825, 7826, 7827, 7828, 7850, 7860, 7861, 7862, 7863, 7864, 7865, 7866,
    7867, 7868, 7869, 7870, 7871, 7872, 7873, 7874, 7875, 7876, 7877, 7878, 207800, 207810,
    207811, 207812, 207813, 207814, 207815, 207816, 207817, 207818, 207819, 207820, 207821,
    207822, 207823, 207824, 207825, 207826, 207827, 207828, 207829, 207830, 207831, 207832,
    207833, 207834, 207835, 207836, 207837, 207838, 207839, 207840, 207841, 207842, 207843,
    207844,
}};

constexpr std::array<std::uint32_t, 16> kItemActionButtonIds{{
    4000, 4110, 4200, 4201, 4202, 4250, 4251, 4252, 4253, 4260, 4270, 4280, 4300, 4350,
    6361, 9532,
}};

constexpr std::array<std::uint32_t, 1> kCorpseLootActionButtonIds{{4100}};
constexpr std::array<std::uint32_t, 1> kLostRunesActionButtonIds{{1000}};

template <std::size_t N>
bool ContainsActionButtonId(const std::array<std::uint32_t, N>& ids, int entry_id) {
    return std::binary_search(ids.begin(), ids.end(), static_cast<std::uint32_t>(entry_id));
}

bool IsSelectedActionButtonId(int entry_id) {
    const auto& status = ERD::Main::g_FeatureStatus;
    if (!status.auto_pickup_enabled.load()) {
        return false;
    }

    if (status.auto_pickup_materials.load() && ContainsActionButtonId(kMaterialActionButtonIds, entry_id)) {
        return true;
    }
    if (status.auto_pickup_items.load() && ContainsActionButtonId(kItemActionButtonIds, entry_id)) {
        return true;
    }
    if (status.auto_pickup_corpse_loot.load() && ContainsActionButtonId(kCorpseLootActionButtonIds, entry_id)) {
        return true;
    }
    if (status.auto_pickup_lost_runes.load() && ContainsActionButtonId(kLostRunesActionButtonIds, entry_id)) {
        return true;
    }

    return false;
}

void AddRowsForIds(
    const Game::SingletonRegistry& singletons,
    std::span<const std::uint32_t> ids,
    std::vector<AutoPickup::ActionButtonRowState>& rows) {
    for (const std::uint32_t row_id : ids) {
        if (auto* row = Game::FindParamRow<ActionButtonParam>(singletons, L"ActionButtonParam", row_id)) {
            auto* bytes = reinterpret_cast<std::uint8_t*>(row);
            rows.push_back({
                .row_id = row_id,
                .row = bytes,
                .original_radius = *reinterpret_cast<float*>(bytes + ERD_OFFSET_ACTIONBUTTON_PARAM_ST_radius),
                .original_height = *reinterpret_cast<float*>(bytes + ERD_OFFSET_ACTIONBUTTON_PARAM_ST_height),
                .original_base_height_offset =
                    *reinterpret_cast<float*>(bytes + ERD_OFFSET_ACTIONBUTTON_PARAM_ST_baseHeightOffset),
            });
        }
    }
}

extern "C" int CheckExecActionButtonParamFilters(uintptr_t action_button_region_system_imp, int entry_id) {
    (void)action_button_region_system_imp;
    if (g_auto_pickup_instance == nullptr) {
        return -1;
    }
    return g_auto_pickup_instance->FilterEntry(entry_id);
}

struct SoundCombatStruct {
    std::uint8_t reserved[0x4];
    std::uint8_t is_in_combat = 0;
};

struct CSSound {
    std::uint8_t reserved[0x328];
    SoundCombatStruct* sound_combat_struct = nullptr;
};

}  // namespace

void AutoPickup::Tick(const Game::SingletonRegistry& singletons) {
    g_auto_pickup_instance = this;
    cssound_ = singletons.GetObjectPointer("CSSound");
    EnsureHookInstalled();
    EnsureRowsCaptured(singletons);
    SyncActionButtonParams(singletons);
}

void AutoPickup::EnsureHookInstalled() {
    if (hook_installed_) {
        return;
    }

    execute_action_button_param_proxy_ = reinterpret_cast<uintptr_t>(modutils::scan<void>({
        .aob = "48 89 5C 24 08 57 48 81 EC 90 00 00 00 48 8B 84 24 E0 00 00 00 41 0F B6 D9 48 8B 0D ?? ?? ?? ?? 8B FA 0F 29 B4 24 80 00 00 00",
    }));
    if (execute_action_button_param_proxy_ == 0) {
        return;
    }

    const MH_STATUS create_status = MH_CreateHook(
        reinterpret_cast<void*>(execute_action_button_param_proxy_),
        reinterpret_cast<void*>(&ExecActionButtonParamProxyWrapper),
        reinterpret_cast<void**>(&ExecuteActionButtonParamProxyReturn));
    if (create_status != MH_OK) {
        spdlog::warn("Auto pickup hook create failed: {}", MH_StatusToString(create_status));
        return;
    }

    const MH_STATUS enable_status = MH_EnableHook(reinterpret_cast<void*>(execute_action_button_param_proxy_));
    if (enable_status != MH_OK) {
        spdlog::warn("Auto pickup hook enable failed: {}", MH_StatusToString(enable_status));
        return;
    }

    hook_installed_ = true;
    spdlog::info("Auto pickup hook installed at 0x{:X}.", execute_action_button_param_proxy_);
}

void AutoPickup::EnsureRowsCaptured(const Game::SingletonRegistry& singletons) {
    if (rows_captured_) {
        return;
    }

    action_button_rows_.clear();
    AddRowsForIds(singletons, kMaterialActionButtonIds, action_button_rows_);
    AddRowsForIds(singletons, kItemActionButtonIds, action_button_rows_);
    AddRowsForIds(singletons, kCorpseLootActionButtonIds, action_button_rows_);
    AddRowsForIds(singletons, kLostRunesActionButtonIds, action_button_rows_);

    std::sort(action_button_rows_.begin(), action_button_rows_.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.row_id < rhs.row_id;
    });
    action_button_rows_.erase(
        std::unique(action_button_rows_.begin(), action_button_rows_.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.row_id == rhs.row_id;
        }),
        action_button_rows_.end());

    rows_captured_ = !action_button_rows_.empty();
    if (rows_captured_) {
        spdlog::info("Auto pickup captured {} ActionButtonParam rows.", action_button_rows_.size());
    }
}

void AutoPickup::SyncActionButtonParams(const Game::SingletonRegistry& singletons) {
    (void)singletons;
    if (!rows_captured_) {
        return;
    }

    const bool enabled = ERD::Main::g_FeatureStatus.auto_pickup_enabled.load();
    const float multiplier =
        static_cast<float>(std::clamp(ERD::Main::g_FeatureStatus.auto_pickup_range_percent.load(), 100, 500)) / 100.0f;

    for (const auto& state : action_button_rows_) {
        if (state.row == nullptr) {
            continue;
        }

        float radius = state.original_radius;
        float height = state.original_height;
        float base_height_offset = state.original_base_height_offset;
        if (enabled && IsSelectedActionButtonId(static_cast<int>(state.row_id))) {
            radius *= multiplier;
            height *= multiplier;
            base_height_offset *= multiplier;
        }

        *reinterpret_cast<float*>(state.row + ERD_OFFSET_ACTIONBUTTON_PARAM_ST_radius) = radius;
        *reinterpret_cast<float*>(state.row + ERD_OFFSET_ACTIONBUTTON_PARAM_ST_height) = height;
        *reinterpret_cast<float*>(state.row + ERD_OFFSET_ACTIONBUTTON_PARAM_ST_baseHeightOffset) = base_height_offset;
    }
}

bool AutoPickup::IsInCombat() const {
    if (cssound_ == 0) {
        return false;
    }

    const auto* sound = reinterpret_cast<const CSSound*>(cssound_);
    if (sound == nullptr || sound->sound_combat_struct == nullptr) {
        return false;
    }

    return sound->sound_combat_struct->is_in_combat != 0;
}

int AutoPickup::FilterEntry(int entry_id) const {
    if (!IsSelectedActionButtonId(entry_id)) {
        return -1;
    }
    if (!ERD::Main::g_FeatureStatus.auto_pickup_in_combat.load() && IsInCombat()) {
        return -1;
    }
    return 1;
}

}  // namespace ERD::Features
