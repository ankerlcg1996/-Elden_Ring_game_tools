#include "grace_test_talkscript.hpp"

#include "grace_test_config.hpp"
#include "grace_test_localization.hpp"
#include "grace_test_messages.hpp"
#include "utils/talkscript_utils.hpp"

#include "Main/FeatureStatus.hpp"

#include <elden-x/ezstate/ezstate.hpp>
#include <elden-x/ezstate/talk_commands.hpp>
#include <elden-x/utils/modutils.hpp>

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

namespace grace_test::talkscript {
namespace {

enum class ToggleAction {
    FreePurchase = 0,
    NoCraftMaterialCost,
    NoUpgradeMaterialCost,
    AllWeaponsEnchantable,
    NoMagicRequirements,
    AllMagicOneSlot,
    SilentFootsteps,
    SpiritAshesAnywhere,
    TorrentNoDeath,
    TorrentAnywhere,
};

struct ToggleDescriptor {
    int menu_index = 0;
    int message_id = 0;
    const char* label_key = "";
    ToggleAction action = ToggleAction::FreePurchase;
};

constexpr std::array<int, 10> kPercentSteps{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
constexpr std::array<int, 5> kWeaponRequirementPercentSteps{20, 40, 60, 80, 100};

constexpr ToggleDescriptor kPlayerToggleDescriptors[] = {
    {3, msg::event_text_for_talk_toggle_no_magic_req, "toggle_no_magic_req", ToggleAction::NoMagicRequirements},
    {4, msg::event_text_for_talk_toggle_one_slot_magic, "toggle_one_slot_magic", ToggleAction::AllMagicOneSlot},
    {5, msg::event_text_for_talk_toggle_silent_footsteps, "toggle_silent_footsteps", ToggleAction::SilentFootsteps},
};

constexpr ToggleDescriptor kGameToggleDescriptors[] = {
    {1, msg::event_text_for_talk_toggle_free_purchase, "toggle_free_purchase", ToggleAction::FreePurchase},
    {2, msg::event_text_for_talk_toggle_no_craft_cost, "toggle_no_craft_cost", ToggleAction::NoCraftMaterialCost},
    {3, msg::event_text_for_talk_toggle_no_upgrade_cost, "toggle_no_upgrade_cost", ToggleAction::NoUpgradeMaterialCost},
    {4, msg::event_text_for_talk_toggle_all_weapons_enchantable, "toggle_all_weapons_enchantable", ToggleAction::AllWeaponsEnchantable},
    {5, msg::event_text_for_talk_toggle_spirit_anywhere, "toggle_spirit_anywhere", ToggleAction::SpiritAshesAnywhere},
    {6, msg::event_text_for_talk_toggle_torrent_no_death, "toggle_torrent_no_death", ToggleAction::TorrentNoDeath},
    {7, msg::event_text_for_talk_toggle_torrent_anywhere, "toggle_torrent_anywhere", ToggleAction::TorrentAnywhere},
};

talkscript_menu_state g_root_menu_state;
talkscript_menu_state g_player_mods_menu_state;
talkscript_menu_state g_game_mods_menu_state;
talkscript_menu_state g_equipment_reduction_menu_state;
talkscript_menu_state g_weapon_requirement_reduction_menu_state;
talkscript_menu_state g_easier_parry_menu_state;
talkscript_menu_option g_top_level_option{70, msg::event_text_for_talk_root, &g_root_menu_state};

std::array<std::unique_ptr<callback_state>, std::size(kPlayerToggleDescriptors)> g_player_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kGameToggleDescriptors)> g_game_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kPercentSteps)> g_equipment_reduction_states{};
std::array<std::unique_ptr<callback_state>, std::size(kWeaponRequirementPercentSteps)> g_weapon_requirement_reduction_states{};
std::array<std::unique_ptr<callback_state>, std::size(kPercentSteps)> g_easier_parry_states{};

std::unordered_map<int, std::wstring> g_message_cache;

er::ezstate::state* g_grace_initial_state = nullptr;
static auto g_patched_events = std::array<er::ezstate::event, 128>{};
static auto g_patched_transitions = std::array<er::ezstate::transition*, 128>{};

void set_message(int message_id, std::wstring text) {
    g_message_cache[message_id] = std::move(text);
}

int current_equipment_reduction_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.equipment_weight_reduction_percent.load(), 0, 100);
}

