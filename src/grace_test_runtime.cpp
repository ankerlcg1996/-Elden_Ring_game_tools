#include "grace_test_runtime.hpp"

#include "grace_test_config.hpp"
#include "grace_test_localization.hpp"
#include "grace_test_messages.hpp"
#include "grace_test_overlay.hpp"
#include "grace_test_talkscript.hpp"

#include "Common.hpp"
#include "Features/CharacterFlags.hpp"
#include "Features/InspectorTools.hpp"
#include "Features/ParamPatches.hpp"
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
ERD::Features::ParamPatches g_param_patches;
ERD::Features::InspectorTools g_inspector_tools;
ERD::Features::SpEffectMonitor g_sp_effect_monitor;
bool g_last_game_ready = false;
bool g_beast_not_hostile_applied = false;
ULONGLONG g_next_reveal_invisible_check_tick = 0;
std::filesystem::path g_runtime_folder;

constexpr uintptr_t kSaveSlotOffset = 0xAC0;
constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kCharacterFlagsBlockOffset = 0x190;
constexpr uintptr_t kChrBehaviorModuleOffset = 0x28;
constexpr uintptr_t kChrBehaviorAnimSpeedOffset = 0x17C8;
constexpr uintptr_t kCommonFlagsOffset = 0x19B;
constexpr uintptr_t kCsDlcCheckOffset = 0x11;
constexpr int kDlcShadowOfTheErdtreeIndex = 1;
constexpr int kTalkCommandAcquireGesture = 131;
constexpr int kTalkCommandOpenUpgrade = 24;
constexpr int kTalkCommandRebirth = 113;
constexpr int kTalkCommandOpenMirror = 81;
constexpr int kTalkCommandUpgradeFlag = 49;
constexpr std::uint32_t kSpEffectBeastNotHostileId = 460;
constexpr std::uint32_t kSpEffectRevealInvisibleId = 416;
constexpr std::uint32_t kSpEffectBuffDurationExtendId = 330600;
constexpr ULONGLONG kRevealInvisibleCheckIntervalMs = 500;
constexpr std::size_t kNpcParamInitializeDeadOffset = 0x14D;
constexpr int kNpcParamInitializeDeadBitIndex = 3;

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

std::vector<BossReviveEntry> g_boss_revive_entries;
std::vector<grace_test::runtime::BossReviveMenuItem> g_boss_revive_menu_items;
bool g_boss_revives_loaded = false;
std::string g_boss_revives_loaded_language = "en-US";
using WarpToBlockFn = void (*)(std::int32_t, std::int32_t, std::int32_t, std::int32_t);
WarpToBlockFn g_warp_to_block_fn = nullptr;

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

std::vector<std::string> Split(const std::string& value, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(value);
    std::string item;
    while (std::getline(stream, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
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
    grace_test::overlay::tick();

    const bool game_ready = IsCharacterLoaded(g_singletons);
    ERD::Main::g_FeatureStatus.game_ready = game_ready;

    if (game_ready && !g_last_game_ready) {
        grace_test::config::apply_persisted_feature_state();
        spdlog::info("Game ready detected. Re-applied persisted game mod settings.");
    } else if (!game_ready && g_last_game_ready) {
        spdlog::info("Character unloaded. Waiting for next in-game load.");
    }
    g_last_game_ready = game_ready;

    if (!game_ready) {
        g_beast_not_hostile_applied = false;
        g_next_reveal_invisible_check_tick = 0;
        g_sp_effect_monitor.Clear();
        return;
    }

    g_character_flags.Tick(g_singletons);
    g_param_patches.Tick(g_singletons);
    g_inspector_tools.Tick(g_singletons);
    g_sp_effect_monitor.Tick(g_singletons);

    const ERD::Features::SpEffectMonitorSnapshot sp_snapshot = g_sp_effect_monitor.Snapshot();
    if (sp_snapshot.valid && sp_snapshot.player_base != 0) {
        SyncPlayerAnimationSpeed(sp_snapshot.player_base);

        SyncPlayerSpEffectToggle(
            g_sp_effect_monitor,
            sp_snapshot.player_base,
            ERD::Main::g_FeatureStatus.beast_not_hostile.load(),
            kSpEffectBeastNotHostileId,
            g_beast_not_hostile_applied,
            "Beast not hostile");

        const ULONGLONG now = GetTickCount64();
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
    }
}

}  // namespace

void run(const std::filesystem::path& folder) {
    g_runtime_folder = folder;
    g_boss_revives_loaded = false;
    LoadBossReviveData();

    ERD::Main::Logger::Instance().Initialize(folder / "logs" / "erd_game_tools.log", true);
    grace_test::overlay::initialize(folder);

    while (ERD::g_Running && !g_singletons.Initialize()) {
        spdlog::warn("Waiting for FD4 singletons...");
        Sleep(1000);
    }

    if (!ERD::g_Running) {
        return;
    }

    spdlog::info("FD4 singleton registry initialized.");

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

    grace_test::overlay::shutdown();
}

void request_stop() {
    ERD::g_Running = false;
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

bool open_portable_upgrade() {
    bool all_succeeded = true;
    for (const int flag : {232, 233, 234, 235}) {
        all_succeeded =
            ERD::Features::ExecutePlayerTalkCommand(g_singletons, kTalkCommandUpgradeFlag, {6001, flag, 0, 0}, 2) &&
            all_succeeded;
    }
    all_succeeded =
        ERD::Features::ExecutePlayerTalkCommand(g_singletons, kTalkCommandOpenUpgrade, {0, 0, 0, 0}, 1) &&
        all_succeeded;
    spdlog::info("Portable grace: open upgrade {}", all_succeeded ? "succeeded" : "failed");
    return all_succeeded;
}

bool open_portable_rebirth() {
    const bool ok = ERD::Features::ExecutePlayerTalkCommand(g_singletons, kTalkCommandRebirth, {0, 0, 0, 0}, 0);
    spdlog::info("Portable grace: open rebirth {}", ok ? "succeeded" : "failed");
    return ok;
}

bool open_portable_mirror() {
    const bool ok = ERD::Features::ExecutePlayerTalkCommand(g_singletons, kTalkCommandOpenMirror, {0, 0, 0, 0}, 0);
    spdlog::info("Portable grace: open mirror {}", ok ? "succeeded" : "failed");
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
                all_succeeded = false;
                continue;
            }
            const uintptr_t flag_byte =
                reinterpret_cast<uintptr_t>(row) + kNpcParamInitializeDeadOffset;
            all_succeeded =
                ERD::Game::SetBitFlag(flag_byte, kNpcParamInitializeDeadBitIndex, true) && all_succeeded;
        }
    }

    for (const BossFlagSetting& flag : it->boss_flags) {
        all_succeeded = ERD::Game::SetEventFlag(g_singletons, flag.event_id, flag.enabled) && all_succeeded;
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
