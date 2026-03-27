#include "grace_test_talkscript.hpp"

#include "grace_test_config.hpp"
#include "grace_test_localization.hpp"
#include "grace_test_messages.hpp"
#include "grace_test_runtime.hpp"
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
    AllWeaponsAshOfWarChangeable,
    NoMagicRequirements,
    AllMagicOneSlot,
    BeastNotHostile,
    SilentFootsteps,
    InfiniteConsumables,
    InfiniteArrows,
    InfiniteStamina,
    InfiniteFp,
    NoRuneLossOnDeath,
    InfiniteJump,
    SpiritAshesAnywhere,
    TorrentNoDeath,
    TorrentAnywhere,
};

enum class UnlockAction {
    Whetblades = 0,
    Gestures = 1,
};

enum class UnlockMapsAction {
    All = 0,
    BaseGame = 1,
    Dlc = 2,
};

enum class PortableGraceAction {
    Upgrade = 0,
    Rebirth = 1,
    Mirror = 2,
};

enum class BossReviveAction {
    Revive = 0,
    Warp = 1,
};

struct ToggleDescriptor {
    int menu_index = 0;
    int message_id = 0;
    const char* label_key = "";
    ToggleAction action = ToggleAction::FreePurchase;
};

constexpr std::array<int, 11> kPercentSteps{0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
constexpr std::array<int, 6> kWeaponRequirementPercentSteps{0, 20, 40, 60, 80, 100};
constexpr std::array<int, 21> kPlayerSpeedPercentSteps{
    0,   10,  20,  30,  40,  50,  60,  70,  80,  90,  100,
    110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
constexpr std::array<int, 7> kBuffDurationExtendSteps{{0, 50, 100, 200, 300, 500, 99999}};
constexpr std::array<int, 27> kEnemyHpPercentSteps{
    0,   20,  40,  60,  80,  100, 120, 140, 160, 180, 200, 250, 300, 350,
    400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000};
constexpr std::array<int, 16> kDamageMultiplierPercentSteps{{0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 150, 200, 300, 400, 500}};

constexpr ToggleDescriptor kPlayerToggleDescriptors[] = {
    {9, msg::event_text_for_talk_toggle_no_magic_req, "toggle_no_magic_req", ToggleAction::NoMagicRequirements},
    {10, msg::event_text_for_talk_toggle_one_slot_magic, "toggle_one_slot_magic", ToggleAction::AllMagicOneSlot},
    {11, msg::event_text_for_talk_toggle_infinite_consumables, "toggle_infinite_consumables", ToggleAction::InfiniteConsumables},
    {12, msg::event_text_for_talk_toggle_infinite_arrows, "toggle_infinite_arrows", ToggleAction::InfiniteArrows},
    {13, msg::event_text_for_talk_toggle_infinite_stamina, "toggle_infinite_stamina", ToggleAction::InfiniteStamina},
    {14, msg::event_text_for_talk_toggle_infinite_fp, "toggle_infinite_fp", ToggleAction::InfiniteFp},
    {15, msg::event_text_for_talk_toggle_no_rune_loss_on_death, "toggle_no_rune_loss_on_death", ToggleAction::NoRuneLossOnDeath},
};

constexpr ToggleDescriptor kGameToggleDescriptors[] = {
    {1, msg::event_text_for_talk_toggle_free_purchase, "toggle_free_purchase", ToggleAction::FreePurchase},
    {2, msg::event_text_for_talk_toggle_no_craft_cost, "toggle_no_craft_cost", ToggleAction::NoCraftMaterialCost},
    {3, msg::event_text_for_talk_toggle_no_upgrade_cost, "toggle_no_upgrade_cost", ToggleAction::NoUpgradeMaterialCost},
    {4, msg::event_text_for_talk_toggle_all_weapons_enchantable, "toggle_all_weapons_enchantable", ToggleAction::AllWeaponsEnchantable},
    {5, msg::event_text_for_talk_toggle_spirit_anywhere, "toggle_spirit_anywhere", ToggleAction::SpiritAshesAnywhere},
    {6, msg::event_text_for_talk_toggle_torrent_no_death, "toggle_torrent_no_death", ToggleAction::TorrentNoDeath},
    {7, msg::event_text_for_talk_toggle_torrent_anywhere, "toggle_torrent_anywhere", ToggleAction::TorrentAnywhere},
    {8, msg::event_text_for_talk_toggle_all_weapons_ash_of_war_changeable, "toggle_all_weapons_ash_of_war_changeable", ToggleAction::AllWeaponsAshOfWarChangeable},
};

talkscript_menu_state g_root_menu_state;
talkscript_menu_state g_player_mods_menu_state;
talkscript_menu_state g_game_mods_menu_state;
talkscript_menu_state g_enemy_mods_menu_state;
talkscript_menu_state g_enemy_hp_increase_menu_state;
talkscript_menu_state g_enemy_hp_decrease_menu_state;
talkscript_menu_state g_boss_revive_menu_state;
talkscript_menu_state g_boss_revive_action_menu_state;
talkscript_menu_state g_unlocks_menu_state;
talkscript_menu_state g_unlock_maps_menu_state;
talkscript_menu_state g_portable_grace_menu_state;
talkscript_menu_state g_equipment_reduction_menu_state;
talkscript_menu_state g_weapon_requirement_reduction_menu_state;
talkscript_menu_state g_player_speed_menu_state;
talkscript_menu_state g_buff_duration_extend_menu_state;
talkscript_menu_state g_easier_parry_menu_state;
talkscript_menu_state g_damage_multiplier_up_menu_state;
talkscript_menu_state g_damage_multiplier_cut_menu_state;
talkscript_menu_state g_damage_multiplier_reduce_menu_state;
talkscript_menu_state g_damage_multiplier_taken_menu_state;
talkscript_menu_option g_top_level_option{70, msg::event_text_for_talk_root, &g_root_menu_state};

std::array<std::unique_ptr<callback_state>, std::size(kPlayerToggleDescriptors)> g_player_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kGameToggleDescriptors)> g_game_toggle_states{};
std::array<std::unique_ptr<callback_state>, 2> g_unlock_states{};
std::array<std::unique_ptr<callback_state>, 3> g_unlock_map_states{};
std::array<std::unique_ptr<callback_state>, 3> g_portable_grace_states{};
std::array<std::unique_ptr<callback_state>, std::size(kPercentSteps)> g_equipment_reduction_states{};
std::array<std::unique_ptr<callback_state>, std::size(kWeaponRequirementPercentSteps)> g_weapon_requirement_reduction_states{};
std::array<std::unique_ptr<callback_state>, std::size(kPlayerSpeedPercentSteps)> g_player_speed_states{};
std::array<std::unique_ptr<callback_state>, std::size(kBuffDurationExtendSteps)> g_buff_duration_extend_states{};
std::array<std::unique_ptr<callback_state>, std::size(kEnemyHpPercentSteps)> g_enemy_hp_increase_states{};
std::array<std::unique_ptr<callback_state>, std::size(kEnemyHpPercentSteps)> g_enemy_hp_decrease_states{};
std::array<std::unique_ptr<callback_state>, std::size(kPercentSteps)> g_easier_parry_states{};
std::array<std::unique_ptr<callback_state>, std::size(kDamageMultiplierPercentSteps)> g_damage_multiplier_up_states{};
std::array<std::unique_ptr<callback_state>, std::size(kDamageMultiplierPercentSteps)> g_damage_multiplier_cut_states{};
std::array<std::unique_ptr<callback_state>, std::size(kDamageMultiplierPercentSteps)> g_damage_multiplier_reduce_states{};
std::array<std::unique_ptr<callback_state>, std::size(kDamageMultiplierPercentSteps)> g_damage_multiplier_taken_states{};
std::vector<std::unique_ptr<talkscript_menu_state>> g_boss_revive_area_menu_states{};
std::vector<std::unique_ptr<callback_state>> g_boss_revive_callback_states{};
std::array<std::unique_ptr<callback_state>, 2> g_boss_revive_action_states{};
std::unordered_map<int, talkscript_menu_state*> g_boss_revive_parent_menu_by_id{};
std::unordered_map<int, std::string> g_boss_revive_name_by_id{};
int g_selected_boss_revive_menu_id = -1;
talkscript_menu_state* g_selected_boss_parent_menu = &g_boss_revive_menu_state;

constexpr int kBossReviveAreaMessageBase = 69012000;
constexpr int kBossReviveBossMessageBase = 69013000;

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

int current_player_speed_increase_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.player_speed_increase_percent.load(), 0, 200);
}

