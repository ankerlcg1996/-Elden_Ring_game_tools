#pragma once

#include "../Common.hpp"
#include "../Game/SingletonRegistry.hpp"

#include <vector>

namespace ERD::Features {

class AutoPickup {
public:
    struct ActionButtonRowState {
        std::uint32_t row_id = 0;
        std::uint8_t* row = nullptr;
        float original_radius = 0.0f;
        float original_height = 0.0f;
        float original_base_height_offset = 0.0f;
    };

    void Tick(const Game::SingletonRegistry& singletons);
    int FilterEntry(int entry_id) const;

private:
    void EnsureHookInstalled();
    void EnsureRowsCaptured(const Game::SingletonRegistry& singletons);
    void SyncActionButtonParams(const Game::SingletonRegistry& singletons);
    bool IsInCombat() const;

    uintptr_t execute_action_button_param_proxy_ = 0;
    bool hook_installed_ = false;
    bool rows_captured_ = false;
    std::vector<ActionButtonRowState> action_button_rows_;
    std::uintptr_t cssound_ = 0;
};

}  // namespace ERD::Features
