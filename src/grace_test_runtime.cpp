#include "grace_test_runtime.hpp"

#include "grace_test_config.hpp"
#include "grace_test_localization.hpp"
#include "grace_test_messages.hpp"
#include "grace_test_overlay.hpp"
#include "grace_test_talkscript.hpp"

#include "Common.hpp"
#include "Features/CharacterFlags.hpp"
#include "Features/AutoPickup.hpp"
#include "Features/EzStateTalkCommand.hpp"
#include "Features/InspectorTools.hpp"
#include "Features/ParamPatches.hpp"
#include "Features/RallyHooks.hpp"
#include "Features/SpEffectMonitor.hpp"
#include "Game/EventFlags.hpp"
#include "Game/Memory.hpp"
#include "Game/SingletonRegistry.hpp"
#include "Main/FeatureStatus.hpp"
#include "Main/Logger.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

namespace grace_test::runtime {
namespace {

ERD::Game::SingletonRegistry g_singletons;
ERD::Features::CharacterFlags g_character_flags;
ERD::Features::AutoPickup g_auto_pickup;
ERD::Features::ParamPatches g_param_patches;
ERD::Features::InspectorTools g_inspector_tools;
ERD::Features::RallyHooks g_rally_hooks;
ERD::Features::SpEffectMonitor g_sp_effect_monitor;
bool g_last_game_ready = false;
bool g_beast_not_hostile_applied = false;
bool g_no_fall_death_applied = false;
bool g_required_goods_ensured = false;
ULONGLONG g_next_reveal_invisible_check_tick = 0;
ULONGLONG g_next_required_goods_check_tick = 0;
std::filesystem::path g_runtime_folder;

constexpr uintptr_t kSaveSlotOffset = 0xAC0;
constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kGoodsInventoryDataOffset = 0x8D0;
constexpr uintptr_t kCharacterFlagsBlockOffset = 0x190;
constexpr uintptr_t kChrBehaviorModuleOffset = 0x28;
constexpr uintptr_t kChrBehaviorAnimSpeedOffset = 0x17C8;
constexpr uintptr_t kCommonFlagsOffset = 0x19B;
constexpr uintptr_t kCsDlcCheckOffset = 0x11;
constexpr int kDlcShadowOfTheErdtreeIndex = 1;
constexpr int kTalkCommandAcquireGesture = 131;
constexpr int kTalkCommandOpenUpgrade = 24;
constexpr int kTalkCommandLevelUp = 31;
constexpr int kTalkCommandOpenAttunement = 28;
constexpr int kTalkCommandOpenChest = 30;
constexpr int kTalkCommandOpenSell = 46;
constexpr int kTalkCommandOpenAshesOfWar = 48;
constexpr int kTalkCommandOpenRemembrances = 111;
constexpr int kTalkCommandRebirth = 113;
constexpr int kTalkCommandOpenAllot = 105;
constexpr int kTalkCommandOpenPhysick = 130;
constexpr int kTalkCommandOpenDragonCommunion = 135;
constexpr int kTalkCommandOpenGreatRunes = 137;
constexpr int kTalkCommandOpenSpiritTuning = 136;
constexpr int kTalkCommandOpenMirror = 81;
constexpr int kTalkCommandOpenAlterGarments = 142;
constexpr int kTalkCommandUpgradeFlag = 49;
constexpr int kTalkCommandOpenEquipmentShop = 148;
constexpr std::uint32_t kSpEffectBeastNotHostileId = 460;
constexpr std::uint32_t kSpEffectRevealInvisibleId = 416;
constexpr std::uint32_t kSpEffectBuffDurationExtendId = 330600;
constexpr std::uint32_t kSpEffectNoFallDeathId = 360400;
constexpr std::uint32_t kSpEffectNoFallDeathMiddleId = 185;
constexpr std::uint32_t kSpEffectNoFallDeathTriggerId = 184;
constexpr std::array<std::uint32_t, 8> kSpEffectAttackLifeStealOnHitIds{{650, 651, 652, 653, 654, 655, 656, 657}};
constexpr ULONGLONG kRevealInvisibleCheckIntervalMs = 500;
constexpr std::size_t kNpcParamInitializeDeadOffset = 0x14D;
constexpr int kNpcParamInitializeDeadBitIndex = 3;
constexpr uintptr_t kPlayerGameDataOffset = 0x8;
constexpr uintptr_t kPlayerModelScaleBlockOffset = 0x58;
constexpr uintptr_t kPlayerVigorOffset = 0x3C;
constexpr uintptr_t kPlayerMindOffset = 0x40;
constexpr uintptr_t kPlayerEnduranceOffset = 0x44;
constexpr uintptr_t kPlayerStrengthOffset = 0x48;
constexpr uintptr_t kPlayerDexterityOffset = 0x4C;
constexpr uintptr_t kPlayerIntelligenceOffset = 0x50;
constexpr uintptr_t kPlayerFaithOffset = 0x54;
constexpr uintptr_t kPlayerArcaneOffset = 0x58;
constexpr uintptr_t kPlayerLevelOffset = 0x68;
constexpr uintptr_t kPlayerRunesOffset = 0x6C;
constexpr uintptr_t kPlayerRuneMemoryOffset = 0x70;
constexpr uintptr_t kPlayerNewGameOffset = 0x120;
constexpr uintptr_t kPlayerScadutreeBlessingOffset = 0xFC;
constexpr uintptr_t kPlayerReveredSpiritAshBlessingOffset = 0xFD;
constexpr uintptr_t kPlayerHandStyleOffset = 0x324;
constexpr uintptr_t kPlayerCurrentWeaponLeftOffset = 0x328;
constexpr uintptr_t kPlayerCurrentWeaponRightOffset = 0x32C;
constexpr uintptr_t kPlayerCurrentHpOffset = 0x138;
constexpr uintptr_t kPlayerMaxHpOffset = 0x13C;
constexpr uintptr_t kPlayerCurrentFpOffset = 0x148;
constexpr uintptr_t kPlayerMaxFpOffset = 0x150;
constexpr uintptr_t kPlayerCurrentSpOffset = 0x154;
constexpr uintptr_t kPlayerMaxSpOffset = 0x158;
constexpr std::array<uintptr_t, 6> kPlayerWeaponOffsets{{0x398, 0x39C, 0x3A0, 0x3A4, 0x3A8, 0x3AC}};
constexpr std::array<uintptr_t, 6> kPlayerAmmoOffsets{{0x3B0, 0x3B4, 0x3B8, 0x3BC, 0x3C0, 0x3C4}};
constexpr std::array<uintptr_t, 5> kPlayerTalismanOffsets{{0x3DC, 0x3E0, 0x3E4, 0x3E8, 0x3EC}};
constexpr uintptr_t kPlayerModelScaleWidthOffset = 0x2D4;
constexpr uintptr_t kPlayerModelScaleHeightOffset = 0x2D8;
constexpr uintptr_t kPlayerModelScaleDepthOffset = 0x2DC;
constexpr uintptr_t kSpiritTuningCtFunctionOffset = 0x80DAF0;
constexpr ULONGLONG kRequiredGoodsRetryIntervalMs = 1000;
constexpr std::array<std::int32_t, 5> kRequiredGoodsIds{{2070, 130, 181, 250, 251}};
struct BossFlagSetting {
    std::uint32_t event_id = 0;
    bool enabled = false;
};

struct BossReviveEntry {
    int menu_id = -1;
    bool is_dlc = false;
    bool is_initialize_dead_set = false;
    std::uint32_t block_id = 0;
    std::string area;
    std::string boss_name;
    std::vector<std::uint32_t> npc_param_ids;
    std::vector<BossFlagSetting> boss_flags;
};

struct QuickAccessShopEntry {
    int menu_id = -1;
    bool is_dlc = false;
    std::string name;
    ERD::Features::EzStateTalkCommand command{};
};

std::vector<BossReviveEntry> g_boss_revive_entries;
std::vector<grace_test::runtime::BossReviveMenuItem> g_boss_revive_menu_items;
bool g_boss_revives_loaded = false;
std::string g_boss_revives_loaded_language = "en-US";
std::vector<QuickAccessShopEntry> g_quick_access_shop_entries;
std::vector<grace_test::runtime::QuickAccessShopMenuItem> g_quick_access_shop_menu_items;
bool g_quick_access_shops_loaded = false;
std::string g_quick_access_shops_loaded_language = "en-US";
using WarpToBlockFn = void (*)(std::int32_t, std::int32_t, std::int32_t, std::int32_t);
WarpToBlockFn g_warp_to_block_fn = nullptr;

void normalize_feature_state_for_plugins() {}

const std::array<std::uint32_t, 5> kUnlockWhetbladeFlags{{
    65720,
    65680,
    65610,
    65640,
    60130,
}};

const std::array<std::uint32_t, 28> kUnlockBaseMapFlags{{
    62004, 62005, 62006, 62007, 62008, 62009, 62010, 62011, 62012, 62022, 62021, 62020, 62031, 62030,
    62032, 62041, 62040, 62052, 62051, 62050, 62063, 62062, 62061, 62060, 62064, 62103, 62102, 82001,
}};

const std::array<std::uint32_t, 6> kUnlockDlcMapFlags{{
    62080,
    62081,
    62082,
    62083,
    62084,
    82002,
}};

const std::array<std::int32_t, 46> kBaseGestureIds{{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  20,  21,  22,  23,  24,
    25,  30,  40,  41,  50,  51,  52,  53,  54,  60,  70,  71,  72,  73,  80,  90,
    91,  92,  93,  94,  95,  97,  98,  100, 101, 102, 103, 104, 105, 106,
}};

const std::array<std::int32_t, 4> kDlcGestureIds{{
    111,
    112,
    114,
    115,
}};

std::string Trim(std::string value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return {};
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

bool ParseBool(std::string value) {
    value = Trim(std::move(value));
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value == "TRUE" || value == "1";
}

template <typename T>
bool ReadPlayerGameDataValue(uintptr_t player_game_data, uintptr_t offset, T& value) {
    return player_game_data != 0 && ERD::Game::ReadValue(player_game_data + offset, value);
}

bool ResolveLocalPlayerBase(uintptr_t world_chr_man, uintptr_t& player_base) {
    player_base = 0;
    uintptr_t slots_root = 0;
    return world_chr_man != 0 &&
           ERD::Game::ReadValue(world_chr_man + kNetPlayersOffset, slots_root) &&
           slots_root != 0 &&
           ERD::Game::ReadValue(slots_root, player_base) &&
           player_base != 0;
}

bool ResolveLocalPlayerModelScaleRoot(uintptr_t world_chr_man, uintptr_t& model_scale_root) {
    model_scale_root = 0;
    uintptr_t player_base = 0;
    return ResolveLocalPlayerBase(world_chr_man, player_base) &&
           ERD::Game::ReadValue(player_base + kPlayerModelScaleBlockOffset, model_scale_root) &&
           model_scale_root != 0;
}

bool ExecuteCodeNoArgs(uintptr_t absolute_target) {
    if (absolute_target == 0) {
        return false;
    }

    constexpr std::uint8_t kStubTemplate[] = {
        0x48, 0x83, 0xEC, 0x28,
        0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0,
        0xFF, 0xD0,
        0x48, 0x83, 0xC4, 0x28,
        0xC3
    };

    void* stub = VirtualAlloc(nullptr, sizeof(kStubTemplate), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (stub == nullptr) {
        return false;
    }

    std::memcpy(stub, kStubTemplate, sizeof(kStubTemplate));
    std::memcpy(reinterpret_cast<std::uint8_t*>(stub) + 6, &absolute_target, sizeof(absolute_target));

    HANDLE thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(stub), nullptr, 0, nullptr);
    if (thread == nullptr) {
        VirtualFree(stub, 0, MEM_RELEASE);
        return false;
    }

    WaitForSingleObject(thread, 1000);
    DWORD exit_code = 0;
    GetExitCodeThread(thread, &exit_code);
    CloseHandle(thread);
    VirtualFree(stub, 0, MEM_RELEASE);
    return exit_code != STILL_ACTIVE;
}

std::vector<std::string> Split(const std::string& value, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(value);
    std::string item;
    while (std::getline(stream, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}

std::optional<ERD::Features::EzStateTalkCommand> MakeQuickAccessTalkCommand(
    int command_id,
    const std::vector<std::string>& params) {
    ERD::Features::EzStateTalkCommand command{};
    command.command_id = command_id;
    command.param_count = static_cast<int>(std::min<std::size_t>(params.size(), command.params.size()));
    command.use_player_handle = command_id == kTalkCommandOpenSell;
    for (int index = 0; index < command.param_count; ++index) {
        try {
            command.params[static_cast<std::size_t>(index)] = std::stoll(Trim(params[static_cast<std::size_t>(index)]));
        } catch (...) {
            return std::nullopt;
        }
    }
    return command;
}

bool RegionFromMainModuleText(uintptr_t& base, std::size_t& size) {
    HMODULE module = GetModuleHandleW(nullptr);
    if (module == nullptr) {
        return false;
    }

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        reinterpret_cast<uintptr_t>(module) + dos->e_lfanew);
    const IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const IMAGE_SECTION_HEADER* section = sections + i;
        if (_stricmp(reinterpret_cast<const char*>(section->Name), ".text") == 0) {
            base = reinterpret_cast<uintptr_t>(module) + section->VirtualAddress;
            size = section->Misc.VirtualSize;
            return true;
        }
    }
    return false;
}

uintptr_t FindItemSpawnFunction() {
    static uintptr_t cached = 0;
    if (cached != 0) {
        return cached;
    }

    uintptr_t text_base = 0;
    std::size_t text_size = 0;
    if (!RegionFromMainModuleText(text_base, text_size) || text_size < 0x20) {
        return 0;
    }

    const std::array<std::uint8_t, 4> marker{0x4C, 0x8D, 0x45, 0x34};
    const auto* begin = reinterpret_cast<const std::uint8_t*>(text_base);
    const auto* end = begin + text_size - marker.size();
    for (const auto* cursor = begin; cursor <= end; ++cursor) {
        if (std::memcmp(cursor, marker.data(), marker.size()) != 0) {
            continue;
        }

        std::uint8_t opcode = 0;
        const uintptr_t match = reinterpret_cast<uintptr_t>(cursor);
        if (!ERD::Game::ReadValue(match + 0x0B, opcode) || opcode != 0xE8) {
            continue;
        }

        std::int32_t rel32 = 0;
        if (!ERD::Game::ReadValue(match + 0x0C, rel32)) {
            continue;
        }

        const uintptr_t target = (match + 0x0B + 5) + static_cast<std::int64_t>(rel32);
        if (target < text_base || target >= (text_base + text_size)) {
            continue;
        }

        cached = target;
        spdlog::info("Resolved ItemSpawn function at 0x{:X}.", target);
        return cached;
    }

    return 0;
}

bool HasHeldGoodsItem(uintptr_t player_game_data, std::int32_t goods_row_id) {
    if (player_game_data == 0 || goods_row_id < 0) {
        return false;
    }

    uintptr_t inventory_data = 0;
    uintptr_t list_address = 0;
    int count = 0;
    if (!ERD::Game::ReadValue(player_game_data + kGoodsInventoryDataOffset, inventory_data) ||
        inventory_data == 0 ||
        !ERD::Game::ReadValue(inventory_data + 0x10, list_address) ||
        list_address == 0 ||
        !ERD::Game::ReadValue(inventory_data + 0x18, count)) {
        return false;
    }

    const std::int32_t target_item_id = 0x40000000 | goods_row_id;
    const std::size_t slot_limit = 1920;
    int populated = 0;
    for (std::size_t index = 0; index <= slot_limit; ++index) {
        std::int32_t item_id = -1;
        if (!ERD::Game::ReadValue(list_address + index * 0x18 + 0x4, item_id)) {
            break;
        }
        if (item_id == -1) {
            continue;
        }

        ++populated;
        if (item_id == target_item_id) {
            int quantity = 0;
            if (ERD::Game::ReadValue(list_address + index * 0x18 + 0x8, quantity) && quantity > 0) {
                return true;
            }
            return true;
        }

        if (count > 0 && populated >= count) {
            break;
        }
    }

    return false;
}

bool SpawnGoodsItem(const ERD::Game::SingletonRegistry& singletons, std::int32_t goods_row_id, int quantity) {
    const uintptr_t item_spawn = FindItemSpawnFunction();
    const uintptr_t map_item_man_storage = singletons.GetStorage("MapItemManImpl");
    if (item_spawn == 0 || map_item_man_storage == 0 || goods_row_id < 0 || quantity <= 0) {
        return false;
    }

    void* block = VirtualAlloc(nullptr, 0x400, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (block == nullptr) {
        return false;
    }

    auto* bytes = reinterpret_cast<std::uint8_t*>(block);
    const uintptr_t base = reinterpret_cast<uintptr_t>(block);
    const uintptr_t item_struct = base;
    const uintptr_t should_adjust_quantity = base + 0x200;
    const uintptr_t max_quantity = base + 0x204;
    const uintptr_t code = base + 0x280;

    ERD::Game::WriteValue<std::uint8_t>(should_adjust_quantity, 0);
    ERD::Game::WriteValue<std::int32_t>(max_quantity, 9999);
    ERD::Game::WriteValue<std::int32_t>(item_struct + 0x40, 1);
    ERD::Game::WriteValue<std::int32_t>(item_struct + 0x44, 0x40000000 | goods_row_id);
    ERD::Game::WriteValue<std::int32_t>(item_struct + 0x48, quantity);
    ERD::Game::WriteValue<std::int32_t>(item_struct + 0x4C, -1);
    ERD::Game::WriteValue<std::int32_t>(item_struct + 0x50, -1);

    constexpr std::array<std::uint8_t, 80> kItemSpawnCode{{
        0x48, 0x83, 0xEC, 0x28,
        0x48, 0x8D, 0x1D, 0x00, 0x00, 0x00, 0x00,
        0x80, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x74, 0x1D,
        0x48, 0x8D, 0x4B, 0x44,
        0xE8, 0x00, 0x00, 0x00, 0x00,
        0x8B, 0x3D, 0x00, 0x00, 0x00, 0x00,
        0x8B, 0x53, 0x48,
        0x01, 0xC2,
        0x39, 0xFA,
        0x7E, 0x05,
        0x29, 0xC7,
        0x89, 0x7B, 0x48,
        0x48, 0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00,
        0x48, 0x8D, 0x53, 0x40,
        0x4C, 0x8D, 0x83, 0xE4, 0x00, 0x00, 0x00,
        0x45, 0x31, 0xC9,
        0xE8, 0x00, 0x00, 0x00, 0x00,
        0x48, 0x83, 0xC4, 0x28,
        0xC3,
    }};
    std::memcpy(reinterpret_cast<void*>(code), kItemSpawnCode.data(), kItemSpawnCode.size());

    auto patch_rip = [&](uintptr_t instruction, uintptr_t target, std::size_t instruction_size, std::size_t disp_offset) {
        const std::int64_t delta =
            static_cast<std::int64_t>(target) - static_cast<std::int64_t>(instruction + instruction_size);
        if (delta < INT32_MIN || delta > INT32_MAX) {
            return false;
        }
        const auto rel32 = static_cast<std::int32_t>(delta);
        return ERD::Game::WriteMemory(instruction + disp_offset, &rel32, sizeof(rel32));
    };

    const bool patched =
        patch_rip(code + 0x04, item_struct, 7, 3) &&
        patch_rip(code + 0x0B, should_adjust_quantity, 7, 2) &&
        patch_rip(code + 0x18, item_spawn, 5, 1) &&
        patch_rip(code + 0x1D, max_quantity, 6, 2) &&
        patch_rip(code + 0x31, map_item_man_storage, 7, 3) &&
        patch_rip(code + 0x46, item_spawn, 5, 1);

    bool executed = false;
    if (patched) {
        executed = ExecuteCodeNoArgs(code);
    }

    VirtualFree(block, 0, MEM_RELEASE);
    return executed;
}

void EnsureRequiredGoods(const ERD::Game::SingletonRegistry& singletons) {
    const uintptr_t game_data_man = singletons.GetObjectPointer("GameDataMan");
    uintptr_t player_game_data = 0;
    if (game_data_man == 0 ||
        !ERD::Game::ReadValue(game_data_man + kPlayerGameDataOffset, player_game_data) ||
        player_game_data == 0) {
        return;
    }

    bool missing_any = false;
    for (const std::int32_t goods_id : kRequiredGoodsIds) {
        if (HasHeldGoodsItem(player_game_data, goods_id)) {
            continue;
        }

        missing_any = true;
        if (SpawnGoodsItem(singletons, goods_id, 1)) {
            spdlog::info("Granted missing required goods item {}.", goods_id);
        } else {
            spdlog::warn("Failed to grant missing required goods item {}.", goods_id);
        }
    }

    if (!missing_any) {
        g_required_goods_ensured = true;
    } else {
        g_next_required_goods_check_tick = GetTickCount64() + kRequiredGoodsRetryIntervalMs;
    }
}

void SyncPlayerReadOnlySnapshot() {
    const uintptr_t game_data_man = g_singletons.GetObjectPointer("GameDataMan");
    const uintptr_t world_chr_man = g_singletons.GetObjectPointer("WorldChrMan");
    uintptr_t player_game_data = 0;
    if (game_data_man == 0 || !ERD::Game::ReadValue(game_data_man + kPlayerGameDataOffset, player_game_data) || player_game_data == 0) {
        ERD::Main::g_FeatureStatus.player_equipment_snapshot_valid = false;
        return;
    }

    ERD::Main::g_FeatureStatus.player_equipment_snapshot_valid = true;

    if (ERD::Main::g_FeatureStatus.requested_ng_cycle_apply.exchange(false)) {
        const int requested_ng_cycle =
            std::clamp(ERD::Main::g_FeatureStatus.requested_ng_cycle.load(), 0, 10);
        ERD::Game::WriteValue(player_game_data + kPlayerNewGameOffset, requested_ng_cycle);
    }

    int value = 0;
    if (ReadPlayerGameDataValue(player_game_data, kPlayerVigorOffset, value)) {
        ERD::Main::g_FeatureStatus.character_vigor = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerMindOffset, value)) {
        ERD::Main::g_FeatureStatus.character_mind = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerEnduranceOffset, value)) {
        ERD::Main::g_FeatureStatus.character_endurance = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerStrengthOffset, value)) {
        ERD::Main::g_FeatureStatus.character_strength = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerDexterityOffset, value)) {
        ERD::Main::g_FeatureStatus.character_dexterity = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerIntelligenceOffset, value)) {
        ERD::Main::g_FeatureStatus.character_intelligence = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerFaithOffset, value)) {
        ERD::Main::g_FeatureStatus.character_faith = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerArcaneOffset, value)) {
        ERD::Main::g_FeatureStatus.character_arcane = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerLevelOffset, value)) {
        ERD::Main::g_FeatureStatus.character_level = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerRunesOffset, value)) {
        ERD::Main::g_FeatureStatus.character_runes = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerRuneMemoryOffset, value)) {
        ERD::Main::g_FeatureStatus.character_rune_memory = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerNewGameOffset, value)) {
        ERD::Main::g_FeatureStatus.current_ng_cycle = std::clamp(value, 0, 10);
    }

    std::uint8_t tiny = 0;
    if (ReadPlayerGameDataValue(player_game_data, kPlayerScadutreeBlessingOffset, tiny)) {
        ERD::Main::g_FeatureStatus.character_scadutree_blessing = static_cast<int>(tiny);
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerReveredSpiritAshBlessingOffset, tiny)) {
        ERD::Main::g_FeatureStatus.character_revered_spirit_ash_blessing = static_cast<int>(tiny);
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerHandStyleOffset, tiny)) {
        ERD::Main::g_FeatureStatus.player_hand_style = static_cast<int>(tiny);
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerCurrentWeaponLeftOffset, value)) {
        ERD::Main::g_FeatureStatus.player_current_weapon_slot_left = value;
    }
    if (ReadPlayerGameDataValue(player_game_data, kPlayerCurrentWeaponRightOffset, value)) {
        ERD::Main::g_FeatureStatus.player_current_weapon_slot_right = value;
    }

    std::array<std::atomic_int*, 6> weapon_targets{{
        &ERD::Main::g_FeatureStatus.player_left_weapon_1_id,
        &ERD::Main::g_FeatureStatus.player_right_weapon_1_id,
        &ERD::Main::g_FeatureStatus.player_left_weapon_2_id,
        &ERD::Main::g_FeatureStatus.player_right_weapon_2_id,
        &ERD::Main::g_FeatureStatus.player_left_weapon_3_id,
        &ERD::Main::g_FeatureStatus.player_right_weapon_3_id,
    }};
    for (std::size_t index = 0; index < kPlayerWeaponOffsets.size(); ++index) {
        if (ReadPlayerGameDataValue(player_game_data, kPlayerWeaponOffsets[index], value)) {
            *weapon_targets[index] = value;
        }
    }

    std::array<std::atomic_int*, 6> ammo_targets{{
        &ERD::Main::g_FeatureStatus.player_arrow_1_id,
        &ERD::Main::g_FeatureStatus.player_bolt_1_id,
        &ERD::Main::g_FeatureStatus.player_arrow_2_id,
        &ERD::Main::g_FeatureStatus.player_bolt_2_id,
        &ERD::Main::g_FeatureStatus.player_arrow_3_id,
        &ERD::Main::g_FeatureStatus.player_bolt_3_id,
    }};
    for (std::size_t index = 0; index < kPlayerAmmoOffsets.size(); ++index) {
        if (ReadPlayerGameDataValue(player_game_data, kPlayerAmmoOffsets[index], value)) {
            *ammo_targets[index] = value;
        }
    }

    std::array<std::atomic_int*, 5> talisman_targets{{
        &ERD::Main::g_FeatureStatus.player_talisman_1_id,
        &ERD::Main::g_FeatureStatus.player_talisman_2_id,
        &ERD::Main::g_FeatureStatus.player_talisman_3_id,
        &ERD::Main::g_FeatureStatus.player_talisman_4_id,
        &ERD::Main::g_FeatureStatus.player_talisman_5_id,
    }};
    for (std::size_t index = 0; index < kPlayerTalismanOffsets.size(); ++index) {
        if (ReadPlayerGameDataValue(player_game_data, kPlayerTalismanOffsets[index], value)) {
            *talisman_targets[index] = value;
        }
    }

    uintptr_t model_scale_root = 0;
    float model_scale_value = 1.0f;
    if (ResolveLocalPlayerModelScaleRoot(world_chr_man, model_scale_root) &&
        ERD::Game::ReadValue(model_scale_root + kPlayerModelScaleWidthOffset, model_scale_value)) {
        ERD::Main::g_FeatureStatus.player_model_scale_width_current = model_scale_value;
    }
    if (model_scale_root != 0 &&
        ERD::Game::ReadValue(model_scale_root + kPlayerModelScaleHeightOffset, model_scale_value)) {
        ERD::Main::g_FeatureStatus.player_model_scale_height_current = model_scale_value;
    }
    if (model_scale_root != 0 &&
        ERD::Game::ReadValue(model_scale_root + kPlayerModelScaleDepthOffset, model_scale_value)) {
        ERD::Main::g_FeatureStatus.player_model_scale_depth_current = model_scale_value;
    }

    uintptr_t slots_root = 0;
    uintptr_t player_base = 0;
    uintptr_t resource_block = 0;
    if (world_chr_man != 0 &&
        ERD::Game::ReadValue(world_chr_man + kNetPlayersOffset, slots_root) &&
        slots_root != 0 &&
        ERD::Game::ReadValue(slots_root, player_base) &&
        player_base != 0 &&
        ERD::Game::ReadValue(player_base + kCharacterFlagsBlockOffset, resource_block) &&
        resource_block != 0) {
        if (ERD::Game::ReadValue(resource_block + kPlayerCurrentHpOffset, value)) {
            ERD::Main::g_FeatureStatus.current_hp = value;
            ERD::Main::g_FeatureStatus.edit_hp = value;
        }
        if (ERD::Game::ReadValue(resource_block + kPlayerMaxHpOffset, value)) {
            ERD::Main::g_FeatureStatus.max_hp = value;
        }
        if (ERD::Game::ReadValue(resource_block + kPlayerCurrentFpOffset, value)) {
            ERD::Main::g_FeatureStatus.current_mp = value;
            ERD::Main::g_FeatureStatus.edit_mp = value;
        }
        if (ERD::Game::ReadValue(resource_block + kPlayerMaxFpOffset, value)) {
            ERD::Main::g_FeatureStatus.max_mp = value;
        }
        if (ERD::Game::ReadValue(resource_block + kPlayerCurrentSpOffset, value)) {
            ERD::Main::g_FeatureStatus.current_sp = value;
            ERD::Main::g_FeatureStatus.edit_sp = value;
        }
        if (ERD::Game::ReadValue(resource_block + kPlayerMaxSpOffset, value)) {
            ERD::Main::g_FeatureStatus.max_sp = value;
        }
    }
}

