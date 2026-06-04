#pragma once

#include "Game/SingletonRegistry.hpp"

#include <cstdint>

namespace ERD::Plugin {

enum class FeatureGroup : std::uint32_t {
    None = 0,
    EnemyVisualization = 1u << 0,
    PlayerMods = 1u << 1,
    GameMods = 1u << 2,
    EnemyMods = 1u << 3,
    BossRevive = 1u << 4,
    AutoPickup = 1u << 5,
    QuickAccess = 1u << 6,
};

constexpr std::uint32_t ToMask(FeatureGroup group) {
    return static_cast<std::uint32_t>(group);
}

struct HostApi {
    std::uint32_t abi_version = 1;
    const wchar_t* runtime_folder = nullptr;
    const ERD::Game::SingletonRegistry* singletons = nullptr;
};

struct PluginApi {
    std::uint32_t abi_version = 1;
    const char* plugin_id = nullptr;
    const char* display_name = nullptr;
    std::uint32_t feature_group_mask = 0;
    bool (*on_initialize)() = nullptr;
    void (*on_tick)() = nullptr;
    void (*on_shutdown)() = nullptr;
};

using InitPluginFn = bool (*)(const HostApi* host_api, PluginApi* plugin_api);

}  // namespace ERD::Plugin