int current_buff_duration_extend_mode() {
    const int mode = ERD::Main::g_FeatureStatus.buff_duration_extend_mode.load();
    switch (mode) {
    case 0:
    case 50:
    case 100:
    case 200:
    case 300:
    case 500:
    case 99999:
        return mode;
    default:
        return 0;
    }
}

int current_enemy_hp_increase_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.enemy_hp_increase_percent.load(), 0, 1000);
}

int current_enemy_hp_decrease_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent.load(), 0, 1000);
}

int current_damage_multiplier_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.damage_multiplier_percent.load(), 0, 500);
}

int current_damage_cut_multiplier_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.damage_cut_multiplier_percent.load(), 0, 500);
}

int current_damage_reduce_multiplier_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.damage_reduce_multiplier_percent.load(), 0, 500);
}

int current_damage_taken_multiplier_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.damage_taken_multiplier_percent.load(), 0, 500);
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
    case ToggleAction::AllWeaponsAshOfWarChangeable:
        return ERD::Main::g_FeatureStatus.all_weapons_ash_of_war_changeable.load();
    case ToggleAction::NoMagicRequirements:
        return ERD::Main::g_FeatureStatus.no_magic_requirements.load();
    case ToggleAction::AllMagicOneSlot:
        return ERD::Main::g_FeatureStatus.all_magic_one_slot.load();
    case ToggleAction::BeastNotHostile:
        return ERD::Main::g_FeatureStatus.beast_not_hostile.load();
    case ToggleAction::SilentFootsteps:
        return ERD::Main::g_FeatureStatus.silent_footsteps.load();
    case ToggleAction::InfiniteConsumables:
        return ERD::Main::g_FeatureStatus.infinite_consumables.load();
    case ToggleAction::InfiniteArrows:
        return ERD::Main::g_FeatureStatus.infinite_arrows.load();
    case ToggleAction::InfiniteStamina:
        return ERD::Main::g_FeatureStatus.infinite_stamina.load();
    case ToggleAction::InfiniteFp:
        return ERD::Main::g_FeatureStatus.infinite_fp.load();
    case ToggleAction::NoRuneLossOnDeath:
        return ERD::Main::g_FeatureStatus.no_rune_loss_on_death.load();
    case ToggleAction::InfiniteJump:
        return ERD::Main::g_FeatureStatus.infinite_jump.load();
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
    case ToggleAction::AllWeaponsAshOfWarChangeable:
        ERD::Main::g_FeatureStatus.all_weapons_ash_of_war_changeable =
            !ERD::Main::g_FeatureStatus.all_weapons_ash_of_war_changeable.load();
        break;
    case ToggleAction::NoMagicRequirements:
        ERD::Main::g_FeatureStatus.no_magic_requirements =
            !ERD::Main::g_FeatureStatus.no_magic_requirements.load();
        break;
    case ToggleAction::AllMagicOneSlot:
        ERD::Main::g_FeatureStatus.all_magic_one_slot =
            !ERD::Main::g_FeatureStatus.all_magic_one_slot.load();
        break;
    case ToggleAction::BeastNotHostile:
        ERD::Main::g_FeatureStatus.beast_not_hostile =
            !ERD::Main::g_FeatureStatus.beast_not_hostile.load();
        break;
    case ToggleAction::SilentFootsteps:
        ERD::Main::g_FeatureStatus.silent_footsteps =
            !ERD::Main::g_FeatureStatus.silent_footsteps.load();
        break;
    case ToggleAction::InfiniteConsumables:
        ERD::Main::g_FeatureStatus.infinite_consumables =
            !ERD::Main::g_FeatureStatus.infinite_consumables.load();
        break;
    case ToggleAction::InfiniteArrows:
        ERD::Main::g_FeatureStatus.infinite_arrows =
            !ERD::Main::g_FeatureStatus.infinite_arrows.load();
        break;
    case ToggleAction::InfiniteStamina:
        ERD::Main::g_FeatureStatus.infinite_stamina =
            !ERD::Main::g_FeatureStatus.infinite_stamina.load();
        break;
    case ToggleAction::InfiniteFp:
        ERD::Main::g_FeatureStatus.infinite_fp =
            !ERD::Main::g_FeatureStatus.infinite_fp.load();
        break;
    case ToggleAction::NoRuneLossOnDeath:
        ERD::Main::g_FeatureStatus.no_rune_loss_on_death =
            !ERD::Main::g_FeatureStatus.no_rune_loss_on_death.load();
        break;
    case ToggleAction::InfiniteJump:
        ERD::Main::g_FeatureStatus.infinite_jump =
            !ERD::Main::g_FeatureStatus.infinite_jump.load();
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

