#include "Config.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace ERD::Main {
namespace {

std::string Trim(std::string value) {
    const auto not_space = [](unsigned char ch) { return !std::isspace(ch); };

    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

bool ParseBool(const std::string& value, bool fallback) {
    const std::string lowered = ToLower(Trim(value));
    if (lowered == "1" || lowered == "true" || lowered == "yes" || lowered == "on") {
        return true;
    }
    if (lowered == "0" || lowered == "false" || lowered == "no" || lowered == "off") {
        return false;
    }
    return fallback;
}

float ParseFloat(const std::string& value, float fallback) {
    try {
        return std::stof(Trim(value));
    } catch (...) {
        return fallback;
    }
}

int ParseInt(const std::string& value, int fallback) {
    try {
        return std::stoi(Trim(value));
    } catch (...) {
        return fallback;
    }
}

std::string WideToUtf8(const std::wstring& value) {
    if (value.empty()) {
        return {};
    }

    const int required = WideCharToMultiByte(
        CP_UTF8,
        0,
        value.c_str(),
        static_cast<int>(value.size()),
        nullptr,
        0,
        nullptr,
        nullptr
    );
    if (required <= 0) {
        return {};
    }

    std::string utf8(static_cast<std::size_t>(required), '\0');
    WideCharToMultiByte(
        CP_UTF8,
        0,
        value.c_str(),
        static_cast<int>(value.size()),
        utf8.data(),
        required,
        nullptr,
        nullptr
    );
    return utf8;
}

void ApplyBool(ModConfig& config, const std::string& key, const std::string& value) {
    if (key == "general.create_console") {
        config.create_console = ParseBool(value, config.create_console);
    } else if (key == "general.enable_file_log") {
        config.enable_file_log = ParseBool(value, config.enable_file_log);
    } else if (key == "general.show_message_box") {
        config.show_message_box = ParseBool(value, config.show_message_box);
    } else if (key == "overlay.menu_minimized" || key == "menu_minimized") {
        config.menu_minimized = ParseBool(value, config.menu_minimized);
    } else if (key == "characterflags.no_dead" || key == "no_dead") {
        config.no_dead = ParseBool(value, config.no_dead);
    } else if (key == "characterflags.no_damage" || key == "no_damage") {
        config.no_damage = ParseBool(value, config.no_damage);
    } else if (key == "characterflags.infinite_fp" || key == "infinite_fp" || key == "enable_no_fp_consumption") {
        config.infinite_fp = ParseBool(value, config.infinite_fp);
    } else if (key == "characterflags.infinite_items" || key == "infinite_items" || key == "enable_no_goods_consume") {
        config.infinite_items = ParseBool(value, config.infinite_items);
    } else if (key == "characterflags.no_stamina_consumption" || key == "no_stamina_consumption") {
        config.no_stamina_consumption = ParseBool(value, config.no_stamina_consumption);
    } else if (key == "worldunlocks.unlock_all_maps" || key == "unlock_all_maps") {
        config.unlock_all_maps = ParseBool(value, config.unlock_all_maps);
    } else if (key == "worldunlocks.unlock_all_cookbooks" || key == "unlock_all_cookbooks") {
        config.unlock_all_cookbooks = ParseBool(value, config.unlock_all_cookbooks);
    } else if (key == "worldunlocks.unlock_all_whetblades" || key == "unlock_all_whetblades") {
        config.unlock_all_whetblades = ParseBool(value, config.unlock_all_whetblades);
    } else if (key == "worldunlocks.unlock_all_graces" || key == "unlock_all_graces") {
        config.unlock_all_graces = ParseBool(value, config.unlock_all_graces);
    } else if (key == "worldunlocks.unlock_all_summoning_pools" || key == "unlock_all_summoning_pools") {
        config.unlock_all_summoning_pools = ParseBool(value, config.unlock_all_summoning_pools);
    } else if (key == "worldunlocks.unlock_all_colosseums" || key == "unlock_all_colosseums") {
        config.unlock_all_colosseums = ParseBool(value, config.unlock_all_colosseums);
    } else if (key == "worldunlocks.unlock_all_gestures" || key == "unlock_all_gestures") {
        config.unlock_all_gestures = ParseBool(value, config.unlock_all_gestures);
    } else if (key == "parampatches.faster_respawn" || key == "faster_respawn") {
        config.faster_respawn = ParseBool(value, config.faster_respawn);
    } else if (key == "parampatches.warp_out_of_uncleared_minidungeons" || key == "warp_out_of_uncleared_minidungeons") {
        config.warp_out_of_uncleared_minidungeons =
            ParseBool(value, config.warp_out_of_uncleared_minidungeons);
    } else if (key == "parampatches.free_purchase" || key == "free_purchase") {
        config.free_purchase = ParseBool(value, config.free_purchase);
    } else if (key == "parampatches.no_crafting_material_cost" || key == "no_crafting_material_cost") {
        config.no_crafting_material_cost = ParseBool(value, config.no_crafting_material_cost);
    } else if (key == "parampatches.no_upgrade_material_cost" || key == "no_upgrade_material_cost") {
        config.no_upgrade_material_cost = ParseBool(value, config.no_upgrade_material_cost);
    } else if (key == "parampatches.no_magic_requirements" || key == "no_magic_requirements") {
        config.no_magic_requirements = ParseBool(value, config.no_magic_requirements);
    } else if (key == "parampatches.all_magic_one_slot" || key == "all_magic_one_slot") {
        config.all_magic_one_slot = ParseBool(value, config.all_magic_one_slot);
    } else if (key == "parampatches.weightless_equipment" || key == "weightless_equipment") {
        config.weightless_equipment = ParseBool(value, config.weightless_equipment);
    } else if (key == "parampatches.custom_fov_enabled" || key == "custom_fov_enabled") {
        config.custom_fov_enabled = ParseBool(value, config.custom_fov_enabled);
    } else if (key == "parampatches.custom_camera_distance_enabled" || key == "custom_camera_distance_enabled") {
        config.custom_camera_distance_enabled = ParseBool(value, config.custom_camera_distance_enabled);
    } else if (key == "parampatches.mount_anywhere" || key == "mount_anywhere") {
        config.mount_anywhere = ParseBool(value, config.mount_anywhere);
    } else if (key == "parampatches.spirit_ashes_anywhere" || key == "spirit_ashes_anywhere") {
        config.spirit_ashes_anywhere = ParseBool(value, config.spirit_ashes_anywhere);
    } else if (key == "parampatches.item_discovery_x10" || key == "item_discovery_x10") {
        config.item_discovery_multiplier = ParseBool(value, false) ? 10 : 1;
    } else if (key == "parampatches.permanent_lantern" || key == "permanent_lantern") {
        config.permanent_lantern = ParseBool(value, config.permanent_lantern);
    } else if (key == "parampatches.invisible_helmets" || key == "invisible_helmets") {
        config.invisible_helmets = ParseBool(value, config.invisible_helmets);
    } else if (key == "movement.no_clip_enabled" || key == "no_clip_enabled") {
        config.no_clip_enabled = ParseBool(value, config.no_clip_enabled);
    } else if (key == "movement.freecam_enabled" || key == "freecam_enabled") {
        config.freecam_enabled = ParseBool(value, config.freecam_enabled);
    } else if (key == "movement.no_fall_camera" || key == "no_fall_camera") {
        config.no_fall_camera = ParseBool(value, config.no_fall_camera);
    } else if (key == "gameactions.start_next_cycle" || key == "start_next_cycle") {
        config.start_next_cycle = ParseBool(value, config.start_next_cycle);
    } else if (key == "pendingactions.give_runes_requested" || key == "give_runes_requested") {
        config.give_runes_requested = ParseBool(value, config.give_runes_requested);
    } else if (key == "pendingactions.custom_item_give_requested" || key == "custom_item_give_requested") {
        config.custom_item_give_requested = ParseBool(value, config.custom_item_give_requested);
    } else if (key == "pendingactions.add_flask_charge_requested" || key == "add_flask_charge_requested") {
        config.add_flask_charge_requested = ParseBool(value, config.add_flask_charge_requested);
    } else if (key == "pendingactions.set_flask_level_requested" || key == "set_flask_level_requested") {
        config.set_flask_level_requested = ParseBool(value, config.set_flask_level_requested);
    } else if (key == "pendingactions.debug_event_flag_write_requested" || key == "debug_event_flag_write_requested") {
        config.debug_event_flag_write_requested = ParseBool(value, config.debug_event_flag_write_requested);
    } else if (key == "pendingactions.teleport_player_to_input_coords_requested" || key == "teleport_player_to_input_coords_requested") {
        config.teleport_player_to_input_coords_requested =
            ParseBool(value, config.teleport_player_to_input_coords_requested);
    } else if (key == "pendingactions.character_apply_requested" || key == "character_apply_requested") {
        config.character_apply_requested = ParseBool(value, config.character_apply_requested);
    } else if (key == "pendingactions.resource_apply_requested" || key == "resource_apply_requested") {
        config.resource_apply_requested = ParseBool(value, config.resource_apply_requested);
    } else if (key == "pendingactions.equip_apply_requested" || key == "equip_apply_requested") {
        config.equip_apply_requested = ParseBool(value, config.equip_apply_requested);
    } else if (key == "pendingactions.equip_unequip_requested" || key == "equip_unequip_requested") {
        config.equip_unequip_requested = ParseBool(value, config.equip_unequip_requested);
    } else if (key == "pendingactions.magic_attune_requested" || key == "magic_attune_requested") {
        config.magic_attune_requested = ParseBool(value, config.magic_attune_requested);
    } else if (key == "pendingactions.magic_remove_requested" || key == "magic_remove_requested") {
        config.magic_remove_requested = ParseBool(value, config.magic_remove_requested);
    } else if (key == "pendingactions.gesture_apply_requested" || key == "gesture_apply_requested") {
        config.gesture_apply_requested = ParseBool(value, config.gesture_apply_requested);
    } else if (key == "pendingactions.statistics_load_save_slot_requested" || key == "statistics_load_save_slot_requested") {
        config.statistics_load_save_slot_requested = ParseBool(value, config.statistics_load_save_slot_requested);
    } else if (key == "pendingactions.statistics_save_game_requested" || key == "statistics_save_game_requested") {
        config.statistics_save_game_requested = ParseBool(value, config.statistics_save_game_requested);
    } else if (key == "pendingactions.param_tool_write_requested" || key == "param_tool_write_requested") {
        config.param_tool_write_requested = ParseBool(value, config.param_tool_write_requested);
    } else if (key == "debug.debug_event_flag_desired_state" || key == "debug_event_flag_desired_state") {
        config.debug_event_flag_desired_state = ParseBool(value, config.debug_event_flag_desired_state);
    } else if (key == "statistics.statistics_disable_auto_save" || key == "statistics_disable_auto_save") {
        config.statistics_disable_auto_save = ParseBool(value, config.statistics_disable_auto_save);
    }
}

void ApplyInt(ModConfig& config, const std::string& key, const std::string& value) {
    if (key == "parampatches.item_discovery_multiplier" || key == "item_discovery_multiplier") {
        config.item_discovery_multiplier = std::clamp(ParseInt(value, config.item_discovery_multiplier), 1, 100);
    } else if (key == "resources.give_runes_amount" || key == "give_runes_amount") {
        config.give_runes_amount = std::max(ParseInt(value, config.give_runes_amount), 1);
    } else if (key == "resources.custom_item_id" || key == "custom_item_id") {
        config.custom_item_id = ParseInt(value, config.custom_item_id);
    } else if (key == "resources.custom_item_quantity" || key == "custom_item_quantity") {
        config.custom_item_quantity = std::clamp(ParseInt(value, config.custom_item_quantity), 1, 9999);
    } else if (key == "resources.custom_item_reinforce_level" || key == "custom_item_reinforce_level") {
        config.custom_item_reinforce_level = std::clamp(ParseInt(value, config.custom_item_reinforce_level), 0, 25);
    } else if (key == "resources.custom_item_upgrade_value" || key == "custom_item_upgrade_value") {
        config.custom_item_upgrade_value = std::clamp(ParseInt(value, config.custom_item_upgrade_value), 0, 1200);
    } else if (key == "resources.custom_item_gem_id" || key == "custom_item_gem_id") {
        config.custom_item_gem_id = ParseInt(value, config.custom_item_gem_id);
    } else if (key == "pendingactions.mass_item_gib_request_id" || key == "mass_item_gib_request_id") {
        config.mass_item_gib_request_id = std::max(ParseInt(value, config.mass_item_gib_request_id), 0);
    } else if (key == "resources.add_flask_charge_type" || key == "add_flask_charge_type") {
        config.add_flask_charge_type = std::clamp(ParseInt(value, config.add_flask_charge_type), 0, 1);
    } else if (key == "resources.set_flask_level_value" || key == "set_flask_level_value") {
        config.set_flask_level_value = std::clamp(ParseInt(value, config.set_flask_level_value), 0, 12);
    } else if (key == "character.character_level" || key == "character_level") {
        config.character_level = std::clamp(ParseInt(value, config.character_level), 1, 713);
    } else if (key == "character.character_vigor" || key == "character_vigor") {
        config.character_vigor = std::clamp(ParseInt(value, config.character_vigor), 1, 99);
    } else if (key == "character.character_mind" || key == "character_mind") {
        config.character_mind = std::clamp(ParseInt(value, config.character_mind), 1, 99);
    } else if (key == "character.character_endurance" || key == "character_endurance") {
        config.character_endurance = std::clamp(ParseInt(value, config.character_endurance), 1, 99);
    } else if (key == "character.character_strength" || key == "character_strength") {
        config.character_strength = std::clamp(ParseInt(value, config.character_strength), 1, 99);
    } else if (key == "character.character_dexterity" || key == "character_dexterity") {
        config.character_dexterity = std::clamp(ParseInt(value, config.character_dexterity), 1, 99);
    } else if (key == "character.character_intelligence" || key == "character_intelligence") {
        config.character_intelligence = std::clamp(ParseInt(value, config.character_intelligence), 1, 99);
    } else if (key == "character.character_faith" || key == "character_faith") {
        config.character_faith = std::clamp(ParseInt(value, config.character_faith), 1, 99);
    } else if (key == "character.character_arcane" || key == "character_arcane") {
        config.character_arcane = std::clamp(ParseInt(value, config.character_arcane), 1, 99);
    } else if (key == "character.edit_hp" || key == "edit_hp") {
        config.edit_hp = std::max(ParseInt(value, config.edit_hp), 0);
    } else if (key == "character.edit_mp" || key == "edit_mp") {
        config.edit_mp = std::max(ParseInt(value, config.edit_mp), 0);
    } else if (key == "character.edit_sp" || key == "edit_sp") {
        config.edit_sp = std::max(ParseInt(value, config.edit_sp), 0);
    } else if (key == "equipment.equip_slot" || key == "equip_slot") {
        config.equip_slot = std::clamp(ParseInt(value, config.equip_slot), 0, 38);
    } else if (key == "equipment.equip_item_id" || key == "equip_item_id") {
        config.equip_item_id = ParseInt(value, config.equip_item_id);
    } else if (key == "magic.magic_slot" || key == "magic_slot") {
        config.magic_slot = std::clamp(ParseInt(value, config.magic_slot), 0, 13);
    } else if (key == "magic.magic_item_id" || key == "magic_item_id") {
        config.magic_item_id = ParseInt(value, config.magic_item_id);
    } else if (key == "gestures.gesture_slot" || key == "gesture_slot") {
        config.gesture_slot = std::clamp(ParseInt(value, config.gesture_slot), 0, 6);
    } else if (key == "gestures.gesture_item_id" || key == "gesture_item_id") {
        config.gesture_item_id = ParseInt(value, config.gesture_item_id);
    } else if (key == "npcmenus.npc_menu_request_id" || key == "npc_menu_request_id") {
        config.npc_menu_request_id = std::max(ParseInt(value, config.npc_menu_request_id), 0);
    } else if (key == "statistics.statistics_load_save_slot" || key == "statistics_load_save_slot") {
        config.statistics_load_save_slot = std::max(ParseInt(value, config.statistics_load_save_slot), 0);
    } else if (key == "paramtools.param_tool_selected_table" || key == "param_tool_selected_table") {
        config.param_tool_selected_table = std::max(ParseInt(value, config.param_tool_selected_table), 0);
    } else if (key == "paramtools.param_tool_row_id" || key == "param_tool_row_id") {
        config.param_tool_row_id = std::max(ParseInt(value, config.param_tool_row_id), 0);
    } else if (key == "paramtools.param_tool_field_offset" || key == "param_tool_field_offset") {
        config.param_tool_field_offset = std::max(ParseInt(value, config.param_tool_field_offset), 0);
    } else if (key == "paramtools.param_tool_value_type" || key == "param_tool_value_type") {
        config.param_tool_value_type = std::clamp(ParseInt(value, config.param_tool_value_type), 0, 6);
    } else if (key == "paramtools.param_tool_int_value" || key == "param_tool_int_value") {
        config.param_tool_int_value = ParseInt(value, config.param_tool_int_value);
    } else if (key == "pendingactions.param_tool_script_request_id" || key == "param_tool_script_request_id") {
        config.param_tool_script_request_id = std::max(ParseInt(value, config.param_tool_script_request_id), 0);
    } else if (key == "debug.debug_event_flag_id" || key == "debug_event_flag_id") {
        config.debug_event_flag_id = std::max(ParseInt(value, config.debug_event_flag_id), 0);
    }
}

void ApplyFloat(ModConfig& config, const std::string& key, const std::string& value) {
    if (key == "parampatches.custom_fov_value" || key == "custom_fov_value") {
        config.custom_fov_value = std::clamp(ParseFloat(value, config.custom_fov_value), 20.0f, 120.0f);
    } else if (key == "parampatches.custom_camera_distance_value" || key == "custom_camera_distance_value") {
        config.custom_camera_distance_value =
            std::clamp(ParseFloat(value, config.custom_camera_distance_value), 1.0f, 20.0f);
    } else if (key == "movement.movement_speed" || key == "movement_speed") {
        config.movement_speed = std::clamp(ParseFloat(value, config.movement_speed), 0.1f, 20.0f);
    } else if (key == "coordinates.teleport_coord_x" || key == "teleport_coord_x") {
        config.teleport_coord_x = ParseFloat(value, config.teleport_coord_x);
    } else if (key == "coordinates.teleport_coord_y" || key == "teleport_coord_y") {
        config.teleport_coord_y = ParseFloat(value, config.teleport_coord_y);
    } else if (key == "coordinates.teleport_coord_z" || key == "teleport_coord_z") {
        config.teleport_coord_z = ParseFloat(value, config.teleport_coord_z);
    } else if (key == "paramtools.param_tool_float_value" || key == "param_tool_float_value") {
        config.param_tool_float_value = ParseFloat(value, config.param_tool_float_value);
    }
}

}  // namespace

ModConfig LoadConfig(const fs::path& config_path) {
    ModConfig config;

    std::ifstream input(config_path);
    if (!input.is_open()) {
        return config;
    }

    std::string current_section;
    std::string line;
    while (std::getline(input, line)) {
        const std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') {
            continue;
        }

        if (trimmed.front() == '[' && trimmed.back() == ']') {
            current_section = ToLower(Trim(trimmed.substr(1, trimmed.size() - 2)));
            continue;
        }

        const std::size_t equals = trimmed.find('=');
        if (equals == std::string::npos) {
            continue;
        }

        const std::string key_name = ToLower(Trim(trimmed.substr(0, equals)));
        const std::string value = Trim(trimmed.substr(equals + 1));
        const std::string full_key = current_section.empty() ? key_name : current_section + "." + key_name;

        ApplyBool(config, full_key, value);
        ApplyInt(config, full_key, value);
        ApplyFloat(config, full_key, value);
        if (full_key == "general.message_box_text" || key_name == "message_box_text") {
            config.message_box_text.assign(value.begin(), value.end());
        } else if (full_key == "overlay.menu_opacity" || key_name == "menu_opacity") {
            config.menu_opacity = ParseFloat(value, config.menu_opacity);
        }
    }

