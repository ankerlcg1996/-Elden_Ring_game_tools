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
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>
#include <windows.h>

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
    NoRuneLossOnDeath,
    NoRuneArcLossOnDeath,
    NoTimePassOnDeath,
    AttackLifeStealOnHit,
    InfiniteJump,
    SpiritAshesAnywhere,
    TorrentNoDeath,
    TorrentAnywhere,
    NoFallDeath,
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

enum class QuickAccessAction {
    Upgrade = 0,
    SpiritTuning,
    Rebirth,
    Mirror,
    Sell,
};

enum class BossReviveAction {
    Revive = 0,
    Warp = 1,
};

enum class AutoPickupToggleAction {
    Master = 0,
    Materials,
    Items,
    CorpseLoot,
    LostRunes,
    InCombat,
};

enum class EnemyVisualizationToggleAction {
    PoiseOverlay = 0,
    ResistanceOverlay,
    BleedBar,
    FrostBar,
    RotBar,
    PoisonBar,
    SleepBar,
    MadnessBar,
    EnableAllBars,
    DisableAllBars,
};

enum class OverlayTuningAction {
    PoiseWidth = 0,
    PoiseHeight,
    StatusWidth,
    StatusHeight,
    PoiseOffset,
    EntityStatusOffset,
    BossStatusGap,
};

struct ToggleDescriptor {
    int menu_index = 0;
    int message_id = 0;
    const char* label_key = "";
    ToggleAction action = ToggleAction::FreePurchase;
};

struct AutoPickupToggleDescriptor {
    int menu_index = 0;
    int message_id = 0;
    const char* label_key = "";
    AutoPickupToggleAction action = AutoPickupToggleAction::Master;
};

struct EnemyVisualizationToggleDescriptor {
    int menu_index = 0;
    int message_id = 0;
    const char* label_key = "";
    EnemyVisualizationToggleAction action = EnemyVisualizationToggleAction::ResistanceOverlay;
};

struct OverlayTuningDescriptor {
    int menu_index = 0;
    int message_id = 0;
    const char* label_key = "";
    OverlayTuningAction action = OverlayTuningAction::PoiseWidth;
};

constexpr std::array<int, 11> kPercentSteps{0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
constexpr std::array<int, 16> kOverlayScaleSteps{{50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200}};
constexpr std::array<int, 31> kOverlayOffsetSteps{{-100, -90, -80, -70, -60, -50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200}};
constexpr std::array<int, 13> kOverlayBossGapSteps{{-20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100}};
constexpr std::array<int, 6> kWeaponRequirementPercentSteps{0, 20, 40, 60, 80, 100};
constexpr std::array<int, 21> kPlayerSpeedPercentSteps{
    0,   10,  20,  30,  40,  50,  60,  70,  80,  90,  100,
    110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
constexpr std::array<int, 11> kNgCycleSteps{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};
constexpr std::array<int, 7> kBuffDurationExtendSteps{{0, 50, 100, 200, 300, 500, 99999}};
constexpr std::array<int, 5> kWeaponParryModes{{0, 60, 40, 20, 10}};
constexpr std::array<int, 27> kEnemyHpPercentSteps{
    0,   20,  40,  60,  80,  100, 120, 140, 160, 180, 200, 250, 300, 350,
    400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000};
constexpr std::array<int, 16> kDamageMultiplierPercentSteps{{0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 150, 200, 300, 400, 500}};
constexpr std::array<int, 5> kAutoPickupRangeSteps{{100, 150, 200, 300, 500}};
constexpr std::array<int, 31> kModelScaleSteps{
    0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
    110, 120, 130, 140, 150, 160, 170, 180, 190, 200,
    210, 220, 230, 240, 250, 260, 270, 280, 290, 300};

constexpr ToggleDescriptor kPlayerToggleDescriptors[] = {
    {9, msg::event_text_for_talk_toggle_no_magic_req, "toggle_no_magic_req", ToggleAction::NoMagicRequirements},
    {10, msg::event_text_for_talk_toggle_one_slot_magic, "toggle_one_slot_magic", ToggleAction::AllMagicOneSlot},
    {12, msg::event_text_for_talk_toggle_attack_life_steal_on_hit, "toggle_attack_life_steal_on_hit", ToggleAction::AttackLifeStealOnHit},
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
    {9, msg::event_text_for_talk_toggle_infinite_consumables, "toggle_infinite_consumables", ToggleAction::InfiniteConsumables},
    {10, msg::event_text_for_talk_toggle_infinite_arrows, "toggle_infinite_arrows", ToggleAction::InfiniteArrows},
    {11, msg::event_text_for_talk_toggle_infinite_jump, "toggle_infinite_jump", ToggleAction::InfiniteJump},
    {12, msg::event_text_for_talk_toggle_no_rune_loss_on_death, "toggle_no_rune_loss_on_death", ToggleAction::NoRuneLossOnDeath},
    {13, msg::event_text_for_talk_toggle_no_fall_death, "toggle_no_fall_death", ToggleAction::NoFallDeath},
    {14, msg::event_text_for_talk_toggle_no_rune_arc_loss_on_death, "toggle_no_rune_arc_loss_on_death", ToggleAction::NoRuneArcLossOnDeath},
    {15, msg::event_text_for_talk_toggle_no_time_pass_on_death, "toggle_no_time_pass_on_death", ToggleAction::NoTimePassOnDeath},
};

constexpr AutoPickupToggleDescriptor kAutoPickupRootToggleDescriptors[] = {
    {1, msg::event_text_for_talk_toggle_auto_pickup_enabled, "toggle_auto_pickup_enabled", AutoPickupToggleAction::Master},
};

constexpr AutoPickupToggleDescriptor kAutoPickupSettingsToggleDescriptors[] = {
    {1, msg::event_text_for_talk_toggle_auto_pickup_materials, "toggle_auto_pickup_materials", AutoPickupToggleAction::Materials},
    {2, msg::event_text_for_talk_toggle_auto_pickup_items, "toggle_auto_pickup_items", AutoPickupToggleAction::Items},
    {3, msg::event_text_for_talk_toggle_auto_pickup_corpse_loot, "toggle_auto_pickup_corpse_loot", AutoPickupToggleAction::CorpseLoot},
    {4, msg::event_text_for_talk_toggle_auto_pickup_lost_runes, "toggle_auto_pickup_lost_runes", AutoPickupToggleAction::LostRunes},
    {5, msg::event_text_for_talk_toggle_auto_pickup_in_combat, "toggle_auto_pickup_in_combat", AutoPickupToggleAction::InCombat},
};

constexpr EnemyVisualizationToggleDescriptor kEnemyVisualizationToggleDescriptors[] = {
    {1, msg::event_text_for_talk_toggle_enemy_poise_overlay, "toggle_enemy_poise_overlay", EnemyVisualizationToggleAction::PoiseOverlay},
    {2, msg::event_text_for_talk_toggle_enemy_resistance_overlay, "toggle_enemy_resistance_overlay", EnemyVisualizationToggleAction::ResistanceOverlay},
    {3, msg::event_text_for_talk_toggle_enemy_resistance_bleed, "toggle_enemy_resistance_bleed", EnemyVisualizationToggleAction::BleedBar},
    {4, msg::event_text_for_talk_toggle_enemy_resistance_frost, "toggle_enemy_resistance_frost", EnemyVisualizationToggleAction::FrostBar},
    {5, msg::event_text_for_talk_toggle_enemy_resistance_rot, "toggle_enemy_resistance_rot", EnemyVisualizationToggleAction::RotBar},
    {6, msg::event_text_for_talk_toggle_enemy_resistance_poison, "toggle_enemy_resistance_poison", EnemyVisualizationToggleAction::PoisonBar},
    {7, msg::event_text_for_talk_toggle_enemy_resistance_sleep, "toggle_enemy_resistance_sleep", EnemyVisualizationToggleAction::SleepBar},
    {8, msg::event_text_for_talk_toggle_enemy_resistance_madness, "toggle_enemy_resistance_madness", EnemyVisualizationToggleAction::MadnessBar},
};

constexpr OverlayTuningDescriptor kOverlayTuningDescriptors[] = {
    {20, msg::event_text_for_talk_overlay_poise_width, "overlay_poise_width", OverlayTuningAction::PoiseWidth},
    {21, msg::event_text_for_talk_overlay_poise_height, "overlay_poise_height", OverlayTuningAction::PoiseHeight},
    {22, msg::event_text_for_talk_overlay_status_width, "overlay_status_width", OverlayTuningAction::StatusWidth},
    {23, msg::event_text_for_talk_overlay_status_height, "overlay_status_height", OverlayTuningAction::StatusHeight},
    {24, msg::event_text_for_talk_overlay_poise_offset, "overlay_poise_offset", OverlayTuningAction::PoiseOffset},
    {25, msg::event_text_for_talk_overlay_entity_status_offset, "overlay_entity_status_offset", OverlayTuningAction::EntityStatusOffset},
    {26, msg::event_text_for_talk_overlay_boss_status_gap, "overlay_boss_status_gap", OverlayTuningAction::BossStatusGap},
};

talkscript_menu_state g_root_menu_state;
talkscript_menu_state g_player_mods_menu_state;
talkscript_menu_state g_game_mods_menu_state;
talkscript_menu_state g_ng_mods_menu_state;
talkscript_menu_state g_enemy_mods_menu_state;
talkscript_menu_state g_enemy_visualization_menu_state;
talkscript_menu_state g_auto_pickup_menu_state;
talkscript_menu_state g_auto_pickup_settings_menu_state;
talkscript_menu_state g_auto_pickup_range_menu_state;
talkscript_menu_state g_enemy_hp_increase_menu_state;
talkscript_menu_state g_enemy_hp_decrease_menu_state;
talkscript_menu_state g_boss_revive_menu_state;
talkscript_menu_state g_boss_revive_action_menu_state;
talkscript_menu_state g_unlocks_menu_state;
talkscript_menu_state g_unlock_maps_menu_state;
talkscript_menu_state g_quick_access_menu_state;
talkscript_menu_state g_model_modification_menu_state;
talkscript_menu_state g_equipment_reduction_menu_state;
talkscript_menu_state g_weapon_requirement_reduction_menu_state;
talkscript_menu_state g_player_speed_menu_state;
talkscript_menu_state g_buff_duration_extend_menu_state;
talkscript_menu_state g_weapon_parry_menu_state;
talkscript_menu_state g_stamina_cost_reduction_menu_state;
talkscript_menu_state g_fp_cost_reduction_menu_state;
talkscript_menu_state g_damage_multiplier_menu_state;
talkscript_menu_state g_damage_cut_menu_state;
talkscript_menu_state g_damage_multiplier_up_menu_state;
talkscript_menu_state g_damage_multiplier_cut_menu_state;
talkscript_menu_state g_damage_multiplier_reduce_menu_state;
talkscript_menu_state g_damage_multiplier_taken_menu_state;
talkscript_menu_option g_top_level_option{70, msg::event_text_for_talk_root, &g_root_menu_state};

std::array<std::unique_ptr<callback_state>, std::size(kPlayerToggleDescriptors)> g_player_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kGameToggleDescriptors)> g_game_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kAutoPickupRootToggleDescriptors)> g_auto_pickup_root_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kAutoPickupSettingsToggleDescriptors)> g_auto_pickup_settings_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kEnemyVisualizationToggleDescriptors)> g_enemy_visualization_toggle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kOverlayTuningDescriptors)> g_overlay_tuning_states{};
std::unique_ptr<callback_state> g_enable_all_enemy_resistance_bars_state{};
std::unique_ptr<callback_state> g_disable_all_enemy_resistance_bars_state{};
std::array<std::unique_ptr<callback_state>, std::size(kAutoPickupRangeSteps)> g_auto_pickup_range_states{};
std::array<std::unique_ptr<callback_state>, 2> g_unlock_states{};
std::array<std::unique_ptr<callback_state>, 3> g_unlock_map_states{};
std::array<std::unique_ptr<callback_state>, 5> g_quick_access_states{};
std::array<std::unique_ptr<callback_state>, std::size(kPercentSteps)> g_equipment_reduction_states{};
std::array<std::unique_ptr<callback_state>, std::size(kWeaponRequirementPercentSteps)> g_weapon_requirement_reduction_states{};
std::array<std::unique_ptr<callback_state>, std::size(kPlayerSpeedPercentSteps)> g_player_speed_states{};
std::array<std::unique_ptr<callback_state>, std::size(kNgCycleSteps)> g_ng_cycle_states{};
std::array<std::unique_ptr<callback_state>, std::size(kBuffDurationExtendSteps)> g_buff_duration_extend_states{};
std::array<std::unique_ptr<callback_state>, std::size(kWeaponParryModes)> g_weapon_parry_states{};
std::array<std::unique_ptr<callback_state>, std::size(kEnemyHpPercentSteps)> g_enemy_hp_increase_states{};
std::array<std::unique_ptr<callback_state>, std::size(kEnemyHpPercentSteps)> g_enemy_hp_decrease_states{};
std::array<std::unique_ptr<callback_state>, std::size(kDamageMultiplierPercentSteps)> g_damage_multiplier_up_states{};
std::array<std::unique_ptr<callback_state>, std::size(kDamageMultiplierPercentSteps)> g_damage_multiplier_cut_states{};
std::array<std::unique_ptr<callback_state>, std::size(kDamageMultiplierPercentSteps)> g_damage_multiplier_reduce_states{};
std::array<std::unique_ptr<callback_state>, std::size(kDamageMultiplierPercentSteps)> g_damage_multiplier_taken_states{};
std::array<std::unique_ptr<callback_state>, std::size(kPercentSteps)> g_stamina_cost_reduction_states{};
std::array<std::unique_ptr<callback_state>, std::size(kPercentSteps)> g_fp_cost_reduction_states{};
std::array<std::unique_ptr<callback_state>, std::size(kModelScaleSteps)> g_model_scale_states{};
std::vector<std::unique_ptr<talkscript_menu_state>> g_boss_revive_area_menu_states{};
std::vector<std::unique_ptr<callback_state>> g_boss_revive_callback_states{};
std::array<std::unique_ptr<callback_state>, 2> g_boss_revive_action_states{};
std::unordered_map<int, talkscript_menu_state*> g_boss_revive_parent_menu_by_id{};
std::unordered_map<int, std::string> g_boss_revive_name_by_id{};
std::vector<std::unique_ptr<callback_state>> g_quick_access_shop_states{};
std::unordered_map<int, std::string> g_quick_access_shop_name_by_id{};
int g_selected_boss_revive_menu_id = -1;
talkscript_menu_state* g_selected_boss_parent_menu = &g_boss_revive_menu_state;

