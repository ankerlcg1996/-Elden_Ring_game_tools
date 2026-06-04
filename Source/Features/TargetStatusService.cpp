#include "TargetStatusService.hpp"

#include "../Game/Memory.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"

#include <algorithm>
#include <format>

#include <spdlog/spdlog.h>

namespace ERD::Features {
namespace {

constexpr uintptr_t kChrInsHandleOffset = 0x8;
constexpr uintptr_t kChrInsTargetHandleOffset = 0x6B0;
constexpr uintptr_t kChrInsNpcParamIdOffset = 0x60;
constexpr uintptr_t kChrInsChrIdOffset = 0x64;
constexpr uintptr_t kChrInsTeamTypeOffset = 0x6C;
constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kChrSetEntriesOffset = 0x18;
constexpr uintptr_t kChrSetEntryStride = 0x10;
constexpr std::array<uintptr_t, 2> kChrSetPoolOffsets{0x1DED8, 0x18038};
constexpr uintptr_t kChrInsModulesOffset = 0x190;
constexpr uintptr_t kChrDataModuleOffset = 0x0;
constexpr uintptr_t kChrResistModuleOffset = 0x20;
constexpr uintptr_t kChrSuperArmorModuleOffset = 0x40;
constexpr uintptr_t kChrPhysicsModuleOffset = 0x68;
constexpr uintptr_t kCurrentHpOffset = 0x138;
constexpr uintptr_t kMaxHpOffset = 0x13C;
constexpr uintptr_t kCurrentPoiseOffset = 0x10;
constexpr uintptr_t kMaxPoiseOffset = 0x14;
constexpr uintptr_t kCurrentPoisonOffset = 0x10;
constexpr uintptr_t kCurrentRotOffset = 0x14;
constexpr uintptr_t kCurrentBleedOffset = 0x18;
constexpr uintptr_t kCurrentBlightOffset = 0x1C;
constexpr uintptr_t kCurrentFrostOffset = 0x20;
constexpr uintptr_t kCurrentSleepOffset = 0x24;
constexpr uintptr_t kCurrentMadnessOffset = 0x28;
constexpr uintptr_t kMaxPoisonOffset = 0x2C;
constexpr uintptr_t kMaxRotOffset = 0x30;
constexpr uintptr_t kMaxBleedOffset = 0x34;
constexpr uintptr_t kMaxBlightOffset = 0x38;
constexpr uintptr_t kMaxFrostOffset = 0x3C;
constexpr uintptr_t kMaxSleepOffset = 0x40;
constexpr uintptr_t kMaxMadnessOffset = 0x44;
constexpr uintptr_t kPoisonModOffset = 0x48;
constexpr uintptr_t kRotModOffset = 0x4C;
constexpr uintptr_t kBleedModOffset = 0x50;
constexpr uintptr_t kBlightModOffset = 0x54;
constexpr uintptr_t kFrostModOffset = 0x58;
constexpr uintptr_t kSleepModOffset = 0x5C;
constexpr uintptr_t kMadnessModOffset = 0x60;
constexpr uintptr_t kCoordsOffset = 0x70;

constexpr const char* kCsFeManStoragePattern =
    "48 8B 0D ? ? ? ? 8B DA 48 85 C9 75 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 4C 8B C8 4C 8D 05 ? ? ? ? BA B4 00 00 00 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 0D ? ? ? ? 8B D3 E8 ? ? ? ? 48 8B D8";
constexpr const char* kGetChrInsFromHandlePattern =
    "48 83 EC 28 E8 17 FF FF FF 48 85 C0 74 08 48 8B 00 48 83 C4 28 C3";
constexpr std::ptrdiff_t kRipAdjust = 3;
constexpr int kEntityBarCount = 8;
constexpr int kBossBarCount = 3;
constexpr float kBossScreenX = 963.0f;
constexpr float kBossScreenY = 945.0f;
constexpr float kBossNextDiffY = 55.0f;

struct Pattern {
    std::vector<std::uint8_t> bytes;
    std::vector<std::uint8_t> masks;
};

struct Region {
    uintptr_t base = 0;
    std::size_t size = 0;
};

struct EntityHpBar {
    std::uint8_t padding[0x40]{};
};
static_assert(sizeof(EntityHpBar) == 0x40);

struct BossHpBar {
    std::int32_t display_id = -1;
    std::uint32_t pad0 = 0;
    std::uint64_t boss_handle = 0;
    std::int32_t current_display_damage = 0;
    std::int32_t unknown0 = 0;
    bool is_hit = false;
    std::uint8_t pad1[3]{};
    float display_time = 0.0f;
};
static_assert(sizeof(BossHpBar) == 0x20);

using GetChrInsFromHandleFn = uintptr_t (*)(uintptr_t world_chr_man, std::uint64_t* chr_handle_ptr);

int HexValue(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    return -1;
}

bool ParsePattern(const char* aob, Pattern& pattern) {
    pattern.bytes.clear();
    pattern.masks.clear();

    std::string token;
    for (const char* cursor = aob;; ++cursor) {
        const char ch = *cursor;
        if (ch == ' ' || ch == '\0') {
            if (!token.empty()) {
                std::uint8_t byte = 0;
                std::uint8_t mask = 0;
                if (token.size() == 1) {
                    const int hi = HexValue(token[0]);
                    if (hi >= 0) {
                        byte = static_cast<std::uint8_t>(hi);
                        mask = 0x0F;
                    } else if (token[0] != '?') {
                        return false;
                    }
                } else if (token.size() == 2) {
                    const int hi = HexValue(token[0]);
                    const int lo = HexValue(token[1]);
                    if (hi >= 0) {
                        byte |= static_cast<std::uint8_t>(hi << 4);
                        mask |= 0xF0;
                    } else if (token[0] != '?') {
                        return false;
                    }
                    if (lo >= 0) {
                        byte |= static_cast<std::uint8_t>(lo);
                        mask |= 0x0F;
                    } else if (token[1] != '?') {
                        return false;
                    }
                } else {
                    return false;
                }

                pattern.bytes.push_back(byte);
                pattern.masks.push_back(mask);
                token.clear();
            }

            if (ch == '\0') {
                break;
            }
        } else {
            token.push_back(ch);
        }
    }

    return !pattern.bytes.empty();
}

bool RegionFromMainModuleText(Region& region) {
    HMODULE module = GetModuleHandleW(nullptr);
    if (module == nullptr) {
        return false;
    }

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    const auto* nt =
        reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<uintptr_t>(module) + dos->e_lfanew);
    const IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const IMAGE_SECTION_HEADER* section = sections + i;
        if (_stricmp(reinterpret_cast<const char*>(section->Name), ".text") == 0) {
            region.base = reinterpret_cast<uintptr_t>(module) + section->VirtualAddress;
            region.size = section->Misc.VirtualSize;
            return true;
        }
    }