void handle_unlock_callback(int payload) {
    switch (static_cast<UnlockAction>(payload)) {
    case UnlockAction::Whetblades:
        grace_test::runtime::unlock_all_whetblades();
        break;
    case UnlockAction::Gestures:
        grace_test::runtime::unlock_all_gestures();
        break;
    }
}

void handle_unlock_maps_callback(int payload) {
    switch (static_cast<UnlockMapsAction>(payload)) {
    case UnlockMapsAction::All:
        grace_test::runtime::unlock_maps(grace_test::runtime::UnlockMapsMode::All);
        break;
    case UnlockMapsAction::BaseGame:
        grace_test::runtime::unlock_maps(grace_test::runtime::UnlockMapsMode::BaseGame);
        break;
    case UnlockMapsAction::Dlc:
        grace_test::runtime::unlock_maps(grace_test::runtime::UnlockMapsMode::Dlc);
        break;
    }
}

void handle_portable_grace_callback(int payload) {
    switch (static_cast<PortableGraceAction>(payload)) {
    case PortableGraceAction::Upgrade:
        grace_test::runtime::open_portable_upgrade();
        break;
    case PortableGraceAction::Rebirth:
        grace_test::runtime::open_portable_rebirth();
        break;
    case PortableGraceAction::Mirror:
        grace_test::runtime::open_portable_mirror();
        break;
    }
}