constexpr int kBossReviveAreaMessageBase = 69012000;
constexpr int kBossReviveBossMessageBase = 69013000;
constexpr int kActionFeedbackMessageId = msg::event_text_for_talk_action_feedback;

std::unordered_map<int, std::wstring> g_message_cache;

struct QuickAccessDispatchRequest {
    int payload = 0;
    bool is_shop = false;
};

bool toggle_state(ToggleAction action);
bool auto_pickup_toggle_state(AutoPickupToggleAction action);
bool enemy_visualization_toggle_state(EnemyVisualizationToggleAction action);
void handle_overlay_tuning_callback(int payload);

er::ezstate::state* g_grace_initial_state = nullptr;
static auto g_patched_events = std::array<er::ezstate::event, 128>{};
static auto g_patched_transitions = std::array<er::ezstate::transition*, 128>{};

void set_message(int message_id, std::wstring text) {
    g_message_cache[message_id] = std::move(text);
}

void set_action_feedback_message(std::wstring text) {
    set_message(kActionFeedbackMessageId, std::move(text));
}

std::wstring enabled_state_label(bool enabled) {
    return grace_test::localization::get(enabled ? "state_enabled" : "state_disabled");
}

const ToggleDescriptor* find_toggle_descriptor(ToggleAction action) {
    for (const ToggleDescriptor& descriptor : kPlayerToggleDescriptors) {
        if (descriptor.action == action) {
            return &descriptor;
        }
    }
    for (const ToggleDescriptor& descriptor : kGameToggleDescriptors) {
        if (descriptor.action == action) {
            return &descriptor;
        }
    }
    return nullptr;
}

void set_toggle_feedback_message(ToggleAction action) {
    const ToggleDescriptor* descriptor = find_toggle_descriptor(action);
    if (descriptor == nullptr) {
        return;
    }

    set_action_feedback_message(grace_test::localization::make_feedback_set_label(
        grace_test::localization::get(descriptor->label_key),
        enabled_state_label(toggle_state(action))));
}

void set_percent_feedback_message(std::string_view key, int percent) {
    set_action_feedback_message(grace_test::localization::make_feedback_set_label(
        grace_test::localization::get(key),
        grace_test::localization::make_percent_option_label(percent)));
}

void set_action_feedback_message_for_label(std::wstring action) {
    set_action_feedback_message(grace_test::localization::make_feedback_action_label(action));
}

std::wstring make_signed_pixel_label(int value) {
    const std::wstring sign = value > 0 ? L"+" : L"";
    return sign + std::to_wstring(value) + L" px";
}

std::wstring overlay_tuning_label(OverlayTuningAction action) {
    auto& status = ERD::Main::g_FeatureStatus;
    switch (action) {
    case OverlayTuningAction::PoiseWidth:
        return grace_test::localization::get("overlay_poise_width") + L": " +
               grace_test::localization::make_percent_option_label(
                   static_cast<int>(std::lround(status.overlay_poise_width_scale.load() * 100.0f)));
    case OverlayTuningAction::PoiseHeight:
        return grace_test::localization::get("overlay_poise_height") + L": " +
               grace_test::localization::make_percent_option_label(
                   static_cast<int>(std::lround(status.overlay_poise_height_scale.load() * 100.0f)));
    case OverlayTuningAction::StatusWidth:
        return grace_test::localization::get("overlay_status_width") + L": " +
               grace_test::localization::make_percent_option_label(
                   static_cast<int>(std::lround(status.overlay_status_width_scale.load() * 100.0f)));
    case OverlayTuningAction::StatusHeight:
        return grace_test::localization::get("overlay_status_height") + L": " +
               grace_test::localization::make_percent_option_label(
                   static_cast<int>(std::lround(status.overlay_status_height_scale.load() * 100.0f)));
    case OverlayTuningAction::PoiseOffset:
        return grace_test::localization::get("overlay_poise_offset") + L": " +
               make_signed_pixel_label(static_cast<int>(std::lround(status.overlay_poise_vertical_offset.load())));
    case OverlayTuningAction::EntityStatusOffset:
        return grace_test::localization::get("overlay_entity_status_offset") + L": " +
               make_signed_pixel_label(static_cast<int>(std::lround(status.overlay_entity_status_vertical_offset.load())));
    case OverlayTuningAction::BossStatusGap:
        return grace_test::localization::get("overlay_boss_status_gap") + L": " +
               make_signed_pixel_label(static_cast<int>(std::lround(status.overlay_boss_status_gap.load())));
    }
    return {};
}

const EnemyVisualizationToggleDescriptor* find_enemy_visualization_toggle_descriptor(EnemyVisualizationToggleAction action) {
    for (const EnemyVisualizationToggleDescriptor& descriptor : kEnemyVisualizationToggleDescriptors) {
        if (descriptor.action == action) {
            return &descriptor;
        }
    }
    return nullptr;
}