void SyncPlayerModelScale(uintptr_t world_chr_man) {
    uintptr_t model_scale_root = 0;
    if (!ResolveLocalPlayerModelScaleRoot(world_chr_man, model_scale_root)) {
        return;
    }

    const std::array<std::pair<uintptr_t, float>, 3> values{{
        {kPlayerModelScaleWidthOffset, std::clamp(ERD::Main::g_FeatureStatus.model_scale_uniform.load(), 0.0f, 3.0f)},
        {kPlayerModelScaleHeightOffset, std::clamp(ERD::Main::g_FeatureStatus.model_scale_uniform.load(), 0.0f, 3.0f)},
        {kPlayerModelScaleDepthOffset, std::clamp(ERD::Main::g_FeatureStatus.model_scale_uniform.load(), 0.0f, 3.0f)},
    }};

    for (const auto& [offset, desired] : values) {
        float current = 0.0f;
        if (!ERD::Game::ReadValue(model_scale_root + offset, current) || std::fabs(current - desired) < 0.0001f) {
            continue;
        }
        ERD::Game::WriteValue(model_scale_root + offset, desired);
    }
}

bool CallGameFunctionNoArgs(uintptr_t module_relative_offset) {
    HMODULE module = GetModuleHandleW(nullptr);
    if (module == nullptr) {
        return false;
    }

    const uintptr_t address = reinterpret_cast<uintptr_t>(module) + module_relative_offset;
    auto fn = reinterpret_cast<std::uint64_t(__fastcall*)(std::uint64_t, std::uint64_t, std::uint64_t, std::uint64_t)>(address);
    __try {
        fn(0, 0, 0, 0);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
    return true;
}

uintptr_t ResolveWarpToBlockAddress() {
    if (g_warp_to_block_fn != nullptr) {
        return reinterpret_cast<uintptr_t>(g_warp_to_block_fn);
    }

    uintptr_t text_base = 0;
    std::size_t text_size = 0;
    if (!RegionFromMainModuleText(text_base, text_size) || text_size < 0x30) {
        return 0;
    }

    const std::array<std::uint8_t, 4> marker{0x0F, 0xB6, 0x93, 0xAE};
    const auto* begin = reinterpret_cast<const std::uint8_t*>(text_base);
    const auto* end = begin + text_size - marker.size();
    for (const auto* cursor = begin; cursor <= end; ++cursor) {
        if (std::memcmp(cursor, marker.data(), marker.size()) != 0) {
            continue;
        }

        const uintptr_t match = reinterpret_cast<uintptr_t>(cursor);
        std::uint8_t opcode = 0;
        if (!ERD::Game::ReadValue(match + 0x13, opcode) || opcode != 0xE8) {
            continue;
        }

        std::int32_t rel32 = 0;
        if (!ERD::Game::ReadValue(match + 0x14, rel32)) {
            continue;
        }

        const uintptr_t target = (match + 0x13 + 5) + static_cast<std::int64_t>(rel32);
        if (target < text_base || target >= (text_base + text_size)) {
            continue;
        }

        g_warp_to_block_fn = reinterpret_cast<WarpToBlockFn>(target);
        spdlog::info("Resolved WarpToBlock function at 0x{:X}.", target);
        return target;
    }

    return 0;
}

void LoadBossReviveData() {
    grace_test::localization::refresh();
    std::string desired_language = grace_test::localization::active_language_code();
    if (desired_language.empty()) {
        desired_language = "en-US";
    }

    if (g_boss_revives_loaded && _stricmp(g_boss_revives_loaded_language.c_str(), desired_language.c_str()) == 0) {
        return;
    }

    g_boss_revives_loaded = true;
    g_boss_revives_loaded_language = desired_language;
    g_boss_revive_entries.clear();
    g_boss_revive_menu_items.clear();

    const std::filesystem::path resources_dir = g_runtime_folder / "Resources";
    const std::filesystem::path lang_csv_path = resources_dir / ("BossRevives-" + desired_language + ".csv");
    const std::filesystem::path english_csv_path = resources_dir / "BossRevives-en-US.csv";
    const std::filesystem::path legacy_csv_path = resources_dir / "BossRevives.csv";
    std::filesystem::path csv_path;
    if (std::filesystem::exists(lang_csv_path)) {
        csv_path = lang_csv_path;
    } else if (std::filesystem::exists(english_csv_path)) {
        csv_path = english_csv_path;
    } else {
        csv_path = legacy_csv_path;
    }

    std::ifstream input(csv_path, std::ios::binary);
    if (!input.is_open()) {
        spdlog::warn("Boss revive data not found (lang={}): {}", desired_language, csv_path.string());
        return;
    }

    std::string line;
    bool first_line = true;
    int menu_id = 1;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (first_line) {
            first_line = false;
            continue;
        }
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> parts = Split(line, ',');
        if (parts.size() < 14) {
            continue;
        }

        BossReviveEntry entry{};
        entry.menu_id = menu_id++;
        entry.is_dlc = ParseBool(parts[0]);
        entry.area = Trim(parts[1]);
        entry.boss_name = Trim(parts[2]);
        entry.is_initialize_dead_set = ParseBool(parts[3]);
        try {
            entry.block_id = static_cast<std::uint32_t>(std::stoul(Trim(parts[5])));
        } catch (...) {
            entry.block_id = 0;
        }

        for (const std::string& token : Split(parts[4], '|')) {
            const std::string trimmed = Trim(token);
            if (trimmed.empty()) {
                continue;
            }
            try {
                entry.npc_param_ids.push_back(static_cast<std::uint32_t>(std::stoul(trimmed)));
            } catch (...) {
            }
        }

        for (const std::string& token : Split(parts[7], '|')) {
            const std::string trimmed = Trim(token);
            if (trimmed.empty()) {
                continue;
            }
            const std::size_t sep = trimmed.find(':');
            if (sep == std::string::npos) {
                continue;
            }

            try {
                BossFlagSetting flag{};
                flag.event_id = static_cast<std::uint32_t>(std::stoul(Trim(trimmed.substr(0, sep))));
                flag.enabled = ParseBool(trimmed.substr(sep + 1));
                entry.boss_flags.push_back(flag);
            } catch (...) {
            }
        }

        g_boss_revive_menu_items.push_back({entry.menu_id, entry.area, entry.boss_name});
        g_boss_revive_entries.push_back(std::move(entry));
    }

    spdlog::info(
        "Loaded {} boss revive entries from {} (lang={}).",
        g_boss_revive_entries.size(),
        csv_path.string(),
        desired_language);
}

