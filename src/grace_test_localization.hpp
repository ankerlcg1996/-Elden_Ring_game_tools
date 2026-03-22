#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace grace_test::localization {

void initialize(const std::filesystem::path& folder);
void refresh();
std::wstring get(std::string_view key);
std::wstring make_toggle_label(std::string_view key, bool enabled);
std::string active_language_code();

}  // namespace grace_test::localization
