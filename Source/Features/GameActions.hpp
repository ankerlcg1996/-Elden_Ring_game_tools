#pragma once

#include "../Common.hpp"

#include "../Game/SingletonRegistry.hpp"

namespace ERD::Features {

// 一次性游戏动作。
// 这里先承接 "开始下一周目" 这种不是改 flag、也不是改 param 的直接写内存功能。
class GameActions {
public:
    void Tick(const Game::SingletonRegistry& singletons);

private:
    bool next_cycle_applied_ = false;
};

}  // namespace ERD::Features