void LoadQuickAccessShops() {
    grace_test::localization::refresh();
    std::string desired_language = grace_test::localization::active_language_code();
    if (desired_language.empty()) {
        desired_language = "en-US";
    }

    if (g_quick_access_shops_loaded &&
        _stricmp(g_quick_access_shops_loaded_language.c_str(), desired_language.c_str()) == 0) {
        return;
    }

    g_quick_access_shops_loaded = true;
    g_quick_access_shops_loaded_language = desired_language;
    g_quick_access_shop_entries.clear();
    g_quick_access_shop_menu_items.clear();

    const std::filesystem::path resources_dir = g_runtime_folder / "Resources";
    const std::filesystem::path lang_csv_path = resources_dir / ("Shops-" + desired_language + ".csv");
    const std::filesystem::path english_csv_path = resources_dir / "Shops-en-US.csv";
    const std::filesystem::path legacy_csv_path = resources_dir / "Shops.csv";
    std::filesystem::path csv_path;
    if (std::filesystem::exists(lang_csv_path)) {
        csv_path = lang_csv_path;
    } else if (std::filesystem::exists(english_csv_path)) {
        csv_path = english_csv_path;
    } else {
        csv_path = legacy_csv_path;
    }

    std::ifstream input(csv_path, std::ios::binary);
    if (!input.is_open()) {
        spdlog::warn("Quick access shops data not found (lang={}): {}", desired_language, csv_path.string());
        return;
    }

    std::string line;
    int menu_id = 1;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> parts = Split(line, ',');
        if (parts.size() < 4) {
            continue;
        }

        int command_id = 0;
        try {
            command_id = std::stoi(Trim(parts[2]));
        } catch (...) {
            continue;
        }

        const auto command = MakeQuickAccessTalkCommand(
            command_id,
            std::vector<std::string>(parts.begin() + 3, parts.end()));
        if (!command.has_value()) {
            continue;
        }

        QuickAccessShopEntry entry{};
        entry.menu_id = menu_id++;
        entry.is_dlc = ParseBool(parts[0]);
        entry.name = Trim(parts[1]);
        entry.command = command.value();
        g_quick_access_shop_menu_items.push_back({entry.menu_id, entry.name});
        g_quick_access_shop_entries.push_back(std::move(entry));
    }

    spdlog::info(
        "Loaded {} quick access shops from {} (lang={}).",
        g_quick_access_shop_entries.size(),
        csv_path.string(),
        desired_language);
}

