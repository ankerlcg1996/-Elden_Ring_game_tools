#include "grace_test_talkscript.hpp"

#include "grace_test_config.hpp"
#include "grace_test_localization.hpp"
#include "grace_test_messages.hpp"
#include "utils/talkscript_utils.hpp"

#include "Features/InspectorTools.hpp"
#include "Main/FeatureStatus.hpp"

#include <elden-x/ezstate/ezstate.hpp>
#include <elden-x/ezstate/talk_commands.hpp>
#include <elden-x/utils/modutils.hpp>

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

namespace grace_test::talkscript {
namespace {

enum class ToggleAction {
    WeightlessEquipment = 0,
    InfiniteItems,
    NoDamage,
    NoStamina,
    FreePurchase,
    NoCraftMaterialCost,
    NoUpgradeMaterialCost,
    NoMagicRequirements,
    AllMagicOneSlot,
    MountAnywhere,
    SpiritAshesAnywhere,
    PostureBars,
    DamagePopups,
    UnlockMaps,
    UnlockCookbooks,
    UnlockWhetblades,
    UnlockGraces,
    UnlockSummoningPools,
    UnlockColosseums,
    UnlockGestures,
};

struct ToggleDescriptor {
    int menu_index = 0;
    int message_id = 0;
    const char* label_key = "";
    ToggleAction action = ToggleAction::WeightlessEquipment;
};

constexpr ToggleDescriptor kRootToggleDescriptors[] = {
    {1, msg::event_text_for_talk_toggle_posture_root, "toggle_posture_bars", ToggleAction::PostureBars},
    {2, msg::event_text_for_talk_toggle_damage_root, "toggle_damage_popups", ToggleAction::DamagePopups},
};

constexpr ToggleDescriptor kGameToggleDescriptors[] = {
    {1, msg::event_text_for_talk_toggle_weightless, "toggle_weightless_equipment", ToggleAction::WeightlessEquipment},
    {2, msg::event_text_for_talk_toggle_no_item_consume, "toggle_no_item_consume", ToggleAction::InfiniteItems},
    {3, msg::event_text_for_talk_toggle_no_damage, "toggle_no_damage", ToggleAction::NoDamage},
    {4, msg::event_text_for_talk_toggle_no_stamina, "toggle_no_stamina", ToggleAction::NoStamina},
    {5, msg::event_text_for_talk_toggle_free_purchase, "toggle_free_purchase", ToggleAction::FreePurchase},
    {6, msg::event_text_for_talk_toggle_no_craft_cost, "toggle_no_craft_cost", ToggleAction::NoCraftMaterialCost},
    {7, msg::event_text_for_talk_toggle_no_upgrade_cost, "toggle_no_upgrade_cost", ToggleAction::NoUpgradeMaterialCost},
    {8, msg::event_text_for_talk_toggle_no_magic_req, "toggle_no_magic_req", ToggleAction::NoMagicRequirements},
    {9, msg::event_text_for_talk_toggle_one_slot_magic, "toggle_one_slot_magic", ToggleAction::AllMagicOneSlot},
    {10, msg::event_text_for_talk_toggle_mount_anywhere, "toggle_mount_anywhere", ToggleAction::MountAnywhere},
    {11, msg::event_text_for_talk_toggle_spirit_anywhere, "toggle_spirit_anywhere", ToggleAction::SpiritAshesAnywhere},
};

constexpr ToggleDescriptor kUnlockToggleDescriptors[] = {
    {1, msg::event_text_for_talk_unlock_maps, "unlock_maps", ToggleAction::UnlockMaps},
    {2, msg::event_text_for_talk_unlock_cookbooks, "unlock_cookbooks", ToggleAction::UnlockCookbooks},
    {3, msg::event_text_for_talk_unlock_whetblades, "unlock_whetblades", ToggleAction::UnlockWhetblades},
    {4, msg::event_text_for_talk_unlock_graces, "unlock_graces", ToggleAction::UnlockGraces},
    {5, msg::event_text_for_talk_unlock_summoning_pools, "unlock_summoning_pools", ToggleAction::UnlockSummoningPools},
    {6, msg::event_text_for_talk_unlock_colosseums, "unlock_colosseums", ToggleAction::UnlockColosseums},
    {7, msg::event_text_for_talk_unlock_gestures, "unlock_gestures", ToggleAction::UnlockGestures},
};

talkscript_menu_state g_root_menu_state;
talkscript_menu_state g_game_mods_menu_state;
talkscript_menu_state g_unlocks_menu_state;
talkscript_menu_state g_quick_access_menu_state;
talkscript_menu_option g_top_level_option{70, msg::event_text_for_talk_root, &g_root_menu_state};

std::array<std::unique_ptr<callback_state>, std::size(kRootToggleDescriptors)> g_root_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kGameToggleDescriptors)> g_game_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kUnlockToggleDescriptors)> g_unlock_toggle_states{};
std::vector<std::unique_ptr<callback_state>> g_quick_access_states;
std::unique_ptr<callback_state> g_no_entries_state;

