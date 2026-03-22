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
    void SyncStartNextCycle(const Game::SingletonRegistry& singletons);
    void SyncGiveRunes(const Game::SingletonRegistry& singletons);
    void SyncCustomItemGive(const Game::SingletonRegistry& singletons);
    void SyncMassItemGib(const Game::SingletonRegistry& singletons);
    void SyncAddFlaskCharge(const Game::SingletonRegistry& singletons);
    void SyncSetFlaskLevel(const Game::SingletonRegistry& singletons);
    void SyncCoordinates(const Game::SingletonRegistry& singletons);
    void SyncCharacterEditor(const Game::SingletonRegistry& singletons);
    void SyncResourceEditor(const Game::SingletonRegistry& singletons);
    void SyncEquipmentEditor(const Game::SingletonRegistry& singletons);
    void SyncMagicEditor(const Game::SingletonRegistry& singletons);
    void SyncGestureEditor(const Game::SingletonRegistry& singletons);
    void SyncEventFlagDebug(const Game::SingletonRegistry& singletons);

    bool next_cycle_applied_ = false;
};

}  // namespace ERD::Features