ERD::NpcParam* FindNpcParamRowWithFallback(
    const ERD::Game::SingletonRegistry& singletons,
    std::uint32_t row_id) {
    constexpr std::array<const wchar_t*, 3> kNpcParamNames{{
        L"NpcParam",
        L"NPC_PARAM_ST",
        L"NpcParam_PC",
    }};
    for (const wchar_t* table_name : kNpcParamNames) {
        if (ERD::NpcParam* row = ERD::Game::FindParamRow<ERD::NpcParam>(singletons, table_name, row_id)) {
            return row;
        }
    }
    return nullptr;
}

bool ReadTarnishedToolDlcCheckByte(const ERD::Game::SingletonRegistry& singletons, std::uint8_t& value) {
    const uintptr_t cs_dlc = singletons.GetObjectPointer("CsDlc");
    return cs_dlc != 0 && ERD::Game::ReadValue(cs_dlc + kCsDlcCheckOffset, value);
}

bool AcquireGesture(const ERD::Game::SingletonRegistry& singletons, std::int32_t gesture_id) {
    const std::array<std::int64_t, 4> params{gesture_id, 0, 0, 0};
    return ERD::Features::ExecutePlayerTalkCommand(singletons, kTalkCommandAcquireGesture, params, 1);
}

bool IsCharacterLoaded(const ERD::Game::SingletonRegistry& singletons) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    if (world_chr_man == 0) {
        return false;
    }

    uintptr_t common_flags_address = 0;
    if (ERD::Game::ResolvePointerChain(
            world_chr_man,
            {kNetPlayersOffset, 0x0, kCharacterFlagsBlockOffset, 0x0},
            kCommonFlagsOffset,
            common_flags_address) &&
        common_flags_address != 0) {
        return true;
    }

    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    if (game_man != 0) {
        std::int32_t save_slot = -1;
        if (ERD::Game::ReadValue(game_man + kSaveSlotOffset, save_slot) && save_slot != -1) {
            return true;
        }
    }

    return false;
}