void set_enemy_visualization_feedback_message(EnemyVisualizationToggleAction action) {
    if (action == EnemyVisualizationToggleAction::EnableAllBars) {
        set_action_feedback_message(grace_test::localization::make_feedback_action_label(
            grace_test::localization::get("enable_all_enemy_resistance_bars")));
        return;
    }
    if (action == EnemyVisualizationToggleAction::DisableAllBars) {
        set_action_feedback_message(grace_test::localization::make_feedback_action_label(
            grace_test::localization::get("disable_all_enemy_resistance_bars")));
        return;
    }

    const EnemyVisualizationToggleDescriptor* descriptor = find_enemy_visualization_toggle_descriptor(action);
    if (descriptor == nullptr) {
        return;
    }

    set_action_feedback_message(grace_test::localization::make_feedback_set_label(
        grace_test::localization::get(descriptor->label_key),
        enabled_state_label(enemy_visualization_toggle_state(action))));
}

DWORD WINAPI quick_access_dispatch_thread(LPVOID parameter) {
    std::unique_ptr<QuickAccessDispatchRequest> request(static_cast<QuickAccessDispatchRequest*>(parameter));
    Sleep(50);

    bool ok = false;
    if (request->is_shop) {
        ok = grace_test::runtime::open_quick_access_shop_by_menu_id(request->payload);
    } else {
        switch (static_cast<QuickAccessAction>(request->payload)) {
        case QuickAccessAction::Upgrade:
            ok = grace_test::runtime::open_quick_access_upgrade();
            break;
        case QuickAccessAction::SpiritTuning:
            ok = grace_test::runtime::open_quick_access_spirit_tuning();
            break;
        case QuickAccessAction::Rebirth:
            ok = grace_test::runtime::open_quick_access_rebirth();
            break;
        case QuickAccessAction::Mirror:
            ok = grace_test::runtime::open_quick_access_mirror();
            break;
        case QuickAccessAction::Sell:
            ok = grace_test::runtime::open_quick_access_sell();
            break;
        }
    }

    SPDLOG_INFO(
        "Quick access async payload={} kind={} result={}",
        request->payload,
        request->is_shop ? "shop" : "fixed",
        ok ? "ok" : "failed");
    return 0;
}

void queue_quick_access_dispatch(int payload, bool is_shop = false) {
    auto* request = new QuickAccessDispatchRequest{payload, is_shop};
    HANDLE thread = CreateThread(nullptr, 0, &quick_access_dispatch_thread, request, 0, nullptr);
    if (thread != nullptr) {
        CloseHandle(thread);
    } else {
        delete request;
        SPDLOG_ERROR("Failed to create quick access dispatch thread.");
    }
}

std::wstring append_erdtools_source(std::wstring text) {
    if (_stricmp(grace_test::localization::active_language_code().c_str(), "zh-CN") == 0) {
        text += L"\n功能来源：ErdTools-CPP";
    } else {
        text += L"\nFeature source: ErdTools-CPP";
    }
    return text;
}

const AutoPickupToggleDescriptor* find_auto_pickup_toggle_descriptor(AutoPickupToggleAction action) {
    for (const AutoPickupToggleDescriptor& descriptor : kAutoPickupRootToggleDescriptors) {
        if (descriptor.action == action) {
            return &descriptor;
        }
    }
    for (const AutoPickupToggleDescriptor& descriptor : kAutoPickupSettingsToggleDescriptors) {
        if (descriptor.action == action) {
            return &descriptor;
        }
    }
    return nullptr;
}

void set_auto_pickup_feedback_message(AutoPickupToggleAction action) {
    const AutoPickupToggleDescriptor* descriptor = find_auto_pickup_toggle_descriptor(action);
    if (descriptor == nullptr) {
        return;
    }
    set_action_feedback_message(append_erdtools_source(
        grace_test::localization::make_feedback_set_label(
            grace_test::localization::get(descriptor->label_key),
            enabled_state_label(auto_pickup_toggle_state(action)))));
}

int current_equipment_reduction_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.equipment_weight_reduction_percent.load(), 0, 100);
}

int current_weapon_parry_mode() {
    const int mode = ERD::Main::g_FeatureStatus.weapon_parry_mode.load();
    switch (mode) {
    case 0:
    case 60:
    case 40:
    case 20:
    case 10:
        return mode;
    default:
        return 0;
    }
}

std::wstring weapon_parry_mode_label(int mode) {
    switch (mode) {
    case 60:
        return grace_test::localization::get("option_weapon_parry_full");
    case 40:
        return grace_test::localization::get("option_weapon_parry_easy");
    case 20:
        return grace_test::localization::get("option_weapon_parry_medium");
    case 10:
        return grace_test::localization::get("option_weapon_parry_master");
    default:
        return grace_test::localization::get("option_off");
    }
}

int current_weapon_requirement_reduction_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.weapon_requirement_reduction_percent.load(), 0, 100);
}

int current_player_speed_increase_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.player_speed_increase_percent.load(), 0, 200);
}

int current_ng_cycle() {
    return std::clamp(ERD::Main::g_FeatureStatus.current_ng_cycle.load(), 0, 10);
}

std::wstring ng_cycle_option_label(int cycle) {
    return cycle <= 0 ? L"NG" : (L"NG+" + std::to_wstring(cycle));
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

int current_stamina_cost_reduction_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.stamina_cost_reduction_percent.load(), 0, 100);
}

int current_fp_cost_reduction_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.fp_cost_reduction_percent.load(), 0, 100);
}

int current_auto_pickup_range_percent() {
    return std::clamp(ERD::Main::g_FeatureStatus.auto_pickup_range_percent.load(), 100, 500);
}

int current_model_scale_percent() {
    return std::clamp(static_cast<int>(std::lround(ERD::Main::g_FeatureStatus.model_scale_uniform.load() * 100.0f)), 0, 300);
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
    case ToggleAction::NoRuneLossOnDeath:
        return ERD::Main::g_FeatureStatus.no_rune_loss_on_death.load();
    case ToggleAction::NoRuneArcLossOnDeath:
        return ERD::Main::g_FeatureStatus.no_rune_arc_loss_on_death.load();
    case ToggleAction::NoTimePassOnDeath:
        return ERD::Main::g_FeatureStatus.no_time_pass_on_death.load();
    case ToggleAction::AttackLifeStealOnHit:
        return ERD::Main::g_FeatureStatus.attack_life_steal_on_hit.load();
    case ToggleAction::InfiniteJump:
        return ERD::Main::g_FeatureStatus.infinite_jump.load();
    case ToggleAction::SpiritAshesAnywhere:
        return ERD::Main::g_FeatureStatus.spirit_ashes_anywhere.load();
    case ToggleAction::TorrentNoDeath:
        return ERD::Main::g_FeatureStatus.torrent_no_death.load();
    case ToggleAction::TorrentAnywhere:
        return ERD::Main::g_FeatureStatus.torrent_anywhere.load();
    case ToggleAction::NoFallDeath:
        return ERD::Main::g_FeatureStatus.no_fall_death.load();
    }
    return false;
}

bool auto_pickup_toggle_state(AutoPickupToggleAction action) {
    switch (action) {
    case AutoPickupToggleAction::Master:
        return ERD::Main::g_FeatureStatus.auto_pickup_enabled.load();
    case AutoPickupToggleAction::Materials:
        return ERD::Main::g_FeatureStatus.auto_pickup_materials.load();
    case AutoPickupToggleAction::Items:
        return ERD::Main::g_FeatureStatus.auto_pickup_items.load();
    case AutoPickupToggleAction::CorpseLoot:
        return ERD::Main::g_FeatureStatus.auto_pickup_corpse_loot.load();
    case AutoPickupToggleAction::LostRunes:
        return ERD::Main::g_FeatureStatus.auto_pickup_lost_runes.load();
    case AutoPickupToggleAction::InCombat:
        return ERD::Main::g_FeatureStatus.auto_pickup_in_combat.load();
    }
    return false;
}

