#include "grace_test_localization.hpp"

#include "Common.hpp"
#include "grace_test_config.hpp"
#include "grace_test_messages.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <mutex>
#include <string>
#include <unordered_map>

namespace grace_test::localization {
namespace {

std::mutex g_mutex;
std::filesystem::path g_lang_dir;
std::string g_active_language = "en-US";
std::filesystem::path g_loaded_path;
std::filesystem::file_time_type g_loaded_write_time{};
std::unordered_map<std::string, std::wstring> g_strings;

std::wstring utf8_to_wide(const std::string& value) {
    if (value.empty()) {
        return {};
    }

    const int required =
        MultiByteToWideChar(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0);
    if (required <= 0) {
        return {};
    }

    std::wstring result(static_cast<std::size_t>(required), L'\0');
    MultiByteToWideChar(
        CP_UTF8,
        0,
        value.c_str(),
        static_cast<int>(value.size()),
        result.data(),
        required);
    return result;
}

std::string trim_copy(std::string value) {
    auto is_space = [](unsigned char ch) { return std::isspace(ch) != 0; };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), [&](unsigned char ch) { return !is_space(ch); }));
    value.erase(std::find_if(value.rbegin(), value.rend(), [&](unsigned char ch) { return !is_space(ch); }).base(), value.end());
    return value;
}

std::unordered_map<std::string, std::wstring> default_strings_for(const std::string& language) {
    if (_stricmp(language.c_str(), "zh-CN") == 0) {
        return {
            {"root", L"erdGameTools"},
            {"game_mods", L"游戏修改"},
            {"unlocks", L"解锁功能"},
            {"quick_access", L"快捷入口"},
            {"back", L"返回"},
            {"no_entries", L"未找到可用快捷项"},
            {"toggle_suffix_on", L"[开]"},
            {"toggle_suffix_off", L"[关]"},
            {"toggle_weightless_equipment", L"装备无重量"},
            {"toggle_no_item_consume", L"道具不消耗"},
            {"toggle_no_damage", L"无伤"},
            {"toggle_no_stamina", L"不耗精力"},
            {"toggle_free_purchase", L"商店免费购买"},
            {"toggle_no_craft_cost", L"制作不消耗材料"},
            {"toggle_no_upgrade_cost", L"强化不消耗材料"},
            {"toggle_no_magic_req", L"魔法没有使用需求"},
            {"toggle_one_slot_magic", L"所有魔法只占一个栏位"},
            {"toggle_mount_anywhere", L"随处骑马"},
            {"toggle_spirit_anywhere", L"随处召唤骨灰"},
            {"toggle_posture_bars", L"架势条显示"},
            {"toggle_damage_popups", L"伤害显示"},
            {"unlock_maps", L"解锁全部地图"},
            {"unlock_cookbooks", L"解锁全部制作笔记"},
            {"unlock_whetblades", L"解锁全部砥石刀"},
            {"unlock_graces", L"解锁全部赐福"},
            {"unlock_summoning_pools", L"解锁全部召唤池"},
            {"unlock_colosseums", L"解锁全部竞技场"},
            {"unlock_gestures", L"解锁全部肢体动作"},
        };
    }

    return {
        {"root", L"erdGameTools"},
        {"game_mods", L"Game Mods"},
        {"unlocks", L"Unlocks"},
        {"quick_access", L"Quick Access"},
        {"back", L"Back"},
        {"no_entries", L"No quick access entries"},
        {"toggle_suffix_on", L"[On]"},
        {"toggle_suffix_off", L"[Off]"},
        {"toggle_weightless_equipment", L"Weightless Equipment"},
        {"toggle_no_item_consume", L"Infinite Item Usage"},
        {"toggle_no_damage", L"No Damage"},
        {"toggle_no_stamina", L"No Stamina Cost"},
        {"toggle_free_purchase", L"Free Purchase"},
        {"toggle_no_craft_cost", L"No Craft Material Cost"},
        {"toggle_no_upgrade_cost", L"No Upgrade Material Cost"},
        {"toggle_no_magic_req", L"No Magic Requirements"},
        {"toggle_one_slot_magic", L"All Magic Uses One Slot"},
        {"toggle_mount_anywhere", L"Mount Anywhere"},
        {"toggle_spirit_anywhere", L"Spirit Ashes Anywhere"},
        {"toggle_posture_bars", L"Posture Bars"},
        {"toggle_damage_popups", L"Damage Popups"},
        {"unlock_maps", L"Unlock All Maps"},
        {"unlock_cookbooks", L"Unlock All Cookbooks"},
        {"unlock_whetblades", L"Unlock All Whetblades"},
        {"unlock_graces", L"Unlock All Graces"},
        {"unlock_summoning_pools", L"Unlock All Summoning Pools"},
        {"unlock_colosseums", L"Unlock All Colosseums"},
        {"unlock_gestures", L"Unlock All Gestures"},
    };
}