    return config;
}

bool SaveConfig(const fs::path& config_path, const ModConfig& config) {
    std::error_code error;
    const fs::path parent = config_path.parent_path();
    if (!parent.empty()) {
        fs::create_directories(parent, error);
        if (error) {
            return false;
        }
    }

    std::ofstream output(config_path, std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    output <<
        "; ERDMod 配置文件\n"
        "; 说明：持续开关会在游戏内菜单改动后自动保存，下次启动会自动恢复上次选择。\n"
        "; 但真正写入游戏内存会等角色已经进档后才开始，避免标题界面或加载途中误改。\n\n"
        "[General]\n"
        "; 是否创建调试控制台窗口\n"
        "create_console=" << (config.create_console ? "true" : "false") << "\n\n"
        "; 是否把日志写到 Logs/ERDMod.log\n"
        "enable_file_log=" << (config.enable_file_log ? "true" : "false") << "\n\n"
        "; 插件加载时是否弹出提示框\n"
        "show_message_box=" << (config.show_message_box ? "true" : "false") << "\n\n"
        "; 提示框显示的文字\n"
        "message_box_text=" << WideToUtf8(config.message_box_text) << "\n\n"
        "[Overlay]\n"
        "; 菜单默认是否以最小化状态打开\n"
        "menu_minimized=" << (config.menu_minimized ? "true" : "false") << "\n\n"
        "; 菜单透明度，范围 0.20 - 1.00\n"
        "menu_opacity=" << config.menu_opacity << "\n\n"
        "[CharacterFlags]\n"
        "no_dead=" << (config.no_dead ? "true" : "false") << "\n\n"
        "no_damage=" << (config.no_damage ? "true" : "false") << "\n\n"
        "; 无限 FP / 不耗蓝\n"
        "infinite_fp=" << (config.infinite_fp ? "true" : "false") << "\n\n"
        "; 道具不消耗\n"
        "infinite_items=" << (config.infinite_items ? "true" : "false") << "\n\n"
        "; 不耗精力\n"
        "no_stamina_consumption=" << (config.no_stamina_consumption ? "true" : "false") << "\n\n"
        "[WorldUnlocks]\n"
        "; 以下开关会被记录，下次进档后仍会自动触发对应解锁\n"
        "unlock_all_maps=" << (config.unlock_all_maps ? "true" : "false") << "\n"
        "unlock_all_cookbooks=" << (config.unlock_all_cookbooks ? "true" : "false") << "\n"
        "unlock_all_whetblades=" << (config.unlock_all_whetblades ? "true" : "false") << "\n"
        "unlock_all_graces=" << (config.unlock_all_graces ? "true" : "false") << "\n"
        "unlock_all_summoning_pools=" << (config.unlock_all_summoning_pools ? "true" : "false") << "\n"
        "unlock_all_colosseums=" << (config.unlock_all_colosseums ? "true" : "false") << "\n"
        "unlock_all_gestures=" << (config.unlock_all_gestures ? "true" : "false") << "\n\n"
        "[ParamPatches]\n"
        "faster_respawn=" << (config.faster_respawn ? "true" : "false") << "\n"
        "warp_out_of_uncleared_minidungeons=" << (config.warp_out_of_uncleared_minidungeons ? "true" : "false") << "\n"
        "free_purchase=" << (config.free_purchase ? "true" : "false") << "\n"
        "no_crafting_material_cost=" << (config.no_crafting_material_cost ? "true" : "false") << "\n"
        "no_upgrade_material_cost=" << (config.no_upgrade_material_cost ? "true" : "false") << "\n"
        "no_magic_requirements=" << (config.no_magic_requirements ? "true" : "false") << "\n"
        "all_magic_one_slot=" << (config.all_magic_one_slot ? "true" : "false") << "\n"
        "weightless_equipment=" << (config.weightless_equipment ? "true" : "false") << "\n"
        "custom_fov_enabled=" << (config.custom_fov_enabled ? "true" : "false") << "\n"
        "custom_fov_value=" << std::clamp(config.custom_fov_value, 20.0f, 120.0f) << "\n"
        "custom_camera_distance_enabled=" << (config.custom_camera_distance_enabled ? "true" : "false") << "\n"
        "custom_camera_distance_value=" << std::clamp(config.custom_camera_distance_value, 1.0f, 20.0f) << "\n"
        "mount_anywhere=" << (config.mount_anywhere ? "true" : "false") << "\n"
        "spirit_ashes_anywhere=" << (config.spirit_ashes_anywhere ? "true" : "false") << "\n"
        "item_discovery_multiplier=" << std::clamp(config.item_discovery_multiplier, 1, 100) << "\n"
        "permanent_lantern=" << (config.permanent_lantern ? "true" : "false") << "\n"
        "invisible_helmets=" << (config.invisible_helmets ? "true" : "false") << "\n\n"
        "[Movement]\n"
        "no_clip_enabled=" << (config.no_clip_enabled ? "true" : "false") << "\n"
        "freecam_enabled=" << (config.freecam_enabled ? "true" : "false") << "\n"
        "no_fall_camera=" << (config.no_fall_camera ? "true" : "false") << "\n"
        "movement_speed=" << std::clamp(config.movement_speed, 0.1f, 20.0f) << "\n\n"
        "[Resources]\n"
        "give_runes_amount=" << std::max(config.give_runes_amount, 1) << "\n"
        "custom_item_id=" << config.custom_item_id << "\n"
        "custom_item_quantity=" << std::clamp(config.custom_item_quantity, 1, 9999) << "\n"
        "custom_item_reinforce_level=" << std::clamp(config.custom_item_reinforce_level, 0, 25) << "\n"
        "custom_item_upgrade_value=" << std::clamp(config.custom_item_upgrade_value, 0, 1200) << "\n"
        "custom_item_gem_id=" << config.custom_item_gem_id << "\n"
        "add_flask_charge_type=" << std::clamp(config.add_flask_charge_type, 0, 1) << "\n"
        "set_flask_level_value=" << std::clamp(config.set_flask_level_value, 0, 12) << "\n\n"
        "[Character]\n"
        "character_level=" << std::clamp(config.character_level, 1, 713) << "\n"
        "character_vigor=" << std::clamp(config.character_vigor, 1, 99) << "\n"
        "character_mind=" << std::clamp(config.character_mind, 1, 99) << "\n"
        "character_endurance=" << std::clamp(config.character_endurance, 1, 99) << "\n"
        "character_strength=" << std::clamp(config.character_strength, 1, 99) << "\n"
        "character_dexterity=" << std::clamp(config.character_dexterity, 1, 99) << "\n"
        "character_intelligence=" << std::clamp(config.character_intelligence, 1, 99) << "\n"
        "character_faith=" << std::clamp(config.character_faith, 1, 99) << "\n"
        "character_arcane=" << std::clamp(config.character_arcane, 1, 99) << "\n"
        "edit_hp=" << std::max(config.edit_hp, 0) << "\n"
        "edit_mp=" << std::max(config.edit_mp, 0) << "\n"
        "edit_sp=" << std::max(config.edit_sp, 0) << "\n\n"
        "[Equipment]\n"
        "equip_slot=" << std::clamp(config.equip_slot, 0, 38) << "\n"
        "equip_item_id=" << config.equip_item_id << "\n\n"
        "[Magic]\n"
        "magic_slot=" << std::clamp(config.magic_slot, 0, 13) << "\n"
        "magic_item_id=" << config.magic_item_id << "\n\n"
        "[Gestures]\n"
        "gesture_slot=" << std::clamp(config.gesture_slot, 0, 6) << "\n"
        "gesture_item_id=" << config.gesture_item_id << "\n\n"
        "[NpcMenus]\n"
        "npc_menu_request_id=" << std::max(config.npc_menu_request_id, 0) << "\n\n"
        "[Statistics]\n"
        "statistics_load_save_slot=" << std::max(config.statistics_load_save_slot, 0) << "\n"
        "statistics_disable_auto_save=" << (config.statistics_disable_auto_save ? "true" : "false") << "\n\n"
        "[ParamTools]\n"
        "param_tool_selected_table=" << std::max(config.param_tool_selected_table, 0) << "\n"
        "param_tool_row_id=" << std::max(config.param_tool_row_id, 0) << "\n"
        "param_tool_field_offset=" << std::max(config.param_tool_field_offset, 0) << "\n"
        "param_tool_value_type=" << std::clamp(config.param_tool_value_type, 0, 6) << "\n"
        "param_tool_int_value=" << config.param_tool_int_value << "\n"
        "param_tool_float_value=" << config.param_tool_float_value << "\n\n"
        "[Coordinates]\n"
        "teleport_coord_x=" << config.teleport_coord_x << "\n"
        "teleport_coord_y=" << config.teleport_coord_y << "\n"
        "teleport_coord_z=" << config.teleport_coord_z << "\n\n"
        "[Debug]\n"
        "debug_event_flag_id=" << std::max(config.debug_event_flag_id, 0) << "\n"
        "debug_event_flag_desired_state=" << (config.debug_event_flag_desired_state ? "true" : "false") << "\n\n"
        "[GameActions]\n"
        "start_next_cycle=" << (config.start_next_cycle ? "true" : "false") << "\n";

    output <<
        "\n[PendingActions]\n"
        "; 以下是待执行动作。下次检测到角色已进档后，会自动执行一次。\n"
        "give_runes_requested=" << (config.give_runes_requested ? "true" : "false") << "\n"
        "custom_item_give_requested=" << (config.custom_item_give_requested ? "true" : "false") << "\n"
        "mass_item_gib_request_id=" << std::max(config.mass_item_gib_request_id, 0) << "\n"
        "add_flask_charge_requested=" << (config.add_flask_charge_requested ? "true" : "false") << "\n"
        "set_flask_level_requested=" << (config.set_flask_level_requested ? "true" : "false") << "\n"
        "debug_event_flag_write_requested=" << (config.debug_event_flag_write_requested ? "true" : "false") << "\n"
        "teleport_player_to_input_coords_requested=" << (config.teleport_player_to_input_coords_requested ? "true" : "false") << "\n"
        "character_apply_requested=" << (config.character_apply_requested ? "true" : "false") << "\n"
        "resource_apply_requested=" << (config.resource_apply_requested ? "true" : "false") << "\n"
        "equip_apply_requested=" << (config.equip_apply_requested ? "true" : "false") << "\n"
        "equip_unequip_requested=" << (config.equip_unequip_requested ? "true" : "false") << "\n"
        "magic_attune_requested=" << (config.magic_attune_requested ? "true" : "false") << "\n"
        "magic_remove_requested=" << (config.magic_remove_requested ? "true" : "false") << "\n"
        "gesture_apply_requested=" << (config.gesture_apply_requested ? "true" : "false") << "\n";

    output <<
        "statistics_load_save_slot_requested=" << (config.statistics_load_save_slot_requested ? "true" : "false") << "\n"
        "statistics_save_game_requested=" << (config.statistics_save_game_requested ? "true" : "false") << "\n"
        "param_tool_write_requested=" << (config.param_tool_write_requested ? "true" : "false") << "\n"
        "param_tool_script_request_id=" << std::max(config.param_tool_script_request_id, 0) << "\n";

    return output.good();
}

}  // namespace ERD::Main
