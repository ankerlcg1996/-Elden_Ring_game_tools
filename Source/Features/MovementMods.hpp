#pragma once

#include "../Common.hpp"

#include "../Game/SingletonRegistry.hpp"

#include <thread>

namespace ERD::Features {

class MovementMods {
public:
    struct CodePatchState {
        uintptr_t target = 0;
        void* cave = nullptr;
        std::array<std::uint8_t, 16> original_bytes{};
        std::size_t patch_size = 0;
        bool captured = false;
        bool active = false;
    };

    ~MovementMods() noexcept;

    void Tick(const Game::SingletonRegistry& singletons);

private:
    void EnsureHooksInstalled();
    void SyncNoFallCamera();
    void SyncWorker(const Game::SingletonRegistry& singletons);
    void StartWorker();
    void StopWorker();
    void WorkerLoop();

    CodePatchState follow_cam_hook_;
    CodePatchState freecam_position_hook_;
    CodePatchState freecam_rotation_hook_;
    CodePatchState no_fall_camera_patch_;
    bool follow_cam_error_logged_ = false;
    bool freecam_position_error_logged_ = false;
    bool freecam_rotation_error_logged_ = false;
    bool no_fall_camera_error_logged_ = false;
    uintptr_t world_chr_storage_address_ = 0;
    std::atomic_bool worker_running_ = false;
    std::thread worker_thread_;
};

}  // namespace ERD::Features
