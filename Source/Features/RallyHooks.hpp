#pragma once

#include "../Common.hpp"
#include "../Game/SingletonRegistry.hpp"

namespace ERD::Features {

class RallyHooks {
public:
    void Tick(const Game::SingletonRegistry& singletons);
    void Clear();

private:
    void EnsureHooksInstalled();
    void RefreshRuntimeState(const Game::SingletonRegistry& singletons);
    void EnsureRallySpEffectApplied(const Game::SingletonRegistry& singletons);
    void ResolveRallySpEffectIds(const Game::SingletonRegistry& singletons);
    bool HasRallyStateInfo() const;
    bool IsRallyDisabled() const;
    bool IsInCombat() const;

    uintptr_t rally_update_target_ = 0;
    uintptr_t rally_hp_change_target_ = 0;
    bool hooks_installed_ = false;
    bool rally_sp_effect_ids_initialized_ = false;
    std::vector<std::uint32_t> rally_sp_effect_ids_;
    std::uint32_t applied_rally_sp_effect_id_ = 0;
    bool logged_missing_hook_targets_ = false;
};

}  // namespace ERD::Features