void handle_select_boss_revive_callback(int payload) {
    g_selected_boss_revive_menu_id = payload;
    const auto found_parent = g_boss_revive_parent_menu_by_id.find(payload);
    g_selected_boss_parent_menu =
        (found_parent != g_boss_revive_parent_menu_by_id.end() && found_parent->second != nullptr)
            ? found_parent->second
            : &g_boss_revive_menu_state;
}

void handle_boss_revive_action_callback(int payload) {
    switch (static_cast<BossReviveAction>(payload)) {
    case BossReviveAction::Revive:
        grace_test::runtime::revive_boss_by_menu_id(g_selected_boss_revive_menu_id);
        break;
    case BossReviveAction::Warp:
        grace_test::runtime::warp_boss_by_menu_id(g_selected_boss_revive_menu_id);
        break;
    }
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

void handle_set_player_speed_increase_callback(int payload) {
    const int percent = std::clamp(payload, 0, 200);
    ERD::Main::g_FeatureStatus.player_speed_increase_percent = percent;
    ERD::Main::g_FeatureStatus.movement_speed = 1.0f + static_cast<float>(percent) / 100.0f;
    grace_test::config::persist_current_feature_state();
}

void handle_set_buff_duration_extend_callback(int payload) {
    int mode = 0;
    switch (payload) {
    case 0:
    case 50:
    case 100:
    case 200:
    case 300:
    case 500:
    case 99999:
        mode = payload;
        break;
    default:
        mode = 0;
        break;
    }
    ERD::Main::g_FeatureStatus.buff_duration_extend_mode = mode;
    grace_test::config::persist_current_feature_state();
}

void handle_set_enemy_hp_increase_callback(int payload) {
    const int percent = std::clamp(payload, 0, 1000);
    ERD::Main::g_FeatureStatus.enemy_hp_increase_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
}

void handle_set_enemy_hp_decrease_callback(int payload) {
    const int percent = std::clamp(payload, 0, 1000);
    ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.enemy_hp_increase_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
}

void handle_set_damage_multiplier_callback(int payload) {
    const int percent = std::clamp(payload, 0, 500);
    ERD::Main::g_FeatureStatus.damage_multiplier_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.damage_reduce_multiplier_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
}

void handle_set_damage_cut_multiplier_callback(int payload) {
    const int percent = std::clamp(payload, 0, 500);
    ERD::Main::g_FeatureStatus.damage_cut_multiplier_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.damage_taken_multiplier_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
}

void handle_set_damage_reduce_multiplier_callback(int payload) {
    const int percent = std::clamp(payload, 0, 500);
    ERD::Main::g_FeatureStatus.damage_reduce_multiplier_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.damage_multiplier_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
}

void handle_set_damage_taken_multiplier_callback(int payload) {
    const int percent = std::clamp(payload, 0, 500);
    ERD::Main::g_FeatureStatus.damage_taken_multiplier_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.damage_cut_multiplier_percent = 0;
    }
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
    set_message(msg::event_text_for_talk_enemy_mods, grace_test::localization::get("enemy_mods"));
    set_message(msg::event_text_for_talk_boss_revive, grace_test::localization::get("boss_revive"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    options.emplace_back(10, msg::event_text_for_talk_player_mods, &g_player_mods_menu_state);
    options.emplace_back(11, msg::event_text_for_talk_game_mods, &g_game_mods_menu_state);
    options.emplace_back(12, msg::event_text_for_talk_enemy_mods, &g_enemy_mods_menu_state);
    options.emplace_back(13, msg::event_text_for_talk_boss_revive, &g_boss_revive_menu_state);
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
        msg::event_text_for_talk_weapon_requirement_reduction,
        grace_test::localization::make_percent_setting_label(
            "setting_weapon_requirement_reduction",
            current_weapon_requirement_reduction_percent()));
    set_message(
        msg::event_text_for_talk_player_speed_increase,
        grace_test::localization::make_percent_setting_label(
            "setting_player_speed_increase",
            current_player_speed_increase_percent()));
    const int buff_extend_mode = current_buff_duration_extend_mode();
    if (buff_extend_mode == 99999) {
        set_message(
            msg::event_text_for_talk_buff_duration_extend,
            grace_test::localization::get("setting_buff_duration_extend") + L": " +
                grace_test::localization::get("option_permanent"));
    } else {
        set_message(
            msg::event_text_for_talk_buff_duration_extend,
            grace_test::localization::make_percent_setting_label(
                "setting_buff_duration_extend",
                buff_extend_mode));
    }
    set_message(
        msg::event_text_for_talk_damage_multiplier_up,
        grace_test::localization::make_percent_setting_label(
            "setting_damage_multiplier_up",
            current_damage_multiplier_percent()));
    set_message(
        msg::event_text_for_talk_damage_multiplier_cut,
        grace_test::localization::make_percent_setting_label(
            "setting_damage_multiplier_cut",
            current_damage_cut_multiplier_percent()));
    set_message(
        msg::event_text_for_talk_damage_multiplier_reduce,
        grace_test::localization::make_percent_setting_label(
            "setting_damage_multiplier_reduce",
            current_damage_reduce_multiplier_percent()));
    set_message(
        msg::event_text_for_talk_damage_multiplier_taken,
        grace_test::localization::make_percent_setting_label(
            "setting_damage_multiplier_taken",
            current_damage_taken_multiplier_percent()));

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_equipment_weight_reduction, &g_equipment_reduction_menu_state);
    options.emplace_back(2, msg::event_text_for_talk_weapon_requirement_reduction, &g_weapon_requirement_reduction_menu_state);
    options.emplace_back(3, msg::event_text_for_talk_player_speed_increase, &g_player_speed_menu_state);
    options.emplace_back(8, msg::event_text_for_talk_buff_duration_extend, &g_buff_duration_extend_menu_state);
    options.emplace_back(4, msg::event_text_for_talk_damage_multiplier_up, &g_damage_multiplier_up_menu_state);
    options.emplace_back(5, msg::event_text_for_talk_damage_multiplier_cut, &g_damage_multiplier_cut_menu_state);
    options.emplace_back(6, msg::event_text_for_talk_damage_multiplier_reduce, &g_damage_multiplier_reduce_menu_state);
    options.emplace_back(7, msg::event_text_for_talk_damage_multiplier_taken, &g_damage_multiplier_taken_menu_state);

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

    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_player_mods_menu_state.rebuild(std::move(options));
}

