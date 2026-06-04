#pragma once

#include "erd_plugin_api.hpp"

#include <filesystem>
#include <string>

namespace grace_test::plugins {

void initialize(const std::filesystem::path& folder);
void set_singletons(const ERD::Game::SingletonRegistry* singletons);
void tick();
void shutdown();
bool is_feature_group_available(ERD::Plugin::FeatureGroup group);
bool is_feature_group_mask_available(std::uint32_t mask);
bool has_any_plugins();
std::string loaded_plugins_summary();

}  // namespace grace_test::plugins