    return false;
}

bool MatchesPattern(const std::uint8_t* address, const Pattern& pattern) {
    for (std::size_t i = 0; i < pattern.bytes.size(); ++i) {
        if (((address[i] ^ pattern.bytes[i]) & pattern.masks[i]) != 0) {
            return false;
        }
    }
    return true;
}

uintptr_t FindPatternInText(const char* aob) {
    Pattern pattern{};
    Region region{};
    if (!ParsePattern(aob, pattern) || !RegionFromMainModuleText(region) || region.size < pattern.bytes.size()) {
        return 0;
    }

    const auto* begin = reinterpret_cast<const std::uint8_t*>(region.base);
    const auto* end = begin + region.size - pattern.bytes.size();
    for (const auto* cursor = begin; cursor <= end; ++cursor) {
        if (MatchesPattern(cursor, pattern)) {
            return reinterpret_cast<uintptr_t>(cursor);
        }
    }

    return 0;
}

uintptr_t ResolveRipPointer(uintptr_t instruction, std::ptrdiff_t rip_adjust) {
    std::int32_t rel32 = 0;
    if (!Game::ReadValue(instruction + rip_adjust, rel32)) {
        return 0;
    }
    return instruction + rip_adjust + sizeof(rel32) + rel32;
}

template <typename T>
bool ReadModulePointer(uintptr_t chr_ins, uintptr_t module_offset, T& out_ptr) {
    uintptr_t modules = 0;
    return Game::ReadValue(chr_ins + kChrInsModulesOffset, modules) &&
           modules != 0 &&
           Game::ReadValue(modules + module_offset, out_ptr) &&
           out_ptr != 0;
}

