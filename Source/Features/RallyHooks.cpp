#include "RallyHooks.hpp"

#include "../Game/Params.hpp"
#include "../Game/Memory.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../ThirdParty/MinHook/MinHook.h"
#include "SpEffectMonitor.hpp"

#include <elden-x/utils/modutils.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>

#include <spdlog/spdlog.h>

namespace ERD::Features {
namespace {

constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kCharacterFlagsBlockOffset = 0x190;
constexpr uintptr_t kGameManRallyDisabledOffset = 0xDB7;
constexpr std::uint16_t kStateInfoRally = 449;

constexpr const char* kRallyUpdateAob =
    "48 8B 09 E8 ? ? ? ? 48 8B 87 90 01 00 00 48 8B 08 E8";
constexpr std::size_t kRallyUpdateCallOffset = 4;

constexpr const char* kRallyHpChangeAob =
    "C6 44 24 28 01 33 D2 F3 0F 11 44 24 20 48 8B 09 E8 ? ? ? ? 48 8B 4B 58 33 D2 E8";
constexpr std::size_t kRallyHpChangeCallOffset = 17;

std::atomic_uintptr_t g_local_player_chr_data{0};
std::atomic_uintptr_t g_game_man{0};
std::atomic_uintptr_t g_cssound{0};
std::atomic_bool g_has_rally_state_info{false};
std::atomic_bool g_logged_missing_rally_state_info{false};

using RallyUpdateFn = void (*)(void* chr_data, float delta_time);
using RallyHpChangeFn = void (*)(
    void* chr_data,
    int new_hp,
    std::uint8_t reset_rally_flag,
    float rally_gain_multiplier,
    float rally_time_multiplier,
    std::uint8_t force_timer_refresh_flag);

RallyUpdateFn g_rally_update_original = nullptr;
RallyHpChangeFn g_rally_hp_change_original = nullptr;

#pragma pack(push, 1)
struct RallyData {
    float rally_potential;
    float rally_cap;
    float rally_timer;
    float rally_regain;
};

struct CSChrDataModule {
    std::uint8_t reserved0[0x138];
    std::int32_t current_hp;
    std::int32_t max_hp;
    std::uint8_t reserved1[0x20];
    RallyData rally_data;
};

struct SoundCombatStruct {
    std::uint8_t reserved[0x4];
    std::uint8_t is_in_combat;
};

struct CSSound {
    std::uint8_t reserved[0x328];
    SoundCombatStruct* sound_combat_struct;
};
#pragma pack(pop)

uintptr_t ResolveRel32CallTarget(uintptr_t call_instruction) {
    if (call_instruction == 0) {
        return 0;
    }

    std::int32_t rel32 = 0;
    if (!Game::ReadValue(call_instruction + 1, rel32)) {
        return 0;
    }

    const auto next_ip = static_cast<std::int64_t>(call_instruction + 5);
    const auto target = next_ip + static_cast<std::int64_t>(rel32);
    return target > 0 ? static_cast<uintptr_t>(target) : 0;
}

bool IsMainPlayerChrData(const void* chr_data) {
    return reinterpret_cast<uintptr_t>(chr_data) == g_local_player_chr_data.load();
}

bool IsRallyFeatureEnabled() {
    return Main::g_FeatureStatus.attack_life_steal_on_hit.load();
}

bool IsRallyDisabledFlagSet() {
    const uintptr_t game_man = g_game_man.load();
    bool disabled = false;
    return game_man != 0 &&
           Game::ReadValue(game_man + kGameManRallyDisabledOffset, disabled) &&
           disabled;
}

bool IsInCombatNow() {
    const auto* sound = reinterpret_cast<const CSSound*>(g_cssound.load());
    return sound != nullptr &&
           sound->sound_combat_struct != nullptr &&
           sound->sound_combat_struct->is_in_combat != 0;
}

float ComputeExponentialDecay(float beyond_cap, float half_life, float delta_time) {
    if (beyond_cap <= 0.0f || half_life <= 0.0f || delta_time <= 0.0f) {
        return 0.0f;
    }

    const float k = std::log(2.0f) / half_life;
    return beyond_cap - (beyond_cap * std::exp(-k * delta_time));
}

float ComputeLinearDecay(float max_hp, float rally_decay, float delta_time) {
    if (max_hp <= 0.0f || rally_decay <= 0.0f || delta_time <= 0.0f) {
        return 0.0f;
    }

    return max_hp * (1.0f / rally_decay) * delta_time;
}

void RallyUpdateDetour(void* chr_data, float delta_time) {
    auto* data = static_cast<CSChrDataModule*>(chr_data);
    if (data == nullptr) {
        return;
    }

    if (!IsRallyFeatureEnabled() ||
        IsRallyDisabledFlagSet() ||
        !IsMainPlayerChrData(chr_data)) {
        if (g_rally_update_original != nullptr) {
            g_rally_update_original(chr_data, delta_time);
        }
        return;
    }

    float current_hp = static_cast<float>(data->current_hp);
    const float max_hp = static_cast<float>(data->max_hp);
    float rally_potential = data->rally_data.rally_potential;
    float rally_cap = data->rally_data.rally_cap;
    float rally_timer = data->rally_data.rally_timer;
    float rally_regain = data->rally_data.rally_regain;

    if (rally_timer < delta_time) {
        rally_cap = 0.0f;
        rally_timer = 0.0f;
    } else {
        rally_timer -= delta_time;
    }

    if (rally_potential >= rally_cap) {
        float change = 0.0f;
        if (Main::g_FeatureStatus.rally_exponential_decay.load()) {
            change = ComputeExponentialDecay(
                rally_potential - rally_cap,
                std::max(Main::g_FeatureStatus.rally_half_life_seconds.load(), 0.001f),
                delta_time);
        } else {
            change = ComputeLinearDecay(
                max_hp,
                std::max(Main::g_FeatureStatus.rally_decay_seconds.load(), 0.001f),
                delta_time);
        }

        if ((rally_potential - change) < rally_cap) {
            rally_potential = rally_cap;
        } else {
            rally_potential -= change;
        }
    }

    const float hp_to_full = std::max(0.0f, max_hp - current_hp);
    if (rally_potential > hp_to_full) {
        rally_potential = hp_to_full;
    }

    float to_regain = 0.0f;
    if (rally_regain > 1.0f && current_hp > 0.0f) {
        to_regain = rally_regain;
        if (rally_potential < to_regain) {
            to_regain = rally_potential;
        }
        if (hp_to_full < to_regain) {
            to_regain = hp_to_full;
        }
    }

    if (Main::g_FeatureStatus.rally_hit_reset.load() && rally_regain > 0.0f) {
        rally_timer = std::max(Main::g_FeatureStatus.rally_time_seconds.load(), 0.0f);
        rally_cap = rally_potential;
    }

    current_hp += std::ceil(to_regain);
    rally_potential -= to_regain;
    rally_cap -= to_regain;
    rally_regain = 0.0f;

    data->current_hp = std::clamp(static_cast<int>(current_hp), 0, data->max_hp);
    data->rally_data.rally_potential = std::max(rally_potential, 0.0f);
    data->rally_data.rally_cap = std::max(rally_cap, 0.0f);
    data->rally_data.rally_timer = std::max(rally_timer, 0.0f);
    data->rally_data.rally_regain = rally_regain;
}

void RallyHpChangeDetour(
    void* chr_data,
    int new_hp,
    std::uint8_t reset_rally_flag,
    float rally_gain_multiplier,
    float rally_time_multiplier,
    std::uint8_t force_timer_refresh_flag) {
    auto* data = static_cast<CSChrDataModule*>(chr_data);
    if (data == nullptr) {
        return;
    }

    const bool reset_rally = reset_rally_flag != 0;
    const bool force_timer_refresh = force_timer_refresh_flag != 0;

    const int old_hp = data->current_hp;
    const int max_hp = data->max_hp;
    if (!IsRallyFeatureEnabled() ||
        IsRallyDisabledFlagSet() ||
        !IsMainPlayerChrData(chr_data)) {
        if (g_rally_hp_change_original != nullptr) {
            g_rally_hp_change_original(
                chr_data,
                new_hp,
                reset_rally_flag,
                rally_gain_multiplier,
                rally_time_multiplier,
                force_timer_refresh_flag);
        }
        return;
    }

    const int clamped_hp = std::clamp(new_hp, 0, max_hp);
    data->current_hp = clamped_hp;
    const int hp_delta = clamped_hp - old_hp;

    RallyData& rally = data->rally_data;
    if (reset_rally) {
        rally.rally_cap = 0.0f;
        rally.rally_timer = 0.0f;
        rally.rally_regain = 0.0f;
    }

    const float missing_hp = static_cast<float>(max_hp - clamped_hp);
    if (hp_delta < 0 && rally_gain_multiplier > 0.0f) {
        const float damage_taken = static_cast<float>(-hp_delta);
        const float rally_gain = damage_taken * rally_gain_multiplier;

        rally.rally_potential += rally_gain;
        rally.rally_cap += rally_gain;

        rally.rally_potential = std::clamp(rally.rally_potential, 0.0f, missing_hp);
        rally.rally_cap = std::clamp(rally.rally_cap, 0.0f, rally.rally_potential);
        rally.rally_regain = std::clamp(rally.rally_regain, 0.0f, rally.rally_potential);

        if (force_timer_refresh || rally.rally_timer <= 0.0f) {
            rally.rally_timer =
                std::max(Main::g_FeatureStatus.rally_time_seconds.load(), 0.0f) * rally_time_multiplier;
        }
    } else {
        rally.rally_potential = std::clamp(rally.rally_potential, 0.0f, missing_hp);
        rally.rally_cap = std::clamp(rally.rally_cap, 0.0f, rally.rally_potential);
        rally.rally_regain = std::clamp(rally.rally_regain, 0.0f, rally.rally_potential);
    }

    if (Main::g_FeatureStatus.rally_only_heal.load() && hp_delta > 0 && IsInCombatNow()) {
        rally.rally_potential += static_cast<float>(hp_delta);
        data->current_hp -= hp_delta - 1;

        const float new_timer = static_cast<float>(hp_delta) / 200.0f;
        if (new_timer >= 0.5f) {
            rally.rally_cap = rally.rally_potential;
        }
        if (rally.rally_timer < new_timer) {
            rally.rally_timer = new_timer;
        }
    }
}

}  // namespace

void RallyHooks::Tick(const Game::SingletonRegistry& singletons) {
    RefreshRuntimeState(singletons);
    EnsureRallySpEffectApplied(singletons);
    EnsureHooksInstalled();

    if (IsRallyFeatureEnabled() && !g_has_rally_state_info.load() && !g_logged_missing_rally_state_info.exchange(true)) {
        spdlog::warn("Rally: state_info 449 not present; running in forced mode from menu toggle.");
    }
    if ((!IsRallyFeatureEnabled() || g_has_rally_state_info.load()) && g_logged_missing_rally_state_info.load()) {
        g_logged_missing_rally_state_info = false;
    }
}

void RallyHooks::Clear() {
    g_local_player_chr_data = 0;
    g_game_man = 0;
    g_cssound = 0;
    g_has_rally_state_info = false;
    g_logged_missing_rally_state_info = false;
    applied_rally_sp_effect_id_ = 0;
}

void RallyHooks::EnsureHooksInstalled() {
    if (hooks_installed_) {
        return;
    }

    if (rally_update_target_ == 0) {
        if (void* match = modutils::scan<void>({.aob = kRallyUpdateAob})) {
            rally_update_target_ = ResolveRel32CallTarget(reinterpret_cast<uintptr_t>(match) + kRallyUpdateCallOffset);
        }
    }
    if (rally_hp_change_target_ == 0) {
        if (void* match = modutils::scan<void>({.aob = kRallyHpChangeAob})) {
            rally_hp_change_target_ =
                ResolveRel32CallTarget(reinterpret_cast<uintptr_t>(match) + kRallyHpChangeCallOffset);
        }
    }

    if (rally_update_target_ == 0 || rally_hp_change_target_ == 0) {
        if (!logged_missing_hook_targets_) {
            logged_missing_hook_targets_ = true;
            spdlog::warn(
                "Rally: hook targets unresolved (update=0x{:X}, hp_change=0x{:X}).",
                static_cast<unsigned long long>(rally_update_target_),
                static_cast<unsigned long long>(rally_hp_change_target_));
        }
        return;
    }
    logged_missing_hook_targets_ = false;

    const MH_STATUS create_update_status = MH_CreateHook(
        reinterpret_cast<void*>(rally_update_target_),
        reinterpret_cast<void*>(&RallyUpdateDetour),
        reinterpret_cast<void**>(&g_rally_update_original));
    if (create_update_status != MH_OK && create_update_status != MH_ERROR_ALREADY_CREATED) {
        spdlog::warn("Rally update hook create failed: {}", MH_StatusToString(create_update_status));
        return;
    }

    const MH_STATUS create_hp_change_status = MH_CreateHook(
        reinterpret_cast<void*>(rally_hp_change_target_),
        reinterpret_cast<void*>(&RallyHpChangeDetour),
        reinterpret_cast<void**>(&g_rally_hp_change_original));
    if (create_hp_change_status != MH_OK && create_hp_change_status != MH_ERROR_ALREADY_CREATED) {
        spdlog::warn("Rally hp change hook create failed: {}", MH_StatusToString(create_hp_change_status));
        return;
    }

    const MH_STATUS enable_update_status = MH_EnableHook(reinterpret_cast<void*>(rally_update_target_));
    if (enable_update_status != MH_OK && enable_update_status != MH_ERROR_ENABLED) {
        spdlog::warn("Rally update hook enable failed: {}", MH_StatusToString(enable_update_status));
        return;
    }

    const MH_STATUS enable_hp_change_status = MH_EnableHook(reinterpret_cast<void*>(rally_hp_change_target_));
    if (enable_hp_change_status != MH_OK && enable_hp_change_status != MH_ERROR_ENABLED) {
        spdlog::warn("Rally hp change hook enable failed: {}", MH_StatusToString(enable_hp_change_status));
        return;
    }

    hooks_installed_ = true;
    spdlog::info(
        "Rally hooks installed at 0x{:X} and 0x{:X}.",
        static_cast<unsigned long long>(rally_update_target_),
        static_cast<unsigned long long>(rally_hp_change_target_));
}

void RallyHooks::RefreshRuntimeState(const Game::SingletonRegistry& singletons) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    uintptr_t slots_root = 0;
    uintptr_t player_base = 0;
    uintptr_t chr_data = 0;
    if (world_chr_man != 0 &&
        Game::ReadValue(world_chr_man + kNetPlayersOffset, slots_root) &&
        slots_root != 0 &&
        Game::ReadValue(slots_root, player_base) &&
        player_base != 0) {
        chr_data = player_base + kCharacterFlagsBlockOffset;
    }

