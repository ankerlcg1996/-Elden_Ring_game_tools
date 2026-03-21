#pragma once

#include "../Common.hpp"

#include "../Features/CharacterFlags.hpp"
#include "../Features/GameActions.hpp"
#include "../Features/ParamPatches.hpp"
#include "../Features/Unlocks.hpp"
#include "../Game/SingletonRegistry.hpp"
#include "Config.hpp"
#include "D3D12Overlay.hpp"
#include "FeatureStatus.hpp"
#include "Paths.hpp"

namespace ERD::Main {

// 插件运行时调度器。
// 负责把配置、运行时共享状态、功能模块和渲染层串起来。
class Runtime {
public:
    Runtime(ModPaths paths, ModConfig config);

    void Run();

private:
    void MaybeCreateConsole() const;
    void RedirectStdoutToConsole() const;
    void LogStartup() const;
    void InitializeFeatureStatus() const;
    ModConfig CapturePersistentFeatureConfig() const;
    void SyncPersistentConfig();
    bool IsCharacterLoaded(const Game::SingletonRegistry& singletons) const;
    void RunFeatureLoop();

    ModPaths paths_;
    ModConfig config_;
    bool last_game_ready_ = false;
    Features::CharacterFlags character_flags_;
    Features::Unlocks unlocks_;
    Features::ParamPatches param_patches_;
    Features::GameActions game_actions_;
};

}  // namespace ERD::Main