void SyncPlayerSpEffectToggle(
    ERD::Features::SpEffectMonitor& monitor,
    uintptr_t player_base,
    bool desired,
    std::uint32_t sp_effect_id,
    bool& applied_state,
    const char* tag) {
    if (player_base == 0 || sp_effect_id == 0) {
        return;
    }

    if (desired && !applied_state) {
        if (monitor.ApplySpEffect(player_base, sp_effect_id)) {
            applied_state = true;
            spdlog::info("{} enabled (SpEffect {}).", tag, sp_effect_id);
        }
        return;
    }

    if (!desired && applied_state) {
        if (monitor.RemoveSpEffect(player_base, sp_effect_id)) {
            applied_state = false;
            spdlog::info("{} disabled (SpEffect {}).", tag, sp_effect_id);
        }
    }
}

bool HasSpEffect(const ERD::Features::SpEffectMonitorSnapshot& snapshot, std::uint32_t sp_effect_id) {
    for (const ERD::Features::SpEffectEntrySnapshot& entry : snapshot.active_effects) {
        if (entry.id == static_cast<int>(sp_effect_id)) {
            return true;
        }
    }
    return false;
}

bool HasAllSpEffects(
    const ERD::Features::SpEffectMonitorSnapshot& snapshot,
    const std::array<std::uint32_t, 8>& sp_effect_ids) {
    for (const std::uint32_t sp_effect_id : sp_effect_ids) {
        if (!HasSpEffect(snapshot, sp_effect_id)) {
            return false;
        }
    }
    return true;
}