bool enemy_visualization_toggle_state(EnemyVisualizationToggleAction action) {
    switch (action) {
    case EnemyVisualizationToggleAction::PoiseOverlay:
        return ERD::Main::g_FeatureStatus.enemy_poise_overlay_enabled.load();
    case EnemyVisualizationToggleAction::ResistanceOverlay:
        return ERD::Main::g_FeatureStatus.enemy_resistance_overlay_enabled.load();
    case EnemyVisualizationToggleAction::BleedBar:
        return ERD::Main::g_FeatureStatus.enemy_resistance_bleed_enabled.load();
    case EnemyVisualizationToggleAction::FrostBar:
        return ERD::Main::g_FeatureStatus.enemy_resistance_frost_enabled.load();
    case EnemyVisualizationToggleAction::RotBar:
        return ERD::Main::g_FeatureStatus.enemy_resistance_rot_enabled.load();
    case EnemyVisualizationToggleAction::PoisonBar:
        return ERD::Main::g_FeatureStatus.enemy_resistance_poison_enabled.load();
    case EnemyVisualizationToggleAction::SleepBar:
        return ERD::Main::g_FeatureStatus.enemy_resistance_sleep_enabled.load();
    case EnemyVisualizationToggleAction::MadnessBar:
        return ERD::Main::g_FeatureStatus.enemy_resistance_madness_enabled.load();
    case EnemyVisualizationToggleAction::EnableAllBars:
        return ERD::Main::g_FeatureStatus.enemy_poise_overlay_enabled.load() &&
               ERD::Main::g_FeatureStatus.enemy_resistance_overlay_enabled.load() &&
               ERD::Main::g_FeatureStatus.enemy_resistance_bleed_enabled.load() &&
               ERD::Main::g_FeatureStatus.enemy_resistance_frost_enabled.load() &&
               ERD::Main::g_FeatureStatus.enemy_resistance_rot_enabled.load() &&
               ERD::Main::g_FeatureStatus.enemy_resistance_poison_enabled.load() &&
               ERD::Main::g_FeatureStatus.enemy_resistance_sleep_enabled.load() &&
               ERD::Main::g_FeatureStatus.enemy_resistance_madness_enabled.load();
    case EnemyVisualizationToggleAction::DisableAllBars:
        return !ERD::Main::g_FeatureStatus.enemy_poise_overlay_enabled.load() &&
               !ERD::Main::g_FeatureStatus.enemy_resistance_overlay_enabled.load() &&
               !ERD::Main::g_FeatureStatus.enemy_resistance_bleed_enabled.load() &&
               !ERD::Main::g_FeatureStatus.enemy_resistance_frost_enabled.load() &&
               !ERD::Main::g_FeatureStatus.enemy_resistance_rot_enabled.load() &&
               !ERD::Main::g_FeatureStatus.enemy_resistance_poison_enabled.load() &&
               !ERD::Main::g_FeatureStatus.enemy_resistance_sleep_enabled.load() &&
               !ERD::Main::g_FeatureStatus.enemy_resistance_madness_enabled.load();
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
    case ToggleAction::NoRuneLossOnDeath:
        ERD::Main::g_FeatureStatus.no_rune_loss_on_death =
            !ERD::Main::g_FeatureStatus.no_rune_loss_on_death.load();
        break;
    case ToggleAction::NoRuneArcLossOnDeath:
        ERD::Main::g_FeatureStatus.no_rune_arc_loss_on_death =
            !ERD::Main::g_FeatureStatus.no_rune_arc_loss_on_death.load();
        break;
    case ToggleAction::NoTimePassOnDeath:
        ERD::Main::g_FeatureStatus.no_time_pass_on_death =
            !ERD::Main::g_FeatureStatus.no_time_pass_on_death.load();
        break;
    case ToggleAction::AttackLifeStealOnHit:
        ERD::Main::g_FeatureStatus.attack_life_steal_on_hit =
            !ERD::Main::g_FeatureStatus.attack_life_steal_on_hit.load();
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
    case ToggleAction::NoFallDeath:
        ERD::Main::g_FeatureStatus.no_fall_death =
            !ERD::Main::g_FeatureStatus.no_fall_death.load();
        break;
    }
}

void toggle_auto_pickup_action(AutoPickupToggleAction action) {
    switch (action) {
    case AutoPickupToggleAction::Master:
        ERD::Main::g_FeatureStatus.auto_pickup_enabled =
            !ERD::Main::g_FeatureStatus.auto_pickup_enabled.load();
        break;
    case AutoPickupToggleAction::Materials:
        ERD::Main::g_FeatureStatus.auto_pickup_materials =
            !ERD::Main::g_FeatureStatus.auto_pickup_materials.load();
        break;
    case AutoPickupToggleAction::Items:
        ERD::Main::g_FeatureStatus.auto_pickup_items =
            !ERD::Main::g_FeatureStatus.auto_pickup_items.load();
        break;
    case AutoPickupToggleAction::CorpseLoot:
        ERD::Main::g_FeatureStatus.auto_pickup_corpse_loot =
            !ERD::Main::g_FeatureStatus.auto_pickup_corpse_loot.load();
        break;
    case AutoPickupToggleAction::LostRunes:
        ERD::Main::g_FeatureStatus.auto_pickup_lost_runes =
            !ERD::Main::g_FeatureStatus.auto_pickup_lost_runes.load();
        break;
    case AutoPickupToggleAction::InCombat:
        ERD::Main::g_FeatureStatus.auto_pickup_in_combat =
            !ERD::Main::g_FeatureStatus.auto_pickup_in_combat.load();
        break;
    }
}

void toggle_enemy_visualization_action(EnemyVisualizationToggleAction action) {
    switch (action) {
    case EnemyVisualizationToggleAction::PoiseOverlay:
        ERD::Main::g_FeatureStatus.enemy_poise_overlay_enabled =
            !ERD::Main::g_FeatureStatus.enemy_poise_overlay_enabled.load();
        break;
    case EnemyVisualizationToggleAction::ResistanceOverlay:
        ERD::Main::g_FeatureStatus.enemy_resistance_overlay_enabled =
            !ERD::Main::g_FeatureStatus.enemy_resistance_overlay_enabled.load();
        break;
    case EnemyVisualizationToggleAction::BleedBar:
        ERD::Main::g_FeatureStatus.enemy_resistance_bleed_enabled =
            !ERD::Main::g_FeatureStatus.enemy_resistance_bleed_enabled.load();
        break;
    case EnemyVisualizationToggleAction::FrostBar:
        ERD::Main::g_FeatureStatus.enemy_resistance_frost_enabled =
            !ERD::Main::g_FeatureStatus.enemy_resistance_frost_enabled.load();
        break;
    case EnemyVisualizationToggleAction::RotBar:
        ERD::Main::g_FeatureStatus.enemy_resistance_rot_enabled =
            !ERD::Main::g_FeatureStatus.enemy_resistance_rot_enabled.load();
        break;
    case EnemyVisualizationToggleAction::PoisonBar:
        ERD::Main::g_FeatureStatus.enemy_resistance_poison_enabled =
            !ERD::Main::g_FeatureStatus.enemy_resistance_poison_enabled.load();
        break;
    case EnemyVisualizationToggleAction::SleepBar:
        ERD::Main::g_FeatureStatus.enemy_resistance_sleep_enabled =
            !ERD::Main::g_FeatureStatus.enemy_resistance_sleep_enabled.load();
        break;
    case EnemyVisualizationToggleAction::MadnessBar:
        ERD::Main::g_FeatureStatus.enemy_resistance_madness_enabled =
            !ERD::Main::g_FeatureStatus.enemy_resistance_madness_enabled.load();
        break;
    case EnemyVisualizationToggleAction::EnableAllBars:
        ERD::Main::g_FeatureStatus.enemy_poise_overlay_enabled = true;
        ERD::Main::g_FeatureStatus.enemy_resistance_overlay_enabled = true;
        ERD::Main::g_FeatureStatus.enemy_resistance_bleed_enabled = true;
        ERD::Main::g_FeatureStatus.enemy_resistance_frost_enabled = true;
        ERD::Main::g_FeatureStatus.enemy_resistance_rot_enabled = true;
        ERD::Main::g_FeatureStatus.enemy_resistance_poison_enabled = true;
        ERD::Main::g_FeatureStatus.enemy_resistance_sleep_enabled = true;
        ERD::Main::g_FeatureStatus.enemy_resistance_madness_enabled = true;
        break;
    case EnemyVisualizationToggleAction::DisableAllBars:
        ERD::Main::g_FeatureStatus.enemy_poise_overlay_enabled = false;
        ERD::Main::g_FeatureStatus.enemy_resistance_overlay_enabled = false;
        ERD::Main::g_FeatureStatus.enemy_resistance_bleed_enabled = false;
        ERD::Main::g_FeatureStatus.enemy_resistance_frost_enabled = false;
        ERD::Main::g_FeatureStatus.enemy_resistance_rot_enabled = false;
        ERD::Main::g_FeatureStatus.enemy_resistance_poison_enabled = false;
        ERD::Main::g_FeatureStatus.enemy_resistance_sleep_enabled = false;
        ERD::Main::g_FeatureStatus.enemy_resistance_madness_enabled = false;
        break;
    }
}

void handle_toggle_callback(int payload) {
    const auto action = static_cast<ToggleAction>(payload);
    toggle_action(action);
    grace_test::config::persist_current_feature_state();
    set_toggle_feedback_message(action);
}

void handle_auto_pickup_toggle_callback(int payload) {
    const auto action = static_cast<AutoPickupToggleAction>(payload);
    toggle_auto_pickup_action(action);
    grace_test::config::persist_current_feature_state();
    set_auto_pickup_feedback_message(action);
}

void handle_enemy_visualization_toggle_callback(int payload) {
    const auto action = static_cast<EnemyVisualizationToggleAction>(payload);
    toggle_enemy_visualization_action(action);
    grace_test::config::persist_current_feature_state();
    set_enemy_visualization_feedback_message(action);
}

void handle_unlock_callback(int payload) {
    switch (static_cast<UnlockAction>(payload)) {
    case UnlockAction::Whetblades:
        grace_test::runtime::unlock_all_whetblades();
        set_action_feedback_message_for_label(grace_test::localization::get("unlock_whetblades"));
        break;
    case UnlockAction::Gestures:
        grace_test::runtime::unlock_all_gestures();
        set_action_feedback_message_for_label(grace_test::localization::get("unlock_gestures"));
        break;
    }
}

void handle_unlock_maps_callback(int payload) {
    switch (static_cast<UnlockMapsAction>(payload)) {
    case UnlockMapsAction::All:
        grace_test::runtime::unlock_maps(grace_test::runtime::UnlockMapsMode::All);
        set_action_feedback_message_for_label(grace_test::localization::get("unlock_maps_all"));
        break;
    case UnlockMapsAction::BaseGame:
        grace_test::runtime::unlock_maps(grace_test::runtime::UnlockMapsMode::BaseGame);
        set_action_feedback_message_for_label(grace_test::localization::get("unlock_maps_base"));
        break;
    case UnlockMapsAction::Dlc:
        grace_test::runtime::unlock_maps(grace_test::runtime::UnlockMapsMode::Dlc);
        set_action_feedback_message_for_label(grace_test::localization::get("unlock_maps_dlc"));
        break;
    }
}

