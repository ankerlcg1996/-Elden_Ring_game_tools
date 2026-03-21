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

}  // namespace

Runtime::Runtime(ModPaths paths, ModConfig config)
    : paths_(std::move(paths)), config_(std::move(config)) {}

void Runtime::Run() {
    MaybeCreateConsole();
    LogStartup();
    InitializeFeatureStatus();

    // Overlay 初始化失败不影响功能本体，最多只是菜单不可用。
    if (g_D3D12Overlay && !g_D3D12Overlay->Hook()) {
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
    Logger::Instance().Info(config_.start_next_cycle ? "Start next cycle requested." : "Start next cycle disabled.");

    if (config_.show_message_box) {
        MessageBoxW(nullptr, config_.message_box_text.c_str(), L"ERDMod", MB_OK | MB_ICONINFORMATION);
    }
}

// 把 ini 里的默认值注入运行时共享状态。
// 后面游戏内菜单的切换只改 g_FeatureStatus，不会每帧反复回读配置。
void Runtime::InitializeFeatureStatus() const {
    g_FeatureStatus.infinite_fp = config_.infinite_fp;
    g_FeatureStatus.infinite_items = config_.infinite_items;
    g_FeatureStatus.no_stamina_consumption = config_.no_stamina_consumption;

    g_FeatureStatus.unlock_all_maps = config_.unlock_all_maps;
    g_FeatureStatus.unlock_all_cookbooks = config_.unlock_all_cookbooks;
    g_FeatureStatus.unlock_all_whetblades = config_.unlock_all_whetblades;
    g_FeatureStatus.unlock_all_graces = config_.unlock_all_graces;
    g_FeatureStatus.unlock_all_summoning_pools = config_.unlock_all_summoning_pools;
    g_FeatureStatus.unlock_all_colosseums = config_.unlock_all_colosseums;

    g_FeatureStatus.faster_respawn = config_.faster_respawn;
    g_FeatureStatus.warp_out_of_uncleared_minidungeons = config_.warp_out_of_uncleared_minidungeons;
    g_FeatureStatus.free_purchase = config_.free_purchase;
    g_FeatureStatus.no_crafting_material_cost = config_.no_crafting_material_cost;
    g_FeatureStatus.no_upgrade_material_cost = config_.no_upgrade_material_cost;
    g_FeatureStatus.no_magic_requirements = config_.no_magic_requirements;
    g_FeatureStatus.all_magic_one_slot = config_.all_magic_one_slot;
    g_FeatureStatus.weightless_equipment = config_.weightless_equipment;
    g_FeatureStatus.mount_anywhere = config_.mount_anywhere;
    g_FeatureStatus.spirit_ashes_anywhere = config_.spirit_ashes_anywhere;
    g_FeatureStatus.item_discovery_multiplier = std::clamp(config_.item_discovery_multiplier, 1, 100);
    g_FeatureStatus.permanent_lantern = config_.permanent_lantern;
    g_FeatureStatus.invisible_helmets = config_.invisible_helmets;

    g_FeatureStatus.start_next_cycle = config_.start_next_cycle;

    g_FeatureStatus.menu_visible = true;
    g_FeatureStatus.game_ready = false;
    SetMenuPreferences(config_.menu_opacity, config_.menu_minimized);
}

ModConfig Runtime::CapturePersistentFeatureConfig() const {
    ModConfig snapshot = config_;

    snapshot.menu_minimized = IsMenuMinimized();
    snapshot.menu_opacity = GetMenuOpacity();

    snapshot.infinite_fp = g_FeatureStatus.infinite_fp.load();
    snapshot.infinite_items = g_FeatureStatus.infinite_items.load();
    snapshot.no_stamina_consumption = g_FeatureStatus.no_stamina_consumption.load();

    snapshot.faster_respawn = g_FeatureStatus.faster_respawn.load();
    snapshot.warp_out_of_uncleared_minidungeons =
        g_FeatureStatus.warp_out_of_uncleared_minidungeons.load();
    snapshot.free_purchase = g_FeatureStatus.free_purchase.load();
    snapshot.no_crafting_material_cost = g_FeatureStatus.no_crafting_material_cost.load();
    snapshot.no_upgrade_material_cost = g_FeatureStatus.no_upgrade_material_cost.load();
    snapshot.no_magic_requirements = g_FeatureStatus.no_magic_requirements.load();
    snapshot.all_magic_one_slot = g_FeatureStatus.all_magic_one_slot.load();
    snapshot.weightless_equipment = g_FeatureStatus.weightless_equipment.load();
    snapshot.mount_anywhere = g_FeatureStatus.mount_anywhere.load();
    snapshot.spirit_ashes_anywhere = g_FeatureStatus.spirit_ashes_anywhere.load();
    snapshot.item_discovery_multiplier = std::clamp(g_FeatureStatus.item_discovery_multiplier.load(), 1, 100);
    snapshot.permanent_lantern = g_FeatureStatus.permanent_lantern.load();
    snapshot.invisible_helmets = g_FeatureStatus.invisible_helmets.load();

    return snapshot;
}

void Runtime::SyncPersistentConfig() {
    const ModConfig latest = CapturePersistentFeatureConfig();
    const bool changed =
        latest.menu_minimized != config_.menu_minimized ||
        latest.menu_opacity != config_.menu_opacity ||
        latest.infinite_fp != config_.infinite_fp ||
        latest.infinite_items != config_.infinite_items ||
        latest.no_stamina_consumption != config_.no_stamina_consumption ||
        latest.faster_respawn != config_.faster_respawn ||
        latest.warp_out_of_uncleared_minidungeons != config_.warp_out_of_uncleared_minidungeons ||
        latest.free_purchase != config_.free_purchase ||
        latest.no_crafting_material_cost != config_.no_crafting_material_cost ||
        latest.no_upgrade_material_cost != config_.no_upgrade_material_cost ||
        latest.no_magic_requirements != config_.no_magic_requirements ||
        latest.all_magic_one_slot != config_.all_magic_one_slot ||
        latest.weightless_equipment != config_.weightless_equipment ||
        latest.mount_anywhere != config_.mount_anywhere ||
        latest.spirit_ashes_anywhere != config_.spirit_ashes_anywhere ||
        latest.item_discovery_multiplier != config_.item_discovery_multiplier ||
        latest.permanent_lantern != config_.permanent_lantern ||
        latest.invisible_helmets != config_.invisible_helmets;

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
        SyncPersistentConfig();

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
            character_flags_.Tick(singletons);
            unlocks_.Tick(singletons);
            param_patches_.Tick(singletons);
            game_actions_.Tick(singletons);
        }

        Sleep(kFeaturePollIntervalMs);
    }
}

}  // namespace ERD::Main