void SyncPlayerAnimationSpeed(uintptr_t player_base) {
    if (player_base == 0) {
        return;
    }

    const int percent = std::clamp(ERD::Main::g_FeatureStatus.player_speed_increase_percent.load(), 0, 200);
    const float target_speed = 1.0f + static_cast<float>(percent) / 100.0f;
    ERD::Main::g_FeatureStatus.movement_speed = target_speed;

    uintptr_t speed_address = 0;
    if (!ERD::Game::ResolvePointerChain(
            player_base,
            {kCharacterFlagsBlockOffset, kChrBehaviorModuleOffset},
            kChrBehaviorAnimSpeedOffset,
            speed_address) ||
        speed_address == 0) {
        return;
    }

    float current_speed = 0.0f;
    if (ERD::Game::ReadValue(speed_address, current_speed) && std::fabs(current_speed - target_speed) < 0.0001f) {
        return;
    }

    ERD::Game::WriteValue(speed_address, target_speed);
}

void tick_features() {
    grace_test::config::maybe_reload_if_changed();
    grace_test::overlay::tick();

    const bool game_ready = IsCharacterLoaded(g_singletons);
    ERD::Main::g_FeatureStatus.game_ready = game_ready;

    if (game_ready && !g_last_game_ready) {
        grace_test::config::apply_persisted_feature_state();
        normalize_feature_state_for_plugins();
        spdlog::info("Game ready detected. Re-applied persisted game mod settings.");
    } else if (!game_ready && g_last_game_ready) {
        g_param_patches.Reset(g_singletons);
        spdlog::info("Character unloaded. Waiting for next in-game load.");
    }
    g_last_game_ready = game_ready;

    if (!game_ready) {
        g_beast_not_hostile_applied = false;
        g_no_fall_death_applied = false;
        g_required_goods_ensured = false;
        g_next_reveal_invisible_check_tick = 0;
        g_next_required_goods_check_tick = 0;
        grace_test::talkscript::reset_runtime_state();
        g_rally_hooks.Clear();
        g_sp_effect_monitor.Clear();
        return;
    }

    g_character_flags.Tick(g_singletons);
    g_auto_pickup.Tick(g_singletons);
    g_param_patches.Tick(g_singletons);
    g_inspector_tools.Tick(g_singletons);
    g_sp_effect_monitor.Tick(g_singletons);
    g_rally_hooks.Tick(g_singletons);
    const ULONGLONG now = GetTickCount64();
    if (!g_required_goods_ensured && now >= g_next_required_goods_check_tick) {
        EnsureRequiredGoods(g_singletons);
    }
    SyncPlayerReadOnlySnapshot();
    const uintptr_t world_chr_man = g_singletons.GetObjectPointer("WorldChrMan");
    SyncPlayerModelScale(world_chr_man);
    uintptr_t local_player_base = 0;
    if (ResolveLocalPlayerBase(world_chr_man, local_player_base)) {
        SyncPlayerAnimationSpeed(local_player_base);
    }

    const ERD::Features::SpEffectMonitorSnapshot sp_snapshot = g_sp_effect_monitor.Snapshot();
    if (sp_snapshot.valid && sp_snapshot.player_base != 0) {
        SyncPlayerSpEffectToggle(
            g_sp_effect_monitor,
            sp_snapshot.player_base,
            ERD::Main::g_FeatureStatus.beast_not_hostile.load(),
            kSpEffectBeastNotHostileId,
            g_beast_not_hostile_applied,
            "Beast not hostile");

        if (now >= g_next_reveal_invisible_check_tick) {
            g_next_reveal_invisible_check_tick = now + kRevealInvisibleCheckIntervalMs;
            if (!HasSpEffect(sp_snapshot, kSpEffectRevealInvisibleId)) {
                if (g_sp_effect_monitor.ApplySpEffect(sp_snapshot.player_base, kSpEffectRevealInvisibleId)) {
                    spdlog::info("Reveal invisible (always on) re-applied (SpEffect {}).", kSpEffectRevealInvisibleId);
                }
            }
        }

        const int buff_extend_mode = ERD::Main::g_FeatureStatus.buff_duration_extend_mode.load();
        const bool buff_extend_enabled = buff_extend_mode != 0;
        const bool has_buff_extend = HasSpEffect(sp_snapshot, kSpEffectBuffDurationExtendId);
        if (buff_extend_enabled && !has_buff_extend) {
            if (g_sp_effect_monitor.ApplySpEffect(sp_snapshot.player_base, kSpEffectBuffDurationExtendId)) {
                spdlog::info("Buff duration extend enabled (SpEffect {}).", kSpEffectBuffDurationExtendId);
            }
        } else if (!buff_extend_enabled && has_buff_extend) {
            if (g_sp_effect_monitor.RemoveSpEffect(sp_snapshot.player_base, kSpEffectBuffDurationExtendId)) {
                spdlog::info("Buff duration extend disabled (SpEffect {}).", kSpEffectBuffDurationExtendId);
            }
        }

        const bool no_fall_death_enabled = ERD::Main::g_FeatureStatus.no_fall_death.load();
        const bool has_no_fall_death = HasSpEffect(sp_snapshot, kSpEffectNoFallDeathId);
        const bool has_no_fall_death_middle = HasSpEffect(sp_snapshot, kSpEffectNoFallDeathMiddleId);
        const bool has_no_fall_death_trigger = HasSpEffect(sp_snapshot, kSpEffectNoFallDeathTriggerId);
        const auto try_apply_no_fall_effect = [&](std::uint32_t sp_effect_id) -> bool {
            if (!sp_snapshot.valid || sp_snapshot.player_base == 0) {
                return false;
            }
            return g_sp_effect_monitor.ApplySpEffect(sp_snapshot.player_base, sp_effect_id);
        };
        if (no_fall_death_enabled) {
            if (!has_no_fall_death) {
                if (try_apply_no_fall_effect(kSpEffectNoFallDeathId)) {
                    g_no_fall_death_applied = true;
                    spdlog::info("No fall death enabled (SpEffect {}).", kSpEffectNoFallDeathId);
                }
            }
            if (!has_no_fall_death_middle) {
                if (try_apply_no_fall_effect(kSpEffectNoFallDeathMiddleId)) {
                    g_no_fall_death_applied = true;
                    spdlog::info("No fall death chain restored (SpEffect {}).", kSpEffectNoFallDeathMiddleId);
                }
            }
            if (!has_no_fall_death_trigger) {
                if (try_apply_no_fall_effect(kSpEffectNoFallDeathTriggerId)) {
                    g_no_fall_death_applied = true;
                    spdlog::info("No fall death chain restored (SpEffect {}).", kSpEffectNoFallDeathTriggerId);
                }
            }
        } else if (has_no_fall_death || g_no_fall_death_applied) {
            if (!has_no_fall_death) {
                g_no_fall_death_applied = false;
            } else if (g_sp_effect_monitor.RemoveSpEffect(sp_snapshot.player_base, kSpEffectNoFallDeathId)) {
                g_no_fall_death_applied = false;
                spdlog::info("No fall death disabled (SpEffect {}).", kSpEffectNoFallDeathId);
            }
            if (has_no_fall_death_middle) {
                g_sp_effect_monitor.RemoveSpEffect(sp_snapshot.player_base, kSpEffectNoFallDeathMiddleId);
            }
            if (has_no_fall_death_trigger) {
                g_sp_effect_monitor.RemoveSpEffect(sp_snapshot.player_base, kSpEffectNoFallDeathTriggerId);
            }
        }

    }
}

}  // namespace