void rebuild_game_mods_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_game_mods, grace_test::localization::get("game_mods"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < std::size(kGameToggleDescriptors); ++index) {
        const ToggleDescriptor& descriptor = kGameToggleDescriptors[index];
        set_message(
            descriptor.message_id,
            grace_test::localization::make_toggle_label(descriptor.label_key, toggle_state(descriptor.action)));

        if (!g_game_toggle_states[index]) {
            g_game_toggle_states[index] = std::make_unique<callback_state>(
                &handle_toggle_callback,
                static_cast<int>(descriptor.action),
                &g_game_mods_menu_state);
        } else {
            g_game_toggle_states[index]->set_return_state(&g_game_mods_menu_state);
        }

        options.emplace_back(descriptor.menu_index, descriptor.message_id, g_game_toggle_states[index].get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_game_mods_menu_state.rebuild(std::move(options));
}

void rebuild_enemy_mods_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_enemy_mods, grace_test::localization::get("enemy_mods"));
    set_message(
        msg::event_text_for_talk_enemy_hp_increase,
        grace_test::localization::make_percent_setting_label(
            "setting_enemy_hp_increase",
            current_enemy_hp_increase_percent()));
    set_message(
        msg::event_text_for_talk_enemy_hp_decrease,
        grace_test::localization::make_percent_setting_label(
            "setting_enemy_hp_decrease",
            current_enemy_hp_decrease_percent()));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_enemy_hp_increase, &g_enemy_hp_increase_menu_state);
    options.emplace_back(2, msg::event_text_for_talk_enemy_hp_decrease, &g_enemy_hp_decrease_menu_state);
    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_enemy_mods_menu_state.rebuild(std::move(options));
}

void rebuild_enemy_hp_increase_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kEnemyHpPercentSteps,
        msg::event_text_for_talk_enemy_hp_increase_base,
        g_enemy_hp_increase_states,
        &handle_set_enemy_hp_increase_callback,
        g_enemy_hp_increase_menu_state,
        g_enemy_mods_menu_state);
}

void rebuild_enemy_hp_decrease_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kEnemyHpPercentSteps,
        msg::event_text_for_talk_enemy_hp_decrease_base,
        g_enemy_hp_decrease_states,
        &handle_set_enemy_hp_decrease_callback,
        g_enemy_hp_decrease_menu_state,
        g_enemy_mods_menu_state);
}