int current_easier_parry_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.easier_parry_percent.load(), 0, 100);
}

int current_weapon_requirement_reduction_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.weapon_requirement_reduction_percent.load(), 0, 100);
}

bool toggle_state(ToggleAction action) {
    switch (action) {
    case ToggleAction::FreePurchase:
        return ERD::Main::g_FeatureStatus.free_purchase.load();
    case ToggleAction::NoCraftMaterialCost:
        return ERD::Main::g_FeatureStatus.no_crafting_material_cost.load();
    case ToggleAction::NoUpgradeMaterialCost:
        return ERD::Main::g_FeatureStatus.no_upgrade_material_cost.load();
    case ToggleAction::AllWeaponsEnchantable:
        return ERD::Main::g_FeatureStatus.all_weapons_enchantable.load();
    case ToggleAction::NoMagicRequirements:
        return ERD::Main::g_FeatureStatus.no_magic_requirements.load();
    case ToggleAction::AllMagicOneSlot:
        return ERD::Main::g_FeatureStatus.all_magic_one_slot.load();
    case ToggleAction::SilentFootsteps:
        return ERD::Main::g_FeatureStatus.silent_footsteps.load();
    case ToggleAction::SpiritAshesAnywhere:
        return ERD::Main::g_FeatureStatus.spirit_ashes_anywhere.load();
    case ToggleAction::TorrentNoDeath:
        return ERD::Main::g_FeatureStatus.torrent_no_death.load();
    case ToggleAction::TorrentAnywhere:
        return ERD::Main::g_FeatureStatus.torrent_anywhere.load();
    }
    return false;
}

void toggle_action(ToggleAction action) {
    switch (action) {
    case ToggleAction::FreePurchase:
        ERD::Main::g_FeatureStatus.free_purchase =
            !ERD::Main::g_FeatureStatus.free_purchase.load();
        break;
    case ToggleAction::NoCraftMaterialCost:
        ERD::Main::g_FeatureStatus.no_crafting_material_cost =
            !ERD::Main::g_FeatureStatus.no_crafting_material_cost.load();
        break;
    case ToggleAction::NoUpgradeMaterialCost:
        ERD::Main::g_FeatureStatus.no_upgrade_material_cost =
            !ERD::Main::g_FeatureStatus.no_upgrade_material_cost.load();
        break;
    case ToggleAction::AllWeaponsEnchantable:
        ERD::Main::g_FeatureStatus.all_weapons_enchantable =
            !ERD::Main::g_FeatureStatus.all_weapons_enchantable.load();
        break;
    case ToggleAction::NoMagicRequirements:
        ERD::Main::g_FeatureStatus.no_magic_requirements =
            !ERD::Main::g_FeatureStatus.no_magic_requirements.load();
        break;
    case ToggleAction::AllMagicOneSlot:
        ERD::Main::g_FeatureStatus.all_magic_one_slot =
            !ERD::Main::g_FeatureStatus.all_magic_one_slot.load();
        break;
    case ToggleAction::SilentFootsteps:
        ERD::Main::g_FeatureStatus.silent_footsteps =
            !ERD::Main::g_FeatureStatus.silent_footsteps.load();
        break;
    case ToggleAction::SpiritAshesAnywhere:
        ERD::Main::g_FeatureStatus.spirit_ashes_anywhere =
            !ERD::Main::g_FeatureStatus.spirit_ashes_anywhere.load();
        break;
    case ToggleAction::TorrentNoDeath:
        ERD::Main::g_FeatureStatus.torrent_no_death =
            !ERD::Main::g_FeatureStatus.torrent_no_death.load();
        break;
    case ToggleAction::TorrentAnywhere:
        ERD::Main::g_FeatureStatus.torrent_anywhere =
            !ERD::Main::g_FeatureStatus.torrent_anywhere.load();
        break;
    }
}