void run(const std::filesystem::path& folder) {
    g_runtime_folder = folder;
    normalize_feature_state_for_plugins();
    g_boss_revives_loaded = false;
    g_quick_access_shops_loaded = false;
    LoadBossReviveData();
    LoadQuickAccessShops();

    ERD::Main::Logger::Instance().Initialize(folder / "logs" / "erd_game_tools.log", true);

    while (ERD::g_Running && !g_singletons.Initialize()) {
        spdlog::warn("Waiting for FD4 singletons...");
        Sleep(1000);
    }

    if (!ERD::g_Running) {
        return;
    }

    spdlog::info("FD4 singleton registry initialized.");
    grace_test::overlay::initialize(folder);
    grace_test::talkscript::reset_runtime_state();

    while (ERD::g_Running) {
        try {
            tick_features();
        } catch (const std::exception& error) {
            spdlog::error("Feature tick failed: {}", error.what());
        } catch (...) {
            spdlog::error("Feature tick failed: unknown exception");
        }

        Sleep(ERD::kFeaturePollIntervalMs);
    }

    g_param_patches.Reset(g_singletons);
    grace_test::overlay::shutdown();
}

void request_stop() {
    ERD::g_Running = false;
}

const ERD::Game::SingletonRegistry* get_singletons() {
    return &g_singletons;
}

bool unlock_all_whetblades() {
    return ERD::Game::ApplyFlagList(g_singletons, kUnlockWhetbladeFlags);
}

bool unlock_all_gestures() {
    bool all_succeeded = true;
    for (const std::int32_t gesture_id : kBaseGestureIds) {
        all_succeeded = AcquireGesture(g_singletons, gesture_id) && all_succeeded;
    }

    std::uint8_t dlc_check = 1;
    ReadTarnishedToolDlcCheckByte(g_singletons, dlc_check);
    const bool preorder_base_gesture = dlc_check == 0;
    const bool is_dlc_available = dlc_check == 1;
    const bool preorder_dlc_gesture = dlc_check == 2;

    all_succeeded = AcquireGesture(g_singletons, preorder_base_gesture ? 108 : 109) && all_succeeded;

    if (is_dlc_available) {
        for (const std::int32_t gesture_id : kDlcGestureIds) {
            all_succeeded = AcquireGesture(g_singletons, gesture_id) && all_succeeded;
        }
        all_succeeded = AcquireGesture(g_singletons, preorder_dlc_gesture ? 116 : 113) && all_succeeded;
    }

    return all_succeeded;
}

bool unlock_maps(UnlockMapsMode mode) {
    bool all_succeeded = true;
    const bool has_dlc = ERD::Game::IsDlcOwned(g_singletons, kDlcShadowOfTheErdtreeIndex);
    if (mode == UnlockMapsMode::BaseGame || mode == UnlockMapsMode::All) {
        all_succeeded = ERD::Game::ApplyFlagList(g_singletons, kUnlockBaseMapFlags) && all_succeeded;
    }

    if (mode == UnlockMapsMode::Dlc || (mode == UnlockMapsMode::All && has_dlc)) {
        all_succeeded = ERD::Game::ApplyFlagList(g_singletons, kUnlockDlcMapFlags) && all_succeeded;
    }

    return all_succeeded;
}

bool open_quick_access_upgrade() {
    bool all_succeeded = true;
    for (const int flag : {232, 233, 234, 235}) {
        all_succeeded = ERD::Features::ExecuteEzStateTalkCommand(
                            g_singletons,
                            ERD::Features::EzStateTalkCommand{
                                kTalkCommandUpgradeFlag,
                                {6001, flag, 0, 0},
                                2,
                                false}) &&
                        all_succeeded;
    }
    all_succeeded = ERD::Features::ExecuteEzStateTalkCommand(
                        g_singletons,
                        ERD::Features::EzStateTalkCommand{kTalkCommandOpenUpgrade, {0, 0, 0, 0}, 1, false}) &&
                    all_succeeded;
    spdlog::info("Quick access: open upgrade {}", all_succeeded ? "succeeded" : "failed");
    return all_succeeded;
}

