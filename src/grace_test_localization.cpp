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
            {"player_mods", L"玩家修改"},
            {"game_mods", L"游戏修改"},
            {"ng_mods", L"游戏周目(NG)修改"},
            {"enemy_mods", L"敌人修改"},
            {"enemy_visualization", L"可视化敌人抗性"},
            {"boss_revive", L"复活BOSS"},
            {"auto_pickup", L"自动拾取"},
            {"auto_pickup_settings", L"自动拾取设置"},
            {"auto_pickup_range", L"自动拾取范围"},
            {"boss_revive_action", L"BOSS操作"},
            {"boss_revive_do_revive", L"复活BOSS"},
            {"boss_revive_do_warp", L"传送到BOSS"},
            {"enemy_mods_empty", L"暂无功能"},
            {"unlocks", L"解锁功能"},
            {"quick_access", L"快捷入口"},
            {"quick_access_shop", L"商店"},
            {"portable_grace", L"随身大赐福"},
            {"back", L"返回"},
            {"toggle_suffix_on", L"[开]"},
            {"toggle_suffix_off", L"[关]"},
            {"state_enabled", L"开启"},
            {"state_disabled", L"关闭"},
            {"toggle_weightless_equipment", L"装备无重量"},
            {"setting_equipment_weight_reduction", L"装备减重"},
            {"setting_weapon_requirement_reduction", L"装备需求降低"},
            {"setting_player_speed_increase", L"玩家速度增加"},
            {"setting_ng_cycle", L"当前游戏周目"},
            {"setting_buff_duration_extend", L"BUFF持续时间延长"},
            {"setting_weapon_parry", L"开启武器弹刀"},
            {"setting_enemy_hp_increase", L"增加敌人血量"},
            {"setting_enemy_hp_decrease", L"减少敌人血量[互斥]"},
            {"setting_damage_multiplier", L"伤害倍率调整"},
            {"setting_damage_multiplier_menu", L"调整玩家伤害倍率"},
            {"setting_damage_cut_menu", L"调整玩家减免伤害倍率"},
            {"setting_damage_multiplier_up", L"玩家增加伤害倍率调整"},
            {"setting_damage_multiplier_cut", L"玩家减免伤害倍率调整"},
            {"setting_damage_multiplier_reduce", L"玩家减少伤害倍率[增加难度]"},
            {"setting_damage_multiplier_taken", L"玩家增加伤害倍率[增加难度]"},
            {"setting_stamina_cost_reduction", L"精力消耗减少"},
            {"setting_fp_cost_reduction", L"FP(魔法)消耗减少"},
            {"overlay_tuning", L"HUD微调"},
            {"overlay_poise_width", L"韧性条宽度"},
            {"overlay_poise_height", L"韧性条高度"},
            {"overlay_status_width", L"异常条宽度"},
            {"overlay_status_height", L"异常条高度"},
            {"overlay_poise_offset", L"韧性条上下偏移"},
            {"overlay_entity_status_offset", L"普通敌人异常条上下偏移"},
            {"overlay_boss_status_gap", L"BOSS异常条间距"},
            {"model_modification", L"模型修改"},
            {"model_scale_width", L"模型左右宽度"},
            {"model_scale_height", L"模型高度"},
            {"model_scale_depth", L"模型前后宽度"},
            {"toggle_no_item_consume", L"道具不消耗"},
            {"toggle_no_damage", L"无伤"},
            {"toggle_free_purchase", L"商店免费购买"},
            {"toggle_no_craft_cost", L"制作不消耗材料"},
            {"toggle_no_upgrade_cost", L"强化不消耗材料"},
            {"toggle_all_weapons_enchantable", L"所有武器可附魔"},
            {"toggle_all_weapons_ash_of_war_changeable", L"所有武器可更换战灰"},
            {"toggle_no_magic_req", L"魔法没有使用需求"},
            {"toggle_one_slot_magic", L"所有魔法只占一个栏位"},
            {"toggle_beast_not_hostile", L"野兽不会主动攻击（驱兽火把）"},
            {"toggle_silent_footsteps", L"消除脚步声"},
            {"toggle_infinite_consumables", L"消耗品无限"},
            {"toggle_infinite_arrows", L"箭矢无限"},
            {"toggle_no_rune_loss_on_death", L"死亡不掉卢恩"},
            {"toggle_no_rune_arc_loss_on_death", L"死亡不掉卢恩弯弧"},
            {"toggle_no_time_pass_on_death", L"死亡不推进时间"},
            {"toggle_attack_life_steal_on_hit", L"复血机制（Rally）"},
            {"toggle_infinite_jump", L"无限跳（骑马可用）"},
            {"toggle_mount_anywhere", L"随处骑马"},
            {"toggle_spirit_anywhere", L"随处召唤骨灰"},
            {"toggle_torrent_no_death", L"灵马不死"},
            {"toggle_torrent_anywhere", L"任意地点骑马"},
            {"toggle_no_fall_death", L"不会摔死"},
            {"toggle_auto_pickup_enabled", L"自动拾取总开关"},
            {"toggle_auto_pickup_materials", L"自动拾取材料"},
            {"toggle_auto_pickup_items", L"自动拾取物品"},
            {"toggle_auto_pickup_corpse_loot", L"自动拾取尸体战利品"},
            {"toggle_auto_pickup_lost_runes", L"自动拾取掉落卢恩"},
            {"toggle_auto_pickup_in_combat", L"战斗中自动拾取"},
            {"toggle_enemy_resistance_overlay", L"显示锁定目标抗性条"},
            {"toggle_posture_bars", L"架势条显示"},
            {"unlock_maps", L"解锁地图"},
            {"unlock_maps_all", L"全部解锁"},
            {"unlock_maps_base", L"仅解锁本体"},
            {"unlock_maps_dlc", L"仅解锁DLC"},
            {"unlock_cookbooks", L"解锁全部制作笔记"},
            {"unlock_whetblades", L"解锁全部砥石刀"},
            {"unlock_graces", L"解锁全部赐福"},
            {"unlock_summoning_pools", L"解锁全部召唤池"},
            {"unlock_colosseums", L"解锁全部竞技场"},
            {"unlock_gestures", L"解锁全部肢体动作"},
            {"portable_upgrade", L"强化"},
            {"portable_spirit_tuning", L"灵魂调谐"},
            {"portable_dragon_communion_church", L"龙飨教堂"},
            {"portable_dragon_communion_cathedral", L"龙飨大祭坛"},
            {"portable_rebirth", L"重生"},
            {"portable_mirror", L"打开镜子"},
            {"portable_sell", L"出售物品"},
            {"option_off", L"关闭"},
            {"option_weapon_parry_full", L"100%弹刀"},
            {"option_weapon_parry_easy", L"简单"},
            {"option_weapon_parry_medium", L"中等"},
            {"option_weapon_parry_master", L"大师"},
            {"option_permanent", L"永久"},
        };
    }

    return {
        {"root", L"erdGameTools"},
        {"player_mods", L"Player Mods"},
        {"game_mods", L"Game Mods"},
        {"ng_mods", L"Game Cycle (NG)"},
        {"enemy_mods", L"Enemy Mods"},
        {"enemy_visualization", L"Enemy Resistance HUD"},
        {"boss_revive", L"Boss Revive"},
        {"auto_pickup", L"Auto Pickup"},
        {"auto_pickup_settings", L"Auto Pickup Settings"},
        {"auto_pickup_range", L"Auto Pickup Range"},
        {"boss_revive_action", L"Boss Action"},
        {"boss_revive_do_revive", L"Revive Boss"},
        {"boss_revive_do_warp", L"Warp To Boss"},
        {"enemy_mods_empty", L"No features yet"},
        {"unlocks", L"Unlocks"},
        {"quick_access", L"Quick Access"},
        {"quick_access_shop", L"Shop"},
        {"portable_grace", L"Portable Grace"},
        {"back", L"Back"},
        {"toggle_suffix_on", L"[On]"},
        {"toggle_suffix_off", L"[Off]"},
        {"state_enabled", L"On"},
        {"state_disabled", L"Off"},
        {"toggle_weightless_equipment", L"Weightless Equipment"},
        {"setting_equipment_weight_reduction", L"Equipment Weight Reduction"},
        {"setting_weapon_requirement_reduction", L"Equipment Requirement Reduction"},
        {"setting_player_speed_increase", L"Player Speed Increase"},
        {"setting_ng_cycle", L"Current Game Cycle"},
        {"setting_buff_duration_extend", L"Buff Duration Extension"},
        {"setting_weapon_parry", L"Enable Weapon Parry"},
        {"setting_enemy_hp_increase", L"Increase Enemy HP"},
        {"setting_enemy_hp_decrease", L"Decrease Enemy HP [Exclusive]"},
        {"setting_damage_multiplier", L"Damage Multiplier"},
        {"setting_damage_multiplier_menu", L"Adjust Player Damage Multiplier"},
        {"setting_damage_cut_menu", L"Adjust Player Damage Cut Multiplier"},
        {"setting_damage_multiplier_up", L"Player Damage Increase Multiplier"},
        {"setting_damage_multiplier_cut", L"Player Damage Reduction Multiplier"},
        {"setting_damage_multiplier_reduce", L"Player Damage Decrease Multiplier [Hard]"},
        {"setting_damage_multiplier_taken", L"Player Damage Taken Increase Multiplier [Hard]"},
        {"setting_stamina_cost_reduction", L"Stamina Cost Reduction"},
        {"setting_fp_cost_reduction", L"FP Cost Reduction"},
        {"overlay_tuning", L"HUD Tuning"},
        {"overlay_poise_width", L"Poise Bar Width"},
        {"overlay_poise_height", L"Poise Bar Height"},
        {"overlay_status_width", L"Status Bar Width"},
        {"overlay_status_height", L"Status Bar Height"},
        {"overlay_poise_offset", L"Poise Vertical Offset"},
        {"overlay_entity_status_offset", L"Normal Status Vertical Offset"},
        {"overlay_boss_status_gap", L"Boss Status Gap"},
        {"model_modification", L"Model Modification"},
        {"model_scale_width", L"Model Width"},
        {"model_scale_height", L"Model Height"},
        {"model_scale_depth", L"Model Depth"},
        {"toggle_no_item_consume", L"Infinite Item Usage"},
        {"toggle_no_damage", L"No Damage"},
        {"toggle_free_purchase", L"Free Purchase"},
        {"toggle_no_craft_cost", L"No Craft Material Cost"},
        {"toggle_no_upgrade_cost", L"No Upgrade Material Cost"},
        {"toggle_all_weapons_enchantable", L"All Weapons Enchantable"},
        {"toggle_all_weapons_ash_of_war_changeable", L"All Weapons Ash of War Changeable"},
        {"toggle_no_magic_req", L"No Magic Requirements"},
        {"toggle_one_slot_magic", L"All Magic Uses One Slot"},
        {"toggle_beast_not_hostile", L"Beasts Not Aggressive (Beast-Repellent Torch)"},
        {"toggle_silent_footsteps", L"Silent Footsteps"},
        {"toggle_infinite_consumables", L"Infinite Consumables"},
        {"toggle_infinite_arrows", L"Infinite Arrows"},
        {"toggle_no_rune_loss_on_death", L"No Rune Loss On Death"},
        {"toggle_no_rune_arc_loss_on_death", L"No Rune Arc Loss On Death"},
        {"toggle_no_time_pass_on_death", L"No Time Pass On Death"},
        {"toggle_attack_life_steal_on_hit", L"Rally Redone"},
        {"toggle_infinite_jump", L"Infinite Jump (Works on Torrent)"},
        {"toggle_mount_anywhere", L"Mount Anywhere"},
        {"toggle_spirit_anywhere", L"Spirit Ashes Anywhere"},
        {"toggle_torrent_no_death", L"Torrent No Death"},
        {"toggle_torrent_anywhere", L"Torrent Anywhere"},
        {"toggle_no_fall_death", L"No Fall Death"},
        {"toggle_auto_pickup_enabled", L"Auto Pickup Master Switch"},
        {"toggle_auto_pickup_materials", L"Auto Pickup Materials"},
        {"toggle_auto_pickup_items", L"Auto Pickup Items"},
        {"toggle_auto_pickup_corpse_loot", L"Auto Pickup Corpse Loot"},
        {"toggle_auto_pickup_lost_runes", L"Auto Pickup Lost Runes"},
        {"toggle_auto_pickup_in_combat", L"Auto Pickup In Combat"},
        {"toggle_enemy_resistance_overlay", L"Show Target Resistance HUD"},
        {"toggle_posture_bars", L"Posture Bars"},
        {"unlock_maps", L"Unlock Maps"},
        {"unlock_maps_all", L"Unlock All"},
        {"unlock_maps_base", L"Unlock Base Game"},
        {"unlock_maps_dlc", L"Unlock DLC"},
        {"unlock_cookbooks", L"Unlock All Cookbooks"},
        {"unlock_whetblades", L"Unlock All Whetblades"},
        {"unlock_graces", L"Unlock All Graces"},
        {"unlock_summoning_pools", L"Unlock All Summoning Pools"},
        {"unlock_colosseums", L"Unlock All Colosseums"},
        {"unlock_gestures", L"Unlock All Gestures"},
        {"portable_upgrade", L"Upgrade"},
        {"portable_spirit_tuning", L"Spirit Tuning"},
        {"portable_dragon_communion_church", L"Church of Dragon Communion"},
        {"portable_dragon_communion_cathedral", L"Cathedral of Dragon Communion"},
        {"portable_rebirth", L"Rebirth"},
        {"portable_mirror", L"Open Mirror"},
        {"portable_sell", L"Sell Items"},
        {"option_off", L"Off"},
        {"option_weapon_parry_full", L"100% Parry"},
        {"option_weapon_parry_easy", L"Easy"},
        {"option_weapon_parry_medium", L"Medium"},
        {"option_weapon_parry_master", L"Master"},
        {"option_permanent", L"Permanent"},
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

std::wstring make_percent_setting_label(std::string_view key, int percent) {
    const std::wstring base = get(key);
    return base + L" [" + std::to_wstring(std::max(percent, 0)) + L"%]";
}

std::wstring make_percent_option_label(int percent) {
    return std::to_wstring(std::max(percent, 0)) + L"%";
}

std::wstring make_feedback_set_label(std::wstring_view name, std::wstring_view value) {
    if (_stricmp(active_language_code().c_str(), "zh-CN") == 0) {
        return L"已将 " + std::wstring(name) + L" 设置为 " + std::wstring(value);
    }
    return L"Set " + std::wstring(name) + L" to " + std::wstring(value);
}

std::wstring make_feedback_action_label(std::wstring_view action) {
    if (_stricmp(active_language_code().c_str(), "zh-CN") == 0) {
        return L"已执行 " + std::wstring(action);
    }
    return L"Executed " + std::wstring(action);
}

std::string active_language_code() {
    std::scoped_lock lock(g_mutex);
    return g_active_language;
}

std::wstring utf8_to_wide_copy(std::string_view value) {
    return utf8_to_wide(std::string(value));
}

}  // namespace grace_test::localization