uintptr_t ResolveChrInsByHandleFallback(uintptr_t world_chr_man, std::uint64_t handle) {
    const std::uint32_t compressed_handle = static_cast<std::uint32_t>(handle);
    const std::uint32_t pool_index = (compressed_handle >> 20) & 0xFFu;
    const std::uint32_t slot_index = compressed_handle & 0xFFFFFu;

    for (const uintptr_t chr_set_pool_offset : kChrSetPoolOffsets) {
        uintptr_t chr_set = 0;
        if (!Game::ReadValue(
                world_chr_man + chr_set_pool_offset + static_cast<uintptr_t>(pool_index) * sizeof(uintptr_t),
                chr_set) ||
            chr_set == 0) {
            continue;
        }

        uintptr_t entries_base = 0;
        if (!Game::ReadValue(chr_set + kChrSetEntriesOffset, entries_base) || entries_base == 0) {
            continue;
        }

        uintptr_t chr_ins = 0;
        if (Game::ReadValue(
                entries_base + static_cast<uintptr_t>(slot_index) * kChrSetEntryStride,
                chr_ins) &&
            chr_ins != 0) {
            return chr_ins;
        }
    }

    return 0;
}

void WriteAtomicInt(std::atomic_int& target, int value) {
    target = value;
}

void WriteAtomicFloat(std::atomic<float>& target, float value) {
    target = value;
}

}  // namespace

void TargetStatusService::Tick(const Game::SingletonRegistry& singletons) {
    if (!EnsureResolved()) {
        ClearPublishedSnapshot();
        return;
    }

    const uintptr_t target_chr = ResolveTargetChrIns(singletons);
    if (target_chr == 0) {
        ClearPublishedSnapshot();
        return;
    }

    PublishSnapshot(target_chr);
}

void TargetStatusService::Clear() {
    ClearPublishedSnapshot();
}

bool TargetStatusService::EnsureResolved() {
    if (resolved_) {
        return true;
    }
    if (resolve_attempted_) {
        return false;
    }

    resolve_attempted_ = true;
    resolved_ = ResolveAddresses();
    return resolved_;
}

bool TargetStatusService::ResolveAddresses() {
    const uintptr_t cs_fe_man_match = FindPatternInText(kCsFeManStoragePattern);
    if (cs_fe_man_match == 0) {
        spdlog::warn("TargetStatusService: CSFeMan storage pattern not found.");
        return false;
    }

    cs_fe_man_storage_ = ResolveRipPointer(cs_fe_man_match, kRipAdjust);
    if (cs_fe_man_storage_ == 0) {
        spdlog::warn("TargetStatusService: failed to resolve CSFeMan storage pointer.");
        return false;
    }

    get_chr_ins_from_handle_ = FindPatternInText(kGetChrInsFromHandlePattern);
    if (get_chr_ins_from_handle_ == 0) {
        spdlog::warn("TargetStatusService: GetChrInsFromHandle pattern not found.");
        return false;
    }

    Main::Logger::Instance().Info(
        "TargetStatus",
        "resolved CSFeMan storage at 0x" + std::format("{:X}", cs_fe_man_storage_) +
            ", GetChrInsFromHandle at 0x" + std::format("{:X}", get_chr_ins_from_handle_));
    return true;
}

uintptr_t TargetStatusService::ResolveTargetChrIns(const Game::SingletonRegistry& singletons) {
    ClearUiSnapshot();

    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    if (world_chr_man == 0) {
        return 0;
    }

    const uintptr_t local_player_chr = ResolveLocalPlayerChr(world_chr_man);
    if (local_player_chr == 0) {
        return 0;
    }

    std::uint64_t target_handle = 0;
    if (!ResolveTargetHandle(local_player_chr, target_handle)) {
        return 0;
    }

    bool used_fallback = false;
    const uintptr_t target_chr = ResolveTargetChrFromHandle(world_chr_man, target_handle, used_fallback);
    if (target_chr != 0) {
        UpdateBossUiPlacement(target_handle);
        spdlog::debug("TargetStatusService: using local player target handle 0x{:X}.", target_handle);
    }
    if (target_chr == 0 || used_fallback) {
        LogDiagnostics(target_handle, target_chr, used_fallback, false, false, false, false);
    }
    return target_chr;
}

uintptr_t TargetStatusService::ResolveLocalPlayerChr(uintptr_t world_chr_man) {
    uintptr_t player_slots_root = 0;
    uintptr_t local_player_chr = 0;
    if (Game::ReadValue(world_chr_man + kNetPlayersOffset, player_slots_root) && player_slots_root != 0) {
        Game::ReadValue(player_slots_root + 0x0, local_player_chr);
    }
    return local_player_chr;
}