void load_strings_for_language(const std::string& language) {
    const std::filesystem::path requested_path = g_lang_dir / (utf8_to_wide(language) + L".txt");
    const std::filesystem::path fallback_path = g_lang_dir / L"en-US.txt";

    std::filesystem::path path_to_load = requested_path;
    if (!std::filesystem::exists(path_to_load)) {
        path_to_load = fallback_path;
    }

    std::unordered_map<std::string, std::wstring> next = default_strings_for(language);
    if (path_to_load.empty() || !std::filesystem::exists(path_to_load)) {
        g_strings = std::move(next);
        g_loaded_path.clear();
        g_loaded_write_time = {};
        return;
    }

    std::ifstream input(path_to_load, std::ios::binary);
    std::string line;
    bool first_line = true;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (first_line && line.size() >= 3 &&
            static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF) {
            line.erase(0, 3);
        }
        first_line = false;

        line = trim_copy(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::size_t separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        const std::string key = trim_copy(line.substr(0, separator));
        const std::string value = trim_copy(line.substr(separator + 1));
        if (!key.empty()) {
            next[key] = utf8_to_wide(value);
        }
    }

    g_strings = std::move(next);
    g_loaded_path = path_to_load;
    g_loaded_write_time = std::filesystem::last_write_time(path_to_load);
}

}  // namespace

void initialize(const std::filesystem::path& folder) {
    std::scoped_lock lock(g_mutex);
    g_lang_dir = folder / "Resources" / "Lang";
    g_active_language = "en-US";
    g_loaded_path.clear();
    g_loaded_write_time = {};
    g_strings = default_strings_for(g_active_language);
}

void refresh() {
    std::scoped_lock lock(g_mutex);

    std::string desired_language = config::get_ui_language_override();
    if (desired_language.empty() || _stricmp(desired_language.c_str(), "auto") == 0) {
        desired_language = msg::detect_game_text_language();
    }
    if (desired_language.empty()) {
        desired_language = "en-US";
    }

    const std::filesystem::path expected_path = g_lang_dir / (utf8_to_wide(desired_language) + L".txt");
    const bool needs_reload =
        _stricmp(desired_language.c_str(), g_active_language.c_str()) != 0 ||
        g_loaded_path.empty() ||
        (std::filesystem::exists(expected_path) &&
         (g_loaded_path != expected_path || std::filesystem::last_write_time(expected_path) != g_loaded_write_time));

    if (needs_reload) {
        g_active_language = desired_language;
        load_strings_for_language(desired_language);
    }
}

std::wstring get(std::string_view key) {
    std::scoped_lock lock(g_mutex);
    const auto found = g_strings.find(std::string(key));
    if (found != g_strings.end()) {
        return found->second;
    }
    return utf8_to_wide(std::string(key));
}

std::wstring make_toggle_label(std::string_view key, bool enabled) {
    const std::wstring base = get(key);
    const std::wstring suffix = get(enabled ? "toggle_suffix_on" : "toggle_suffix_off");
    if (base.empty()) {
        return suffix;
    }
    return base + L" " + suffix;
}

std::string active_language_code() {
    std::scoped_lock lock(g_mutex);
    return g_active_language;
}

}  // namespace grace_test::localization