std::unordered_map<int, std::wstring> g_message_cache;

er::ezstate::state* g_grace_initial_state = nullptr;
static auto g_patched_events = std::array<er::ezstate::event, 128>{};
static auto g_patched_transitions = std::array<er::ezstate::transition*, 128>{};

std::wstring utf8_to_wide(const std::string& value) {
    if (value.empty()) {
        return {};
    }

    const int required =
        MultiByteToWideChar(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0);
    if (required <= 0) {
        return {};
    }

    std::wstring wide(static_cast<std::size_t>(required), L'\0');
    MultiByteToWideChar(
        CP_UTF8,
        0,
        value.c_str(),
        static_cast<int>(value.size()),
        wide.data(),
        required);
    return wide;
}

void set_message(int message_id, std::wstring text) {
    g_message_cache[message_id] = std::move(text);
}

bool toggle_state(ToggleAction action) {
    switch (action) {
    case ToggleAction::WeightlessEquipment:
        return ERD::Main::g_FeatureStatus.weightless_equipment.load();
    case ToggleAction::InfiniteItems:
        return ERD::Main::g_FeatureStatus.infinite_items.load();
    case ToggleAction::NoDamage:
        return ERD::Main::g_FeatureStatus.no_damage.load();
    case ToggleAction::NoStamina:
        return ERD::Main::g_FeatureStatus.no_stamina_consumption.load();
    case ToggleAction::FreePurchase:
        return ERD::Main::g_FeatureStatus.free_purchase.load();
    case ToggleAction::NoCraftMaterialCost:
        return ERD::Main::g_FeatureStatus.no_crafting_material_cost.load();
    case ToggleAction::NoUpgradeMaterialCost:
        return ERD::Main::g_FeatureStatus.no_upgrade_material_cost.load();
    case ToggleAction::NoMagicRequirements:
        return ERD::Main::g_FeatureStatus.no_magic_requirements.load();
    case ToggleAction::AllMagicOneSlot:
        return ERD::Main::g_FeatureStatus.all_magic_one_slot.load();
    case ToggleAction::MountAnywhere:
        return ERD::Main::g_FeatureStatus.mount_anywhere.load();
    case ToggleAction::SpiritAshesAnywhere:
        return ERD::Main::g_FeatureStatus.spirit_ashes_anywhere.load();
    case ToggleAction::PostureBars:
        return ERD::Main::g_FeatureStatus.posture_bar_overlay_enabled.load();
    case ToggleAction::DamagePopups:
        return ERD::Main::g_FeatureStatus.damage_popup_overlay_enabled.load();
    case ToggleAction::UnlockMaps:
        return ERD::Main::g_FeatureStatus.unlock_all_maps.load();
    case ToggleAction::UnlockCookbooks:
        return ERD::Main::g_FeatureStatus.unlock_all_cookbooks.load();
    case ToggleAction::UnlockWhetblades:
        return ERD::Main::g_FeatureStatus.unlock_all_whetblades.load();
    case ToggleAction::UnlockGraces:
        return ERD::Main::g_FeatureStatus.unlock_all_graces.load();
    case ToggleAction::UnlockSummoningPools:
        return ERD::Main::g_FeatureStatus.unlock_all_summoning_pools.load();
    case ToggleAction::UnlockColosseums:
        return ERD::Main::g_FeatureStatus.unlock_all_colosseums.load();
    case ToggleAction::UnlockGestures:
        return ERD::Main::g_FeatureStatus.unlock_all_gestures.load();
    }
    return false;
}

