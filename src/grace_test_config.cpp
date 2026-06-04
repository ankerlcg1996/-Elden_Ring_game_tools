#include "grace_test_config.hpp"

#include "Main/FeatureStatus.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <fstream>
#include <mutex>
#include <string>

#include <windows.h>

namespace grace_test::config {
namespace {

struct PersistedState {
    int equipment_weight_reduction_percent = 0;
    int weapon_requirement_reduction_percent = 0;
    int player_speed_increase_percent = 0;
    int buff_duration_extend_mode = 0;
    int weapon_parry_mode = 0;
    int enemy_hp_increase_percent = 0;
    int enemy_hp_decrease_percent = 0;
    int damage_multiplier_percent = 0;
    int damage_cut_multiplier_percent = 0;
    int damage_reduce_multiplier_percent = 0;
    int damage_taken_multiplier_percent = 0;
    int stamina_cost_reduction_percent = 0;
    int fp_cost_reduction_percent = 0;
    bool target_status_debug_logging = false;
    float overlay_poise_width_scale = 1.0f;
    float overlay_poise_height_scale = 1.0f;
    float overlay_status_width_scale = 1.0f;
    float overlay_status_height_scale = 1.0f;
    float overlay_poise_vertical_offset = 86.0f;
    float overlay_entity_status_vertical_offset = 0.0f;
    float overlay_boss_status_gap = 12.0f;
    bool free_purchase = false;
    bool no_crafting_material_cost = false;
    bool no_upgrade_material_cost = false;
    bool all_weapons_enchantable = false;
    bool all_weapons_ash_of_war_changeable = false;
    bool no_magic_requirements = false;
    bool all_magic_one_slot = false;
    bool beast_not_hostile = false;
    bool silent_footsteps = false;
    bool infinite_consumables = false;
    bool infinite_arrows = false;
    bool infinite_stamina = false;
    bool infinite_fp = false;
    bool no_rune_loss_on_death = false;
    bool no_rune_arc_loss_on_death = false;
    bool no_time_pass_on_death = false;
    bool attack_life_steal_on_hit = false;
    float rally_time_seconds = 4.0f;
    bool rally_hit_reset = true;
    bool rally_only_heal = true;
    bool rally_exponential_decay = true;
    float rally_half_life_seconds = 7.5f;
    float rally_decay_seconds = 15.0f;
    float model_scale_uniform = 1.0f;
    bool infinite_jump = false;
    bool spirit_ashes_anywhere = false;
    bool torrent_no_death = false;
    bool torrent_anywhere = false;
    bool no_fall_death = false;
    bool enemy_poise_overlay_enabled = true;
    bool enemy_resistance_overlay_enabled = true;
    bool enemy_resistance_bleed_enabled = true;
    bool enemy_resistance_frost_enabled = true;
    bool enemy_resistance_rot_enabled = true;
    bool enemy_resistance_poison_enabled = true;
    bool enemy_resistance_sleep_enabled = true;
    bool enemy_resistance_madness_enabled = true;
    bool auto_pickup_enabled = false;
    bool auto_pickup_materials = true;
    bool auto_pickup_items = false;
    bool auto_pickup_corpse_loot = false;
    bool auto_pickup_lost_runes = true;
    bool auto_pickup_in_combat = true;
    int auto_pickup_range_percent = 100;
    int ng_cycle = 0;
    bool debug = false;
    bool debug_explicitly_configured = false;
    std::string ui_language = "auto";
};

std::mutex g_config_mutex;
std::filesystem::path g_config_path;
PersistedState g_state;
std::filesystem::file_time_type g_last_write_time{};
ULONGLONG g_next_reload_check_tick = 0;
constexpr ULONGLONG kReloadPollIntervalMs = 1000;

bool parse_bool(const std::string& value, bool fallback) {
    if (_stricmp(value.c_str(), "true") == 0 || value == "1" || _stricmp(value.c_str(), "yes") == 0) {
        return true;
    }
    if (_stricmp(value.c_str(), "false") == 0 || value == "0" || _stricmp(value.c_str(), "no") == 0) {
        return false;
    }
    return fallback;
}

int parse_int_clamped(const std::string& value, int fallback, int min_value, int max_value) {
    try {
        return std::clamp(std::stoi(value), min_value, max_value);
    } catch (...) {
        return fallback;
    }
}

float parse_float_clamped(const std::string& value, float fallback, float min_value, float max_value) {
    try {
        return std::clamp(std::stof(value), min_value, max_value);
    } catch (...) {
        return fallback;
    }
}

void apply_state_to_features(const PersistedState& state) {
    ERD::Main::g_FeatureStatus.equipment_weight_reduction_percent =
        std::clamp(state.equipment_weight_reduction_percent, 0, 100);
    ERD::Main::g_FeatureStatus.weapon_requirement_reduction_percent =
        std::clamp(state.weapon_requirement_reduction_percent, 0, 100);
    ERD::Main::g_FeatureStatus.player_speed_increase_percent =
        std::clamp(state.player_speed_increase_percent, 0, 200);
    switch (state.buff_duration_extend_mode) {
    case 0:
    case 50:
    case 100:
    case 200:
    case 300:
    case 500:
    case 99999:
        ERD::Main::g_FeatureStatus.buff_duration_extend_mode = state.buff_duration_extend_mode;
        break;
    default:
        ERD::Main::g_FeatureStatus.buff_duration_extend_mode = 0;
        break;
    }
    switch (state.weapon_parry_mode) {
    case 0:
    case 60:
    case 40:
    case 20:
    case 10:
        ERD::Main::g_FeatureStatus.weapon_parry_mode = state.weapon_parry_mode;
        break;
    default:
        ERD::Main::g_FeatureStatus.weapon_parry_mode = 0;
        break;
    }
    ERD::Main::g_FeatureStatus.enemy_hp_increase_percent =
        std::clamp(state.enemy_hp_increase_percent, 0, 1000);
    ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent =
        std::clamp(state.enemy_hp_decrease_percent, 0, 1000);
    if (ERD::Main::g_FeatureStatus.enemy_hp_increase_percent.load() > 0) {
        ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent = 0;
    }
    ERD::Main::g_FeatureStatus.movement_speed =
        1.0f + static_cast<float>(ERD::Main::g_FeatureStatus.player_speed_increase_percent.load()) / 100.0f;
    ERD::Main::g_FeatureStatus.damage_multiplier_percent = std::clamp(state.damage_multiplier_percent, 0, 500);
    ERD::Main::g_FeatureStatus.damage_cut_multiplier_percent =
        std::clamp(state.damage_cut_multiplier_percent, 0, 500);
    ERD::Main::g_FeatureStatus.damage_reduce_multiplier_percent =
        std::clamp(state.damage_reduce_multiplier_percent, 0, 500);
    ERD::Main::g_FeatureStatus.damage_taken_multiplier_percent =
        std::clamp(state.damage_taken_multiplier_percent, 0, 500);
    ERD::Main::g_FeatureStatus.stamina_cost_reduction_percent =
        std::clamp(state.stamina_cost_reduction_percent, 0, 100);
    ERD::Main::g_FeatureStatus.fp_cost_reduction_percent =
        std::clamp(state.fp_cost_reduction_percent, 0, 100);
    ERD::Main::g_FeatureStatus.target_status_debug_logging = state.target_status_debug_logging;
    ERD::Main::g_FeatureStatus.overlay_poise_width_scale =
        std::clamp(state.overlay_poise_width_scale, 0.5f, 2.0f);
    ERD::Main::g_FeatureStatus.overlay_poise_height_scale =
        std::clamp(state.overlay_poise_height_scale, 0.5f, 2.0f);
    ERD::Main::g_FeatureStatus.overlay_status_width_scale =
        std::clamp(state.overlay_status_width_scale, 0.5f, 2.0f);
    ERD::Main::g_FeatureStatus.overlay_status_height_scale =
        std::clamp(state.overlay_status_height_scale, 0.5f, 2.0f);
    ERD::Main::g_FeatureStatus.overlay_poise_vertical_offset =
        std::clamp(state.overlay_poise_vertical_offset, -300.0f, 300.0f);
    ERD::Main::g_FeatureStatus.overlay_entity_status_vertical_offset =
        std::clamp(state.overlay_entity_status_vertical_offset, -300.0f, 300.0f);
    ERD::Main::g_FeatureStatus.overlay_boss_status_gap =
        std::clamp(state.overlay_boss_status_gap, -100.0f, 200.0f);
    ERD::Main::g_FeatureStatus.weightless_equipment =
        ERD::Main::g_FeatureStatus.equipment_weight_reduction_percent.load() >= 100;
    ERD::Main::g_FeatureStatus.free_purchase = state.free_purchase;
    ERD::Main::g_FeatureStatus.no_crafting_material_cost = state.no_crafting_material_cost;
    ERD::Main::g_FeatureStatus.no_upgrade_material_cost = state.no_upgrade_material_cost;
    ERD::Main::g_FeatureStatus.all_weapons_enchantable = state.all_weapons_enchantable;
    ERD::Main::g_FeatureStatus.all_weapons_ash_of_war_changeable = state.all_weapons_ash_of_war_changeable;
    ERD::Main::g_FeatureStatus.no_magic_requirements = state.no_magic_requirements;
    ERD::Main::g_FeatureStatus.all_magic_one_slot = state.all_magic_one_slot;
    ERD::Main::g_FeatureStatus.beast_not_hostile = state.beast_not_hostile;
    ERD::Main::g_FeatureStatus.silent_footsteps = state.silent_footsteps;
    ERD::Main::g_FeatureStatus.infinite_consumables = state.infinite_consumables;
    ERD::Main::g_FeatureStatus.infinite_arrows = state.infinite_arrows;
    ERD::Main::g_FeatureStatus.no_rune_loss_on_death = state.no_rune_loss_on_death;
    ERD::Main::g_FeatureStatus.no_rune_arc_loss_on_death = state.no_rune_arc_loss_on_death;
    ERD::Main::g_FeatureStatus.no_time_pass_on_death = state.no_time_pass_on_death;
    ERD::Main::g_FeatureStatus.attack_life_steal_on_hit = state.attack_life_steal_on_hit;
    ERD::Main::g_FeatureStatus.rally_time_seconds =
        std::clamp(state.rally_time_seconds, 0.1f, 60.0f);
    ERD::Main::g_FeatureStatus.rally_hit_reset = state.rally_hit_reset;
    ERD::Main::g_FeatureStatus.rally_only_heal = state.rally_only_heal;
    ERD::Main::g_FeatureStatus.rally_exponential_decay = state.rally_exponential_decay;
    ERD::Main::g_FeatureStatus.rally_half_life_seconds =
        std::clamp(state.rally_half_life_seconds, 0.1f, 60.0f);
    ERD::Main::g_FeatureStatus.rally_decay_seconds =
        std::clamp(state.rally_decay_seconds, 0.1f, 120.0f);
    ERD::Main::g_FeatureStatus.model_scale_uniform = std::clamp(state.model_scale_uniform, 0.0f, 3.0f);
    ERD::Main::g_FeatureStatus.infinite_jump = state.infinite_jump;
    ERD::Main::g_FeatureStatus.spirit_ashes_anywhere = state.spirit_ashes_anywhere;
    ERD::Main::g_FeatureStatus.torrent_no_death = state.torrent_no_death;
    ERD::Main::g_FeatureStatus.torrent_anywhere = state.torrent_anywhere;
    ERD::Main::g_FeatureStatus.no_fall_death = state.no_fall_death;
    ERD::Main::g_FeatureStatus.enemy_poise_overlay_enabled = state.enemy_poise_overlay_enabled;
    ERD::Main::g_FeatureStatus.enemy_resistance_overlay_enabled = state.enemy_resistance_overlay_enabled;
    ERD::Main::g_FeatureStatus.enemy_resistance_bleed_enabled = state.enemy_resistance_bleed_enabled;
    ERD::Main::g_FeatureStatus.enemy_resistance_frost_enabled = state.enemy_resistance_frost_enabled;
    ERD::Main::g_FeatureStatus.enemy_resistance_rot_enabled = state.enemy_resistance_rot_enabled;
    ERD::Main::g_FeatureStatus.enemy_resistance_poison_enabled = state.enemy_resistance_poison_enabled;
    ERD::Main::g_FeatureStatus.enemy_resistance_sleep_enabled = state.enemy_resistance_sleep_enabled;
    ERD::Main::g_FeatureStatus.enemy_resistance_madness_enabled = state.enemy_resistance_madness_enabled;
    ERD::Main::g_FeatureStatus.auto_pickup_enabled = state.auto_pickup_enabled;
    ERD::Main::g_FeatureStatus.auto_pickup_materials = state.auto_pickup_materials;
    ERD::Main::g_FeatureStatus.auto_pickup_items = state.auto_pickup_items;
    ERD::Main::g_FeatureStatus.auto_pickup_corpse_loot = state.auto_pickup_corpse_loot;
    ERD::Main::g_FeatureStatus.auto_pickup_lost_runes = state.auto_pickup_lost_runes;
    ERD::Main::g_FeatureStatus.auto_pickup_in_combat = state.auto_pickup_in_combat;
    ERD::Main::g_FeatureStatus.auto_pickup_range_percent =
        std::clamp(state.auto_pickup_range_percent, 100, 500);
    ERD::Main::g_FeatureStatus.requested_ng_cycle = std::clamp(state.ng_cycle, 0, 10);
    ERD::Main::g_FeatureStatus.requested_ng_cycle_apply = true;
    ERD::Main::g_FeatureStatus.debug_mode = state.debug;
    ERD::Main::g_FeatureStatus.faster_respawn = true;
    ERD::Main::g_FeatureStatus.warp_out_of_uncleared_minidungeons = true;
    ERD::Main::g_FeatureStatus.open_map_in_combat = true;
    ERD::Main::g_FeatureStatus.permanent_lantern = true;
}

PersistedState capture_current_feature_state() {
    PersistedState state{};
    state.equipment_weight_reduction_percent =
        std::clamp(ERD::Main::g_FeatureStatus.equipment_weight_reduction_percent.load(), 0, 100);
    state.weapon_requirement_reduction_percent =
        std::clamp(ERD::Main::g_FeatureStatus.weapon_requirement_reduction_percent.load(), 0, 100);
    state.player_speed_increase_percent =
        std::clamp(ERD::Main::g_FeatureStatus.player_speed_increase_percent.load(), 0, 200);
    state.buff_duration_extend_mode = ERD::Main::g_FeatureStatus.buff_duration_extend_mode.load();
    state.weapon_parry_mode = ERD::Main::g_FeatureStatus.weapon_parry_mode.load();
    state.enemy_hp_increase_percent =
        std::clamp(ERD::Main::g_FeatureStatus.enemy_hp_increase_percent.load(), 0, 1000);
    state.enemy_hp_decrease_percent =
        std::clamp(ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent.load(), 0, 1000);
    state.damage_multiplier_percent = std::clamp(ERD::Main::g_FeatureStatus.damage_multiplier_percent.load(), 0, 500);
    state.damage_cut_multiplier_percent =
        std::clamp(ERD::Main::g_FeatureStatus.damage_cut_multiplier_percent.load(), 0, 500);
    state.damage_reduce_multiplier_percent =
        std::clamp(ERD::Main::g_FeatureStatus.damage_reduce_multiplier_percent.load(), 0, 500);
    state.damage_taken_multiplier_percent =
        std::clamp(ERD::Main::g_FeatureStatus.damage_taken_multiplier_percent.load(), 0, 500);
    state.stamina_cost_reduction_percent =
        std::clamp(ERD::Main::g_FeatureStatus.stamina_cost_reduction_percent.load(), 0, 100);
    state.fp_cost_reduction_percent =
        std::clamp(ERD::Main::g_FeatureStatus.fp_cost_reduction_percent.load(), 0, 100);
    state.target_status_debug_logging = ERD::Main::g_FeatureStatus.target_status_debug_logging.load();
    state.overlay_poise_width_scale =
        std::clamp(ERD::Main::g_FeatureStatus.overlay_poise_width_scale.load(), 0.5f, 2.0f);
    state.overlay_poise_height_scale =
        std::clamp(ERD::Main::g_FeatureStatus.overlay_poise_height_scale.load(), 0.5f, 2.0f);
    state.overlay_status_width_scale =
        std::clamp(ERD::Main::g_FeatureStatus.overlay_status_width_scale.load(), 0.5f, 2.0f);
    state.overlay_status_height_scale =
        std::clamp(ERD::Main::g_FeatureStatus.overlay_status_height_scale.load(), 0.5f, 2.0f);
    state.overlay_poise_vertical_offset =
        std::clamp(ERD::Main::g_FeatureStatus.overlay_poise_vertical_offset.load(), -300.0f, 300.0f);
    state.overlay_entity_status_vertical_offset =
        std::clamp(ERD::Main::g_FeatureStatus.overlay_entity_status_vertical_offset.load(), -300.0f, 300.0f);
    state.overlay_boss_status_gap =
        std::clamp(ERD::Main::g_FeatureStatus.overlay_boss_status_gap.load(), -100.0f, 200.0f);
    state.free_purchase = ERD::Main::g_FeatureStatus.free_purchase.load();
    state.no_crafting_material_cost = ERD::Main::g_FeatureStatus.no_crafting_material_cost.load();
    state.no_upgrade_material_cost = ERD::Main::g_FeatureStatus.no_upgrade_material_cost.load();
    state.all_weapons_enchantable = ERD::Main::g_FeatureStatus.all_weapons_enchantable.load();
    state.all_weapons_ash_of_war_changeable = ERD::Main::g_FeatureStatus.all_weapons_ash_of_war_changeable.load();
    state.no_magic_requirements = ERD::Main::g_FeatureStatus.no_magic_requirements.load();
    state.all_magic_one_slot = ERD::Main::g_FeatureStatus.all_magic_one_slot.load();
    state.beast_not_hostile = ERD::Main::g_FeatureStatus.beast_not_hostile.load();
    state.silent_footsteps = ERD::Main::g_FeatureStatus.silent_footsteps.load();
    state.infinite_consumables = ERD::Main::g_FeatureStatus.infinite_consumables.load();
    state.infinite_arrows = ERD::Main::g_FeatureStatus.infinite_arrows.load();
    state.no_rune_loss_on_death = ERD::Main::g_FeatureStatus.no_rune_loss_on_death.load();
    state.no_rune_arc_loss_on_death = ERD::Main::g_FeatureStatus.no_rune_arc_loss_on_death.load();
    state.no_time_pass_on_death = ERD::Main::g_FeatureStatus.no_time_pass_on_death.load();
    state.attack_life_steal_on_hit = ERD::Main::g_FeatureStatus.attack_life_steal_on_hit.load();
    state.rally_time_seconds = std::clamp(ERD::Main::g_FeatureStatus.rally_time_seconds.load(), 0.1f, 60.0f);
    state.rally_hit_reset = ERD::Main::g_FeatureStatus.rally_hit_reset.load();
    state.rally_only_heal = ERD::Main::g_FeatureStatus.rally_only_heal.load();
    state.rally_exponential_decay = ERD::Main::g_FeatureStatus.rally_exponential_decay.load();
    state.rally_half_life_seconds =
        std::clamp(ERD::Main::g_FeatureStatus.rally_half_life_seconds.load(), 0.1f, 60.0f);
    state.rally_decay_seconds =
        std::clamp(ERD::Main::g_FeatureStatus.rally_decay_seconds.load(), 0.1f, 120.0f);
    state.model_scale_uniform = std::clamp(ERD::Main::g_FeatureStatus.model_scale_uniform.load(), 0.0f, 3.0f);
    state.infinite_jump = ERD::Main::g_FeatureStatus.infinite_jump.load();
    state.spirit_ashes_anywhere = ERD::Main::g_FeatureStatus.spirit_ashes_anywhere.load();
    state.torrent_no_death = ERD::Main::g_FeatureStatus.torrent_no_death.load();
    state.torrent_anywhere = ERD::Main::g_FeatureStatus.torrent_anywhere.load();
    state.no_fall_death = ERD::Main::g_FeatureStatus.no_fall_death.load();
    state.enemy_poise_overlay_enabled = ERD::Main::g_FeatureStatus.enemy_poise_overlay_enabled.load();
    state.enemy_resistance_overlay_enabled = ERD::Main::g_FeatureStatus.enemy_resistance_overlay_enabled.load();
    state.enemy_resistance_bleed_enabled = ERD::Main::g_FeatureStatus.enemy_resistance_bleed_enabled.load();
    state.enemy_resistance_frost_enabled = ERD::Main::g_FeatureStatus.enemy_resistance_frost_enabled.load();
    state.enemy_resistance_rot_enabled = ERD::Main::g_FeatureStatus.enemy_resistance_rot_enabled.load();
    state.enemy_resistance_poison_enabled = ERD::Main::g_FeatureStatus.enemy_resistance_poison_enabled.load();
    state.enemy_resistance_sleep_enabled = ERD::Main::g_FeatureStatus.enemy_resistance_sleep_enabled.load();
    state.enemy_resistance_madness_enabled = ERD::Main::g_FeatureStatus.enemy_resistance_madness_enabled.load();
    state.auto_pickup_enabled = ERD::Main::g_FeatureStatus.auto_pickup_enabled.load();
    state.auto_pickup_materials = ERD::Main::g_FeatureStatus.auto_pickup_materials.load();
    state.auto_pickup_items = ERD::Main::g_FeatureStatus.auto_pickup_items.load();
    state.auto_pickup_corpse_loot = ERD::Main::g_FeatureStatus.auto_pickup_corpse_loot.load();
    state.auto_pickup_lost_runes = ERD::Main::g_FeatureStatus.auto_pickup_lost_runes.load();
    state.auto_pickup_in_combat = ERD::Main::g_FeatureStatus.auto_pickup_in_combat.load();
    state.auto_pickup_range_percent =
        std::clamp(ERD::Main::g_FeatureStatus.auto_pickup_range_percent.load(), 100, 500);
    state.ng_cycle = std::clamp(ERD::Main::g_FeatureStatus.requested_ng_cycle.load(), 0, 10);
    state.debug = g_state.debug;
    state.debug_explicitly_configured = g_state.debug_explicitly_configured;
    state.ui_language = g_state.ui_language;
    return state;
}

void write_state_to_disk(const PersistedState& state) {
    if (g_config_path.empty()) {
        return;
    }

    std::ofstream output(g_config_path, std::ios::out | std::ios::trunc);
    output << "[GameMods]\n"
           << "equipment_weight_reduction_percent=" << state.equipment_weight_reduction_percent << "\n"
           << "weapon_requirement_reduction_percent=" << state.weapon_requirement_reduction_percent << "\n"
           << "player_speed_increase_percent=" << state.player_speed_increase_percent << "\n"
           << "buff_duration_extend_mode=" << state.buff_duration_extend_mode << "\n"
           << "weapon_parry_mode=" << state.weapon_parry_mode << "\n"
           << "enemy_hp_increase_percent=" << state.enemy_hp_increase_percent << "\n"
           << "enemy_hp_decrease_percent=" << state.enemy_hp_decrease_percent << "\n"
           << "damage_multiplier_percent=" << state.damage_multiplier_percent << "\n"
           << "damage_cut_multiplier_percent=" << state.damage_cut_multiplier_percent << "\n"
           << "damage_reduce_multiplier_percent=" << state.damage_reduce_multiplier_percent << "\n"
           << "damage_taken_multiplier_percent=" << state.damage_taken_multiplier_percent << "\n"
           << "stamina_cost_reduction_percent=" << state.stamina_cost_reduction_percent << "\n"
           << "fp_cost_reduction_percent=" << state.fp_cost_reduction_percent << "\n"
           << "free_purchase=" << (state.free_purchase ? "true" : "false") << "\n"
           << "no_crafting_material_cost=" << (state.no_crafting_material_cost ? "true" : "false") << "\n"
           << "no_upgrade_material_cost=" << (state.no_upgrade_material_cost ? "true" : "false") << "\n"
           << "all_weapons_enchantable=" << (state.all_weapons_enchantable ? "true" : "false") << "\n"
           << "all_weapons_ash_of_war_changeable=" << (state.all_weapons_ash_of_war_changeable ? "true" : "false") << "\n"
           << "no_magic_requirements=" << (state.no_magic_requirements ? "true" : "false") << "\n"
           << "all_magic_one_slot=" << (state.all_magic_one_slot ? "true" : "false") << "\n"
           << "beast_not_hostile=" << (state.beast_not_hostile ? "true" : "false") << "\n"
           << "silent_footsteps=" << (state.silent_footsteps ? "true" : "false") << "\n"
           << "infinite_consumables=" << (state.infinite_consumables ? "true" : "false") << "\n"
           << "infinite_arrows=" << (state.infinite_arrows ? "true" : "false") << "\n"
           << "no_rune_loss_on_death=" << (state.no_rune_loss_on_death ? "true" : "false") << "\n"
           << "no_rune_arc_loss_on_death=" << (state.no_rune_arc_loss_on_death ? "true" : "false") << "\n"
           << "no_time_pass_on_death=" << (state.no_time_pass_on_death ? "true" : "false") << "\n"
           << "attack_life_steal_on_hit=" << (state.attack_life_steal_on_hit ? "true" : "false") << "\n"
           << "model_scale_uniform=" << state.model_scale_uniform << "\n"
           << "infinite_jump=" << (state.infinite_jump ? "true" : "false") << "\n"
           << "spirit_ashes_anywhere=" << (state.spirit_ashes_anywhere ? "true" : "false") << "\n"
           << "torrent_no_death=" << (state.torrent_no_death ? "true" : "false") << "\n"
           << "torrent_anywhere=" << (state.torrent_anywhere ? "true" : "false") << "\n"
           << "no_fall_death=" << (state.no_fall_death ? "true" : "false") << "\n"
           << "enemy_poise_overlay_enabled=" << (state.enemy_poise_overlay_enabled ? "true" : "false") << "\n"
           << "enemy_resistance_overlay_enabled=" << (state.enemy_resistance_overlay_enabled ? "true" : "false") << "\n"
           << "enemy_resistance_bleed_enabled=" << (state.enemy_resistance_bleed_enabled ? "true" : "false") << "\n"
           << "enemy_resistance_frost_enabled=" << (state.enemy_resistance_frost_enabled ? "true" : "false") << "\n"
           << "enemy_resistance_rot_enabled=" << (state.enemy_resistance_rot_enabled ? "true" : "false") << "\n"
           << "enemy_resistance_poison_enabled=" << (state.enemy_resistance_poison_enabled ? "true" : "false") << "\n"
           << "enemy_resistance_sleep_enabled=" << (state.enemy_resistance_sleep_enabled ? "true" : "false") << "\n"
           << "enemy_resistance_madness_enabled=" << (state.enemy_resistance_madness_enabled ? "true" : "false") << "\n"
           << "auto_pickup_enabled=" << (state.auto_pickup_enabled ? "true" : "false") << "\n"
           << "auto_pickup_materials=" << (state.auto_pickup_materials ? "true" : "false") << "\n"
           << "auto_pickup_items=" << (state.auto_pickup_items ? "true" : "false") << "\n"
           << "auto_pickup_corpse_loot=" << (state.auto_pickup_corpse_loot ? "true" : "false") << "\n"
           << "auto_pickup_lost_runes=" << (state.auto_pickup_lost_runes ? "true" : "false") << "\n"
           << "auto_pickup_in_combat=" << (state.auto_pickup_in_combat ? "true" : "false") << "\n"
           << "auto_pickup_range_percent=" << state.auto_pickup_range_percent << "\n"
           << "ng_cycle=" << state.ng_cycle << "\n"
           << "\n[RallyMod]\n"
           << "rally_time_seconds=" << state.rally_time_seconds << "\n"
           << "rally_hit_reset=" << (state.rally_hit_reset ? "true" : "false") << "\n"
           << "rally_only_heal=" << (state.rally_only_heal ? "true" : "false") << "\n"
           << "rally_exponential_decay=" << (state.rally_exponential_decay ? "true" : "false") << "\n"
           << "rally_half_life_seconds=" << state.rally_half_life_seconds << "\n"
           << "rally_decay_seconds=" << state.rally_decay_seconds << "\n";

    output << "\n[Overlay]\n"
           << "target_status_debug_logging=" << (state.target_status_debug_logging ? "true" : "false") << "\n"
           << "overlay_poise_width_scale=" << state.overlay_poise_width_scale << "\n"
           << "overlay_poise_height_scale=" << state.overlay_poise_height_scale << "\n"
           << "overlay_status_width_scale=" << state.overlay_status_width_scale << "\n"
           << "overlay_status_height_scale=" << state.overlay_status_height_scale << "\n"
           << "overlay_poise_vertical_offset=" << state.overlay_poise_vertical_offset << "\n"
           << "overlay_entity_status_vertical_offset=" << state.overlay_entity_status_vertical_offset << "\n"
           << "overlay_boss_status_gap=" << state.overlay_boss_status_gap << "\n";

    if (state.debug_explicitly_configured) {
        output << "\n[Debug]\n"
               << "debug=" << (state.debug ? "true" : "false") << "\n";
    }

    output << "\n[Localization]\n"
           << "ui_language=" << state.ui_language << "\n";
}

void load_state_from_disk(PersistedState& state) {
    if (g_config_path.empty() || !std::filesystem::exists(g_config_path)) {
        return;
    }

    std::ifstream input(g_config_path);
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty() || line[0] == '#' || line[0] == '[') {
            continue;
        }