bool TargetStatusService::ResolveTargetHandle(uintptr_t local_player_chr, std::uint64_t& target_handle) {
    target_handle = 0;
    return Game::ReadValue(local_player_chr + kChrInsTargetHandleOffset, target_handle) &&
           target_handle != 0 &&
           target_handle != UINT64_MAX;
}

uintptr_t TargetStatusService::ResolveTargetChrFromHandle(
    uintptr_t world_chr_man,
    std::uint64_t& target_handle,
    bool& used_fallback) {
    used_fallback = false;

    uintptr_t target_chr = 0;
    if (get_chr_ins_from_handle_ != 0) {
        const auto get_chr_ins_from_handle = reinterpret_cast<GetChrInsFromHandleFn>(get_chr_ins_from_handle_);
        target_chr = get_chr_ins_from_handle(world_chr_man, &target_handle);
    }
    if (target_chr == 0) {
        target_chr = ResolveChrInsByHandleFallback(world_chr_man, target_handle);
        used_fallback = target_chr != 0;
    }
    return target_chr;
}

void TargetStatusService::UpdateBossUiPlacement(std::uint64_t target_handle) {
    auto& status = Main::g_FeatureStatus;

    uintptr_t fe_man = 0;
    if (!Game::ReadValue(cs_fe_man_storage_, fe_man) || fe_man == 0) {
        return;
    }

    BossHpBar boss_bar{};
    for (int i = 0; i < kBossBarCount; ++i) {
        if (!Game::ReadValue(
                fe_man + 0x59F0 + (sizeof(EntityHpBar) * kEntityBarCount) + (sizeof(BossHpBar) * i),
                boss_bar)) {
            continue;
        }
        if (boss_bar.boss_handle != target_handle || boss_bar.display_id < 0) {
            continue;
        }

        status.targeted_npc_ui_x = kBossScreenX;
        status.targeted_npc_ui_y = kBossScreenY - (kBossNextDiffY * static_cast<float>(i));
        status.targeted_npc_ui_mod = 1.0f;
        status.targeted_npc_ui_slot = i;
        status.targeted_npc_ui_is_boss = true;
        return;
    }
}

