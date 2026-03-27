#pragma once

#include "../Common.hpp"
#include "../Game/SingletonRegistry.hpp"

namespace ERD::Features {

struct SpEffectEntrySnapshot {
    int id = 0;
    float time_left = 0.0f;
    float duration = 0.0f;
    std::uint16_t state_info = 0;
};

struct SpEffectMonitorSnapshot {
    bool valid = false;
    uintptr_t player_base = 0;
    std::vector<SpEffectEntrySnapshot> active_effects;
};

class SpEffectMonitor {
public:
    void Tick(const Game::SingletonRegistry& singletons);
    void Clear();
    SpEffectMonitorSnapshot Snapshot() const;

    // 从 TarnishedTool-CN 提取：先保留底层接口，暂不接菜单调用。
    bool ApplySpEffect(uintptr_t chr_ins, std::uint32_t sp_effect_id);
    bool RemoveSpEffect(uintptr_t chr_ins, std::uint32_t sp_effect_id);

    static SpEffectMonitor* Instance();

private:
    bool EnsureFunctionAddresses();
    uintptr_t GetPlayerBase(const Game::SingletonRegistry& singletons) const;
    void RefreshSnapshot(uintptr_t player_base);

    mutable std::mutex snapshot_mutex_;
    SpEffectMonitorSnapshot snapshot_;

    uintptr_t set_sp_effect_fn_ = 0;
    uintptr_t find_and_remove_sp_effect_fn_ = 0;
    bool function_addresses_initialized_ = false;

    static SpEffectMonitor* instance_;
};

SpEffectMonitorSnapshot GetSpEffectMonitorSnapshot();

}  // namespace ERD::Features

