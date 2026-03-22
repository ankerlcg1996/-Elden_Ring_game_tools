#pragma once

#include <filesystem>
#include <string>

namespace grace_test::config {

void initialize(const std::filesystem::path& folder);
void apply_persisted_feature_state();
void persist_current_feature_state();
std::string get_ui_language_override();

}  // namespace grace_test::config