void rebuild_boss_revive_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_boss_revive, grace_test::localization::get("boss_revive"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    g_boss_revive_area_menu_states.clear();
    g_boss_revive_callback_states.clear();
    g_boss_revive_parent_menu_by_id.clear();
    g_boss_revive_name_by_id.clear();

    const std::vector<grace_test::runtime::BossReviveMenuItem> all_items =
        grace_test::runtime::get_boss_revive_menu_items();

    std::vector<std::string> areas;
    std::unordered_map<std::string, std::vector<grace_test::runtime::BossReviveMenuItem>> grouped;
    for (const auto& item : all_items) {
        if (item.area.empty() || item.boss_name.empty()) {
            continue;
        }
        if (grouped.find(item.area) == grouped.end()) {
            areas.push_back(item.area);
        }
        grouped[item.area].push_back(item);
    }

    std::sort(areas.begin(), areas.end());
    std::vector<talkscript_menu_option> root_options;
    int area_idx = 0;
    int boss_idx = 0;

    for (const std::string& area : areas) {
        auto area_state = std::make_unique<talkscript_menu_state>();
        std::vector<talkscript_menu_option> area_options;
        auto& bosses = grouped[area];
        std::sort(bosses.begin(), bosses.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.boss_name < rhs.boss_name;
        });

        int menu_index = 1;
        for (const auto& boss : bosses) {
            const int message_id = kBossReviveBossMessageBase + boss_idx++;
            set_message(message_id, grace_test::localization::utf8_to_wide_copy(boss.boss_name));
            g_boss_revive_callback_states.push_back(
                std::make_unique<callback_state>(&handle_select_boss_revive_callback, boss.id, &g_boss_revive_action_menu_state));
            area_options.emplace_back(
                menu_index++,
                message_id,
                g_boss_revive_callback_states.back().get());
            g_boss_revive_parent_menu_by_id[boss.id] = area_state.get();
            g_boss_revive_name_by_id[boss.id] = boss.boss_name;
        }

        area_options.emplace_back(99, msg::event_text_for_talk_back, &g_boss_revive_menu_state, true);
        area_state->rebuild(std::move(area_options));

        const int area_message_id = kBossReviveAreaMessageBase + area_idx++;
        set_message(area_message_id, grace_test::localization::utf8_to_wide_copy(area));
        root_options.emplace_back(
            static_cast<int>(root_options.size()) + 1,
            area_message_id,
            area_state.get());
        g_boss_revive_area_menu_states.push_back(std::move(area_state));
    }

    root_options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_boss_revive_menu_state.rebuild(std::move(root_options));
}

void rebuild_boss_revive_action_menu() {
    grace_test::localization::refresh();

    std::wstring title = grace_test::localization::get("boss_revive_action");
    const auto found_name = g_boss_revive_name_by_id.find(g_selected_boss_revive_menu_id);
    if (found_name != g_boss_revive_name_by_id.end()) {
        title += L": ";
        title += grace_test::localization::utf8_to_wide_copy(found_name->second);
    }
    set_message(msg::event_text_for_talk_boss_revive_action, title);
    set_message(
        msg::event_text_for_talk_boss_revive_do_revive,
        grace_test::localization::get("boss_revive_do_revive"));
    set_message(
        msg::event_text_for_talk_boss_revive_do_warp,
        grace_test::localization::get("boss_revive_do_warp"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    if (!g_boss_revive_action_states[0]) {
        g_boss_revive_action_states[0] = std::make_unique<callback_state>(
            &handle_boss_revive_action_callback,
            static_cast<int>(BossReviveAction::Revive),
            &g_boss_revive_action_menu_state);
    } else {
        g_boss_revive_action_states[0]->set_return_state(&g_boss_revive_action_menu_state);
    }

    if (!g_boss_revive_action_states[1]) {
        g_boss_revive_action_states[1] = std::make_unique<callback_state>(
            &handle_boss_revive_action_callback,
            static_cast<int>(BossReviveAction::Warp),
            &g_boss_revive_action_menu_state);
    } else {
        g_boss_revive_action_states[1]->set_return_state(&g_boss_revive_action_menu_state);
    }

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_boss_revive_do_revive, g_boss_revive_action_states[0].get());
    options.emplace_back(2, msg::event_text_for_talk_boss_revive_do_warp, g_boss_revive_action_states[1].get());
    options.emplace_back(99, msg::event_text_for_talk_back, g_selected_boss_parent_menu, true);
    g_boss_revive_action_menu_state.rebuild(std::move(options));
}

void rebuild_unlocks_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_unlocks, grace_test::localization::get("unlocks"));
    set_message(msg::event_text_for_talk_unlock_maps, grace_test::localization::get("unlock_maps"));
    set_message(msg::event_text_for_talk_unlock_whetblades, grace_test::localization::get("unlock_whetblades"));
    set_message(msg::event_text_for_talk_unlock_gestures, grace_test::localization::get("unlock_gestures"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    if (!g_unlock_states[0]) {
        g_unlock_states[0] = std::make_unique<callback_state>(
            &handle_unlock_callback,
            static_cast<int>(UnlockAction::Whetblades),
            &g_unlocks_menu_state);
    } else {
        g_unlock_states[0]->set_return_state(&g_unlocks_menu_state);
    }

    if (!g_unlock_states[1]) {
        g_unlock_states[1] = std::make_unique<callback_state>(
            &handle_unlock_callback,
            static_cast<int>(UnlockAction::Gestures),
            &g_unlocks_menu_state);
    } else {
        g_unlock_states[1]->set_return_state(&g_unlocks_menu_state);
    }

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_unlock_maps, &g_unlock_maps_menu_state);
    options.emplace_back(2, msg::event_text_for_talk_unlock_whetblades, g_unlock_states[0].get());
    options.emplace_back(3, msg::event_text_for_talk_unlock_gestures, g_unlock_states[1].get());
    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_unlocks_menu_state.rebuild(std::move(options));
}

