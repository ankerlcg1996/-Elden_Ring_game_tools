#include "Runtime.hpp"

#include "../Game/Memory.hpp"
#include "Logger.hpp"

#include <cstdio>

namespace ERD::Main {
namespace {

constexpr uintptr_t kSaveSlotOffset = 0xAC0;
constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kCharacterFlagsBlockOffset = 0x190;
constexpr uintptr_t kCommonFlagsOffset = 0x19B;

void LogProtectedException(const char* scope, const char* detail) {
    Logger::Instance().Error((std::string(scope) + " failed: " + detail).c_str());
}

#define ERD_PROTECTED_STEP(SCOPE, CALL)                           \
    do {                                                          \
        try {                                                     \
            CALL;                                                 \
        } catch (const std::exception& ex) {                      \
            LogProtectedException(SCOPE, ex.what());              \
        } catch (...) {                                           \
            LogProtectedException(SCOPE, "unknown exception");     \
        }                                                         \
    } while (false)

bool IsKnownOverlayConflictLoaded() {
    return GetModuleHandleW(L"PostureBarMod.dll") != nullptr;
}

}  // namespace

Runtime::Runtime(ModPaths paths, ModConfig config)
    : paths_(std::move(paths)), config_(std::move(config)) {}

void Runtime::Run() {
    MaybeCreateConsole();
    LogStartup();
    InitializeFeatureStatus();

    // Overlay 初始化失败不影响功能本体，最多只是菜单不可用。
    // PostureBarMod 也会 hook DX12 Present，和当前菜单实现会互相踩。
    if (IsKnownOverlayConflictLoaded()) {
        Logger::Instance().Error(
            "Detected PostureBarMod.dll. Skipping ERDMod D3D12 overlay hook to avoid a DX12 hook conflict."
        );
        Logger::Instance().Info(
            "ERDMod now includes an integrated posture bar overlay. Remove PostureBarMod.dll if you want to use it."
        );
        Logger::Instance().Info("Core features stay available via Config/ERDMod.ini, but the in-game ERDMod menu is disabled.");
    } else if (g_D3D12Overlay && !g_D3D12Overlay->Hook()) {
        Logger::Instance().Error("Failed to hook D3D12 overlay. Overlay UI will stay unavailable.");
    }

    RunFeatureLoop();
}

void Runtime::MaybeCreateConsole() const {
    if (!config_.create_console) {
        return;
    }

    if (GetConsoleWindow() == nullptr) {
        AllocConsole();
        RedirectStdoutToConsole();
    }
}

void Runtime::RedirectStdoutToConsole() const {
    FILE* ignored = nullptr;
    freopen_s(&ignored, "CONOUT$", "w", stdout);
    freopen_s(&ignored, "CONOUT$", "w", stderr);
}

void Runtime::LogStartup() const {
    Logger::Instance().Info("ERDMod runtime initialized.");
    Logger::Instance().Info(("DLL directory: " + paths_.DllDirectory().string()).c_str());
    Logger::Instance().Info(config_.no_dead ? "No dead enabled." : "No dead disabled.");
    Logger::Instance().Info(config_.no_damage ? "No damage enabled." : "No damage disabled.");
    Logger::Instance().Info(config_.infinite_fp ? "Infinite FP enabled." : "Infinite FP disabled.");
    Logger::Instance().Info(config_.infinite_items ? "Infinite item usage enabled." : "Infinite item usage disabled.");
    Logger::Instance().Info(config_.no_stamina_consumption ? "No stamina consumption enabled." : "No stamina consumption disabled.");
    Logger::Instance().Info(config_.unlock_all_maps ? "Unlock all maps enabled." : "Unlock all maps disabled.");
    Logger::Instance().Info(config_.unlock_all_cookbooks ? "Unlock all cookbooks enabled." : "Unlock all cookbooks disabled.");
    Logger::Instance().Info(config_.unlock_all_whetblades ? "Unlock all whetblades enabled." : "Unlock all whetblades disabled.");
    Logger::Instance().Info(config_.unlock_all_graces ? "Unlock all graces enabled." : "Unlock all graces disabled.");
    Logger::Instance().Info(
        config_.unlock_all_summoning_pools
            ? "Unlock all summoning pools enabled."
            : "Unlock all summoning pools disabled."
    );
    Logger::Instance().Info(config_.unlock_all_colosseums ? "Unlock all colosseums enabled." : "Unlock all colosseums disabled.");
    Logger::Instance().Info(config_.unlock_all_gestures ? "Unlock all gestures enabled." : "Unlock all gestures disabled.");
    Logger::Instance().Info(config_.faster_respawn ? "Faster respawn enabled." : "Faster respawn disabled.");
    Logger::Instance().Info(
        config_.warp_out_of_uncleared_minidungeons
            ? "Mini-dungeon warp patch enabled."
            : "Mini-dungeon warp patch disabled."
    );
    Logger::Instance().Info(config_.free_purchase ? "Free purchase enabled." : "Free purchase disabled.");
    Logger::Instance().Info(
        config_.no_crafting_material_cost
            ? "No crafting material cost enabled."
            : "No crafting material cost disabled."
    );
    Logger::Instance().Info(
        config_.no_upgrade_material_cost
            ? "No upgrade material cost enabled."
            : "No upgrade material cost disabled."
    );
    Logger::Instance().Info(
        config_.no_magic_requirements
            ? "No magic requirements enabled."
            : "No magic requirements disabled."
    );
    Logger::Instance().Info(
        config_.all_magic_one_slot
            ? "All magic one slot enabled."
            : "All magic one slot disabled."
    );
    Logger::Instance().Info(
        config_.weightless_equipment
            ? "Weightless equipment enabled."
            : "Weightless equipment disabled."
    );
    Logger::Instance().Info(
        config_.custom_fov_enabled
            ? ("Custom FOV enabled: " + std::to_string(config_.custom_fov_value)).c_str()
            : "Custom FOV disabled."
    );
    Logger::Instance().Info(
        config_.custom_camera_distance_enabled
            ? ("Custom camera distance enabled: " + std::to_string(config_.custom_camera_distance_value)).c_str()
            : "Custom camera distance disabled."
    );
    Logger::Instance().Info(config_.mount_anywhere ? "Mount anywhere enabled." : "Mount anywhere disabled.");
    Logger::Instance().Info(
        config_.spirit_ashes_anywhere
            ? "Spirit ashes anywhere enabled."
            : "Spirit ashes anywhere disabled."
    );
    Logger::Instance().Info(
        ("Item discovery multiplier: x" + std::to_string(std::clamp(config_.item_discovery_multiplier, 1, 100))).c_str()
    );
    Logger::Instance().Info(config_.permanent_lantern ? "Permanent lantern enabled." : "Permanent lantern disabled.");
    Logger::Instance().Info(config_.invisible_helmets ? "Invisible helmets enabled." : "Invisible helmets disabled.");
    Logger::Instance().Info(config_.no_clip_enabled ? "NoClip enabled." : "NoClip disabled.");
    Logger::Instance().Info(config_.freecam_enabled ? "FreeCam enabled." : "FreeCam disabled.");
    Logger::Instance().Info(config_.no_fall_camera ? "No Fall Camera enabled." : "No Fall Camera disabled.");
    Logger::Instance().Info(("Movement speed: " + std::to_string(config_.movement_speed)).c_str());
    Logger::Instance().Info(config_.start_next_cycle ? "Start next cycle requested." : "Start next cycle disabled.");
    Logger::Instance().Info(("Default rune amount: " + std::to_string(std::max(config_.give_runes_amount, 1))).c_str());
    Logger::Instance().Info(
        ("Default custom item: id " + std::to_string(config_.custom_item_id) +
         " qty " + std::to_string(std::clamp(config_.custom_item_quantity, 1, 9999))).c_str()
    );
    Logger::Instance().Info(
        ("Default gesture editor: slot " + std::to_string(std::clamp(config_.gesture_slot, 0, 6)) +
         " id " + std::to_string(config_.gesture_item_id)).c_str()
    );
    Logger::Instance().Info(
        ("Default flask level input: " + std::to_string(std::clamp(config_.set_flask_level_value, 0, 12))).c_str()
    );
    Logger::Instance().Info(
        ("Default event flag debug ID: " + std::to_string(std::max(config_.debug_event_flag_id, 0))).c_str()
    );
    Logger::Instance().Info(
        ("Default teleport coords: X " + std::to_string(config_.teleport_coord_x) +
         " | Y " + std::to_string(config_.teleport_coord_y) +
         " | Z " + std::to_string(config_.teleport_coord_z)).c_str()
    );

    if (config_.show_message_box) {
        MessageBoxW(nullptr, config_.message_box_text.c_str(), L"ERDMod", MB_OK | MB_ICONINFORMATION);
    }
}

// 把 ini 里的默认值注入运行时共享状态。
// 后面游戏内菜单的切换只改 g_FeatureStatus，不会每帧反复回读配置。
void Runtime::InitializeFeatureStatus() const {
    g_FeatureStatus.no_dead = config_.no_dead;
    g_FeatureStatus.no_damage = config_.no_damage;
    g_FeatureStatus.infinite_fp = config_.infinite_fp;
    g_FeatureStatus.infinite_items = config_.infinite_items;
    g_FeatureStatus.no_stamina_consumption = config_.no_stamina_consumption;

    g_FeatureStatus.unlock_all_maps = config_.unlock_all_maps;
    g_FeatureStatus.unlock_all_cookbooks = config_.unlock_all_cookbooks;
    g_FeatureStatus.unlock_all_whetblades = config_.unlock_all_whetblades;
    g_FeatureStatus.unlock_all_graces = config_.unlock_all_graces;
    g_FeatureStatus.unlock_all_summoning_pools = config_.unlock_all_summoning_pools;
    g_FeatureStatus.unlock_all_colosseums = config_.unlock_all_colosseums;
    g_FeatureStatus.unlock_all_gestures = config_.unlock_all_gestures;

    g_FeatureStatus.faster_respawn = config_.faster_respawn;
    g_FeatureStatus.warp_out_of_uncleared_minidungeons = config_.warp_out_of_uncleared_minidungeons;
    g_FeatureStatus.free_purchase = config_.free_purchase;
    g_FeatureStatus.no_crafting_material_cost = config_.no_crafting_material_cost;
    g_FeatureStatus.no_upgrade_material_cost = config_.no_upgrade_material_cost;
    g_FeatureStatus.no_magic_requirements = config_.no_magic_requirements;
    g_FeatureStatus.all_magic_one_slot = config_.all_magic_one_slot;
    g_FeatureStatus.weightless_equipment = config_.weightless_equipment;
    g_FeatureStatus.custom_fov_enabled = config_.custom_fov_enabled;
    g_FeatureStatus.custom_fov_value = std::clamp(config_.custom_fov_value, 20.0f, 120.0f);
    g_FeatureStatus.custom_camera_distance_enabled = config_.custom_camera_distance_enabled;
    g_FeatureStatus.custom_camera_distance_value =
        std::clamp(config_.custom_camera_distance_value, 1.0f, 20.0f);
    g_FeatureStatus.mount_anywhere = config_.mount_anywhere;
    g_FeatureStatus.spirit_ashes_anywhere = config_.spirit_ashes_anywhere;
    g_FeatureStatus.item_discovery_multiplier = std::clamp(config_.item_discovery_multiplier, 1, 100);
    g_FeatureStatus.permanent_lantern = config_.permanent_lantern;
    g_FeatureStatus.invisible_helmets = config_.invisible_helmets;
    g_FeatureStatus.no_clip_enabled = config_.no_clip_enabled;
    g_FeatureStatus.freecam_enabled = config_.freecam_enabled;
    g_FeatureStatus.no_fall_camera = config_.no_fall_camera;
    g_FeatureStatus.movement_speed = std::clamp(config_.movement_speed, 0.1f, 20.0f);

    g_FeatureStatus.start_next_cycle = config_.start_next_cycle;
    g_FeatureStatus.give_runes_amount = std::max(config_.give_runes_amount, 1);
    g_FeatureStatus.give_runes_requested = config_.give_runes_requested;
    g_FeatureStatus.custom_item_id = config_.custom_item_id;
    g_FeatureStatus.custom_item_quantity = std::clamp(config_.custom_item_quantity, 1, 9999);
    g_FeatureStatus.custom_item_reinforce_level = std::clamp(config_.custom_item_reinforce_level, 0, 25);
    g_FeatureStatus.custom_item_upgrade_value = std::clamp(config_.custom_item_upgrade_value, 0, 1200);
    g_FeatureStatus.custom_item_gem_id = config_.custom_item_gem_id;
    g_FeatureStatus.custom_item_give_requested = config_.custom_item_give_requested;
    g_FeatureStatus.mass_item_gib_request_id = std::max(config_.mass_item_gib_request_id, 0);
    g_FeatureStatus.add_flask_charge_type = std::clamp(config_.add_flask_charge_type, 0, 1);
    g_FeatureStatus.add_flask_charge_requested = config_.add_flask_charge_requested;
    g_FeatureStatus.set_flask_level_value = std::clamp(config_.set_flask_level_value, 0, 12);
    g_FeatureStatus.set_flask_level_requested = config_.set_flask_level_requested;
    g_FeatureStatus.debug_event_flag_id = std::max(config_.debug_event_flag_id, 0);
    g_FeatureStatus.debug_event_flag_current_state = -1;
    g_FeatureStatus.debug_event_flag_desired_state = config_.debug_event_flag_desired_state;
    g_FeatureStatus.debug_event_flag_write_requested = config_.debug_event_flag_write_requested;
    g_FeatureStatus.teleport_coord_x = config_.teleport_coord_x;
    g_FeatureStatus.teleport_coord_y = config_.teleport_coord_y;
    g_FeatureStatus.teleport_coord_z = config_.teleport_coord_z;
    g_FeatureStatus.teleport_player_to_input_coords_requested = config_.teleport_player_to_input_coords_requested;
    g_FeatureStatus.character_level = std::clamp(config_.character_level, 1, 713);
    g_FeatureStatus.character_vigor = std::clamp(config_.character_vigor, 1, 99);
    g_FeatureStatus.character_mind = std::clamp(config_.character_mind, 1, 99);
    g_FeatureStatus.character_endurance = std::clamp(config_.character_endurance, 1, 99);
    g_FeatureStatus.character_strength = std::clamp(config_.character_strength, 1, 99);
    g_FeatureStatus.character_dexterity = std::clamp(config_.character_dexterity, 1, 99);
    g_FeatureStatus.character_intelligence = std::clamp(config_.character_intelligence, 1, 99);
    g_FeatureStatus.character_faith = std::clamp(config_.character_faith, 1, 99);
    g_FeatureStatus.character_arcane = std::clamp(config_.character_arcane, 1, 99);
    g_FeatureStatus.edit_hp = std::max(config_.edit_hp, 0);
    g_FeatureStatus.edit_mp = std::max(config_.edit_mp, 0);
    g_FeatureStatus.edit_sp = std::max(config_.edit_sp, 0);
    g_FeatureStatus.character_apply_requested = config_.character_apply_requested;
    g_FeatureStatus.resource_apply_requested = config_.resource_apply_requested;
    g_FeatureStatus.equip_slot = std::clamp(config_.equip_slot, 0, 38);
    g_FeatureStatus.equip_item_id = config_.equip_item_id;
    g_FeatureStatus.equip_apply_requested = config_.equip_apply_requested;
    g_FeatureStatus.equip_unequip_requested = config_.equip_unequip_requested;
    g_FeatureStatus.magic_slot = std::clamp(config_.magic_slot, 0, 13);
    g_FeatureStatus.magic_item_id = config_.magic_item_id;
    g_FeatureStatus.magic_attune_requested = config_.magic_attune_requested;
    g_FeatureStatus.magic_remove_requested = config_.magic_remove_requested;
    g_FeatureStatus.gesture_slot = std::clamp(config_.gesture_slot, 0, 6);
    g_FeatureStatus.gesture_item_id = config_.gesture_item_id;
    g_FeatureStatus.gesture_apply_requested = config_.gesture_apply_requested;
    g_FeatureStatus.npc_menu_request_id = std::max(config_.npc_menu_request_id, 0);
    g_FeatureStatus.statistics_load_save_slot = std::max(config_.statistics_load_save_slot, 0);
    g_FeatureStatus.statistics_load_save_slot_requested = config_.statistics_load_save_slot_requested;
    g_FeatureStatus.statistics_save_game_requested = config_.statistics_save_game_requested;
    g_FeatureStatus.statistics_disable_auto_save = config_.statistics_disable_auto_save;
    g_FeatureStatus.statistics_auto_save_dirty = false;
    g_FeatureStatus.param_tool_selected_table = std::max(config_.param_tool_selected_table, 0);
    g_FeatureStatus.param_tool_row_id = std::max(config_.param_tool_row_id, 0);
    g_FeatureStatus.param_tool_field_offset = std::max(config_.param_tool_field_offset, 0);
    g_FeatureStatus.param_tool_value_type = std::clamp(config_.param_tool_value_type, 0, 6);
    g_FeatureStatus.param_tool_int_value = config_.param_tool_int_value;
    g_FeatureStatus.param_tool_float_value = config_.param_tool_float_value;
    g_FeatureStatus.param_tool_write_requested = config_.param_tool_write_requested;
    g_FeatureStatus.param_tool_script_request_id = std::max(config_.param_tool_script_request_id, 0);

    g_FeatureStatus.menu_visible = true;
    g_FeatureStatus.game_ready = false;
    SetMenuPreferences(config_.menu_opacity, config_.menu_minimized);
}

ModConfig Runtime::CapturePersistentFeatureConfig() const {
    ModConfig snapshot = config_;

    snapshot.menu_minimized = IsMenuMinimized();
    snapshot.menu_opacity = GetMenuOpacity();

    snapshot.no_dead = g_FeatureStatus.no_dead.load();
    snapshot.no_damage = g_FeatureStatus.no_damage.load();
    snapshot.infinite_fp = g_FeatureStatus.infinite_fp.load();
    snapshot.infinite_items = g_FeatureStatus.infinite_items.load();
    snapshot.no_stamina_consumption = g_FeatureStatus.no_stamina_consumption.load();
    snapshot.unlock_all_maps = g_FeatureStatus.unlock_all_maps.load();
    snapshot.unlock_all_cookbooks = g_FeatureStatus.unlock_all_cookbooks.load();
    snapshot.unlock_all_whetblades = g_FeatureStatus.unlock_all_whetblades.load();
    snapshot.unlock_all_graces = g_FeatureStatus.unlock_all_graces.load();
    snapshot.unlock_all_summoning_pools = g_FeatureStatus.unlock_all_summoning_pools.load();
    snapshot.unlock_all_colosseums = g_FeatureStatus.unlock_all_colosseums.load();
    snapshot.unlock_all_gestures = g_FeatureStatus.unlock_all_gestures.load();

    snapshot.faster_respawn = g_FeatureStatus.faster_respawn.load();
    snapshot.warp_out_of_uncleared_minidungeons =
        g_FeatureStatus.warp_out_of_uncleared_minidungeons.load();
    snapshot.free_purchase = g_FeatureStatus.free_purchase.load();
    snapshot.no_crafting_material_cost = g_FeatureStatus.no_crafting_material_cost.load();
    snapshot.no_upgrade_material_cost = g_FeatureStatus.no_upgrade_material_cost.load();
    snapshot.no_magic_requirements = g_FeatureStatus.no_magic_requirements.load();
    snapshot.all_magic_one_slot = g_FeatureStatus.all_magic_one_slot.load();
    snapshot.weightless_equipment = g_FeatureStatus.weightless_equipment.load();
    snapshot.custom_fov_enabled = g_FeatureStatus.custom_fov_enabled.load();
    snapshot.custom_fov_value = std::clamp(g_FeatureStatus.custom_fov_value.load(), 20.0f, 120.0f);
    snapshot.custom_camera_distance_enabled = g_FeatureStatus.custom_camera_distance_enabled.load();
    snapshot.custom_camera_distance_value =
        std::clamp(g_FeatureStatus.custom_camera_distance_value.load(), 1.0f, 20.0f);
    snapshot.mount_anywhere = g_FeatureStatus.mount_anywhere.load();
    snapshot.spirit_ashes_anywhere = g_FeatureStatus.spirit_ashes_anywhere.load();
    snapshot.item_discovery_multiplier = std::clamp(g_FeatureStatus.item_discovery_multiplier.load(), 1, 100);
    snapshot.permanent_lantern = g_FeatureStatus.permanent_lantern.load();
    snapshot.invisible_helmets = g_FeatureStatus.invisible_helmets.load();
    snapshot.no_clip_enabled = g_FeatureStatus.no_clip_enabled.load();
    snapshot.freecam_enabled = g_FeatureStatus.freecam_enabled.load();
    snapshot.no_fall_camera = g_FeatureStatus.no_fall_camera.load();
    snapshot.movement_speed = std::clamp(g_FeatureStatus.movement_speed.load(), 0.1f, 20.0f);
    snapshot.start_next_cycle = g_FeatureStatus.start_next_cycle.load();
    snapshot.give_runes_amount = std::max(g_FeatureStatus.give_runes_amount.load(), 1);
    snapshot.give_runes_requested = g_FeatureStatus.give_runes_requested.load();
    snapshot.custom_item_id = g_FeatureStatus.custom_item_id.load();
    snapshot.custom_item_quantity = std::clamp(g_FeatureStatus.custom_item_quantity.load(), 1, 9999);
    snapshot.custom_item_reinforce_level = std::clamp(g_FeatureStatus.custom_item_reinforce_level.load(), 0, 25);
    snapshot.custom_item_upgrade_value = std::clamp(g_FeatureStatus.custom_item_upgrade_value.load(), 0, 1200);
    snapshot.custom_item_gem_id = g_FeatureStatus.custom_item_gem_id.load();
    snapshot.custom_item_give_requested = g_FeatureStatus.custom_item_give_requested.load();
    snapshot.mass_item_gib_request_id = std::max(g_FeatureStatus.mass_item_gib_request_id.load(), 0);
    snapshot.add_flask_charge_type = std::clamp(g_FeatureStatus.add_flask_charge_type.load(), 0, 1);
    snapshot.add_flask_charge_requested = g_FeatureStatus.add_flask_charge_requested.load();
    snapshot.set_flask_level_value = std::clamp(g_FeatureStatus.set_flask_level_value.load(), 0, 12);
    snapshot.set_flask_level_requested = g_FeatureStatus.set_flask_level_requested.load();
    snapshot.debug_event_flag_id = std::max(g_FeatureStatus.debug_event_flag_id.load(), 0);
    snapshot.debug_event_flag_desired_state = g_FeatureStatus.debug_event_flag_desired_state.load();
    snapshot.debug_event_flag_write_requested = g_FeatureStatus.debug_event_flag_write_requested.load();
    snapshot.teleport_coord_x = g_FeatureStatus.teleport_coord_x.load();
    snapshot.teleport_coord_y = g_FeatureStatus.teleport_coord_y.load();
    snapshot.teleport_coord_z = g_FeatureStatus.teleport_coord_z.load();
    snapshot.teleport_player_to_input_coords_requested = g_FeatureStatus.teleport_player_to_input_coords_requested.load();
    snapshot.character_level = std::clamp(g_FeatureStatus.character_level.load(), 1, 713);
    snapshot.character_vigor = std::clamp(g_FeatureStatus.character_vigor.load(), 1, 99);
    snapshot.character_mind = std::clamp(g_FeatureStatus.character_mind.load(), 1, 99);
    snapshot.character_endurance = std::clamp(g_FeatureStatus.character_endurance.load(), 1, 99);
    snapshot.character_strength = std::clamp(g_FeatureStatus.character_strength.load(), 1, 99);
    snapshot.character_dexterity = std::clamp(g_FeatureStatus.character_dexterity.load(), 1, 99);
    snapshot.character_intelligence = std::clamp(g_FeatureStatus.character_intelligence.load(), 1, 99);
    snapshot.character_faith = std::clamp(g_FeatureStatus.character_faith.load(), 1, 99);
    snapshot.character_arcane = std::clamp(g_FeatureStatus.character_arcane.load(), 1, 99);
    snapshot.edit_hp = std::max(g_FeatureStatus.edit_hp.load(), 0);
    snapshot.edit_mp = std::max(g_FeatureStatus.edit_mp.load(), 0);
    snapshot.edit_sp = std::max(g_FeatureStatus.edit_sp.load(), 0);
    snapshot.character_apply_requested = g_FeatureStatus.character_apply_requested.load();
    snapshot.resource_apply_requested = g_FeatureStatus.resource_apply_requested.load();
    snapshot.equip_slot = std::clamp(g_FeatureStatus.equip_slot.load(), 0, 38);
    snapshot.equip_item_id = g_FeatureStatus.equip_item_id.load();
    snapshot.equip_apply_requested = g_FeatureStatus.equip_apply_requested.load();
    snapshot.equip_unequip_requested = g_FeatureStatus.equip_unequip_requested.load();
    snapshot.magic_slot = std::clamp(g_FeatureStatus.magic_slot.load(), 0, 13);
    snapshot.magic_item_id = g_FeatureStatus.magic_item_id.load();
    snapshot.magic_attune_requested = g_FeatureStatus.magic_attune_requested.load();
    snapshot.magic_remove_requested = g_FeatureStatus.magic_remove_requested.load();
    snapshot.gesture_slot = std::clamp(g_FeatureStatus.gesture_slot.load(), 0, 6);
    snapshot.gesture_item_id = g_FeatureStatus.gesture_item_id.load();
    snapshot.gesture_apply_requested = g_FeatureStatus.gesture_apply_requested.load();
    snapshot.npc_menu_request_id = std::max(g_FeatureStatus.npc_menu_request_id.load(), 0);
    snapshot.statistics_load_save_slot = std::max(g_FeatureStatus.statistics_load_save_slot.load(), 0);
    snapshot.statistics_load_save_slot_requested = g_FeatureStatus.statistics_load_save_slot_requested.load();
    snapshot.statistics_save_game_requested = g_FeatureStatus.statistics_save_game_requested.load();
    snapshot.statistics_disable_auto_save = g_FeatureStatus.statistics_disable_auto_save.load();
    snapshot.param_tool_selected_table = std::max(g_FeatureStatus.param_tool_selected_table.load(), 0);
    snapshot.param_tool_row_id = std::max(g_FeatureStatus.param_tool_row_id.load(), 0);
    snapshot.param_tool_field_offset = std::max(g_FeatureStatus.param_tool_field_offset.load(), 0);
    snapshot.param_tool_value_type = std::clamp(g_FeatureStatus.param_tool_value_type.load(), 0, 6);
    snapshot.param_tool_int_value = g_FeatureStatus.param_tool_int_value.load();
    snapshot.param_tool_float_value = g_FeatureStatus.param_tool_float_value.load();
    snapshot.param_tool_write_requested = g_FeatureStatus.param_tool_write_requested.load();
    snapshot.param_tool_script_request_id = std::max(g_FeatureStatus.param_tool_script_request_id.load(), 0);

    return snapshot;
}

void Runtime::SyncPersistentConfig() {
    const ModConfig latest = CapturePersistentFeatureConfig();
    const bool changed =
        latest.menu_minimized != config_.menu_minimized ||
        latest.menu_opacity != config_.menu_opacity ||
        latest.no_dead != config_.no_dead ||
        latest.no_damage != config_.no_damage ||
        latest.infinite_fp != config_.infinite_fp ||
        latest.infinite_items != config_.infinite_items ||
        latest.no_stamina_consumption != config_.no_stamina_consumption ||
        latest.unlock_all_maps != config_.unlock_all_maps ||
        latest.unlock_all_cookbooks != config_.unlock_all_cookbooks ||
        latest.unlock_all_whetblades != config_.unlock_all_whetblades ||
        latest.unlock_all_graces != config_.unlock_all_graces ||
        latest.unlock_all_summoning_pools != config_.unlock_all_summoning_pools ||
        latest.unlock_all_colosseums != config_.unlock_all_colosseums ||
        latest.unlock_all_gestures != config_.unlock_all_gestures ||
        latest.faster_respawn != config_.faster_respawn ||
        latest.warp_out_of_uncleared_minidungeons != config_.warp_out_of_uncleared_minidungeons ||
        latest.free_purchase != config_.free_purchase ||
        latest.no_crafting_material_cost != config_.no_crafting_material_cost ||
        latest.no_upgrade_material_cost != config_.no_upgrade_material_cost ||
        latest.no_magic_requirements != config_.no_magic_requirements ||
        latest.all_magic_one_slot != config_.all_magic_one_slot ||
        latest.weightless_equipment != config_.weightless_equipment ||
        latest.custom_fov_enabled != config_.custom_fov_enabled ||
        latest.custom_fov_value != config_.custom_fov_value ||
        latest.custom_camera_distance_enabled != config_.custom_camera_distance_enabled ||
        latest.custom_camera_distance_value != config_.custom_camera_distance_value ||
        latest.mount_anywhere != config_.mount_anywhere ||
        latest.spirit_ashes_anywhere != config_.spirit_ashes_anywhere ||
        latest.item_discovery_multiplier != config_.item_discovery_multiplier ||
        latest.permanent_lantern != config_.permanent_lantern ||
        latest.invisible_helmets != config_.invisible_helmets ||
        latest.no_clip_enabled != config_.no_clip_enabled ||
        latest.freecam_enabled != config_.freecam_enabled ||
        latest.no_fall_camera != config_.no_fall_camera ||
        latest.movement_speed != config_.movement_speed ||
        latest.start_next_cycle != config_.start_next_cycle ||
        latest.give_runes_amount != config_.give_runes_amount ||
        latest.give_runes_requested != config_.give_runes_requested ||
        latest.custom_item_id != config_.custom_item_id ||
        latest.custom_item_quantity != config_.custom_item_quantity ||
        latest.custom_item_reinforce_level != config_.custom_item_reinforce_level ||
        latest.custom_item_upgrade_value != config_.custom_item_upgrade_value ||
        latest.custom_item_gem_id != config_.custom_item_gem_id ||
        latest.custom_item_give_requested != config_.custom_item_give_requested ||
        latest.mass_item_gib_request_id != config_.mass_item_gib_request_id ||
        latest.add_flask_charge_type != config_.add_flask_charge_type ||
        latest.add_flask_charge_requested != config_.add_flask_charge_requested ||
        latest.set_flask_level_value != config_.set_flask_level_value ||
        latest.set_flask_level_requested != config_.set_flask_level_requested ||
        latest.debug_event_flag_id != config_.debug_event_flag_id ||
        latest.debug_event_flag_desired_state != config_.debug_event_flag_desired_state ||
        latest.debug_event_flag_write_requested != config_.debug_event_flag_write_requested ||
        latest.teleport_coord_x != config_.teleport_coord_x ||
        latest.teleport_coord_y != config_.teleport_coord_y ||
        latest.teleport_coord_z != config_.teleport_coord_z ||
        latest.teleport_player_to_input_coords_requested != config_.teleport_player_to_input_coords_requested ||
        latest.character_level != config_.character_level ||
        latest.character_vigor != config_.character_vigor ||
        latest.character_mind != config_.character_mind ||
        latest.character_endurance != config_.character_endurance ||
        latest.character_strength != config_.character_strength ||
        latest.character_dexterity != config_.character_dexterity ||
        latest.character_intelligence != config_.character_intelligence ||
        latest.character_faith != config_.character_faith ||
        latest.character_arcane != config_.character_arcane ||
        latest.edit_hp != config_.edit_hp ||
        latest.edit_mp != config_.edit_mp ||
        latest.edit_sp != config_.edit_sp ||
        latest.character_apply_requested != config_.character_apply_requested ||
        latest.resource_apply_requested != config_.resource_apply_requested ||
        latest.equip_slot != config_.equip_slot ||
        latest.equip_item_id != config_.equip_item_id ||
        latest.equip_apply_requested != config_.equip_apply_requested ||
        latest.equip_unequip_requested != config_.equip_unequip_requested ||
        latest.magic_slot != config_.magic_slot ||
        latest.magic_item_id != config_.magic_item_id ||
        latest.magic_attune_requested != config_.magic_attune_requested ||
        latest.magic_remove_requested != config_.magic_remove_requested ||
        latest.gesture_slot != config_.gesture_slot ||
        latest.gesture_item_id != config_.gesture_item_id ||
        latest.gesture_apply_requested != config_.gesture_apply_requested ||
        latest.npc_menu_request_id != config_.npc_menu_request_id ||
        latest.statistics_load_save_slot != config_.statistics_load_save_slot ||
        latest.statistics_load_save_slot_requested != config_.statistics_load_save_slot_requested ||
        latest.statistics_save_game_requested != config_.statistics_save_game_requested ||
        latest.statistics_disable_auto_save != config_.statistics_disable_auto_save ||
        latest.param_tool_selected_table != config_.param_tool_selected_table ||
        latest.param_tool_row_id != config_.param_tool_row_id ||
        latest.param_tool_field_offset != config_.param_tool_field_offset ||
        latest.param_tool_value_type != config_.param_tool_value_type ||
        latest.param_tool_int_value != config_.param_tool_int_value ||
        latest.param_tool_float_value != config_.param_tool_float_value ||
        latest.param_tool_write_requested != config_.param_tool_write_requested ||
        latest.param_tool_script_request_id != config_.param_tool_script_request_id;

    if (!changed) {
        return;
    }

    config_ = latest;
    if (SaveConfig(paths_.PreferredConfigPath(), config_)) {
        Logger::Instance().Info("Saved current toggle state to Config/ERDMod.ini.");
    } else {
        Logger::Instance().Error("Failed to save current toggle state to Config/ERDMod.ini.");
    }
}

bool Runtime::IsCharacterLoaded(const Game::SingletonRegistry& singletons) const {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    if (world_chr_man == 0) {
        return false;
    }

    // 优先看玩家链本身是否已经成立。
    // 这一条比 GameMan 更接近我们实际要改的角色数据，也更能避免误判。
    uintptr_t common_flags_address = 0;
    if (Game::ResolvePointerChain(
            world_chr_man,
            {kNetPlayersOffset, 0x0, kCharacterFlagsBlockOffset, 0x0},
            kCommonFlagsOffset,
            common_flags_address) &&
        common_flags_address != 0) {
        return true;
    }

    // 再回退到 CT 常见的判定：GameMan+AC0 不是 -1 说明已经选中了角色存档。
    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    if (game_man != 0) {
        std::int32_t save_slot = -1;
        if (Game::ReadValue(game_man + kSaveSlotOffset, save_slot) && save_slot != -1) {
            return true;
        }
    }

    return false;
}

void Runtime::RunFeatureLoop() {
    Game::SingletonRegistry singletons;
    if (!singletons.Initialize()) {
        Logger::Instance().Error("Failed to build FD4 singleton registry.");
        return;
    }

    if (singletons.GetStorage("WorldChrMan") == 0) {
        Logger::Instance().Error("WorldChrMan singleton was not found.");
        return;
    }

    Logger::Instance().Info("WorldChrMan singleton resolved. Entering main loop.");

    while (g_Running) {
        ERD_PROTECTED_STEP("Runtime.SyncPersistentConfig", SyncPersistentConfig());
        ERD_PROTECTED_STEP("Features.PostureBars", posture_bars_.Tick());

        const bool game_ready = IsCharacterLoaded(singletons);
        g_FeatureStatus.game_ready = game_ready;

        if (game_ready != last_game_ready_) {
            Logger::Instance().Info(
                game_ready
                    ? "Character loaded. Runtime features are now allowed to apply."
                    : "Character not loaded yet. Runtime features are waiting."
            );
            last_game_ready_ = game_ready;
        }

        if (game_ready) {
            ERD_PROTECTED_STEP("Features.CharacterFlags", character_flags_.Tick(singletons));
            ERD_PROTECTED_STEP("Features.Unlocks", unlocks_.Tick(singletons));
            ERD_PROTECTED_STEP("Features.ParamPatches", param_patches_.Tick(singletons));
            ERD_PROTECTED_STEP("Features.MovementMods", movement_mods_.Tick(singletons));
            ERD_PROTECTED_STEP("Features.GameActions", game_actions_.Tick(singletons));
            ERD_PROTECTED_STEP("Features.NpcDebug", npc_debug_.Tick(singletons));
            ERD_PROTECTED_STEP("Features.InspectorTools", inspector_tools_.Tick(singletons));
        }

        Sleep(kFeaturePollIntervalMs);
    }
}

}  // namespace ERD::Main
