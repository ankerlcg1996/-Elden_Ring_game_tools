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
    int easier_parry_percent = 0;
    bool free_purchase = false;
    bool no_crafting_material_cost = false;
    bool no_upgrade_material_cost = false;
    bool all_weapons_enchantable = false;
    bool no_magic_requirements = false;
    bool all_magic_one_slot = false;
    bool silent_footsteps = false;
    bool spirit_ashes_anywhere = false;
    bool torrent_no_death = false;
    bool torrent_anywhere = false;
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
    ERD::Main::g_FeatureStatus.easier_parry_percent = std::clamp(state.easier_parry_percent, 0, 100);
    ERD::Main::g_FeatureStatus.easier_guard_percent = ERD::Main::g_FeatureStatus.easier_parry_percent.load();
    ERD::Main::g_FeatureStatus.weightless_equipment =
        ERD::Main::g_FeatureStatus.equipment_weight_reduction_percent.load() >= 100;
    ERD::Main::g_FeatureStatus.free_purchase = state.free_purchase;
    ERD::Main::g_FeatureStatus.no_crafting_material_cost = state.no_crafting_material_cost;
    ERD::Main::g_FeatureStatus.no_upgrade_material_cost = state.no_upgrade_material_cost;
    ERD::Main::g_FeatureStatus.all_weapons_enchantable = state.all_weapons_enchantable;
    ERD::Main::g_FeatureStatus.no_magic_requirements = state.no_magic_requirements;
    ERD::Main::g_FeatureStatus.all_magic_one_slot = state.all_magic_one_slot;
    ERD::Main::g_FeatureStatus.silent_footsteps = state.silent_footsteps;
    ERD::Main::g_FeatureStatus.spirit_ashes_anywhere = state.spirit_ashes_anywhere;
    ERD::Main::g_FeatureStatus.torrent_no_death = state.torrent_no_death;
    ERD::Main::g_FeatureStatus.torrent_anywhere = state.torrent_anywhere;
    ERD::Main::g_FeatureStatus.damage_popup_overlay_enabled = true;
}

PersistedState capture_current_feature_state() {
    PersistedState state{};
    state.equipment_weight_reduction_percent =
        std::clamp(ERD::Main::g_FeatureStatus.equipment_weight_reduction_percent.load(), 0, 100);
    state.weapon_requirement_reduction_percent =
        std::clamp(ERD::Main::g_FeatureStatus.weapon_requirement_reduction_percent.load(), 0, 100);
    state.easier_parry_percent = std::clamp(ERD::Main::g_FeatureStatus.easier_parry_percent.load(), 0, 100);
    state.free_purchase = ERD::Main::g_FeatureStatus.free_purchase.load();
    state.no_crafting_material_cost = ERD::Main::g_FeatureStatus.no_crafting_material_cost.load();
    state.no_upgrade_material_cost = ERD::Main::g_FeatureStatus.no_upgrade_material_cost.load();
    state.all_weapons_enchantable = ERD::Main::g_FeatureStatus.all_weapons_enchantable.load();
    state.no_magic_requirements = ERD::Main::g_FeatureStatus.no_magic_requirements.load();
    state.all_magic_one_slot = ERD::Main::g_FeatureStatus.all_magic_one_slot.load();
    state.silent_footsteps = ERD::Main::g_FeatureStatus.silent_footsteps.load();
    state.spirit_ashes_anywhere = ERD::Main::g_FeatureStatus.spirit_ashes_anywhere.load();
    state.torrent_no_death = ERD::Main::g_FeatureStatus.torrent_no_death.load();
    state.torrent_anywhere = ERD::Main::g_FeatureStatus.torrent_anywhere.load();
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
           << "easier_parry_percent=" << state.easier_parry_percent << "\n"
           << "free_purchase=" << (state.free_purchase ? "true" : "false") << "\n"
           << "no_crafting_material_cost=" << (state.no_crafting_material_cost ? "true" : "false") << "\n"
           << "no_upgrade_material_cost=" << (state.no_upgrade_material_cost ? "true" : "false") << "\n"
           << "all_weapons_enchantable=" << (state.all_weapons_enchantable ? "true" : "false") << "\n"
           << "no_magic_requirements=" << (state.no_magic_requirements ? "true" : "false") << "\n"
           << "all_magic_one_slot=" << (state.all_magic_one_slot ? "true" : "false") << "\n"
           << "silent_footsteps=" << (state.silent_footsteps ? "true" : "false") << "\n"
           << "spirit_ashes_anywhere=" << (state.spirit_ashes_anywhere ? "true" : "false") << "\n"
           << "torrent_no_death=" << (state.torrent_no_death ? "true" : "false") << "\n"
           << "torrent_anywhere=" << (state.torrent_anywhere ? "true" : "false") << "\n"
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
        } else if (key == "easier_parry_percent") {
            state.easier_parry_percent = parse_int_clamped(value, state.easier_parry_percent, 0, 100);
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
        } else if (key == "no_magic_requirements") {
            state.no_magic_requirements = parse_bool(value, state.no_magic_requirements);
        } else if (key == "all_magic_one_slot") {
            state.all_magic_one_slot = parse_bool(value, state.all_magic_one_slot);
        } else if (key == "silent_footsteps") {
            state.silent_footsteps = parse_bool(value, state.silent_footsteps);
        } else if (key == "spirit_ashes_anywhere") {
            state.spirit_ashes_anywhere = parse_bool(value, state.spirit_ashes_anywhere);
        } else if (key == "torrent_no_death") {
            state.torrent_no_death = parse_bool(value, state.torrent_no_death);
        } else if (key == "torrent_anywhere") {
            state.torrent_anywhere = parse_bool(value, state.torrent_anywhere);
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
