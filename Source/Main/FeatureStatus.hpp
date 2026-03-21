#pragma once

#include "../Common.hpp"

namespace ERD::Main {

struct FeatureStatus {
    std::atomic_bool infinite_fp = false;
    std::atomic_bool infinite_items = false;
    std::atomic_bool no_stamina_consumption = false;

    std::atomic_bool unlock_all_maps = false;
    std::atomic_bool unlock_all_cookbooks = false;
    std::atomic_bool unlock_all_whetblades = false;
    std::atomic_bool unlock_all_graces = false;
    std::atomic_bool unlock_all_summoning_pools = false;
    std::atomic_bool unlock_all_colosseums = false;

    std::atomic_bool faster_respawn = false;
    std::atomic_bool warp_out_of_uncleared_minidungeons = false;
    std::atomic_bool free_purchase = false;
    std::atomic_bool no_crafting_material_cost = false;
    std::atomic_bool no_upgrade_material_cost = false;
    std::atomic_bool no_magic_requirements = false;
    std::atomic_bool all_magic_one_slot = false;
    std::atomic_bool weightless_equipment = false;
    std::atomic_bool mount_anywhere = false;
    std::atomic_bool spirit_ashes_anywhere = false;
    std::atomic_int item_discovery_multiplier = 1;
    std::atomic_bool permanent_lantern = false;
    std::atomic_bool invisible_helmets = false;

    std::atomic_bool start_next_cycle = false;

    std::atomic_bool menu_visible = true;
    std::atomic_bool overlay_ready = false;
    std::atomic_bool game_ready = false;
};

inline FeatureStatus g_FeatureStatus;

}  // namespace ERD::Main
