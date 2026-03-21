#include "GameActions.hpp"

#include "../Game/Memory.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"

namespace ERD::Features {
namespace {

constexpr uintptr_t kStartNextCycleOffset = 0xB7D;

}  // namespace

void GameActions::Tick(const Game::SingletonRegistry& singletons) {
    if (next_cycle_applied_ || !Main::g_FeatureStatus.start_next_cycle.load()) {
        return;
    }

    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    if (game_man == 0) {
        return;
    }

    const std::uint8_t enabled = 1;
    if (Game::WriteValue(game_man + kStartNextCycleOffset, enabled)) {
        Main::Logger::Instance().Info("Start next cycle requested.");
        next_cycle_applied_ = true;
    }
}

}  // namespace ERD::Features
