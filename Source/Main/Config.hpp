#pragma once

#include "../Common.hpp"

namespace ERD::Main {

struct ModConfig {
    bool create_console = false;
    bool enable_file_log = true;
    bool show_message_box = false;
    bool menu_minimized = false;
    float menu_opacity = 0.88f;

    bool infinite_fp = false;
    bool infinite_items = false;
    bool no_stamina_consumption = false;

    bool unlock_all_maps = false;
    bool unlock_all_cookbooks = false;
    bool unlock_all_whetblades = false;
    bool unlock_all_graces = false;
    bool unlock_all_summoning_pools = false;
    bool unlock_all_colosseums = false;

    bool faster_respawn = false;
    bool warp_out_of_uncleared_minidungeons = false;
    bool free_purchase = false;
    bool no_crafting_material_cost = false;
    bool no_upgrade_material_cost = false;
    bool no_magic_requirements = false;
    bool all_magic_one_slot = false;
    bool weightless_equipment = false;
    bool mount_anywhere = false;
    bool spirit_ashes_anywhere = false;
    int item_discovery_multiplier = 1;
    bool permanent_lantern = false;
    bool invisible_helmets = false;

    bool start_next_cycle = false;

    std::wstring message_box_text = L"ERDMod loaded.";
};

ModConfig LoadConfig(const fs::path& config_path);
bool SaveConfig(const fs::path& config_path, const ModConfig& config);

}  // namespace ERD::Main