void handle_quick_access_callback(int payload) {
    SPDLOG_INFO("Quick access callback entered payload={}", payload);
    switch (static_cast<QuickAccessAction>(payload)) {
    case QuickAccessAction::Upgrade:
        set_action_feedback_message_for_label(grace_test::localization::get("portable_upgrade"));
        break;
    case QuickAccessAction::SpiritTuning:
        set_action_feedback_message_for_label(grace_test::localization::get("portable_spirit_tuning"));
        break;
    case QuickAccessAction::Rebirth:
        set_action_feedback_message_for_label(grace_test::localization::get("portable_rebirth"));
        break;
    case QuickAccessAction::Mirror:
        set_action_feedback_message_for_label(grace_test::localization::get("portable_mirror"));
        break;
    case QuickAccessAction::Sell:
        set_action_feedback_message_for_label(grace_test::localization::get("portable_sell"));
        break;
    }
    queue_quick_access_dispatch(payload);
}

void handle_quick_access_shop_callback(int payload) {
    const auto found = g_quick_access_shop_name_by_id.find(payload);
    if (found != g_quick_access_shop_name_by_id.end()) {
        set_action_feedback_message_for_label(grace_test::localization::utf8_to_wide_copy(found->second));
    } else {
        set_action_feedback_message_for_label(grace_test::localization::get("quick_access_shop"));
    }
    queue_quick_access_dispatch(payload, true);
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
        set_action_feedback_message_for_label(
            grace_test::localization::get("boss_revive_do_revive") + L": " +
            grace_test::localization::utf8_to_wide_copy(g_boss_revive_name_by_id[g_selected_boss_revive_menu_id]));
        break;
    case BossReviveAction::Warp:
        grace_test::runtime::warp_boss_by_menu_id(g_selected_boss_revive_menu_id);
        set_action_feedback_message_for_label(
            grace_test::localization::get("boss_revive_do_warp") + L": " +
            grace_test::localization::utf8_to_wide_copy(g_boss_revive_name_by_id[g_selected_boss_revive_menu_id]));
        break;
    }
}

template <typename T>
void apply_percent_setting(
    std::atomic<T>& target,
    int payload,
    int min_value,
    int max_value,
    const char* localization_key) {
    const int percent = std::clamp(payload, min_value, max_value);
    target = static_cast<T>(percent);
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message(localization_key, percent);
}

void handle_set_equipment_reduction_callback(int payload) {
    const int percent = std::clamp(payload, 0, 100);
    ERD::Main::g_FeatureStatus.equipment_weight_reduction_percent = percent;
    ERD::Main::g_FeatureStatus.weightless_equipment = percent >= 100;
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message("setting_equipment_weight_reduction", percent);
}

void handle_set_weapon_requirement_reduction_callback(int payload) {
    const int percent = std::clamp(payload, 0, 100);
    ERD::Main::g_FeatureStatus.weapon_requirement_reduction_percent = percent;
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message("setting_weapon_requirement_reduction", percent);
}

void handle_set_player_speed_increase_callback(int payload) {
    const int percent = std::clamp(payload, 0, 200);
    ERD::Main::g_FeatureStatus.player_speed_increase_percent = percent;
    ERD::Main::g_FeatureStatus.movement_speed = 1.0f + static_cast<float>(percent) / 100.0f;
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message("setting_player_speed_increase", percent);
}

void handle_set_ng_cycle_callback(int payload) {
    const int cycle = std::clamp(payload, 0, 10);
    ERD::Main::g_FeatureStatus.requested_ng_cycle = cycle;
    ERD::Main::g_FeatureStatus.requested_ng_cycle_apply = true;
    grace_test::config::persist_current_feature_state();
    set_action_feedback_message(grace_test::localization::make_feedback_set_label(
        grace_test::localization::get("setting_ng_cycle"),
        ng_cycle_option_label(cycle)));
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
    set_action_feedback_message(grace_test::localization::make_feedback_set_label(
        grace_test::localization::get("setting_buff_duration_extend"),
        mode == 99999 ? grace_test::localization::get("option_permanent")
                      : grace_test::localization::make_percent_option_label(mode)));
}

void handle_set_weapon_parry_callback(int payload) {
    int mode = 0;
    switch (payload) {
    case 0:
    case 60:
    case 40:
    case 20:
    case 10:
        mode = payload;
        break;
    default:
        mode = 0;
        break;
    }
    ERD::Main::g_FeatureStatus.weapon_parry_mode = mode;
    grace_test::config::persist_current_feature_state();
    set_action_feedback_message(grace_test::localization::make_feedback_set_label(
        grace_test::localization::get("setting_weapon_parry"),
        weapon_parry_mode_label(mode)));
}

void handle_set_enemy_hp_increase_callback(int payload) {
    const int percent = std::clamp(payload, 0, 1000);
    ERD::Main::g_FeatureStatus.enemy_hp_increase_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message("setting_enemy_hp_increase", percent);
}

void handle_set_enemy_hp_decrease_callback(int payload) {
    const int percent = std::clamp(payload, 0, 1000);
    ERD::Main::g_FeatureStatus.enemy_hp_decrease_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.enemy_hp_increase_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message("setting_enemy_hp_decrease", percent);
}

void handle_set_damage_multiplier_callback(int payload) {
    const int percent = std::clamp(payload, 0, 500);
    ERD::Main::g_FeatureStatus.damage_multiplier_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.damage_reduce_multiplier_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message("setting_damage_multiplier_up", percent);
}

void handle_set_damage_cut_multiplier_callback(int payload) {
    const int percent = std::clamp(payload, 0, 500);
    ERD::Main::g_FeatureStatus.damage_cut_multiplier_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.damage_taken_multiplier_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message("setting_damage_multiplier_cut", percent);
}

void handle_set_damage_reduce_multiplier_callback(int payload) {
    const int percent = std::clamp(payload, 0, 500);
    ERD::Main::g_FeatureStatus.damage_reduce_multiplier_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.damage_multiplier_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message("setting_damage_multiplier_reduce", percent);
}

void handle_set_damage_taken_multiplier_callback(int payload) {
    const int percent = std::clamp(payload, 0, 500);
    ERD::Main::g_FeatureStatus.damage_taken_multiplier_percent = percent;
    if (percent > 0) {
        ERD::Main::g_FeatureStatus.damage_cut_multiplier_percent = 0;
    }
    grace_test::config::persist_current_feature_state();
    set_percent_feedback_message("setting_damage_multiplier_taken", percent);
}

void handle_set_stamina_cost_reduction_callback(int payload) {
    apply_percent_setting(
        ERD::Main::g_FeatureStatus.stamina_cost_reduction_percent,
        payload,
        0,
        100,
        "setting_stamina_cost_reduction");
}

void handle_set_fp_cost_reduction_callback(int payload) {
    apply_percent_setting(
        ERD::Main::g_FeatureStatus.fp_cost_reduction_percent,
        payload,
        0,
        100,
        "setting_fp_cost_reduction");
}

void handle_set_auto_pickup_range_callback(int payload) {
    const int percent = std::clamp(payload, 100, 500);
    ERD::Main::g_FeatureStatus.auto_pickup_range_percent = percent;
    grace_test::config::persist_current_feature_state();
    set_action_feedback_message(append_erdtools_source(
        grace_test::localization::make_feedback_set_label(
            grace_test::localization::get("auto_pickup_range"),
            grace_test::localization::make_percent_option_label(percent))));
}

void handle_set_model_scale_callback(int payload) {
    const int percent = std::clamp(payload, 0, 300);
    ERD::Main::g_FeatureStatus.model_scale_uniform = static_cast<float>(percent) / 100.0f;
    grace_test::config::persist_current_feature_state();
    set_action_feedback_message(grace_test::localization::make_feedback_set_label(
        grace_test::localization::get("model_modification"),
        grace_test::localization::make_percent_option_label(percent)));
}

template <std::size_t N>
int cycle_step_value(const std::array<int, N>& steps, int current_value) {
    for (std::size_t index = 0; index < steps.size(); ++index) {
        if (steps[index] == current_value) {
            return steps[(index + 1) % steps.size()];
        }
    }
    return steps[0];
}

