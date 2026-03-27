#include "grace_test_runtime.hpp"

#include "grace_test_config.hpp"
#include "grace_test_messages.hpp"
#include "grace_test_overlay.hpp"
#include "grace_test_talkscript.hpp"

#include "Common.hpp"
#include "Features/CharacterFlags.hpp"
#include "Features/InspectorTools.hpp"
#include "Features/ParamPatches.hpp"
#include "Features/SpEffectMonitor.hpp"
#include "Game/Memory.hpp"
#include "Game/SingletonRegistry.hpp"
#include "Main/FeatureStatus.hpp"
#include "Main/Logger.hpp"

#include <spdlog/spdlog.h>

namespace grace_test::runtime {
namespace {

ERD::Game::SingletonRegistry g_singletons;
ERD::Features::CharacterFlags g_character_flags;
ERD::Features::ParamPatches g_param_patches;
ERD::Features::InspectorTools g_inspector_tools;
ERD::Features::SpEffectMonitor g_sp_effect_monitor;
bool g_last_game_ready = false;

constexpr uintptr_t kSaveSlotOffset = 0xAC0;
constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kCharacterFlagsBlockOffset = 0x190;
constexpr uintptr_t kCommonFlagsOffset = 0x19B;

bool IsCharacterLoaded(const ERD::Game::SingletonRegistry& singletons) {
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

void tick_features() {
    grace_test::overlay::tick();

    const bool game_ready = IsCharacterLoaded(g_singletons);
    ERD::Main::g_FeatureStatus.game_ready = game_ready;

    if (game_ready && !g_last_game_ready) {
        grace_test::config::apply_persisted_feature_state();
        spdlog::info("Game ready detected. Re-applied persisted game mod settings.");
    } else if (!game_ready && g_last_game_ready) {
        spdlog::info("Character unloaded. Waiting for next in-game load.");
    }
    g_last_game_ready = game_ready;

    if (!game_ready) {
        g_sp_effect_monitor.Clear();
        return;
    }

    g_character_flags.Tick(g_singletons);
    g_param_patches.Tick(g_singletons);
    g_inspector_tools.Tick(g_singletons);
    g_sp_effect_monitor.Tick(g_singletons);
}

}  // namespace

void run(const std::filesystem::path& folder) {
    ERD::Main::Logger::Instance().Initialize(folder / "logs" / "erd_game_tools.log", true);
    grace_test::overlay::initialize(folder);

    while (ERD::g_Running && !g_singletons.Initialize()) {
        spdlog::warn("Waiting for FD4 singletons...");
        Sleep(1000);
    }

    if (!ERD::g_Running) {
        return;
    }

    spdlog::info("FD4 singleton registry initialized.");

    while (ERD::g_Running) {
        try {
            tick_features();
        } catch (const std::exception& error) {
            spdlog::error("Feature tick failed: {}", error.what());
        } catch (...) {
            spdlog::error("Feature tick failed: unknown exception");
        }

        Sleep(ERD::kFeaturePollIntervalMs);
    }

    grace_test::overlay::shutdown();
}

void request_stop() {
    ERD::g_Running = false;
}

}  // namespace grace_test::runtime
