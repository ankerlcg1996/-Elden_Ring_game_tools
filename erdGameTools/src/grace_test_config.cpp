#include "grace_test_config.hpp"

#include "Main/FeatureStatus.hpp"

#include <spdlog/spdlog.h>

#include <fstream>
#include <mutex>
#include <string>

namespace grace_test::config {
namespace {

struct PersistedState {
    bool weightless_equipment = false;
    bool infinite_items = false;
    bool no_damage = false;
    bool no_stamina_consumption = false;
    bool free_purchase = false;
    bool no_crafting_material_cost = false;
    bool no_upgrade_material_cost = false;
    bool no_magic_requirements = false;
    bool all_magic_one_slot = false;
    bool mount_anywhere = false;
    bool spirit_ashes_anywhere = false;
    bool posture_bar_overlay_enabled = false;
    bool damage_popup_overlay_enabled = true;
    bool unlock_all_maps = false;
    bool unlock_all_cookbooks = false;
    bool unlock_all_whetblades = false;
    bool unlock_all_graces = false;
    bool unlock_all_summoning_pools = false;
    bool unlock_all_colosseums = false;
    bool unlock_all_gestures = false;
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

void apply_state_to_features(const PersistedState& state) {
    ERD::Main::g_FeatureStatus.weightless_equipment = state.weightless_equipment;
    ERD::Main::g_FeatureStatus.infinite_items = state.infinite_items;
    ERD::Main::g_FeatureStatus.no_damage = state.no_damage;
    ERD::Main::g_FeatureStatus.no_stamina_consumption = state.no_stamina_consumption;
    ERD::Main::g_FeatureStatus.free_purchase = state.free_purchase;
    ERD::Main::g_FeatureStatus.no_crafting_material_cost = state.no_crafting_material_cost;
    ERD::Main::g_FeatureStatus.no_upgrade_material_cost = state.no_upgrade_material_cost;
    ERD::Main::g_FeatureStatus.no_magic_requirements = state.no_magic_requirements;
    ERD::Main::g_FeatureStatus.all_magic_one_slot = state.all_magic_one_slot;
    ERD::Main::g_FeatureStatus.mount_anywhere = state.mount_anywhere;
    ERD::Main::g_FeatureStatus.spirit_ashes_anywhere = state.spirit_ashes_anywhere;
    ERD::Main::g_FeatureStatus.posture_bar_overlay_enabled = state.posture_bar_overlay_enabled;
    ERD::Main::g_FeatureStatus.damage_popup_overlay_enabled = state.damage_popup_overlay_enabled;
    ERD::Main::g_FeatureStatus.unlock_all_maps = state.unlock_all_maps;
    ERD::Main::g_FeatureStatus.unlock_all_cookbooks = state.unlock_all_cookbooks;
    ERD::Main::g_FeatureStatus.unlock_all_whetblades = state.unlock_all_whetblades;
    ERD::Main::g_FeatureStatus.unlock_all_graces = state.unlock_all_graces;
    ERD::Main::g_FeatureStatus.unlock_all_summoning_pools = state.unlock_all_summoning_pools;
    ERD::Main::g_FeatureStatus.unlock_all_colosseums = state.unlock_all_colosseums;
    ERD::Main::g_FeatureStatus.unlock_all_gestures = state.unlock_all_gestures;
}

PersistedState capture_current_feature_state() {
    PersistedState state{};
    state.weightless_equipment = ERD::Main::g_FeatureStatus.weightless_equipment.load();
    state.infinite_items = ERD::Main::g_FeatureStatus.infinite_items.load();
    state.no_damage = ERD::Main::g_FeatureStatus.no_damage.load();
    state.no_stamina_consumption = ERD::Main::g_FeatureStatus.no_stamina_consumption.load();
    state.free_purchase = ERD::Main::g_FeatureStatus.free_purchase.load();
    state.no_crafting_material_cost = ERD::Main::g_FeatureStatus.no_crafting_material_cost.load();
    state.no_upgrade_material_cost = ERD::Main::g_FeatureStatus.no_upgrade_material_cost.load();
    state.no_magic_requirements = ERD::Main::g_FeatureStatus.no_magic_requirements.load();
    state.all_magic_one_slot = ERD::Main::g_FeatureStatus.all_magic_one_slot.load();
    state.mount_anywhere = ERD::Main::g_FeatureStatus.mount_anywhere.load();
    state.spirit_ashes_anywhere = ERD::Main::g_FeatureStatus.spirit_ashes_anywhere.load();
    state.posture_bar_overlay_enabled = ERD::Main::g_FeatureStatus.posture_bar_overlay_enabled.load();
    state.damage_popup_overlay_enabled = ERD::Main::g_FeatureStatus.damage_popup_overlay_enabled.load();
    state.unlock_all_maps = ERD::Main::g_FeatureStatus.unlock_all_maps.load();
    state.unlock_all_cookbooks = ERD::Main::g_FeatureStatus.unlock_all_cookbooks.load();
    state.unlock_all_whetblades = ERD::Main::g_FeatureStatus.unlock_all_whetblades.load();
    state.unlock_all_graces = ERD::Main::g_FeatureStatus.unlock_all_graces.load();
    state.unlock_all_summoning_pools = ERD::Main::g_FeatureStatus.unlock_all_summoning_pools.load();
    state.unlock_all_colosseums = ERD::Main::g_FeatureStatus.unlock_all_colosseums.load();
    state.unlock_all_gestures = ERD::Main::g_FeatureStatus.unlock_all_gestures.load();
    state.ui_language = g_state.ui_language;
    return state;
}

void write_state_to_disk(const PersistedState& state) {
    if (g_config_path.empty()) {
        return;
    }

    std::ofstream output(g_config_path, std::ios::out | std::ios::trunc);
    output << "[GameMods]\n"
           << "weightless_equipment=" << (state.weightless_equipment ? "true" : "false") << "\n"
           << "infinite_items=" << (state.infinite_items ? "true" : "false") << "\n"
           << "no_damage=" << (state.no_damage ? "true" : "false") << "\n"
           << "no_stamina_consumption=" << (state.no_stamina_consumption ? "true" : "false") << "\n"
           << "free_purchase=" << (state.free_purchase ? "true" : "false") << "\n"
           << "no_crafting_material_cost=" << (state.no_crafting_material_cost ? "true" : "false") << "\n"
           << "no_upgrade_material_cost=" << (state.no_upgrade_material_cost ? "true" : "false") << "\n"
           << "no_magic_requirements=" << (state.no_magic_requirements ? "true" : "false") << "\n"
           << "all_magic_one_slot=" << (state.all_magic_one_slot ? "true" : "false") << "\n"
           << "mount_anywhere=" << (state.mount_anywhere ? "true" : "false") << "\n"
           << "spirit_ashes_anywhere=" << (state.spirit_ashes_anywhere ? "true" : "false") << "\n"
           << "posture_bar_overlay_enabled=" << (state.posture_bar_overlay_enabled ? "true" : "false") << "\n"
           << "damage_popup_overlay_enabled=" << (state.damage_popup_overlay_enabled ? "true" : "false") << "\n"
           << "\n[Unlocks]\n"
           << "unlock_all_maps=" << (state.unlock_all_maps ? "true" : "false") << "\n"
           << "unlock_all_cookbooks=" << (state.unlock_all_cookbooks ? "true" : "false") << "\n"
           << "unlock_all_whetblades=" << (state.unlock_all_whetblades ? "true" : "false") << "\n"
           << "unlock_all_graces=" << (state.unlock_all_graces ? "true" : "false") << "\n"
           << "unlock_all_summoning_pools=" << (state.unlock_all_summoning_pools ? "true" : "false") << "\n"
           << "unlock_all_colosseums=" << (state.unlock_all_colosseums ? "true" : "false") << "\n"
           << "unlock_all_gestures=" << (state.unlock_all_gestures ? "true" : "false") << "\n"
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

        if (key == "weightless_equipment") {
            state.weightless_equipment = parse_bool(value, state.weightless_equipment);
        } else if (key == "infinite_items") {
            state.infinite_items = parse_bool(value, state.infinite_items);
        } else if (key == "no_damage") {
            state.no_damage = parse_bool(value, state.no_damage);
        } else if (key == "no_stamina_consumption") {
            state.no_stamina_consumption = parse_bool(value, state.no_stamina_consumption);
        } else if (key == "free_purchase") {
            state.free_purchase = parse_bool(value, state.free_purchase);
        } else if (key == "no_crafting_material_cost") {
            state.no_crafting_material_cost = parse_bool(value, state.no_crafting_material_cost);
        } else if (key == "no_upgrade_material_cost") {
            state.no_upgrade_material_cost = parse_bool(value, state.no_upgrade_material_cost);
        } else if (key == "no_magic_requirements") {
            state.no_magic_requirements = parse_bool(value, state.no_magic_requirements);
        } else if (key == "all_magic_one_slot") {
            state.all_magic_one_slot = parse_bool(value, state.all_magic_one_slot);
        } else if (key == "mount_anywhere") {
            state.mount_anywhere = parse_bool(value, state.mount_anywhere);
        } else if (key == "spirit_ashes_anywhere") {
            state.spirit_ashes_anywhere = parse_bool(value, state.spirit_ashes_anywhere);
        } else if (key == "posture_bar_overlay_enabled") {
            state.posture_bar_overlay_enabled = parse_bool(value, state.posture_bar_overlay_enabled);
        } else if (key == "damage_popup_overlay_enabled") {
            state.damage_popup_overlay_enabled = parse_bool(value, state.damage_popup_overlay_enabled);
        } else if (key == "unlock_all_maps") {
            state.unlock_all_maps = parse_bool(value, state.unlock_all_maps);
        } else if (key == "unlock_all_cookbooks") {
            state.unlock_all_cookbooks = parse_bool(value, state.unlock_all_cookbooks);
        } else if (key == "unlock_all_whetblades") {
            state.unlock_all_whetblades = parse_bool(value, state.unlock_all_whetblades);
        } else if (key == "unlock_all_graces") {
            state.unlock_all_graces = parse_bool(value, state.unlock_all_graces);
        } else if (key == "unlock_all_summoning_pools") {
            state.unlock_all_summoning_pools = parse_bool(value, state.unlock_all_summoning_pools);
        } else if (key == "unlock_all_colosseums") {
            state.unlock_all_colosseums = parse_bool(value, state.unlock_all_colosseums);
        } else if (key == "unlock_all_gestures") {
            state.unlock_all_gestures = parse_bool(value, state.unlock_all_gestures);
        } else if (key == "ui_language") {
            state.ui_language = value.empty() ? "auto" : value;
        }
    }
}

}  // namespace

void initialize(const std::filesystem::path& folder) {
    std::scoped_lock lock(g_config_mutex);
    g_config_path = folder / "config.ini";
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