void handle_overlay_tuning_callback(int payload) {
    auto& status = ERD::Main::g_FeatureStatus;
    const auto action = static_cast<OverlayTuningAction>(payload);
    switch (action) {
    case OverlayTuningAction::PoiseWidth:
        status.overlay_poise_width_scale =
            static_cast<float>(cycle_step_value(kOverlayScaleSteps, static_cast<int>(std::lround(status.overlay_poise_width_scale.load() * 100.0f)))) / 100.0f;
        break;
    case OverlayTuningAction::PoiseHeight:
        status.overlay_poise_height_scale =
            static_cast<float>(cycle_step_value(kOverlayScaleSteps, static_cast<int>(std::lround(status.overlay_poise_height_scale.load() * 100.0f)))) / 100.0f;
        break;
    case OverlayTuningAction::StatusWidth:
        status.overlay_status_width_scale =
            static_cast<float>(cycle_step_value(kOverlayScaleSteps, static_cast<int>(std::lround(status.overlay_status_width_scale.load() * 100.0f)))) / 100.0f;
        break;
    case OverlayTuningAction::StatusHeight:
        status.overlay_status_height_scale =
            static_cast<float>(cycle_step_value(kOverlayScaleSteps, static_cast<int>(std::lround(status.overlay_status_height_scale.load() * 100.0f)))) / 100.0f;
        break;
    case OverlayTuningAction::PoiseOffset:
        status.overlay_poise_vertical_offset =
            static_cast<float>(cycle_step_value(kOverlayOffsetSteps, static_cast<int>(std::lround(status.overlay_poise_vertical_offset.load()))));
        break;
    case OverlayTuningAction::EntityStatusOffset:
        status.overlay_entity_status_vertical_offset =
            static_cast<float>(cycle_step_value(kOverlayOffsetSteps, static_cast<int>(std::lround(status.overlay_entity_status_vertical_offset.load()))));
        break;
    case OverlayTuningAction::BossStatusGap:
        status.overlay_boss_status_gap =
            static_cast<float>(cycle_step_value(kOverlayBossGapSteps, static_cast<int>(std::lround(status.overlay_boss_status_gap.load()))));
        break;
    }
    grace_test::config::persist_current_feature_state();
    set_action_feedback_message_for_label(overlay_tuning_label(action));
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
                &menu_state,
                true,
                kActionFeedbackMessageId);
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
            states[index] = std::make_unique<callback_state>(
                callback,
                percent,
                &back_state,
                true,
                kActionFeedbackMessageId);
        } else {
            states[index]->set_return_state(&back_state);
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
    set_message(
        msg::event_text_for_talk_ng_mods,
        grace_test::localization::get("ng_mods") + L": " + ng_cycle_option_label(current_ng_cycle()));
    set_message(msg::event_text_for_talk_enemy_visualization, grace_test::localization::get("enemy_visualization"));
    set_message(msg::event_text_for_talk_enemy_mods, grace_test::localization::get("enemy_mods"));
    set_message(msg::event_text_for_talk_boss_revive, grace_test::localization::get("boss_revive"));
    set_message(msg::event_text_for_talk_auto_pickup, grace_test::localization::get("auto_pickup"));
    set_message(msg::event_text_for_talk_quick_access, grace_test::localization::get("quick_access"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    options.emplace_back(10, msg::event_text_for_talk_player_mods, &g_player_mods_menu_state);
    options.emplace_back(11, msg::event_text_for_talk_game_mods, &g_game_mods_menu_state);
    options.emplace_back(12, msg::event_text_for_talk_ng_mods, &g_ng_mods_menu_state);
    options.emplace_back(13, msg::event_text_for_talk_enemy_visualization, &g_enemy_visualization_menu_state);
    options.emplace_back(14, msg::event_text_for_talk_enemy_mods, &g_enemy_mods_menu_state);
    options.emplace_back(15, msg::event_text_for_talk_boss_revive, &g_boss_revive_menu_state);
    options.emplace_back(16, msg::event_text_for_talk_auto_pickup, &g_auto_pickup_menu_state);
    options.emplace_back(17, msg::event_text_for_talk_quick_access, &g_quick_access_menu_state);
    options.emplace_back(99, msg::event_text_for_talk_back, g_grace_initial_state, true);
    g_root_menu_state.rebuild(std::move(options));
}

void rebuild_enemy_visualization_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_enemy_visualization, grace_test::localization::get("enemy_visualization"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    set_message(
        msg::event_text_for_talk_enable_all_enemy_resistance_bars,
        grace_test::localization::get("enable_all_enemy_resistance_bars"));
    set_message(
        msg::event_text_for_talk_disable_all_enemy_resistance_bars,
        grace_test::localization::get("disable_all_enemy_resistance_bars"));
    set_message(msg::event_text_for_talk_overlay_tuning, grace_test::localization::get("overlay_tuning"));

    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < std::size(kEnemyVisualizationToggleDescriptors); ++index) {
        const EnemyVisualizationToggleDescriptor& descriptor = kEnemyVisualizationToggleDescriptors[index];
        set_message(
            descriptor.message_id,
            grace_test::localization::make_toggle_label(
                descriptor.label_key,
                enemy_visualization_toggle_state(descriptor.action)));

        if (!g_enemy_visualization_toggle_states[index]) {
            g_enemy_visualization_toggle_states[index] = std::make_unique<callback_state>(
                &handle_enemy_visualization_toggle_callback,
                static_cast<int>(descriptor.action),
                &g_enemy_visualization_menu_state,
                true,
                kActionFeedbackMessageId);
        } else {
            g_enemy_visualization_toggle_states[index]->set_return_state(&g_enemy_visualization_menu_state);
        }

        options.emplace_back(descriptor.menu_index, descriptor.message_id, g_enemy_visualization_toggle_states[index].get());
    }

    if (!g_enable_all_enemy_resistance_bars_state) {
        g_enable_all_enemy_resistance_bars_state = std::make_unique<callback_state>(
            &handle_enemy_visualization_toggle_callback,
            static_cast<int>(EnemyVisualizationToggleAction::EnableAllBars),
            &g_enemy_visualization_menu_state,
            true,
            kActionFeedbackMessageId);
    } else {
        g_enable_all_enemy_resistance_bars_state->set_return_state(&g_enemy_visualization_menu_state);
    }

    if (!g_disable_all_enemy_resistance_bars_state) {
        g_disable_all_enemy_resistance_bars_state = std::make_unique<callback_state>(
            &handle_enemy_visualization_toggle_callback,
            static_cast<int>(EnemyVisualizationToggleAction::DisableAllBars),
            &g_enemy_visualization_menu_state,
            true,
            kActionFeedbackMessageId);
    } else {
        g_disable_all_enemy_resistance_bars_state->set_return_state(&g_enemy_visualization_menu_state);
    }

    options.emplace_back(10, msg::event_text_for_talk_enable_all_enemy_resistance_bars, g_enable_all_enemy_resistance_bars_state.get());
    options.emplace_back(11, msg::event_text_for_talk_disable_all_enemy_resistance_bars, g_disable_all_enemy_resistance_bars_state.get());
    for (std::size_t index = 0; index < std::size(kOverlayTuningDescriptors); ++index) {
        const OverlayTuningDescriptor& descriptor = kOverlayTuningDescriptors[index];
        set_message(descriptor.message_id, overlay_tuning_label(descriptor.action));
        if (!g_overlay_tuning_states[index]) {
            g_overlay_tuning_states[index] = std::make_unique<callback_state>(
                &handle_overlay_tuning_callback,
                static_cast<int>(descriptor.action),
                &g_enemy_visualization_menu_state,
                true,
                kActionFeedbackMessageId);
        } else {
            g_overlay_tuning_states[index]->set_return_state(&g_enemy_visualization_menu_state);
        }
        options.emplace_back(descriptor.menu_index, descriptor.message_id, g_overlay_tuning_states[index].get());
    }
    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_enemy_visualization_menu_state.rebuild(std::move(options));
}

void rebuild_auto_pickup_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_auto_pickup, grace_test::localization::get("auto_pickup"));
    set_message(msg::event_text_for_talk_auto_pickup_settings, grace_test::localization::get("auto_pickup_settings"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    const AutoPickupToggleDescriptor& descriptor = kAutoPickupRootToggleDescriptors[0];
    set_message(
        descriptor.message_id,
        grace_test::localization::make_toggle_label(descriptor.label_key, auto_pickup_toggle_state(descriptor.action)));

    if (!g_auto_pickup_root_toggle_states[0]) {
        g_auto_pickup_root_toggle_states[0] = std::make_unique<callback_state>(
            &handle_auto_pickup_toggle_callback,
            static_cast<int>(descriptor.action),
            &g_auto_pickup_menu_state,
            true,
            kActionFeedbackMessageId);
    } else {
        g_auto_pickup_root_toggle_states[0]->set_return_state(&g_auto_pickup_menu_state);
    }

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, descriptor.message_id, g_auto_pickup_root_toggle_states[0].get());
    options.emplace_back(2, msg::event_text_for_talk_auto_pickup_settings, &g_auto_pickup_settings_menu_state);
    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_auto_pickup_menu_state.rebuild(std::move(options));
}

void rebuild_auto_pickup_settings_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_auto_pickup_settings, grace_test::localization::get("auto_pickup_settings"));
    set_message(
        msg::event_text_for_talk_auto_pickup_range,
        grace_test::localization::make_percent_setting_label("auto_pickup_range", current_auto_pickup_range_percent()));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < std::size(kAutoPickupSettingsToggleDescriptors); ++index) {
        const AutoPickupToggleDescriptor& descriptor = kAutoPickupSettingsToggleDescriptors[index];
        set_message(
            descriptor.message_id,
            grace_test::localization::make_toggle_label(
                descriptor.label_key,
                auto_pickup_toggle_state(descriptor.action)));

        if (!g_auto_pickup_settings_toggle_states[index]) {
            g_auto_pickup_settings_toggle_states[index] = std::make_unique<callback_state>(
                &handle_auto_pickup_toggle_callback,
                static_cast<int>(descriptor.action),
                &g_auto_pickup_settings_menu_state,
                true,
                kActionFeedbackMessageId);
        } else {
            g_auto_pickup_settings_toggle_states[index]->set_return_state(&g_auto_pickup_settings_menu_state);
        }

        options.emplace_back(
            descriptor.menu_index,
            descriptor.message_id,
            g_auto_pickup_settings_toggle_states[index].get());
    }

    options.emplace_back(6, msg::event_text_for_talk_auto_pickup_range, &g_auto_pickup_range_menu_state);
    options.emplace_back(99, msg::event_text_for_talk_back, &g_auto_pickup_menu_state, true);
    g_auto_pickup_settings_menu_state.rebuild(std::move(options));
}

