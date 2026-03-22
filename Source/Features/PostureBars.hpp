#pragma once

#include "../Common.hpp"

namespace ERD::Features {

struct PostureBarEntry {
    std::uint64_t handle = 0;
    std::size_t slot = 0;
    float current = 0.0f;
    float maximum = 0.0f;
    int hp = 0;
    int max_hp = 0;
    int recent_damage = 0;
    float screen_x = 0.0f;
    float screen_y = 0.0f;
    float distance_modifier = 1.0f;
    bool visible = false;
    bool use_stamina = false;
};

struct PostureBarsSnapshot {
    std::optional<PostureBarEntry> player_bar;
    std::optional<PostureBarEntry> locked_target_bar;
    std::vector<PostureBarEntry> boss_bars;
    std::vector<PostureBarEntry> entity_bars;
    bool hook_installed = false;
};

class PostureBars {
public:
    PostureBars();
    ~PostureBars() noexcept;

    void Tick();
    PostureBarsSnapshot Snapshot() const;
    bool ResolveCurrentTarget(uintptr_t& base, std::uint64_t& handle);
    static PostureBars* Instance();

private:
    using GetChrInsFromHandleFn = void* (*)(void* world_chr_man, std::uint64_t* handle);
    using UpdateUiBarsFn = void (*)(uintptr_t move_map_step, uintptr_t time);

    bool EnsureHookInstalled();
    void OnUpdateUiBars(uintptr_t move_map_step, uintptr_t time);
    static void HookUpdateUiBars(uintptr_t move_map_step, uintptr_t time);

    mutable std::mutex snapshot_mutex_;
    PostureBarsSnapshot snapshot_;
    bool install_error_logged_ = false;
    bool hook_installed_ = false;
    uintptr_t world_chr_storage_address_ = 0;
    uintptr_t cs_fe_man_storage_address_ = 0;
    uintptr_t update_ui_bars_target_ = 0;
    GetChrInsFromHandleFn get_chr_ins_from_handle_ = nullptr;
    UpdateUiBarsFn original_update_ui_bars_ = nullptr;

    static PostureBars* instance_;
};

PostureBarsSnapshot GetPostureBarsSnapshot();
bool ResolveCurrentTargetFromPostureBars(uintptr_t& base, std::uint64_t& handle);

}  // namespace ERD::Features
