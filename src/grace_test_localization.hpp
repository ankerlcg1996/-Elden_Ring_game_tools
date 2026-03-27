#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace grace_test::localization {

void initialize(const std::filesystem::path& folder);
void refresh();
std::wstring get(std::string_view key);
std::wstring make_toggle_label(std::string_view key, bool enabled);
std::wstring make_percent_setting_label(std::string_view key, int percent);
std::wstring make_percent_option_label(int percent);
std::string active_language_code();
std::wstring utf8_to_wide_copy(std::string_view value);

}  // namespace grace_test::localization