void rebuild_auto_pickup_range_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < kAutoPickupRangeSteps.size(); ++index) {
        const int percent = kAutoPickupRangeSteps[index];
        set_message(
            msg::event_text_for_talk_auto_pickup_range_base + static_cast<int>(index),
            grace_test::localization::make_percent_option_label(percent));

        if (!g_auto_pickup_range_states[index]) {
            g_auto_pickup_range_states[index] = std::make_unique<callback_state>(
                &handle_set_auto_pickup_range_callback,
                percent,
                &g_auto_pickup_settings_menu_state,
                true,
                kActionFeedbackMessageId);
        } else {
            g_auto_pickup_range_states[index]->set_return_state(&g_auto_pickup_settings_menu_state);
        }

        options.emplace_back(
            static_cast<int>(index + 1),
            msg::event_text_for_talk_auto_pickup_range_base + static_cast<int>(index),
            g_auto_pickup_range_states[index].get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &g_auto_pickup_settings_menu_state, true);
    g_auto_pickup_range_menu_state.rebuild(std::move(options));
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
        msg::event_text_for_talk_weapon_parry,
        grace_test::localization::get("setting_weapon_parry") + L": " +
            weapon_parry_mode_label(current_weapon_parry_mode()));
    set_message(
        msg::event_text_for_talk_damage_multiplier,
        grace_test::localization::get("setting_damage_multiplier_menu"));
    set_message(
        msg::event_text_for_talk_damage_multiplier_up,
        grace_test::localization::make_percent_setting_label(
            "setting_damage_multiplier_up",
            current_damage_multiplier_percent()));
    set_message(
        msg::event_text_for_talk_damage_cut_menu,
        grace_test::localization::get("setting_damage_cut_menu"));
    set_message(
        msg::event_text_for_talk_stamina_cost_reduction,
        grace_test::localization::make_percent_setting_label(
            "setting_stamina_cost_reduction",
            current_stamina_cost_reduction_percent()));
    set_message(
        msg::event_text_for_talk_fp_cost_reduction,
        grace_test::localization::make_percent_setting_label(
            "setting_fp_cost_reduction",
            current_fp_cost_reduction_percent()));
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
    options.emplace_back(11, msg::event_text_for_talk_weapon_parry, &g_weapon_parry_menu_state);
    options.emplace_back(4, msg::event_text_for_talk_damage_multiplier, &g_damage_multiplier_menu_state);
    options.emplace_back(5, msg::event_text_for_talk_damage_cut_menu, &g_damage_cut_menu_state);
    options.emplace_back(6, msg::event_text_for_talk_stamina_cost_reduction, &g_stamina_cost_reduction_menu_state);
    options.emplace_back(7, msg::event_text_for_talk_fp_cost_reduction, &g_fp_cost_reduction_menu_state);
    for (std::size_t index = 0; index < std::size(kPlayerToggleDescriptors); ++index) {
        const ToggleDescriptor& descriptor = kPlayerToggleDescriptors[index];
        set_message(
            descriptor.message_id,
            grace_test::localization::make_toggle_label(descriptor.label_key, toggle_state(descriptor.action)));

        if (!g_player_toggle_states[index]) {
            g_player_toggle_states[index] = std::make_unique<callback_state>(
                &handle_toggle_callback,
                static_cast<int>(descriptor.action),
                &g_player_mods_menu_state,
                true,
                kActionFeedbackMessageId);
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
                &g_game_mods_menu_state,
                true,
                kActionFeedbackMessageId);
        } else {
            g_game_toggle_states[index]->set_return_state(&g_game_mods_menu_state);
        }

        options.emplace_back(descriptor.menu_index, descriptor.message_id, g_game_toggle_states[index].get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_game_mods_menu_state.rebuild(std::move(options));
}

void rebuild_ng_mods_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_ng_mods, grace_test::localization::get("ng_mods"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < kNgCycleSteps.size(); ++index) {
        const int cycle = kNgCycleSteps[index];
        const int message_id = msg::event_text_for_talk_ng_cycle_base + static_cast<int>(index);
        set_message(message_id, ng_cycle_option_label(cycle));
        if (!g_ng_cycle_states[index]) {
            g_ng_cycle_states[index] = std::make_unique<callback_state>(
                &handle_set_ng_cycle_callback,
                cycle,
                &g_ng_mods_menu_state,
                true,
                kActionFeedbackMessageId);
        } else {
            g_ng_cycle_states[index]->set_return_state(&g_ng_mods_menu_state);
        }
        options.emplace_back(static_cast<int>(index + 10), message_id, g_ng_cycle_states[index].get());
    }
    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_ng_mods_menu_state.rebuild(std::move(options));
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
            g_selected_boss_parent_menu,
            true,
            kActionFeedbackMessageId);
    } else {
        g_boss_revive_action_states[0]->set_return_state(g_selected_boss_parent_menu);
    }

    if (!g_boss_revive_action_states[1]) {
        g_boss_revive_action_states[1] = std::make_unique<callback_state>(
            &handle_boss_revive_action_callback,
            static_cast<int>(BossReviveAction::Warp),
            g_selected_boss_parent_menu,
            true,
            kActionFeedbackMessageId);
    } else {
        g_boss_revive_action_states[1]->set_return_state(g_selected_boss_parent_menu);
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
            &g_unlocks_menu_state,
            true,
            kActionFeedbackMessageId);
    } else {
        g_unlock_states[0]->set_return_state(&g_unlocks_menu_state);
    }

    if (!g_unlock_states[1]) {
        g_unlock_states[1] = std::make_unique<callback_state>(
            &handle_unlock_callback,
            static_cast<int>(UnlockAction::Gestures),
            &g_unlocks_menu_state,
            true,
            kActionFeedbackMessageId);
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
            &g_unlocks_menu_state,
            true,
            kActionFeedbackMessageId);
    } else {
        g_unlock_map_states[0]->set_return_state(&g_unlocks_menu_state);
    }

    if (!g_unlock_map_states[1]) {
        g_unlock_map_states[1] = std::make_unique<callback_state>(
            &handle_unlock_maps_callback,
            static_cast<int>(UnlockMapsAction::BaseGame),
            &g_unlocks_menu_state,
            true,
            kActionFeedbackMessageId);
    } else {
        g_unlock_map_states[1]->set_return_state(&g_unlocks_menu_state);
    }

    if (!g_unlock_map_states[2]) {
        g_unlock_map_states[2] = std::make_unique<callback_state>(
            &handle_unlock_maps_callback,
            static_cast<int>(UnlockMapsAction::Dlc),
            &g_unlocks_menu_state,
            true,
            kActionFeedbackMessageId);
    } else {
        g_unlock_map_states[2]->set_return_state(&g_unlocks_menu_state);
    }

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_unlock_maps_all, g_unlock_map_states[0].get());
    options.emplace_back(2, msg::event_text_for_talk_unlock_maps_base, g_unlock_map_states[1].get());
    options.emplace_back(3, msg::event_text_for_talk_unlock_maps_dlc, g_unlock_map_states[2].get());
    options.emplace_back(99, msg::event_text_for_talk_back, &g_unlocks_menu_state, true);
    g_unlock_maps_menu_state.rebuild(std::move(options));
}

void rebuild_quick_access_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_quick_access, grace_test::localization::get("quick_access"));
    set_message(msg::event_text_for_talk_portable_upgrade, grace_test::localization::get("portable_upgrade"));
    set_message(msg::event_text_for_talk_portable_spirit_tuning, grace_test::localization::get("portable_spirit_tuning"));
    set_message(msg::event_text_for_talk_portable_rebirth, grace_test::localization::get("portable_rebirth"));
    set_message(msg::event_text_for_talk_portable_mirror, grace_test::localization::get("portable_mirror"));
    set_message(msg::event_text_for_talk_portable_sell, grace_test::localization::get("portable_sell"));
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    constexpr std::array<QuickAccessAction, 5> kQuickAccessActions{{
        QuickAccessAction::Upgrade,
        QuickAccessAction::SpiritTuning,
        QuickAccessAction::Rebirth,
        QuickAccessAction::Mirror,
        QuickAccessAction::Sell,
    }};
    for (std::size_t index = 0; index < kQuickAccessActions.size(); ++index) {
        if (!g_quick_access_states[index]) {
            g_quick_access_states[index] = std::make_unique<callback_state>(
                &handle_quick_access_callback,
                static_cast<int>(kQuickAccessActions[index]),
                &g_quick_access_menu_state);
        } else {
            g_quick_access_states[index]->set_return_state(&g_quick_access_menu_state);
        }
    }

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_portable_upgrade, g_quick_access_states[0].get());
    options.emplace_back(2, msg::event_text_for_talk_portable_spirit_tuning, g_quick_access_states[1].get());
    options.emplace_back(3, msg::event_text_for_talk_portable_rebirth, g_quick_access_states[2].get());
    options.emplace_back(4, msg::event_text_for_talk_portable_mirror, g_quick_access_states[3].get());
    options.emplace_back(5, msg::event_text_for_talk_portable_sell, g_quick_access_states[4].get());

    g_quick_access_shop_states.clear();
    g_quick_access_shop_name_by_id.clear();
    const std::vector<grace_test::runtime::QuickAccessShopMenuItem> shop_items =
        grace_test::runtime::get_quick_access_shop_menu_items();
    g_quick_access_shop_states.reserve(shop_items.size());
    int shop_message_index = 0;
    int menu_index = 10;
    for (const auto& shop : shop_items) {
        const int message_id = msg::event_text_for_talk_quick_access_shop_base + shop_message_index++;
        set_message(message_id, grace_test::localization::utf8_to_wide_copy(shop.name));
        g_quick_access_shop_name_by_id[shop.id] = shop.name;
        g_quick_access_shop_states.push_back(std::make_unique<callback_state>(
            &handle_quick_access_shop_callback,
            shop.id,
            &g_quick_access_menu_state));
        options.emplace_back(menu_index++, message_id, g_quick_access_shop_states.back().get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &g_root_menu_state, true);
    g_quick_access_menu_state.rebuild(std::move(options));
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
            g_buff_duration_extend_states[index] = std::make_unique<callback_state>(
                &handle_set_buff_duration_extend_callback,
                mode,
                &g_player_mods_menu_state,
                true,
                kActionFeedbackMessageId);
        } else {
            g_buff_duration_extend_states[index]->set_return_state(&g_player_mods_menu_state);
        }

        options.emplace_back(
            static_cast<int>(index + 1),
            msg::event_text_for_talk_buff_duration_extend_base + static_cast<int>(index),
            g_buff_duration_extend_states[index].get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &g_player_mods_menu_state, true);
    g_buff_duration_extend_menu_state.rebuild(std::move(options));
}