void toggle_action(ToggleAction action) {
    switch (action) {
    case ToggleAction::WeightlessEquipment:
        ERD::Main::g_FeatureStatus.weightless_equipment =
            !ERD::Main::g_FeatureStatus.weightless_equipment.load();
        break;
    case ToggleAction::InfiniteItems:
        ERD::Main::g_FeatureStatus.infinite_items =
            !ERD::Main::g_FeatureStatus.infinite_items.load();
        break;
    case ToggleAction::NoDamage:
        ERD::Main::g_FeatureStatus.no_damage =
            !ERD::Main::g_FeatureStatus.no_damage.load();
        break;
    case ToggleAction::NoStamina:
        ERD::Main::g_FeatureStatus.no_stamina_consumption =
            !ERD::Main::g_FeatureStatus.no_stamina_consumption.load();
        break;
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
    case ToggleAction::NoMagicRequirements:
        ERD::Main::g_FeatureStatus.no_magic_requirements =
            !ERD::Main::g_FeatureStatus.no_magic_requirements.load();
        break;
    case ToggleAction::AllMagicOneSlot:
        ERD::Main::g_FeatureStatus.all_magic_one_slot =
            !ERD::Main::g_FeatureStatus.all_magic_one_slot.load();
        break;
    case ToggleAction::MountAnywhere:
        ERD::Main::g_FeatureStatus.mount_anywhere =
            !ERD::Main::g_FeatureStatus.mount_anywhere.load();
        break;
    case ToggleAction::SpiritAshesAnywhere:
        ERD::Main::g_FeatureStatus.spirit_ashes_anywhere =
            !ERD::Main::g_FeatureStatus.spirit_ashes_anywhere.load();
        break;
    case ToggleAction::PostureBars:
        ERD::Main::g_FeatureStatus.posture_bar_overlay_enabled =
            !ERD::Main::g_FeatureStatus.posture_bar_overlay_enabled.load();
        break;
    case ToggleAction::DamagePopups:
        ERD::Main::g_FeatureStatus.damage_popup_overlay_enabled =
            !ERD::Main::g_FeatureStatus.damage_popup_overlay_enabled.load();
        break;
    case ToggleAction::UnlockMaps:
        ERD::Main::g_FeatureStatus.unlock_all_maps =
            !ERD::Main::g_FeatureStatus.unlock_all_maps.load();
        break;
    case ToggleAction::UnlockCookbooks:
        ERD::Main::g_FeatureStatus.unlock_all_cookbooks =
            !ERD::Main::g_FeatureStatus.unlock_all_cookbooks.load();
        break;
    case ToggleAction::UnlockWhetblades:
        ERD::Main::g_FeatureStatus.unlock_all_whetblades =
            !ERD::Main::g_FeatureStatus.unlock_all_whetblades.load();
        break;
    case ToggleAction::UnlockGraces:
        ERD::Main::g_FeatureStatus.unlock_all_graces =
            !ERD::Main::g_FeatureStatus.unlock_all_graces.load();
        break;
    case ToggleAction::UnlockSummoningPools:
        ERD::Main::g_FeatureStatus.unlock_all_summoning_pools =
            !ERD::Main::g_FeatureStatus.unlock_all_summoning_pools.load();
        break;
    case ToggleAction::UnlockColosseums:
        ERD::Main::g_FeatureStatus.unlock_all_colosseums =
            !ERD::Main::g_FeatureStatus.unlock_all_colosseums.load();
        break;
    case ToggleAction::UnlockGestures:
        ERD::Main::g_FeatureStatus.unlock_all_gestures =
            !ERD::Main::g_FeatureStatus.unlock_all_gestures.load();
        break;
    }
}

void handle_toggle_callback(int payload) {
    toggle_action(static_cast<ToggleAction>(payload));
    grace_test::config::persist_current_feature_state();
}

void handle_quick_access_callback(int payload) {
    ERD::Main::g_FeatureStatus.npc_menu_request_id = payload;
}

void handle_noop_callback(int) {}

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