    g_local_player_chr_data = chr_data;
    g_game_man = singletons.GetObjectPointer("GameMan");
    g_cssound = singletons.GetObjectPointer("CSSound");
    g_has_rally_state_info = HasRallyStateInfo();
}

void RallyHooks::ResolveRallySpEffectIds(const Game::SingletonRegistry& singletons) {
    if (rally_sp_effect_ids_initialized_) {
        return;
    }
    rally_sp_effect_ids_initialized_ = true;
    rally_sp_effect_ids_.clear();

    Game::ForEachParamRow<ERD::Game::SpEffectParam>(
        singletons,
        L"SpEffectParam",
        [&](std::uint64_t row_id, ERD::Game::SpEffectParam* row) {
            if (row != nullptr && row->stateInfo == kStateInfoRally) {
                rally_sp_effect_ids_.push_back(static_cast<std::uint32_t>(row_id));
            }
        });

    if (rally_sp_effect_ids_.empty()) {
        spdlog::warn("Rally: no SpEffectParam rows found with stateInfo=449.");
        return;
    }

    std::string ids;
    for (std::size_t i = 0; i < rally_sp_effect_ids_.size(); ++i) {
        if (i != 0) {
            ids += ", ";
        }
        ids += std::to_string(rally_sp_effect_ids_[i]);
    }
    spdlog::info("Rally: discovered stateInfo=449 SpEffect IDs: [{}].", ids);
}