bool open_quick_access_spirit_tuning() {
    const bool ok = ERD::Features::ExecuteEzStateTalkCommand(
        g_singletons,
        ERD::Features::EzStateTalkCommand{kTalkCommandOpenSpiritTuning, {0, 0, 0, 0}, 0, false});
    spdlog::info("Quick access: open spirit tuning {}", ok ? "succeeded" : "failed");
    return ok;
}

bool open_quick_access_dragon_communion_church() {
    const bool ok = ERD::Features::ExecuteEzStateTalkCommand(
        g_singletons,
        ERD::Features::EzStateTalkCommand{kTalkCommandOpenDragonCommunion, {102350, 0, 0, 0}, 1, false});
    spdlog::info("Quick access: open dragon communion church {}", ok ? "succeeded" : "failed");
    return ok;
}

bool open_quick_access_dragon_communion_cathedral() {
    const bool ok = ERD::Features::ExecuteEzStateTalkCommand(
        g_singletons,
        ERD::Features::EzStateTalkCommand{kTalkCommandOpenDragonCommunion, {102351, 0, 0, 0}, 1, false});
    spdlog::info("Quick access: open dragon communion cathedral {}", ok ? "succeeded" : "failed");
    return ok;
}

bool open_quick_access_rebirth() {
    const bool ok = ERD::Features::ExecuteEzStateTalkCommand(
        g_singletons,
        ERD::Features::EzStateTalkCommand{kTalkCommandRebirth, {0, 0, 0, 0}, 0, false});
    spdlog::info("Quick access: open rebirth {}", ok ? "succeeded" : "failed");
    return ok;
}

bool open_quick_access_mirror() {
    const bool ok = ERD::Features::ExecuteEzStateTalkCommand(
        g_singletons,
        ERD::Features::EzStateTalkCommand{kTalkCommandOpenMirror, {0, 0, 0, 0}, 0, false});
    spdlog::info("Quick access: open mirror {}", ok ? "succeeded" : "failed");
    return ok;
}

bool open_quick_access_sell() {
    const bool ok = ERD::Features::ExecuteEzStateTalkCommand(
        g_singletons,
        ERD::Features::EzStateTalkCommand{kTalkCommandOpenSell, {0, 0, 0, 0}, 2, true});
    spdlog::info("Quick access: open sell {}", ok ? "succeeded" : "failed");
    return ok;
}

std::vector<QuickAccessShopMenuItem> get_quick_access_shop_menu_items() {
    LoadQuickAccessShops();
    return g_quick_access_shop_menu_items;
}

bool open_quick_access_shop_by_menu_id(int menu_id) {
    LoadQuickAccessShops();
    auto it = std::find_if(
        g_quick_access_shop_entries.begin(),
        g_quick_access_shop_entries.end(),
        [menu_id](const QuickAccessShopEntry& entry) { return entry.menu_id == menu_id; });
    if (it == g_quick_access_shop_entries.end()) {
        spdlog::warn("Quick access shop id {} was not found.", menu_id);
        return false;
    }

    if (it->is_dlc && !ERD::Game::IsDlcOwned(g_singletons, kDlcShadowOfTheErdtreeIndex)) {
        spdlog::info("Quick access shop '{}' skipped because DLC is not owned.", it->name);
        return false;
    }

    const bool ok = ERD::Features::ExecuteEzStateTalkCommand(g_singletons, it->command);
    spdlog::info(
        "Quick access: open shop '{}' command={} params=[{},{},{},{}] count={} player={} {}",
        it->name,
        it->command.command_id,
        it->command.params[0],
        it->command.params[1],
        it->command.params[2],
        it->command.params[3],
        it->command.param_count,
        it->command.use_player_handle ? "true" : "false",
        ok ? "succeeded" : "failed");
    return ok;
}


std::vector<BossReviveMenuItem> get_boss_revive_menu_items() {
    LoadBossReviveData();
    return g_boss_revive_menu_items;
}

bool revive_boss_by_menu_id(int menu_id) {
    LoadBossReviveData();
    auto it = std::find_if(
        g_boss_revive_entries.begin(),
        g_boss_revive_entries.end(),
        [menu_id](const BossReviveEntry& entry) { return entry.menu_id == menu_id; });
    if (it == g_boss_revive_entries.end()) {
        return false;
    }

    if (it->is_dlc && !ERD::Game::IsDlcOwned(g_singletons, kDlcShadowOfTheErdtreeIndex)) {
        spdlog::info("Boss revive skipped for '{}' because DLC is not owned.", it->boss_name);
        return false;
    }

    bool all_succeeded = true;

    if (!it->is_initialize_dead_set) {
        for (const std::uint32_t npc_param_id : it->npc_param_ids) {
            ERD::NpcParam* row = FindNpcParamRowWithFallback(g_singletons, npc_param_id);
            if (row == nullptr) {
                spdlog::warn(
                    "Boss revive '{}' missing NpcParam row {}.",
                    it->boss_name,
                    npc_param_id);
                all_succeeded = false;
                continue;
            }
            const uintptr_t flag_byte =
                reinterpret_cast<uintptr_t>(row) + kNpcParamInitializeDeadOffset;
            const bool ok = ERD::Game::SetBitFlag(flag_byte, kNpcParamInitializeDeadBitIndex, true);
            if (!ok) {
                spdlog::warn(
                    "Boss revive '{}' failed to set initialize-dead bit for NpcParam row {}.",
                    it->boss_name,
                    npc_param_id);
            }
            all_succeeded = ok && all_succeeded;
        }
    }

    for (const BossFlagSetting& flag : it->boss_flags) {
        const bool ok = ERD::Game::SetEventFlag(g_singletons, flag.event_id, flag.enabled);
        if (!ok) {
            spdlog::warn(
                "Boss revive '{}' failed to set event flag {}={}.",
                it->boss_name,
                flag.event_id,
                flag.enabled ? "true" : "false");
        }
        all_succeeded = ok && all_succeeded;
    }

    spdlog::info(
        "Boss revive executed: [{}] {} => {}",
        it->area,
        it->boss_name,
        all_succeeded ? "ok" : "partial");
    return all_succeeded;
}

bool warp_boss_by_menu_id(int menu_id) {
    LoadBossReviveData();
    auto it = std::find_if(
        g_boss_revive_entries.begin(),
        g_boss_revive_entries.end(),
        [menu_id](const BossReviveEntry& entry) { return entry.menu_id == menu_id; });
    if (it == g_boss_revive_entries.end()) {
        return false;
    }

    if (it->is_dlc && !ERD::Game::IsDlcOwned(g_singletons, kDlcShadowOfTheErdtreeIndex)) {
        spdlog::info("Boss warp skipped for '{}' because DLC is not owned.", it->boss_name);
        return false;
    }

    const uintptr_t warp_to_block = ResolveWarpToBlockAddress();
    if (warp_to_block == 0 || g_warp_to_block_fn == nullptr || it->block_id == 0) {
        return false;
    }

    const std::int32_t area = static_cast<std::int32_t>((it->block_id >> 24) & 0xFFu);
    const std::int32_t block = static_cast<std::int32_t>((it->block_id >> 16) & 0xFFu);
    const std::int32_t map = static_cast<std::int32_t>((it->block_id >> 8) & 0xFFu);
    const std::int32_t alt_no = static_cast<std::int32_t>(it->block_id & 0xFFu);

    g_warp_to_block_fn(area, block, map, alt_no);
    spdlog::info(
        "Boss warp executed: [{}] {} (blockId={}).",
        it->area,
        it->boss_name,
        it->block_id);
    return true;
}

}  // namespace grace_test::runtime