void rebuild_unlock_maps_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_unlock_maps, grace_test::localization::get("unlock_maps"));
    set_message(msg::event_text_for_talk_unlock_maps_all, grace_test::localization::get("unlock_maps_all"));
    set_message(msg::event_text_for_talk_unlock_maps_base, grace_test::localization::get("unlock_maps_base"));
    set_message(msg::event_text_for_talk_unlock_maps_dlc, grace_test::localization::get("unlock_maps_dlc"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    if (!g_unlock_map_states[0]) {
        g_unlock_map_states[0] = std::make_unique<callback_state>(
            &handle_unlock_maps_callback,
            static_cast<int>(UnlockMapsAction::All),
            &g_unlock_maps_menu_state);
    } else {
        g_unlock_map_states[0]->set_return_state(&g_unlock_maps_menu_state);
    }

    if (!g_unlock_map_states[1]) {
        g_unlock_map_states[1] = std::make_unique<callback_state>(
            &handle_unlock_maps_callback,
            static_cast<int>(UnlockMapsAction::BaseGame),
            &g_unlock_maps_menu_state);
    } else {
        g_unlock_map_states[1]->set_return_state(&g_unlock_maps_menu_state);
    }

    if (!g_unlock_map_states[2]) {
        g_unlock_map_states[2] = std::make_unique<callback_state>(
            &handle_unlock_maps_callback,
            static_cast<int>(UnlockMapsAction::Dlc),
            &g_unlock_maps_menu_state);
    } else {
        g_unlock_map_states[2]->set_return_state(&g_unlock_maps_menu_state);
    }

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_unlock_maps_all, g_unlock_map_states[0].get());
    options.emplace_back(2, msg::event_text_for_talk_unlock_maps_base, g_unlock_map_states[1].get());
    options.emplace_back(3, msg::event_text_for_talk_unlock_maps_dlc, g_unlock_map_states[2].get());
    options.emplace_back(99, msg::event_text_for_talk_back, &g_unlocks_menu_state, true);
    g_unlock_maps_menu_state.rebuild(std::move(options));
}

void rebuild_portable_grace_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_portable_grace, grace_test::localization::get("portable_grace"));
    set_message(msg::event_text_for_talk_portable_upgrade, grace_test::localization::get("portable_upgrade"));
    set_message(msg::event_text_for_talk_portable_rebirth, grace_test::localization::get("portable_rebirth"));
    set_message(msg::event_text_for_talk_portable_mirror, grace_test::localization::get("portable_mirror"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    if (!g_portable_grace_states[0]) {
        g_portable_grace_states[0] = std::make_unique<callback_state>(
            &handle_portable_grace_callback,
            static_cast<int>(PortableGraceAction::Upgrade),
            &g_portable_grace_menu_state);
    } else {
        g_portable_grace_states[0]->set_return_state(&g_portable_grace_menu_state);
    }

    if (!g_portable_grace_states[1]) {
        g_portable_grace_states[1] = std::make_unique<callback_state>(
            &handle_portable_grace_callback,
            static_cast<int>(PortableGraceAction::Rebirth),
            &g_portable_grace_menu_state);
    } else {
        g_portable_grace_states[1]->set_return_state(&g_portable_grace_menu_state);
    }

    if (!g_portable_grace_states[2]) {
        g_portable_grace_states[2] = std::make_unique<callback_state>(
            &handle_portable_grace_callback,
            static_cast<int>(PortableGraceAction::Mirror),
            &g_portable_grace_menu_state);
    } else {
        g_portable_grace_states[2]->set_return_state(&g_portable_grace_menu_state);
    }

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_portable_upgrade, g_portable_grace_states[0].get());
    options.emplace_back(2, msg::event_text_for_talk_portable_rebirth, g_portable_grace_states[1].get());
    options.emplace_back(3, msg::event_text_for_talk_portable_mirror, g_portable_grace_states[2].get());
    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_portable_grace_menu_state.rebuild(std::move(options));
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

void rebuild_player_speed_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kPlayerSpeedPercentSteps,
        msg::event_text_for_talk_player_speed_increase_base,
        g_player_speed_states,
        &handle_set_player_speed_increase_callback,
        g_player_speed_menu_state,
        g_player_mods_menu_state);
}