        const std::size_t separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        const std::string key = line.substr(0, separator);
        const std::string value = line.substr(separator + 1);

        if (key == "equipment_weight_reduction_percent") {
            state.equipment_weight_reduction_percent =
                parse_int_clamped(value, state.equipment_weight_reduction_percent, 0, 100);
        } else if (key == "weapon_requirement_reduction_percent") {
            state.weapon_requirement_reduction_percent =
                parse_int_clamped(value, state.weapon_requirement_reduction_percent, 0, 100);
        } else if (key == "player_speed_increase_percent") {
            state.player_speed_increase_percent =
                parse_int_clamped(value, state.player_speed_increase_percent, 0, 200);
        } else if (key == "buff_duration_extend_mode") {
            state.buff_duration_extend_mode =
                parse_int_clamped(value, state.buff_duration_extend_mode, 0, 99999);
        } else if (key == "weapon_parry_mode") {
            state.weapon_parry_mode = parse_int_clamped(value, state.weapon_parry_mode, 0, 60);
        } else if (key == "enemy_hp_increase_percent") {
            state.enemy_hp_increase_percent =
                parse_int_clamped(value, state.enemy_hp_increase_percent, 0, 1000);
        } else if (key == "enemy_hp_decrease_percent") {
            state.enemy_hp_decrease_percent =
                parse_int_clamped(value, state.enemy_hp_decrease_percent, 0, 1000);
        } else if (key == "damage_multiplier_percent") {
            state.damage_multiplier_percent = parse_int_clamped(value, state.damage_multiplier_percent, 0, 500);
        } else if (key == "damage_cut_multiplier_percent") {
            state.damage_cut_multiplier_percent = parse_int_clamped(value, state.damage_cut_multiplier_percent, 0, 500);
        } else if (key == "damage_reduce_multiplier_percent") {
            state.damage_reduce_multiplier_percent =
                parse_int_clamped(value, state.damage_reduce_multiplier_percent, 0, 500);
        } else if (key == "damage_taken_multiplier_percent") {
            state.damage_taken_multiplier_percent =
                parse_int_clamped(value, state.damage_taken_multiplier_percent, 0, 500);
        } else if (key == "stamina_cost_reduction_percent") {
            state.stamina_cost_reduction_percent =
                parse_int_clamped(value, state.stamina_cost_reduction_percent, 0, 100);
        } else if (key == "fp_cost_reduction_percent") {
            state.fp_cost_reduction_percent =
                parse_int_clamped(value, state.fp_cost_reduction_percent, 0, 100);
        } else if (key == "target_status_debug_logging") {
            state.target_status_debug_logging = parse_bool(value, state.target_status_debug_logging);
        } else if (key == "overlay_poise_width_scale") {
            state.overlay_poise_width_scale =
                parse_float_clamped(value, state.overlay_poise_width_scale, 0.5f, 2.0f);
        } else if (key == "overlay_poise_height_scale") {
            state.overlay_poise_height_scale =
                parse_float_clamped(value, state.overlay_poise_height_scale, 0.5f, 2.0f);
        } else if (key == "overlay_status_width_scale") {
            state.overlay_status_width_scale =
                parse_float_clamped(value, state.overlay_status_width_scale, 0.5f, 2.0f);
        } else if (key == "overlay_status_height_scale") {
            state.overlay_status_height_scale =
                parse_float_clamped(value, state.overlay_status_height_scale, 0.5f, 2.0f);
        } else if (key == "overlay_poise_vertical_offset") {
            state.overlay_poise_vertical_offset =
                parse_float_clamped(value, state.overlay_poise_vertical_offset, -300.0f, 300.0f);
        } else if (key == "overlay_entity_status_vertical_offset") {
            state.overlay_entity_status_vertical_offset =
                parse_float_clamped(value, state.overlay_entity_status_vertical_offset, -300.0f, 300.0f);
        } else if (key == "overlay_boss_status_gap") {
            state.overlay_boss_status_gap =
                parse_float_clamped(value, state.overlay_boss_status_gap, -100.0f, 200.0f);
        } else if (key == "weightless_equipment") {
            if (parse_bool(value, false)) {
                state.equipment_weight_reduction_percent = 100;
            }
        } else if (key == "free_purchase") {
            state.free_purchase = parse_bool(value, state.free_purchase);
        } else if (key == "no_crafting_material_cost") {
            state.no_crafting_material_cost = parse_bool(value, state.no_crafting_material_cost);
        } else if (key == "no_upgrade_material_cost") {
            state.no_upgrade_material_cost = parse_bool(value, state.no_upgrade_material_cost);
        } else if (key == "all_weapons_enchantable") {
            state.all_weapons_enchantable = parse_bool(value, state.all_weapons_enchantable);
        } else if (key == "all_weapons_ash_of_war_changeable") {
            state.all_weapons_ash_of_war_changeable = parse_bool(value, state.all_weapons_ash_of_war_changeable);
        } else if (key == "no_magic_requirements") {
            state.no_magic_requirements = parse_bool(value, state.no_magic_requirements);
        } else if (key == "all_magic_one_slot") {
            state.all_magic_one_slot = parse_bool(value, state.all_magic_one_slot);
        } else if (key == "beast_not_hostile") {
            state.beast_not_hostile = parse_bool(value, state.beast_not_hostile);
        } else if (key == "silent_footsteps") {
            state.silent_footsteps = parse_bool(value, state.silent_footsteps);
        } else if (key == "infinite_consumables") {
            state.infinite_consumables = parse_bool(value, state.infinite_consumables);
        } else if (key == "infinite_arrows") {
            state.infinite_arrows = parse_bool(value, state.infinite_arrows);
        } else if (key == "infinite_stamina") {
            state.infinite_stamina = parse_bool(value, state.infinite_stamina);
            if (state.infinite_stamina) {
                state.stamina_cost_reduction_percent = 100;
            }
        } else if (key == "infinite_fp") {
            state.infinite_fp = parse_bool(value, state.infinite_fp);
            if (state.infinite_fp) {
                state.fp_cost_reduction_percent = 100;
            }
        } else if (key == "no_rune_loss_on_death") {
            state.no_rune_loss_on_death = parse_bool(value, state.no_rune_loss_on_death);
        } else if (key == "no_rune_arc_loss_on_death") {
            state.no_rune_arc_loss_on_death = parse_bool(value, state.no_rune_arc_loss_on_death);
        } else if (key == "no_time_pass_on_death") {
            state.no_time_pass_on_death = parse_bool(value, state.no_time_pass_on_death);
        } else if (key == "attack_life_steal_on_hit") {
            state.attack_life_steal_on_hit = parse_bool(value, state.attack_life_steal_on_hit);
        } else if (key == "rally_time_seconds") {
            state.rally_time_seconds = parse_float_clamped(value, state.rally_time_seconds, 0.1f, 60.0f);
        } else if (key == "rally_hit_reset") {
            state.rally_hit_reset = parse_bool(value, state.rally_hit_reset);
        } else if (key == "rally_only_heal") {
            state.rally_only_heal = parse_bool(value, state.rally_only_heal);
        } else if (key == "rally_exponential_decay") {
            state.rally_exponential_decay = parse_bool(value, state.rally_exponential_decay);
        } else if (key == "rally_half_life_seconds") {
            state.rally_half_life_seconds =
                parse_float_clamped(value, state.rally_half_life_seconds, 0.1f, 60.0f);
        } else if (key == "rally_decay_seconds") {
            state.rally_decay_seconds =
                parse_float_clamped(value, state.rally_decay_seconds, 0.1f, 120.0f);
        } else if (key == "model_scale_uniform") {
            state.model_scale_uniform = parse_float_clamped(value, state.model_scale_uniform, 0.0f, 3.0f);
        } else if (key == "infinite_jump") {
            state.infinite_jump = parse_bool(value, state.infinite_jump);
        } else if (key == "spirit_ashes_anywhere") {
            state.spirit_ashes_anywhere = parse_bool(value, state.spirit_ashes_anywhere);
        } else if (key == "torrent_no_death") {
            state.torrent_no_death = parse_bool(value, state.torrent_no_death);
        } else if (key == "torrent_anywhere") {
            state.torrent_anywhere = parse_bool(value, state.torrent_anywhere);
        } else if (key == "no_fall_death") {
            state.no_fall_death = parse_bool(value, state.no_fall_death);
        } else if (key == "enemy_poise_overlay_enabled") {
            state.enemy_poise_overlay_enabled = parse_bool(value, state.enemy_poise_overlay_enabled);
        } else if (key == "enemy_resistance_overlay_enabled") {
            state.enemy_resistance_overlay_enabled = parse_bool(value, state.enemy_resistance_overlay_enabled);
        } else if (key == "enemy_resistance_bleed_enabled") {
            state.enemy_resistance_bleed_enabled = parse_bool(value, state.enemy_resistance_bleed_enabled);
        } else if (key == "enemy_resistance_frost_enabled") {
            state.enemy_resistance_frost_enabled = parse_bool(value, state.enemy_resistance_frost_enabled);
        } else if (key == "enemy_resistance_rot_enabled") {
            state.enemy_resistance_rot_enabled = parse_bool(value, state.enemy_resistance_rot_enabled);
        } else if (key == "enemy_resistance_poison_enabled") {
            state.enemy_resistance_poison_enabled = parse_bool(value, state.enemy_resistance_poison_enabled);
        } else if (key == "enemy_resistance_sleep_enabled") {
            state.enemy_resistance_sleep_enabled = parse_bool(value, state.enemy_resistance_sleep_enabled);
        } else if (key == "enemy_resistance_madness_enabled") {
            state.enemy_resistance_madness_enabled = parse_bool(value, state.enemy_resistance_madness_enabled);
        } else if (key == "auto_pickup_enabled") {
            state.auto_pickup_enabled = parse_bool(value, state.auto_pickup_enabled);
        } else if (key == "auto_pickup_materials") {
            state.auto_pickup_materials = parse_bool(value, state.auto_pickup_materials);
        } else if (key == "auto_pickup_items") {
            state.auto_pickup_items = parse_bool(value, state.auto_pickup_items);
        } else if (key == "auto_pickup_corpse_loot") {
            state.auto_pickup_corpse_loot = parse_bool(value, state.auto_pickup_corpse_loot);
        } else if (key == "auto_pickup_lost_runes") {
            state.auto_pickup_lost_runes = parse_bool(value, state.auto_pickup_lost_runes);
        } else if (key == "auto_pickup_in_combat") {
            state.auto_pickup_in_combat = parse_bool(value, state.auto_pickup_in_combat);
        } else if (key == "auto_pickup_range_percent") {
            state.auto_pickup_range_percent =
                parse_int_clamped(value, state.auto_pickup_range_percent, 100, 500);
        } else if (key == "ng_cycle") {
            state.ng_cycle = parse_int_clamped(value, state.ng_cycle, 0, 10);
        } else if (key == "debug") {
            state.debug = parse_bool(value, state.debug);
            state.debug_explicitly_configured = true;
        } else if (key == "ui_language") {
            state.ui_language = value.empty() ? "auto" : value;
        }
    }
}

}  // namespace