void rebuild_weapon_parry_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    std::vector<talkscript_menu_option> options;
    for (std::size_t index = 0; index < kWeaponParryModes.size(); ++index) {
        const int mode = kWeaponParryModes[index];
        set_message(
            msg::event_text_for_talk_weapon_parry_base + static_cast<int>(index),
            weapon_parry_mode_label(mode));

        if (!g_weapon_parry_states[index]) {
            g_weapon_parry_states[index] = std::make_unique<callback_state>(
                &handle_set_weapon_parry_callback,
                mode,
                &g_player_mods_menu_state,
                true,
                kActionFeedbackMessageId);
        } else {
            g_weapon_parry_states[index]->set_return_state(&g_player_mods_menu_state);
        }

        options.emplace_back(
            static_cast<int>(index + 1),
            msg::event_text_for_talk_weapon_parry_base + static_cast<int>(index),
            g_weapon_parry_states[index].get());
    }

    options.emplace_back(99, msg::event_text_for_talk_back, &g_player_mods_menu_state, true);
    g_weapon_parry_menu_state.rebuild(std::move(options));
}

void rebuild_stamina_cost_reduction_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kPercentSteps,
        msg::event_text_for_talk_stamina_cost_reduction_base,
        g_stamina_cost_reduction_states,
        &handle_set_stamina_cost_reduction_callback,
        g_stamina_cost_reduction_menu_state,
        g_player_mods_menu_state);
}

void rebuild_fp_cost_reduction_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    rebuild_percent_menu(
        kPercentSteps,
        msg::event_text_for_talk_fp_cost_reduction_base,
        g_fp_cost_reduction_states,
        &handle_set_fp_cost_reduction_callback,
        g_fp_cost_reduction_menu_state,
        g_player_mods_menu_state);
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
        g_damage_multiplier_menu_state);
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
        g_damage_cut_menu_state);
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
        g_damage_multiplier_menu_state);
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
        g_damage_cut_menu_state);
}

void rebuild_damage_multiplier_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    set_message(
        msg::event_text_for_talk_damage_multiplier,
        grace_test::localization::get("setting_damage_multiplier_menu"));
    set_message(
        msg::event_text_for_talk_damage_multiplier_up,
        grace_test::localization::make_percent_setting_label(
            "setting_damage_multiplier_up",
            current_damage_multiplier_percent()));
    set_message(
        msg::event_text_for_talk_damage_multiplier_reduce,
        grace_test::localization::make_percent_setting_label(
            "setting_damage_multiplier_reduce",
            current_damage_reduce_multiplier_percent()));

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_damage_multiplier_up, &g_damage_multiplier_up_menu_state);
    options.emplace_back(2, msg::event_text_for_talk_damage_multiplier_reduce, &g_damage_multiplier_reduce_menu_state);
    options.emplace_back(99, msg::event_text_for_talk_back, &g_player_mods_menu_state, true);
    g_damage_multiplier_menu_state.rebuild(std::move(options));
}

void rebuild_damage_cut_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));
    set_message(
        msg::event_text_for_talk_damage_cut_menu,
        grace_test::localization::get("setting_damage_cut_menu"));
    set_message(
        msg::event_text_for_talk_damage_multiplier_cut,
        grace_test::localization::make_percent_setting_label(
            "setting_damage_multiplier_cut",
            current_damage_cut_multiplier_percent()));
    set_message(
        msg::event_text_for_talk_damage_multiplier_taken,
        grace_test::localization::make_percent_setting_label(
            "setting_damage_multiplier_taken",
            current_damage_taken_multiplier_percent()));

    std::vector<talkscript_menu_option> options;
    options.emplace_back(1, msg::event_text_for_talk_damage_multiplier_cut, &g_damage_multiplier_cut_menu_state);
    options.emplace_back(2, msg::event_text_for_talk_damage_multiplier_taken, &g_damage_multiplier_taken_menu_state);
    options.emplace_back(99, msg::event_text_for_talk_back, &g_player_mods_menu_state, true);
    g_damage_cut_menu_state.rebuild(std::move(options));
}

void rebuild_model_modification_menu() {
    grace_test::localization::refresh();
    set_message(msg::event_text_for_talk_back, grace_test::localization::get("back"));

    rebuild_percent_menu(
        kModelScaleSteps,
        msg::event_text_for_talk_model_scale_width_base,
        g_model_scale_states,
        &handle_set_model_scale_callback,
        g_model_modification_menu_state,
        g_player_mods_menu_state);
}

void rebuild_all_menus() {
    g_message_cache.clear();
    rebuild_root_menu();
    rebuild_player_mods_menu();
    rebuild_game_mods_menu();
    rebuild_ng_mods_menu();
    rebuild_enemy_visualization_menu();
    rebuild_auto_pickup_menu();
    rebuild_auto_pickup_settings_menu();
    rebuild_auto_pickup_range_menu();
    rebuild_enemy_mods_menu();
    rebuild_enemy_hp_increase_menu();
    rebuild_enemy_hp_decrease_menu();
    rebuild_boss_revive_menu();
    rebuild_boss_revive_action_menu();
    rebuild_unlocks_menu();
    rebuild_unlock_maps_menu();
    rebuild_quick_access_menu();
    rebuild_equipment_reduction_menu();
    rebuild_weapon_requirement_reduction_menu();
    rebuild_player_speed_menu();
    rebuild_buff_duration_extend_menu();
    rebuild_weapon_parry_menu();
    rebuild_stamina_cost_reduction_menu();
    rebuild_fp_cost_reduction_menu();
    rebuild_damage_multiplier_menu();
    rebuild_damage_cut_menu();
    rebuild_damage_multiplier_up_menu();
    rebuild_damage_multiplier_cut_menu();
    rebuild_damage_multiplier_reduce_menu();
    rebuild_damage_multiplier_taken_menu();
    rebuild_model_modification_menu();
}

callback_state* find_callback_state(er::ezstate::state* state) {
    for (auto& callback : g_auto_pickup_root_toggle_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_auto_pickup_settings_toggle_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_enemy_visualization_toggle_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_overlay_tuning_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    if (g_enable_all_enemy_resistance_bars_state && g_enable_all_enemy_resistance_bars_state.get() == state) {
        return g_enable_all_enemy_resistance_bars_state.get();
    }
    if (g_disable_all_enemy_resistance_bars_state && g_disable_all_enemy_resistance_bars_state.get() == state) {
        return g_disable_all_enemy_resistance_bars_state.get();
    }
    for (auto& callback : g_auto_pickup_range_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
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
    for (auto& callback : g_quick_access_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_quick_access_shop_states) {
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
    for (auto& callback : g_ng_cycle_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_buff_duration_extend_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_weapon_parry_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_stamina_cost_reduction_states) {
        if (callback && callback.get() == state) {
            return callback.get();
        }
    }
    for (auto& callback : g_fp_cost_reduction_states) {
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
    for (auto& callback : g_model_scale_states) {
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
        } else if (state == &g_ng_mods_menu_state) {
            rebuild_ng_mods_menu();
        } else if (state == &g_enemy_visualization_menu_state) {
            rebuild_enemy_visualization_menu();
        } else if (state == &g_enemy_mods_menu_state) {
            rebuild_enemy_mods_menu();
        } else if (state == &g_enemy_hp_increase_menu_state) {
            rebuild_enemy_hp_increase_menu();
        } else if (state == &g_enemy_hp_decrease_menu_state) {
            rebuild_enemy_hp_decrease_menu();
        } else if (state == &g_auto_pickup_menu_state) {
            rebuild_auto_pickup_menu();
        } else if (state == &g_auto_pickup_settings_menu_state) {
            rebuild_auto_pickup_settings_menu();
        } else if (state == &g_auto_pickup_range_menu_state) {
            rebuild_auto_pickup_range_menu();
        } else if (state == &g_quick_access_menu_state) {
            rebuild_quick_access_menu();
        } else if (state == &g_boss_revive_menu_state) {
            rebuild_boss_revive_menu();
        } else if (state == &g_boss_revive_action_menu_state) {
            rebuild_boss_revive_action_menu();
        } else if (state == &g_unlocks_menu_state) {
            rebuild_unlocks_menu();
        } else if (state == &g_unlock_maps_menu_state) {
            rebuild_unlock_maps_menu();
        } else if (state == &g_equipment_reduction_menu_state) {
            rebuild_equipment_reduction_menu();
        } else if (state == &g_weapon_requirement_reduction_menu_state) {
            rebuild_weapon_requirement_reduction_menu();
        } else if (state == &g_player_speed_menu_state) {
            rebuild_player_speed_menu();
        } else if (state == &g_buff_duration_extend_menu_state) {
            rebuild_buff_duration_extend_menu();
        } else if (state == &g_weapon_parry_menu_state) {
            rebuild_weapon_parry_menu();
        } else if (state == &g_model_modification_menu_state) {
            rebuild_model_modification_menu();
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

void reset_runtime_state() {
    g_message_cache.clear();
    g_boss_revive_area_menu_states.clear();
    g_boss_revive_callback_states.clear();
    g_boss_revive_parent_menu_by_id.clear();
    g_boss_revive_name_by_id.clear();
    g_quick_access_shop_states.clear();
    g_quick_access_shop_name_by_id.clear();
    g_selected_boss_revive_menu_id = -1;
    g_selected_boss_parent_menu = &g_boss_revive_menu_state;
    g_grace_initial_state = nullptr;
    g_patched_events.fill({});
    g_patched_transitions.fill(nullptr);
}

const wchar_t* try_get_custom_message(int msg_id) {
    const auto found = g_message_cache.find(msg_id);
    return found == g_message_cache.end() ? nullptr : found->second.c_str();
}

}  // namespace grace_test::talkscript
