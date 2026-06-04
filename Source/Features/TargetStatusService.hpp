#pragma once

#include "../Common.hpp"
#include "../Game/SingletonRegistry.hpp"

namespace ERD::Features {

class TargetStatusService {
public:
    void Tick(const Game::SingletonRegistry& singletons);
    void Clear();

private:
    bool EnsureResolved();
    bool ResolveAddresses();
    uintptr_t ResolveLocalPlayerChr(uintptr_t world_chr_man);
    bool ResolveTargetHandle(uintptr_t local_player_chr, std::uint64_t& target_handle);
    uintptr_t ResolveTargetChrFromHandle(uintptr_t world_chr_man, std::uint64_t& target_handle, bool& used_fallback);
    uintptr_t ResolveTargetChrIns(const Game::SingletonRegistry& singletons);
    void UpdateBossUiPlacement(std::uint64_t target_handle);
    void PublishSnapshot(uintptr_t target_chr);
    void ClearPublishedSnapshot();
    void ClearUiSnapshot();
    void LogDiagnostics(
        std::uint64_t target_handle,
        uintptr_t target_chr,
        bool used_fallback,
        bool has_chr_data,
        bool has_chr_resist,
        bool has_chr_super_armor,
        bool has_chr_physics);

    uintptr_t cs_fe_man_storage_ = 0;
    uintptr_t get_chr_ins_from_handle_ = 0;
    bool resolve_attempted_ = false;
    bool resolved_ = false;
    std::uint64_t last_logged_target_handle_ = 0;
    uintptr_t last_logged_target_chr_ = 0;
    bool last_logged_used_fallback_ = false;
    bool last_logged_has_chr_data_ = false;
    bool last_logged_has_chr_resist_ = false;
    bool last_logged_has_chr_super_armor_ = false;
    bool last_logged_has_chr_physics_ = false;
};

}  // namespace ERD::Features