void RallyHooks::EnsureRallySpEffectApplied(const Game::SingletonRegistry& singletons) {
    ResolveRallySpEffectIds(singletons);

    const SpEffectMonitorSnapshot snapshot = GetSpEffectMonitorSnapshot();
    if (!snapshot.valid || snapshot.player_base == 0) {
        return;
    }

    if (!IsRallyFeatureEnabled()) {
        if (applied_rally_sp_effect_id_ != 0) {
            if (SpEffectMonitor* monitor = SpEffectMonitor::Instance()) {
                monitor->RemoveSpEffect(snapshot.player_base, applied_rally_sp_effect_id_);
            }
            applied_rally_sp_effect_id_ = 0;
        }
        return;
    }

    if (g_has_rally_state_info.load() || rally_sp_effect_ids_.empty()) {
        return;
    }

    const std::uint32_t sp_effect_id = rally_sp_effect_ids_.front();
    if (SpEffectMonitor* monitor = SpEffectMonitor::Instance()) {
        if (monitor->ApplySpEffect(snapshot.player_base, sp_effect_id)) {
            applied_rally_sp_effect_id_ = sp_effect_id;
            spdlog::info("Rally: applied fallback SpEffect {} to player.", sp_effect_id);
        }
    }
}

bool RallyHooks::HasRallyStateInfo() const {
    const SpEffectMonitorSnapshot snapshot = GetSpEffectMonitorSnapshot();
    if (!snapshot.valid) {
        return false;
    }

    for (const SpEffectEntrySnapshot& entry : snapshot.active_effects) {
        if (entry.state_info == kStateInfoRally) {
            return true;
        }
    }
    return false;
}

bool RallyHooks::IsRallyDisabled() const {
    return IsRallyDisabledFlagSet();
}

bool RallyHooks::IsInCombat() const {
    return IsInCombatNow();
}

}  // namespace ERD::Features