void initialize(const std::filesystem::path& folder) {
    std::scoped_lock lock(g_config_mutex);
    g_config_path = folder / "erdGameTools.ini";
    load_state_from_disk(g_state);
    apply_state_to_features(g_state);
    write_state_to_disk(g_state);
    if (std::filesystem::exists(g_config_path)) {
        g_last_write_time = std::filesystem::last_write_time(g_config_path);
    }
    SPDLOG_INFO("Loaded persisted game mod settings from {}", g_config_path.string());
}

void apply_persisted_feature_state() {
    std::scoped_lock lock(g_config_mutex);
    apply_state_to_features(g_state);
}

void persist_current_feature_state() {
    std::scoped_lock lock(g_config_mutex);
    g_state = capture_current_feature_state();
    write_state_to_disk(g_state);
    if (std::filesystem::exists(g_config_path)) {
        g_last_write_time = std::filesystem::last_write_time(g_config_path);
    }
}

void maybe_reload_if_changed() {
    const ULONGLONG now = GetTickCount64();
    if (now < g_next_reload_check_tick) {
        return;
    }
    g_next_reload_check_tick = now + kReloadPollIntervalMs;

    std::scoped_lock lock(g_config_mutex);
    if (g_config_path.empty() || !std::filesystem::exists(g_config_path)) {
        return;
    }

    const auto current_write_time = std::filesystem::last_write_time(g_config_path);
    if (current_write_time == g_last_write_time) {
        return;
    }

    PersistedState next_state = g_state;
    load_state_from_disk(next_state);
    g_state = next_state;
    apply_state_to_features(g_state);
    g_last_write_time = current_write_time;
    spdlog::info("Reloaded configuration from {}", g_config_path.string());
}

std::string get_ui_language_override() {
    std::scoped_lock lock(g_config_mutex);
    return g_state.ui_language.empty() ? "auto" : g_state.ui_language;
}

}  // namespace grace_test::config