void handle_toggle_callback(int payload) {
    toggle_action(static_cast<ToggleAction>(payload));
    grace_test::config::persist_current_feature_state();
}

void handle_set_equipment_reduction_callback(int payload) {
    const int percent = std::clamp(payload, 0, 100);
    ERD::Main::g_FeatureStatus.equipment_weight_reduction_percent = percent;
    ERD::Main::g_FeatureStatus.weightless_equipment = percent >= 100;
    grace_test::config::persist_current_feature_state();
}

void handle_set_easier_parry_callback(int payload) {
    const int percent = std::clamp(payload, 0, 100);
    ERD::Main::g_FeatureStatus.easier_parry_percent = percent;
    ERD::Main::g_FeatureStatus.easier_guard_percent = percent;  // 兼容旧实现字段。
    grace_test::config::persist_current_feature_state();
}

void handle_set_weapon_requirement_reduction_callback(int payload) {
    const int percent = std::clamp(payload, 0, 100);
    ERD::Main::g_FeatureStatus.weapon_requirement_reduction_percent = percent;
    grace_test::config::persist_current_feature_state();
}

template <std::size_t N>
void rebuild_toggle_menu(
    const ToggleDescriptor (&descriptors)[N],
    std::array<std::unique_ptr<callback_state>, N>& states,
    talkscript_menu_state& menu_state,
    talkscript_menu_state& back_state) {
    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < N; ++index) {
        const ToggleDescriptor& descriptor = descriptors[index];
        set_message(
            descriptor.message_id,
            grace_test::localization::make_toggle_label(descriptor.label_key, toggle_state(descriptor.action)));

        if (!states[index]) {
            states[index] = std::make_unique<callback_state>(
                &handle_toggle_callback,
                static_cast<int>(descriptor.action),
                &menu_state);
        } else {
            states[index]->set_return_state(&menu_state);
        }

        options.emplace_back(descriptor.menu_index, descriptor.message_id, states[index].get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &back_state, true);
    menu_state.rebuild(std::move(options));
}

template <std::size_t N>
void rebuild_percent_menu(
    const std::array<int, N>& steps,
    int message_base_id,
    std::array<std::unique_ptr<callback_state>, N>& states,
    callback_state::Callback callback,
    talkscript_menu_state& menu_state,
    talkscript_menu_state& back_state) {
    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < N; ++index) {
        const int percent = steps[index];
        set_message(
            message_base_id + static_cast<int>(index),
            grace_test::localization::make_percent_option_label(percent));

        if (!states[index]) {
            states[index] = std::make_unique<callback_state>(callback, percent, &menu_state);
        } else {
            states[index]->set_return_state(&menu_state);
        }

        options.emplace_back(
            static_cast<int>(index + 1),
            message_base_id + static_cast<int>(index),
            states[index].get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &back_state, true);
    menu_state.rebuild(std::move(options));
}

