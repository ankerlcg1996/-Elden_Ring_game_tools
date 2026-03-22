#pragma once

#include "../Common.hpp"

#include "../Game/SingletonRegistry.hpp"

namespace ERD::Features {

std::string GetTargetedNpcNameUtf8();
std::string GetLastHitNpcNameUtf8();

class NpcDebug {
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
    void EnsureHooksInstalled();
    void SyncTrackedNpcStatus(const Game::SingletonRegistry& singletons) const;
    void SyncTeleportActions(const Game::SingletonRegistry& singletons) const;

    bool targeted_hook_error_logged_ = false;
    bool last_hit_hook_error_logged_ = false;
    CodePatchState targeted_npc_hook_;
    CodePatchState last_hit_npc_hook_;
};

}  // namespace ERD::Features
