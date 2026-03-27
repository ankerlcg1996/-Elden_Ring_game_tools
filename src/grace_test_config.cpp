#include "grace_test_config.hpp"

#include "Main/FeatureStatus.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <fstream>
#include <mutex>
#include <string>

namespace grace_test::config {
namespace {

struct PersistedState {
    int equipment_weight_reduction_percent = 0;
    int weapon_requirement_reduction_percent = 0;
    int player_speed_increase_percent = 0;
    int buff_duration_extend_mode = 0;
    int enemy_hp_increase_percent = 0;
    int enemy_hp_decrease_percent = 0;
    int easier_parry_percent = 0;
    int damage_multiplier_percent = 0;
    int damage_cut_multiplier_percent = 0;
    int damage_reduce_multiplier_percent = 0;
    int damage_taken_multiplier_percent = 0;
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
    bool infinite_jump = false;
    bool spirit_ashes_anywhere = false;
    bool torrent_no_death = false;
    bool torrent_anywhere = false;
    bool debug = false;
    std::string ui_language = "auto";
};

std::mutex g_config_mutex;
std::filesystem::path g_config_path;
PersistedState g_state;

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
    ERD::Main::g_FeatureStatus.enemy_hp_increase_percent =
        std::clamp(state.enemy_hp_increase_percent, 0, 1000);
    ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent =
        std::clamp(state.enemy_hp_decrease_percent, 0, 1000);
    if (ERD::Main::g_FeatureStatus.enemy_hp_increase_percent.load() > 0) {
        ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent = 0;
    }
    ERD::Main::g_FeatureStatus.movement_speed =
        1.0f + static_cast<float>(ERD::Main::g_FeatureStatus.player_speed_increase_percent.load()) / 100.0f;
    ERD::Main::g_FeatureStatus.easier_parry_percent = std::clamp(state.easier_parry_percent, 0, 100);
    ERD::Main::g_FeatureStatus.damage_multiplier_percent = std::clamp(state.damage_multiplier_percent, 0, 500);
    ERD::Main::g_FeatureStatus.damage_cut_multiplier_percent =
        std::clamp(state.damage_cut_multiplier_percent, 0, 500);
    ERD::Main::g_FeatureStatus.damage_reduce_multiplier_percent =
        std::clamp(state.damage_reduce_multiplier_percent, 0, 500);
    ERD::Main::g_FeatureStatus.damage_taken_multiplier_percent =
        std::clamp(state.damage_taken_multiplier_percent, 0, 500);
    ERD::Main::g_FeatureStatus.easier_guard_percent = ERD::Main::g_FeatureStatus.easier_parry_percent.load();
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
    ERD::Main::g_FeatureStatus.infinite_stamina = state.infinite_stamina;
    ERD::Main::g_FeatureStatus.infinite_fp = state.infinite_fp;
    ERD::Main::g_FeatureStatus.no_rune_loss_on_death = state.no_rune_loss_on_death;
    ERD::Main::g_FeatureStatus.infinite_jump = state.infinite_jump;
    ERD::Main::g_FeatureStatus.spirit_ashes_anywhere = state.spirit_ashes_anywhere;
    ERD::Main::g_FeatureStatus.torrent_no_death = state.torrent_no_death;
    ERD::Main::g_FeatureStatus.torrent_anywhere = state.torrent_anywhere;
    ERD::Main::g_FeatureStatus.debug_mode = state.debug;
    ERD::Main::g_FeatureStatus.damage_popup_overlay_enabled = true;
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
    state.enemy_hp_increase_percent =
        std::clamp(ERD::Main::g_FeatureStatus.enemy_hp_increase_percent.load(), 0, 1000);
    state.enemy_hp_decrease_percent =
        std::clamp(ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent.load(), 0, 1000);
    state.easier_parry_percent = std::clamp(ERD::Main::g_FeatureStatus.easier_parry_percent.load(), 0, 100);
    state.damage_multiplier_percent = std::clamp(ERD::Main::g_FeatureStatus.damage_multiplier_percent.load(), 0, 500);
    state.damage_cut_multiplier_percent =
        std::clamp(ERD::Main::g_FeatureStatus.damage_cut_multiplier_percent.load(), 0, 500);
    state.damage_reduce_multiplier_percent =
        std::clamp(ERD::Main::g_FeatureStatus.damage_reduce_multiplier_percent.load(), 0, 500);
    state.damage_taken_multiplier_percent =
        std::clamp(ERD::Main::g_FeatureStatus.damage_taken_multiplier_percent.load(), 0, 500);
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
    state.infinite_stamina = ERD::Main::g_FeatureStatus.infinite_stamina.load();
    state.infinite_fp = ERD::Main::g_FeatureStatus.infinite_fp.load();
    state.no_rune_loss_on_death = ERD::Main::g_FeatureStatus.no_rune_loss_on_death.load();
    state.infinite_jump = ERD::Main::g_FeatureStatus.infinite_jump.load();
    state.spirit_ashes_anywhere = ERD::Main::g_FeatureStatus.spirit_ashes_anywhere.load();
    state.torrent_no_death = ERD::Main::g_FeatureStatus.torrent_no_death.load();
    state.torrent_anywhere = ERD::Main::g_FeatureStatus.torrent_anywhere.load();
    state.debug = ERD::Main::g_FeatureStatus.debug_mode.load();
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
           << "enemy_hp_increase_percent=" << state.enemy_hp_increase_percent << "\n"
           << "enemy_hp_decrease_percent=" << state.enemy_hp_decrease_percent << "\n"
           << "easier_parry_percent=" << state.easier_parry_percent << "\n"
           << "damage_multiplier_percent=" << state.damage_multiplier_percent << "\n"
           << "damage_cut_multiplier_percent=" << state.damage_cut_multiplier_percent << "\n"
           << "damage_reduce_multiplier_percent=" << state.damage_reduce_multiplier_percent << "\n"
           << "damage_taken_multiplier_percent=" << state.damage_taken_multiplier_percent << "\n"
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
           << "infinite_stamina=" << (state.infinite_stamina ? "true" : "false") << "\n"
           << "infinite_fp=" << (state.infinite_fp ? "true" : "false") << "\n"
           << "no_rune_loss_on_death=" << (state.no_rune_loss_on_death ? "true" : "false") << "\n"
           << "infinite_jump=" << (state.infinite_jump ? "true" : "false") << "\n"
           << "spirit_ashes_anywhere=" << (state.spirit_ashes_anywhere ? "true" : "false") << "\n"
           << "torrent_no_death=" << (state.torrent_no_death ? "true" : "false") << "\n"
           << "torrent_anywhere=" << (state.torrent_anywhere ? "true" : "false") << "\n"
           << "\n[Debug]\n"
           << "debug=" << (state.debug ? "true" : "false") << "\n"
           << "\n[Localization]\n"
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
        } else if (key == "enemy_hp_increase_percent") {
            state.enemy_hp_increase_percent =
                parse_int_clamped(value, state.enemy_hp_increase_percent, 0, 1000);
        } else if (key == "enemy_hp_decrease_percent") {
            state.enemy_hp_decrease_percent =
                parse_int_clamped(value, state.enemy_hp_decrease_percent, 0, 1000);
        } else if (key == "easier_parry_percent") {
            state.easier_parry_percent = parse_int_clamped(value, state.easier_parry_percent, 0, 100);
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
        } else if (key == "easier_guard_percent") {
            state.easier_parry_percent = parse_int_clamped(value, state.easier_parry_percent, 0, 100);
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
        } else if (key == "infinite_fp") {
            state.infinite_fp = parse_bool(value, state.infinite_fp);
        } else if (key == "no_rune_loss_on_death") {
            state.no_rune_loss_on_death = parse_bool(value, state.no_rune_loss_on_death);
        } else if (key == "infinite_jump") {
            state.infinite_jump = parse_bool(value, state.infinite_jump);
        } else if (key == "spirit_ashes_anywhere") {
            state.spirit_ashes_anywhere = parse_bool(value, state.spirit_ashes_anywhere);
        } else if (key == "torrent_no_death") {
            state.torrent_no_death = parse_bool(value, state.torrent_no_death);
        } else if (key == "torrent_anywhere") {
            state.torrent_anywhere = parse_bool(value, state.torrent_anywhere);
        } else if (key == "debug") {
            state.debug = parse_bool(value, state.debug);
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
}

std::string get_ui_language_override() {
    std::scoped_lock lock(g_config_mutex);
    return g_state.ui_language.empty() ? "auto" : g_state.ui_language;
}

}  // namespace grace_test::config