void rebuild_root_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_root, grace_test::localization::get("root"));
    set_message(msg::event_text_for_talk_player_mods, grace_test::localization::get("player_mods"));
    set_message(msg::event_text_for_talk_game_mods, grace_test::localization::get("game_mods"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    options.emplace_back(10, msg::event_text_for_talk_player_mods, &g_player_mods_menu_state);
    options.emplace_back(11, msg::event_text_for_talk_game_mods, &g_game_mods_menu_state);
    options.emplace_back(99, msg::event_text_for_talk_back, g_grace_initial_state, true);
    g_root_menu_state.rebuild(std::move(options));
}

void rebuild_player_mods_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_player_mods, grace_test::localization::get("player_mods"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    set_message(
        msg::event_text_for_talk_equipment_weight_reduction,
        grace_test::localization::make_percent_setting_label(
            "setting_equipment_weight_reduction",
            current_equipment_reduction_percent()));
    set_message(
        msg::event_text_for_talk_easier_parry,
        grace_test::localization::make_percent_setting_label(
            "setting_easier_parry",
            current_easier_parry_percent()));
    set_message(
        msg::event_text_for_talk_weapon_requirement_reduction,
        grace_test::localization::make_percent_setting_label(
            "setting_weapon_requirement_reduction",
            current_weapon_requirement_reduction_percent()));

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_equipment_weight_reduction, &g_equipment_reduction_menu_state);
    options.emplace_back(2, msg::event_text_for_talk_weapon_requirement_reduction, &g_weapon_requirement_reduction_menu_state);

    for (std::size_t index = 0; index < std::size(kPlayerToggleDescriptors); ++index) {
        const ToggleDescriptor& descriptor = kPlayerToggleDescriptors[index];
        set_message(
            descriptor.message_id,
            grace_test::localization::make_toggle_label(descriptor.label_key, toggle_state(descriptor.action)));

        if (!g_player_toggle_states[index]) {
            g_player_toggle_states[index] = std::make_unique<callback_state>(
                &handle_toggle_callback,
                static_cast<int>(descriptor.action),
                &g_player_mods_menu_state);
        } else {
            g_player_toggle_states[index]->set_return_state(&g_player_mods_menu_state);
        }

        options.emplace_back(descriptor.menu_index, descriptor.message_id, g_player_toggle_states[index].get());
    }

    options.emplace_back(7, msg::event_text_for_talk_easier_parry, &g_easier_parry_menu_state);
    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_player_mods_menu_state.rebuild(std::move(options));
}

void rebuild_game_mods_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_game_mods, grace_test::localization::get("game_mods"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_toggle_menu(kGameToggleDescriptors, g_game_toggle_states, g_game_mods_menu_state, g_root_menu_state);
}

void rebuild_equipment_reduction_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kPercentSteps,
        msg::event_text_for_talk_equipment_weight_reduction_base,
        g_equipment_reduction_states,
        &handle_set_equipment_reduction_callback,
        g_equipment_reduction_menu_state,
        g_player_mods_menu_state);
}

void rebuild_weapon_requirement_reduction_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kWeaponRequirementPercentSteps,
        msg::event_text_for_talk_weapon_requirement_reduction_base,
        g_weapon_requirement_reduction_states,
        &handle_set_weapon_requirement_reduction_callback,
        g_weapon_requirement_reduction_menu_state,
        g_player_mods_menu_state);
}

void rebuild_easier_parry_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kPercentSteps,
        msg::event_text_for_talk_easier_parry_base,
        g_easier_parry_states,
        &handle_set_easier_parry_callback,
        g_easier_parry_menu_state,
        g_player_mods_menu_state);
}

void rebuild_all_menus() {
    rebuild_root_menu();
    rebuild_player_mods_menu();
    rebuild_game_mods_menu();
    rebuild_equipment_reduction_menu();
    rebuild_weapon_requirement_reduction_menu();
    rebuild_easier_parry_menu();
}

