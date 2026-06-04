#pragma once

#include "../Common.hpp"
#include "../Game/SingletonRegistry.hpp"

#include <array>

namespace ERD::Features {

struct EzStateTalkCommand {
    int command_id = 0;
    std::array<std::int64_t, 4> params{};
    int param_count = 0;
    bool use_player_handle = false;
};

bool ExecuteEzStateTalkCommand(
    const Game::SingletonRegistry& singletons,
    const EzStateTalkCommand& command);

}  // namespace ERD::Features
