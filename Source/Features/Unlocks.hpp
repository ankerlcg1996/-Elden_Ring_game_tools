#pragma once

#include "../Common.hpp"

#include "../Game/SingletonRegistry.hpp"

namespace ERD::Features {

// 事件 flag 一次性解锁功能。
// 这类功能和 CT 里的 "Unlock all xxx" 一样，通常只需要执行一次。
class Unlocks {
public:
    void Tick(const Game::SingletonRegistry& singletons);
    void ResetSessionState();

private:
    bool UnlockAllGestures(const Game::SingletonRegistry& singletons) const;

    bool maps_applied_ = false;
    bool cookbooks_applied_ = false;
    bool whetblades_applied_ = false;
    bool graces_applied_ = false;
    bool summoning_pools_applied_ = false;
    bool colosseums_applied_ = false;
    bool gestures_applied_ = false;
};

}  // namespace ERD::Features