callback_state* find_callback_state(er::ezstate::state* state) {
    for (auto& callback : g_player_toggle_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_game_toggle_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_equipment_reduction_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_weapon_requirement_reduction_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_easier_parry_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    return nullptr;
}

bool is_sort_chest_event(er::ezstate::event& event) {
    if (event.command == er::talk_command::add_talk_list_data) {
        return get_ezstate_int_value(event.args[1]) == msg::event_text_for_talk_sort_chest;
    }
    if (event.command == er::talk_command::add_talk_list_data_if ||
        event.command == er::talk_command::add_talk_list_data_alt) {
        return get_ezstate_int_value(event.args[2]) == msg::event_text_for_talk_sort_chest;
    }
    return false;
}

bool is_grace_state_group(er::ezstate::state_group* state_group) {
    for (auto& state : state_group->states) {
        for (auto& event : state.entry_events) {
            if (is_sort_chest_event(event)) {
                return true;
            }
        }
    }
    return false;
}

bool is_sort_chest_transition(const er::ezstate::transition* transition) {
    auto* target_state = transition->target_state;
    return target_state != nullptr &&
           !target_state->entry_events.empty() &&
           target_state->entry_events[0].command == er::talk_command::open_repository;
}

bool patch_state_group(er::ezstate::state_group* state_group) {
    er::ezstate::state* add_menu_state = nullptr;
    er::ezstate::state* menu_transition_state = nullptr;
    int transition_index = -1;

    for (auto& state : state_group->states) {
        for (auto& event : state.entry_events) {
            if (is_sort_chest_event(event)) {
                add_menu_state = &state;
            } else if (event.command == er::talk_command::add_talk_list_data &&
                       get_ezstate_int_value(event.args[1]) == msg::event_text_for_talk_root) {
                return false;
            }
        }

        for (int index = 0; index < state.transitions.size(); ++index) {
            if (is_sort_chest_transition(state.transitions[index])) {
                menu_transition_state = &state;
                transition_index = index;
                break;
            }
        }
    }

    if (add_menu_state == nullptr || menu_transition_state == nullptr || transition_index < 0) {
        return false;
    }

    auto& events = add_menu_state->entry_events;
    std::copy(events.begin(), events.end(), g_patched_events.begin());
    g_patched_events[events.size()] = {er::talk_command::add_talk_list_data, g_top_level_option.args};
    events = {g_patched_events.data(), events.size() + 1};

    auto& transitions = menu_transition_state->transitions;
    std::copy(transitions.begin(), transitions.begin() + transition_index + 1, g_patched_transitions.begin());
    g_patched_transitions[transition_index + 1] = &g_top_level_option.transition;
    std::copy(
        transitions.begin() + transition_index + 1,
        transitions.end(),
        g_patched_transitions.begin() + transition_index + 2);
    transitions = {g_patched_transitions.data(), transitions.size() + 1};

    SPDLOG_INFO("Patched Site of Grace talkscript with erdGameTools menu.");
    return true;
}

void configure_grace_menu(er::ezstate::state_group* state_group) {
    if (state_group == nullptr) {
        return;
    }

    g_grace_initial_state = state_group->initial_state;
    rebuild_all_menus();
    patch_state_group(state_group);
}

static void (*ezstate_enter_state)(er::ezstate::state* state,
                                   er::ezstate::machine* machine,
                                   void* unk);

static void ezstate_enter_state_detour(er::ezstate::state* state,
                                       er::ezstate::machine* machine,
                                       void* unk) {
    if (machine != nullptr && machine->state_group != nullptr && state != nullptr &&
        is_grace_state_group(machine->state_group)) {
        if (state == machine->state_group->initial_state) {
            configure_grace_menu(machine->state_group);
        } else if (state == &g_root_menu_state) {
            rebuild_root_menu();
        } else if (state == &g_player_mods_menu_state) {
            rebuild_player_mods_menu();
        } else if (state == &g_game_mods_menu_state) {
            rebuild_game_mods_menu();
        } else if (state == &g_equipment_reduction_menu_state) {
            rebuild_equipment_reduction_menu();
        } else if (state == &g_weapon_requirement_reduction_menu_state) {
            rebuild_weapon_requirement_reduction_menu();
        } else if (state == &g_easier_parry_menu_state) {
            rebuild_easier_parry_menu();
        } else if (callback_state* callback = find_callback_state(state)) {
            callback->execute();
        }
    }

    ezstate_enter_state(state, machine, unk);
}

}  // namespace

void initialize() {
    modutils::hook(
        {
            .aob = "80 7e 18 00"
                   "74 15"
                   "4c 8d 44 24 40"
                   "48 8b d6"
                   "48 8b 4e 20"
                   "e8 ?? ?? ?? ??",
            .offset = 18,
            .relative_offsets = {{1, 5}},
        },
        ezstate_enter_state_detour,
        ezstate_enter_state);

    SPDLOG_INFO("Hooked EzState::EnterState for erdGameTools.");
}

const wchar_t* try_get_custom_message(int msg_id) {
    const auto found = g_message_cache.find(msg_id);
    return found == g_message_cache.end() ? nullptr : found->second.c_str();
}

}  // namespace grace_test::talkscript
