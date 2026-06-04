#include "grace_test_overlay.hpp"
#include "grace_test_runtime.hpp"

#include "Common.hpp"
#include "Features/OverlayD3DRenderer.hpp"
#include "Features/TargetStatusService.hpp"
#include "Game/Memory.hpp"
#include "Main/FeatureStatus.hpp"

#include <spdlog/spdlog.h>

#include <windows.h>

namespace grace_test::overlay {
namespace {

constexpr uintptr_t kSaveSlotOffset = 0xAC0;
constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kCharacterFlagsBlockOffset = 0x190;
constexpr uintptr_t kCommonFlagsOffset = 0x19B;
constexpr ULONGLONG kOverlayHookDelayMs = 3000;
ULONGLONG g_hook_not_before_tick = 0;
ERD::Features::TargetStatusService g_target_status_service;

bool is_character_loaded(const ERD::Game::SingletonRegistry& singletons) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    if (world_chr_man == 0) {
        return false;
    }

    uintptr_t common_flags_address = 0;
    if (ERD::Game::ResolvePointerChain(
            world_chr_man,
            {kNetPlayersOffset, 0x0, kCharacterFlagsBlockOffset, 0x0},
            kCommonFlagsOffset,
            common_flags_address) &&
        common_flags_address != 0) {
        return true;
    }

    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    if (game_man != 0) {
        std::int32_t save_slot = -1;
        if (ERD::Game::ReadValue(game_man + kSaveSlotOffset, save_slot) && save_slot != -1) {
            return true;
        }
    }

    return false;
}

}  // namespace

void initialize(const std::filesystem::path& folder) {
    ERD::Main::g_FeatureStatus.overlay_ready = false;
    ERD::Features::OverlayD3DRenderer::Instance().SetBaseFolder(folder);
    g_hook_not_before_tick = GetTickCount64() + kOverlayHookDelayMs;
    spdlog::info(
        "DX12 overlay: deferring PostureBarMod-style hook installation for {} ms.",
        kOverlayHookDelayMs);
}

void tick() {
    const ERD::Game::SingletonRegistry* singletons = grace_test::runtime::get_singletons();
    if (singletons == nullptr) {
        return;
    }

    auto& renderer = ERD::Features::OverlayD3DRenderer::Instance();
    if (!renderer.IsHooked() && GetTickCount64() >= g_hook_not_before_tick) {
        renderer.Hook();
    }

    if (!is_character_loaded(*singletons)) {
        g_target_status_service.Clear();
        return;
    }

    g_target_status_service.Tick(*singletons);
}

void shutdown() {
    g_target_status_service.Clear();
    ERD::Features::OverlayD3DRenderer::Instance().Shutdown();
}

}  // namespace grace_test::overlay