void TargetStatusService::PublishSnapshot(uintptr_t target_chr) {
    auto& status = Main::g_FeatureStatus;

    uintptr_t chr_data = 0;
    uintptr_t chr_resist = 0;
    uintptr_t chr_super_armor = 0;
    uintptr_t chr_physics = 0;
    const bool has_chr_data = ReadModulePointer(target_chr, kChrDataModuleOffset, chr_data);
    const bool has_chr_resist = ReadModulePointer(target_chr, kChrResistModuleOffset, chr_resist);
    const bool has_chr_super_armor = ReadModulePointer(target_chr, kChrSuperArmorModuleOffset, chr_super_armor);
    const bool has_chr_physics = ReadModulePointer(target_chr, kChrPhysicsModuleOffset, chr_physics);

    std::uint64_t handle = 0;
    int value = 0;
    float float_value = 0.0f;
    bool any_value_published = false;
    struct Vector3 {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    } coords{};

    Game::ReadValue(target_chr + kChrInsHandleOffset, handle);
    status.targeted_npc_address = target_chr;
    status.targeted_npc_handle = static_cast<uintptr_t>(handle);
    LogDiagnostics(handle, target_chr, false, has_chr_data, has_chr_resist, has_chr_super_armor, has_chr_physics);
    status.targeted_npc_hp = 0;
    status.targeted_npc_max_hp = 0;
    status.targeted_npc_poise = 0;
    status.targeted_npc_max_poise = 0;
    status.targeted_npc_poison = 0;
    status.targeted_npc_poison_max = 0;
    status.targeted_npc_rot = 0;
    status.targeted_npc_rot_max = 0;
    status.targeted_npc_bleed = 0;
    status.targeted_npc_bleed_max = 0;
    status.targeted_npc_blight = 0;
    status.targeted_npc_blight_max = 0;
    status.targeted_npc_frost = 0;
    status.targeted_npc_frost_max = 0;
    status.targeted_npc_sleep = 0;
    status.targeted_npc_sleep_max = 0;
    status.targeted_npc_madness = 0;
    status.targeted_npc_madness_max = 0;
    status.targeted_npc_poison_mod = 0.0f;
    status.targeted_npc_rot_mod = 0.0f;
    status.targeted_npc_bleed_mod = 0.0f;
    status.targeted_npc_blight_mod = 0.0f;
    status.targeted_npc_frost_mod = 0.0f;
    status.targeted_npc_sleep_mod = 0.0f;
    status.targeted_npc_madness_mod = 0.0f;
    status.targeted_npc_x = 0.0f;
    status.targeted_npc_y = 0.0f;
    status.targeted_npc_z = 0.0f;

    if (Game::ReadValue(target_chr + kChrInsNpcParamIdOffset, value)) {
        WriteAtomicInt(status.targeted_npc_id, value);
    }
    if (Game::ReadValue(target_chr + kChrInsChrIdOffset, value)) {
        WriteAtomicInt(status.targeted_npc_character_type, value);
    }
    if (Game::ReadValue(target_chr + kChrInsTeamTypeOffset, value)) {
        WriteAtomicInt(status.targeted_npc_team_type, value);
    }

    if (has_chr_data && Game::ReadValue(chr_data + kCurrentHpOffset, value)) {
        WriteAtomicInt(status.targeted_npc_hp, value);
        any_value_published = true;
    }
    if (has_chr_data && Game::ReadValue(chr_data + kMaxHpOffset, value)) {
        WriteAtomicInt(status.targeted_npc_max_hp, value);
        any_value_published = true;
    }

    if (has_chr_super_armor && Game::ReadValue(chr_super_armor + kCurrentPoiseOffset, float_value)) {
        WriteAtomicInt(status.targeted_npc_poise, static_cast<int>(float_value));
        any_value_published = true;
    }
    if (has_chr_super_armor && Game::ReadValue(chr_super_armor + kMaxPoiseOffset, float_value)) {
        WriteAtomicInt(status.targeted_npc_max_poise, static_cast<int>(float_value));
        any_value_published = true;
    }

    if (has_chr_resist && Game::ReadValue(chr_resist + kCurrentPoisonOffset, value)) {
        WriteAtomicInt(status.targeted_npc_poison, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kMaxPoisonOffset, value)) {
        WriteAtomicInt(status.targeted_npc_poison_max, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kCurrentRotOffset, value)) {
        WriteAtomicInt(status.targeted_npc_rot, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kMaxRotOffset, value)) {
        WriteAtomicInt(status.targeted_npc_rot_max, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kCurrentBleedOffset, value)) {
        WriteAtomicInt(status.targeted_npc_bleed, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kMaxBleedOffset, value)) {
        WriteAtomicInt(status.targeted_npc_bleed_max, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kCurrentBlightOffset, value)) {
        WriteAtomicInt(status.targeted_npc_blight, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kMaxBlightOffset, value)) {
        WriteAtomicInt(status.targeted_npc_blight_max, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kCurrentFrostOffset, value)) {
        WriteAtomicInt(status.targeted_npc_frost, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kMaxFrostOffset, value)) {
        WriteAtomicInt(status.targeted_npc_frost_max, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kCurrentSleepOffset, value)) {
        WriteAtomicInt(status.targeted_npc_sleep, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kMaxSleepOffset, value)) {
        WriteAtomicInt(status.targeted_npc_sleep_max, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kCurrentMadnessOffset, value)) {
        WriteAtomicInt(status.targeted_npc_madness, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kMaxMadnessOffset, value)) {
        WriteAtomicInt(status.targeted_npc_madness_max, value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kPoisonModOffset, float_value)) {
        WriteAtomicFloat(status.targeted_npc_poison_mod, float_value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kRotModOffset, float_value)) {
        WriteAtomicFloat(status.targeted_npc_rot_mod, float_value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kBleedModOffset, float_value)) {
        WriteAtomicFloat(status.targeted_npc_bleed_mod, float_value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kBlightModOffset, float_value)) {
        WriteAtomicFloat(status.targeted_npc_blight_mod, float_value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kFrostModOffset, float_value)) {
        WriteAtomicFloat(status.targeted_npc_frost_mod, float_value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kSleepModOffset, float_value)) {
        WriteAtomicFloat(status.targeted_npc_sleep_mod, float_value);
        any_value_published = true;
    }
    if (has_chr_resist && Game::ReadValue(chr_resist + kMadnessModOffset, float_value)) {
        WriteAtomicFloat(status.targeted_npc_madness_mod, float_value);
        any_value_published = true;
    }

    if (has_chr_physics && Game::ReadValue(chr_physics + kCoordsOffset, coords)) {
        WriteAtomicFloat(status.targeted_npc_x, coords.x);
        WriteAtomicFloat(status.targeted_npc_y, coords.y);
        WriteAtomicFloat(status.targeted_npc_z, coords.z);
        any_value_published = true;
    }

    status.targeted_npc_valid = target_chr != 0;
}