void rebuild_root_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_root, grace_test::localization::get("root"));
    set_message(msg::event_text_for_talk_game_mods, grace_test::localization::get("game_mods"));
    set_message(msg::event_text_for_talk_unlocks, grace_test::localization::get("unlocks"));
    set_message(msg::event_text_for_talk_quick_access, grace_test::localization::get("quick_access"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < std::size(kRootToggleDescriptors); ++index) {
        const ToggleDescriptor& descriptor = kRootToggleDescriptors[index];
        set_message(
            descriptor.message_id,
            grace_test::localization::make_toggle_label(descriptor.label_key, toggle_state(descriptor.action)));

        if (!g_root_toggle_states[index]) {
            g_root_toggle_states[index] = std::make_unique<callback_state>(
                &handle_toggle_callback,
                static_cast<int>(descriptor.action),
                &g_root_menu_state);
        } else {
            g_root_toggle_states[index]->set_return_state(&g_root_menu_state);
        }

        options.emplace_back(descriptor.menu_index, descriptor.message_id, g_root_toggle_states[index].get());
    }
    options.emplace_back(10, msg::event_text_for_talk_game_mods, &g_game_mods_menu_state);
    options.emplace_back(11, msg::event_text_for_talk_unlocks, &g_unlocks_menu_state);
    options.emplace_back(12, msg::event_text_for_talk_quick_access, &g_quick_access_menu_state);
    options.emplace_back(99, msg::event_text_for_talk_back, g_grace_initial_state, true);
    g_root_menu_state.rebuild(std::move(options));
}

void rebuild_game_mods_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_game_mods, grace_test::localization::get("game_mods"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_toggle_menu(kGameToggleDescriptors, g_game_toggle_states, g_game_mods_menu_state, g_root_menu_state);
}

void rebuild_unlocks_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_unlocks, grace_test::localization::get("unlocks"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_toggle_menu(kUnlockToggleDescriptors, g_unlock_toggle_states, g_unlocks_menu_state, g_root_menu_state);
}

void rebuild_quick_access_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_quick_access, grace_test::localization::get("quick_access"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    set_message(msg::event_text_for_talk_no_entries, grace_test::localization::get("no_entries"));

    const ERD::Features::InspectorSnapshot snapshot = ERD::Features::GetInspectorSnapshot();

    g_quick_access_states.clear();
    std::vector<talkscript_menu_option> options;
    int menu_index = 1;

    for (const auto& entry : snapshot.npc_menus) {
        const int message_id = msg::event_text_for_talk_quick_access_base + entry.id;
        std::wstring label = utf8_to_wide(entry.label);
        const std::wstring category = utf8_to_wide(entry.category);
        if (!category.empty()) {
            label = category + L" / " + label;
        }
        if (label.empty()) {
            continue;
        }

        set_message(message_id, label);
        auto action_state = std::make_unique<callback_state>(
            &handle_quick_access_callback,
            entry.id,
            g_grace_initial_state);
        options.emplace_back(menu_index++, message_id, action_state.get());
        g_quick_access_states.push_back(std::move(action_state));
    }

    if (options.empty()) {
        if (!g_no_entries_state) {
            g_no_entries_state =
                std::make_unique<callback_state>(&handle_noop_callback, 0, &g_quick_access_menu_state);
        } else {
            g_no_entries_state->set_return_state(&g_quick_access_menu_state);
        }
        options.emplace_back(1, msg::event_text_for_talk_no_entries, g_no_entries_state.get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_quick_access_menu_state.rebuild(std::move(options));
}

void rebuild_all_menus() {
    rebuild_root_menu();
    rebuild_game_mods_menu();
    rebuild_unlocks_menu();
    rebuild_quick_access_menu();
}

callback_state* find_callback_state(er::ezstate::state* state) {
    for (auto& callback : g_root_toggle_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_game_toggle_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_unlock_toggle_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_quick_access_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    if (g_no_entries_state && g_no_entries_state.get() == state) {
        return g_no_entries_state.get();
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
        } else if (state == &g_game_mods_menu_state) {
            rebuild_game_mods_menu();
        } else if (state == &g_unlocks_menu_state) {
            rebuild_unlocks_menu();
        } else if (state == &g_quick_access_menu_state) {
            rebuild_quick_access_menu();
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