void rebuild_buff_duration_extend_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < kBuffDurationExtendSteps.size(); ++index) {
        const int mode = kBuffDurationExtendSteps[index];
        if (mode == 99999) {
            set_message(
                msg::event_text_for_talk_buff_duration_extend_base + static_cast<int>(index),
                grace_test::localization::get("option_permanent"));
        } else {
            set_message(
                msg::event_text_for_talk_buff_duration_extend_base + static_cast<int>(index),
                grace_test::localization::make_percent_option_label(mode));
        }

        if (!g_buff_duration_extend_states[index]) {
            g_buff_duration_extend_states[index] =
                std::make_unique<callback_state>(&handle_set_buff_duration_extend_callback, mode, &g_buff_duration_extend_menu_state);
        } else {
            g_buff_duration_extend_states[index]->set_return_state(&g_buff_duration_extend_menu_state);
        }

        options.emplace_back(
            static_cast<int>(index + 1),
            msg::event_text_for_talk_buff_duration_extend_base + static_cast<int>(index),
            g_buff_duration_extend_states[index].get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &g_player_mods_menu_state, true);
    g_buff_duration_extend_menu_state.rebuild(std::move(options));
}

void rebuild_damage_multiplier_up_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kDamageMultiplierPercentSteps,
        msg::event_text_for_talk_damage_multiplier_base,
        g_damage_multiplier_up_states,
        &handle_set_damage_multiplier_callback,
        g_damage_multiplier_up_menu_state,
        g_player_mods_menu_state);
}

void rebuild_damage_multiplier_cut_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kDamageMultiplierPercentSteps,
        msg::event_text_for_talk_damage_cut_multiplier_base,
        g_damage_multiplier_cut_states,
        &handle_set_damage_cut_multiplier_callback,
        g_damage_multiplier_cut_menu_state,
        g_player_mods_menu_state);
}

void rebuild_damage_multiplier_reduce_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kDamageMultiplierPercentSteps,
        msg::event_text_for_talk_damage_reduce_multiplier_base,
        g_damage_multiplier_reduce_states,
        &handle_set_damage_reduce_multiplier_callback,
        g_damage_multiplier_reduce_menu_state,
        g_player_mods_menu_state);
}

void rebuild_damage_multiplier_taken_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kDamageMultiplierPercentSteps,
        msg::event_text_for_talk_damage_taken_multiplier_base,
        g_damage_multiplier_taken_states,
        &handle_set_damage_taken_multiplier_callback,
        g_damage_multiplier_taken_menu_state,
        g_player_mods_menu_state);
}

void rebuild_all_menus() {
    rebuild_root_menu();
    rebuild_player_mods_menu();
    rebuild_game_mods_menu();
    rebuild_enemy_mods_menu();
    rebuild_enemy_hp_increase_menu();
    rebuild_enemy_hp_decrease_menu();
    rebuild_boss_revive_menu();
    rebuild_boss_revive_action_menu();
    rebuild_equipment_reduction_menu();
    rebuild_weapon_requirement_reduction_menu();
    rebuild_player_speed_menu();
    rebuild_buff_duration_extend_menu();
    rebuild_damage_multiplier_up_menu();
    rebuild_damage_multiplier_cut_menu();
    rebuild_damage_multiplier_reduce_menu();
    rebuild_damage_multiplier_taken_menu();
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
    for (auto& callback : g_unlock_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_unlock_map_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_portable_grace_states) {
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
    for (auto& callback : g_player_speed_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_buff_duration_extend_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_enemy_hp_increase_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_enemy_hp_decrease_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_boss_revive_callback_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_boss_revive_action_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_easier_parry_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_damage_multiplier_up_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_damage_multiplier_cut_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_damage_multiplier_reduce_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_damage_multiplier_taken_states) {
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
        } else if (state == &g_enemy_mods_menu_state) {
            rebuild_enemy_mods_menu();
        } else if (state == &g_enemy_hp_increase_menu_state) {
            rebuild_enemy_hp_increase_menu();
        } else if (state == &g_enemy_hp_decrease_menu_state) {
            rebuild_enemy_hp_decrease_menu();
        } else if (state == &g_boss_revive_menu_state) {
            rebuild_boss_revive_menu();
        } else if (state == &g_boss_revive_action_menu_state) {
            rebuild_boss_revive_action_menu();
        } else if (state == &g_equipment_reduction_menu_state) {
            rebuild_equipment_reduction_menu();
        } else if (state == &g_weapon_requirement_reduction_menu_state) {
            rebuild_weapon_requirement_reduction_menu();
        } else if (state == &g_player_speed_menu_state) {
            rebuild_player_speed_menu();
        } else if (state == &g_buff_duration_extend_menu_state) {
            rebuild_buff_duration_extend_menu();
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