void TargetStatusService::LogDiagnostics(
    std::uint64_t target_handle,
    uintptr_t target_chr,
    bool used_fallback,
    bool has_chr_data,
    bool has_chr_resist,
    bool has_chr_super_armor,
    bool has_chr_physics) {
    if (!Main::g_FeatureStatus.target_status_debug_logging.load()) {
        return;
    }

    if (target_handle == last_logged_target_handle_ &&
        target_chr == last_logged_target_chr_ &&
        used_fallback == last_logged_used_fallback_ &&
        has_chr_data == last_logged_has_chr_data_ &&
        has_chr_resist == last_logged_has_chr_resist_ &&
        has_chr_super_armor == last_logged_has_chr_super_armor_ &&
        has_chr_physics == last_logged_has_chr_physics_) {
        return;
    }

    last_logged_target_handle_ = target_handle;
    last_logged_target_chr_ = target_chr;
    last_logged_used_fallback_ = used_fallback;
    last_logged_has_chr_data_ = has_chr_data;
    last_logged_has_chr_resist_ = has_chr_resist;
    last_logged_has_chr_super_armor_ = has_chr_super_armor;
    last_logged_has_chr_physics_ = has_chr_physics;

    Main::Logger::Instance().Info(
        "TargetStatus",
        "diag: handle=0x" + std::format("{:X}", target_handle) +
            " chr=0x" + std::format("{:X}", target_chr) +
            " fallback=" + std::string(used_fallback ? "true" : "false") +
            " data=" + std::string(has_chr_data ? "true" : "false") +
            " resist=" + std::string(has_chr_resist ? "true" : "false") +
            " poise=" + std::string(has_chr_super_armor ? "true" : "false") +
            " physics=" + std::string(has_chr_physics ? "true" : "false"));
}

void TargetStatusService::ClearUiSnapshot() {
    auto& status = Main::g_FeatureStatus;
    status.targeted_npc_ui_x = 0.0f;
    status.targeted_npc_ui_y = 0.0f;
    status.targeted_npc_ui_mod = 1.0f;
    status.targeted_npc_ui_slot = -1;
    status.targeted_npc_ui_is_boss = false;
}

void TargetStatusService::ClearPublishedSnapshot() {
    auto& status = Main::g_FeatureStatus;
    status.targeted_npc_valid = false;
    status.targeted_npc_address = 0;
    status.targeted_npc_handle = 0;
    status.targeted_npc_id = -1;
    status.targeted_npc_character_type = 0;
    status.targeted_npc_team_type = 0;
    status.targeted_npc_hp = 0;
    status.targeted_npc_max_hp = 0;
    status.targeted_npc_poise = 0;
    status.targeted_npc_max_poise = 0;
    status.targeted_npc_poison = 0;
    status.targeted_npc_poison_max = 0;
    status.targeted_npc_rot = 0;
    status.targeted_npc_rot_max = 0;
    status.targeted_npc_bleed = 0;
    status.targeted_npc_bleed_max = 0;
    status.targeted_npc_blight = 0;
    status.targeted_npc_blight_max = 0;
    status.targeted_npc_frost = 0;
    status.targeted_npc_frost_max = 0;
    status.targeted_npc_sleep = 0;
    status.targeted_npc_sleep_max = 0;
    status.targeted_npc_madness = 0;
    status.targeted_npc_madness_max = 0;
    status.targeted_npc_poison_mod = 0.0f;
    status.targeted_npc_rot_mod = 0.0f;
    status.targeted_npc_bleed_mod = 0.0f;
    status.targeted_npc_blight_mod = 0.0f;
    status.targeted_npc_frost_mod = 0.0f;
    status.targeted_npc_sleep_mod = 0.0f;
    status.targeted_npc_madness_mod = 0.0f;
    status.targeted_npc_x = 0.0f;
    status.targeted_npc_y = 0.0f;
    status.targeted_npc_z = 0.0f;
    ClearUiSnapshot();
}

}  // namespace ERD::Features
