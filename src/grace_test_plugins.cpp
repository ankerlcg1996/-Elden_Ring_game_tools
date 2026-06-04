#include "grace_test_plugins.hpp"

#include <spdlog/spdlog.h>

namespace grace_test::plugins {
namespace {

bool g_initialized = false;
constexpr std::uint32_t kIntegratedFeatureGroupMask =
    ERD::Plugin::ToMask(ERD::Plugin::FeatureGroup::EnemyVisualization) |
    ERD::Plugin::ToMask(ERD::Plugin::FeatureGroup::PlayerMods) |
    ERD::Plugin::ToMask(ERD::Plugin::FeatureGroup::GameMods) |
    ERD::Plugin::ToMask(ERD::Plugin::FeatureGroup::EnemyMods) |
    ERD::Plugin::ToMask(ERD::Plugin::FeatureGroup::BossRevive) |
    ERD::Plugin::ToMask(ERD::Plugin::FeatureGroup::AutoPickup) |
    ERD::Plugin::ToMask(ERD::Plugin::FeatureGroup::QuickAccess);

}  // namespace

void set_singletons(const ERD::Game::SingletonRegistry* singletons) {}

void initialize(const std::filesystem::path& folder) {
    if (g_initialized) {
        return;
    }

    g_initialized = true;
    spdlog::info(
        "Integrated mode active. External feature DLL loading is disabled; all built-in feature groups are enabled.");
}

void tick() {}

void shutdown() {
    g_initialized = false;
}

bool is_feature_group_available(ERD::Plugin::FeatureGroup group) {
    return (kIntegratedFeatureGroupMask & ERD::Plugin::ToMask(group)) != 0;
}

bool is_feature_group_mask_available(std::uint32_t mask) {
    return (kIntegratedFeatureGroupMask & mask) != 0;
}

bool has_any_plugins() {
    return false;
}

std::string loaded_plugins_summary() {
    return "integrated-mode active, external plugins disabled";
}

}  // namespace grace_test::plugins
