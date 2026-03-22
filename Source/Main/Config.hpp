#pragma once

#include "../Common.hpp"

namespace ERD::Main {

struct ModConfig {
    bool create_console = false;
    bool enable_file_log = true;
    bool show_message_box = false;
    bool menu_minimized = false;
    float menu_opacity = 0.88f;

    bool no_dead = false;
    bool no_damage = false;
    bool infinite_fp = false;
    bool infinite_items = false;
    bool no_stamina_consumption = false;

    bool unlock_all_maps = false;
    bool unlock_all_cookbooks = false;
    bool unlock_all_whetblades = false;
    bool unlock_all_graces = false;
    bool unlock_all_summoning_pools = false;
    bool unlock_all_colosseums = false;
    bool unlock_all_gestures = false;

    bool faster_respawn = false;
    bool warp_out_of_uncleared_minidungeons = false;
    bool free_purchase = false;
    bool no_crafting_material_cost = false;
    bool no_upgrade_material_cost = false;
    bool no_magic_requirements = false;
    bool all_magic_one_slot = false;
    bool weightless_equipment = false;
    bool custom_fov_enabled = false;
    float custom_fov_value = 60.0f;
    bool custom_camera_distance_enabled = false;
    float custom_camera_distance_value = 8.0f;
    bool mount_anywhere = false;
    bool spirit_ashes_anywhere = false;
    int item_discovery_multiplier = 1;
    bool permanent_lantern = false;
    bool invisible_helmets = false;
    bool no_clip_enabled = false;
    bool freecam_enabled = false;
    bool no_fall_camera = false;
    float movement_speed = 1.0f;

    bool start_next_cycle = false;
    int give_runes_amount = 100000;
    bool give_runes_requested = false;
    int custom_item_id = 0x40000000;
    int custom_item_quantity = 1;
    int custom_item_reinforce_level = 0;
    int custom_item_upgrade_value = 0;
    int custom_item_gem_id = -1;
    bool custom_item_give_requested = false;
    int mass_item_gib_request_id = 0;
    int add_flask_charge_type = 0;
    bool add_flask_charge_requested = false;
    int set_flask_level_value = 12;
    bool set_flask_level_requested = false;
    int debug_event_flag_id = 0;
    bool debug_event_flag_desired_state = false;
    bool debug_event_flag_write_requested = false;
    float teleport_coord_x = 0.0f;
    float teleport_coord_y = 0.0f;
    float teleport_coord_z = 0.0f;
    bool teleport_player_to_input_coords_requested = false;
    int character_level = 1;
    int character_vigor = 10;
    int character_mind = 10;
    int character_endurance = 10;
    int character_strength = 10;
    int character_dexterity = 10;
    int character_intelligence = 10;
    int character_faith = 10;
    int character_arcane = 10;
    int edit_hp = 0;
    int edit_mp = 0;
    int edit_sp = 0;
    bool character_apply_requested = false;
    bool resource_apply_requested = false;
    int equip_slot = 1;
    int equip_item_id = 0;
    bool equip_apply_requested = false;
    bool equip_unequip_requested = false;
    int magic_slot = 0;
    int magic_item_id = 0;
    bool magic_attune_requested = false;
    bool magic_remove_requested = false;
    int gesture_slot = 0;
    int gesture_item_id = 0;
    bool gesture_apply_requested = false;
    int npc_menu_request_id = 0;
    int statistics_load_save_slot = 0;
    bool statistics_load_save_slot_requested = false;
    bool statistics_save_game_requested = false;
    bool statistics_disable_auto_save = false;
    int param_tool_selected_table = 0;
    int param_tool_row_id = 0;
    int param_tool_field_offset = 0;
    int param_tool_value_type = 4;
    int param_tool_int_value = 0;
    float param_tool_float_value = 0.0f;
    bool param_tool_write_requested = false;
    int param_tool_script_request_id = 0;

    std::wstring message_box_text = L"ERDMod loaded.";
};

ModConfig LoadConfig(const fs::path& config_path);
bool SaveConfig(const fs::path& config_path, const ModConfig& config);

}  // namespace ERD::Main
