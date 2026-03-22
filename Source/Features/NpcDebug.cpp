#include "NpcDebug.hpp"
#include "PostureBars.hpp"

#include "../Game/Memory.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"

namespace ERD::Features {
namespace {

struct Region {
    uintptr_t base = 0;
    std::size_t size = 0;
};

struct Pattern {
    std::vector<std::uint8_t> bytes;
    std::vector<std::uint8_t> masks;
};

constexpr std::size_t kInlinePatchSize = 5;
constexpr const char* kTargetedNpcPattern = "48 8B 48 08 49 89 8D";
constexpr const char* kLastHitNpcPattern = "48 8B 03 48 8B CB ? ? ? ? FF 50 10 83 F8 01";
constexpr const char* kGetChrInsFromHandlePattern = "48 83 EC 28 E8 17 FF FF FF 48 85 C0 74 08 48 8B 00 48 83 C4 28 C3";
constexpr uintptr_t kWorldChrNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kTargetHandleOffset = 0x6A0;

std::atomic<uintptr_t> g_TargetedNpcBase = 0;
std::atomic<uintptr_t> g_LastHitNpcBase = 0;
std::mutex g_TargetedNpcNameMutex;
std::string g_TargetedNpcNameUtf8;
std::mutex g_LastHitNpcNameMutex;
std::string g_LastHitNpcNameUtf8;
using GetChrInsFromHandleFn = uintptr_t (*)(uintptr_t world_chr_man, std::uint64_t* handle);
GetChrInsFromHandleFn g_GetChrInsFromHandle = nullptr;

void LogProtectedException(const char* scope, const char* detail) {
    Main::Logger::Instance().Error((std::string(scope) + " failed: " + detail).c_str());
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

void SetTargetedNpcName(std::string value);
void SetLastHitNpcName(std::string value);
bool ReadNpcName(uintptr_t base, std::string& name_utf8);
bool EnsureGetChrInsFromHandleResolver();
uintptr_t ResolveNpcBaseFromHandle(const Game::SingletonRegistry& singletons, std::uint64_t handle);
uintptr_t ResolveTargetedNpcBase(const Game::SingletonRegistry& singletons);

bool RegionFromMainModuleText(Region& region) {
    HMODULE module = GetModuleHandleW(nullptr);
    if (module == nullptr) {
        return false;
    }

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        reinterpret_cast<uintptr_t>(module) + dos->e_lfanew
    );
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
    Region text{};
    if (!ParsePattern(aob, pattern) || !RegionFromMainModuleText(text) || text.size < pattern.bytes.size()) {
        return 0;
    }

    const auto* address = reinterpret_cast<const std::uint8_t*>(text.base);
    const auto* end = address + text.size - pattern.bytes.size();
    for (; address <= end; ++address) {
        if (MatchesPattern(address, pattern)) {
            return reinterpret_cast<uintptr_t>(address);
        }
    }

    return 0;
}

bool WriteProtectedMemory(uintptr_t address, const void* data, std::size_t size) {
    DWORD old_protect = 0;
    if (!VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        return false;
    }

    std::memcpy(reinterpret_cast<void*>(address), data, size);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), size);

    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<void*>(address), size, old_protect, &ignored);
    return true;
}

void* AllocateNearbyExecutableMemory(uintptr_t target, std::size_t size) {
    SYSTEM_INFO info{};
    GetSystemInfo(&info);

    const uintptr_t granularity = static_cast<uintptr_t>(info.dwAllocationGranularity);
    const uintptr_t min_address = reinterpret_cast<uintptr_t>(info.lpMinimumApplicationAddress);
    const uintptr_t max_address = reinterpret_cast<uintptr_t>(info.lpMaximumApplicationAddress);
    const uintptr_t max_distance = 0x7FFF0000;
    const uintptr_t aligned_target = target & ~(granularity - 1);

    for (uintptr_t distance = 0; distance <= max_distance; distance += granularity) {
        const uintptr_t high = aligned_target + distance;
        if (high >= min_address && high <= max_address) {
            if (void* memory = VirtualAlloc(
                    reinterpret_cast<void*>(high),
                    size,
                    MEM_RESERVE | MEM_COMMIT,
                    PAGE_EXECUTE_READWRITE)) {
                return memory;
            }
        }

        if (distance == 0 || aligned_target < distance) {
            continue;
        }

        const uintptr_t low = aligned_target - distance;
        if (low >= min_address && low <= max_address) {
            if (void* memory = VirtualAlloc(
                    reinterpret_cast<void*>(low),
                    size,
                    MEM_RESERVE | MEM_COMMIT,
                    PAGE_EXECUTE_READWRITE)) {
                return memory;
            }
        }
    }

    return nullptr;
}

bool MakeRelativeJump(std::array<std::uint8_t, kInlinePatchSize>& patch, uintptr_t source, uintptr_t destination) {
    const std::int64_t delta =
        static_cast<std::int64_t>(destination) - static_cast<std::int64_t>(source + kInlinePatchSize);
    if (delta < INT32_MIN || delta > INT32_MAX) {
        return false;
    }

    patch[0] = 0xE9;
    const auto rel32 = static_cast<std::int32_t>(delta);
    std::memcpy(patch.data() + 1, &rel32, sizeof(rel32));
    return true;
}

bool CapturePatchTarget(NpcDebug::CodePatchState& state, uintptr_t target, std::size_t patch_size) {
    if (patch_size == 0 || patch_size > state.original_bytes.size() || target == 0) {
        return false;
    }

    if (state.captured) {
        return true;
    }

    state.target = target;
    state.patch_size = patch_size;
    if (!Game::ReadMemory(state.target, state.original_bytes.data(), state.patch_size)) {
        state.target = 0;
        state.patch_size = 0;
        return false;
    }

    state.captured = true;
    return true;
}

bool InstallTargetedNpcHook(NpcDebug::CodePatchState& state) {
    if (!state.captured) {
        // CT overwrites the 7-byte pattern plus the 4-byte displacement of the next instruction.
        if (!CapturePatchTarget(state, FindPatternInText(kTargetedNpcPattern), 11)) {
            return false;
        }
    }

    if (state.active) {
        return true;
    }

    state.cave = AllocateNearbyExecutableMemory(state.target, 0x80);
    if (state.cave == nullptr) {
        return false;
    }

    auto* cave = reinterpret_cast<std::uint8_t*>(state.cave);
    std::size_t cursor = 0;

    const std::uint8_t prefix[] = {
        0x41, 0x52,
        0x49, 0xBA,
    };
    std::memcpy(cave + cursor, prefix, sizeof(prefix));
    cursor += sizeof(prefix);

    const uintptr_t capture_address = reinterpret_cast<uintptr_t>(&g_TargetedNpcBase);
    std::memcpy(cave + cursor, &capture_address, sizeof(capture_address));
    cursor += sizeof(capture_address);

    const std::uint8_t store_and_restore[] = {
        0x49, 0x89, 0x02,
        0x41, 0x5A,
    };
    std::memcpy(cave + cursor, store_and_restore, sizeof(store_and_restore));
    cursor += sizeof(store_and_restore);

    std::memcpy(cave + cursor, state.original_bytes.data(), state.patch_size);
    cursor += state.patch_size;

    std::array<std::uint8_t, kInlinePatchSize> cave_jump{};
    if (!MakeRelativeJump(cave_jump, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, cave_jump.data(), cave_jump.size());

    std::array<std::uint8_t, 11> target_patch{};
    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, state.target, reinterpret_cast<uintptr_t>(state.cave))) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(target_patch.data(), target_jump.data(), target_jump.size());
    std::memset(target_patch.data() + kInlinePatchSize, 0x90, target_patch.size() - kInlinePatchSize);

    if (!WriteProtectedMemory(state.target, target_patch.data(), state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    state.active = true;
    return true;
}

bool InstallLastHitNpcHook(NpcDebug::CodePatchState& state) {
    if (!state.captured) {
        const uintptr_t pattern = FindPatternInText(kLastHitNpcPattern);
        if (!CapturePatchTarget(state, pattern == 0 ? 0 : pattern + 6, 7)) {
            return false;
        }
    }

    if (state.active) {
        return true;
    }

    state.cave = AllocateNearbyExecutableMemory(state.target, 0x80);
    if (state.cave == nullptr) {
        return false;
    }

    auto* cave = reinterpret_cast<std::uint8_t*>(state.cave);
    std::size_t cursor = 0;

    const std::uint8_t cmp_and_branch[] = {
        0x83, 0x7B, 0x0C, 0xFF,
        0x74, 0x11,
        0x41, 0x52,
        0x49, 0xBA,
    };
    std::memcpy(cave + cursor, cmp_and_branch, sizeof(cmp_and_branch));
    cursor += sizeof(cmp_and_branch);

    const uintptr_t capture_address = reinterpret_cast<uintptr_t>(&g_LastHitNpcBase);
    std::memcpy(cave + cursor, &capture_address, sizeof(capture_address));
    cursor += sizeof(capture_address);

    const std::uint8_t store_and_restore[] = {
        0x49, 0x89, 0x1A,
        0x41, 0x5A,
    };
    std::memcpy(cave + cursor, store_and_restore, sizeof(store_and_restore));
    cursor += sizeof(store_and_restore);

    std::memcpy(cave + cursor, state.original_bytes.data(), state.patch_size);
    cursor += state.patch_size;

    std::array<std::uint8_t, kInlinePatchSize> cave_jump{};
    if (!MakeRelativeJump(cave_jump, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, cave_jump.data(), cave_jump.size());

    std::array<std::uint8_t, 7> target_patch{};
    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, state.target, reinterpret_cast<uintptr_t>(state.cave))) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(target_patch.data(), target_jump.data(), target_jump.size());
    std::memset(target_patch.data() + target_jump.size(), 0x90, target_patch.size() - target_jump.size());

    if (!WriteProtectedMemory(state.target, target_patch.data(), state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    state.active = true;
    return true;
}

bool ReadNpcSnapshot(uintptr_t base,
                     uintptr_t& handle,
                     int& npc_id,
                     int& character_type,
                     int& team_type,
                     int& hp,
                     int& max_hp,
                     float& x,
                     float& y,
                     float& z) {
    if (base == 0) {
        return false;
    }

    if (!Game::ReadValue(base + 0x8, handle) || handle == 0) {
        return false;
    }

    std::uint8_t team_type_byte = 0;
    if (!Game::ReadValue(base + 0x68, character_type) ||
        !Game::ReadValue(base + 0x6C, team_type_byte)) {
        return false;
    }
    team_type = static_cast<int>(team_type_byte);

    uintptr_t npc_id_address = 0;
    uintptr_t hp_address = 0;
    uintptr_t max_hp_address = 0;
    uintptr_t position_address = 0;
    if (!Game::ResolvePointerChain(base, {0x28}, 0x124, npc_id_address) ||
        !Game::ResolvePointerChain(base, {0x190, 0x0}, 0x138, hp_address) ||
        !Game::ResolvePointerChain(base, {0x190, 0x0}, 0x13C, max_hp_address) ||
        !Game::ResolvePointerChain(base, {0x190, 0x68}, 0x70, position_address)) {
        return false;
    }

    if (!Game::ReadValue(npc_id_address, npc_id) ||
        !Game::ReadValue(hp_address, hp) ||
        !Game::ReadValue(max_hp_address, max_hp) ||
        !Game::ReadValue(position_address + 0x0, x) ||
        !Game::ReadValue(position_address + 0x4, z) ||
        !Game::ReadValue(position_address + 0x8, y)) {
        return false;
    }

    return true;
}

struct NpcResistanceSnapshot {
    int poison = 0;
    int poison_max = 0;
    int rot = 0;
    int rot_max = 0;
    int bleed = 0;
    int bleed_max = 0;
    int blight = 0;
    int blight_max = 0;
    int frost = 0;
    int frost_max = 0;
    int sleep = 0;
    int sleep_max = 0;
    int madness = 0;
    int madness_max = 0;
};

bool ReadNpcResistances(uintptr_t base, NpcResistanceSnapshot& snapshot) {
    uintptr_t resistance_base = 0;
    if (!Game::ResolvePointerChain(base, {0x190, 0x20}, 0x0, resistance_base) || resistance_base == 0) {
        return false;
    }

    return Game::ReadValue(resistance_base + 0x10, snapshot.poison) &&
           Game::ReadValue(resistance_base + 0x2C, snapshot.poison_max) &&
           Game::ReadValue(resistance_base + 0x14, snapshot.rot) &&
           Game::ReadValue(resistance_base + 0x30, snapshot.rot_max) &&
           Game::ReadValue(resistance_base + 0x18, snapshot.bleed) &&
           Game::ReadValue(resistance_base + 0x34, snapshot.bleed_max) &&
           Game::ReadValue(resistance_base + 0x1C, snapshot.blight) &&
           Game::ReadValue(resistance_base + 0x38, snapshot.blight_max) &&
           Game::ReadValue(resistance_base + 0x20, snapshot.frost) &&
           Game::ReadValue(resistance_base + 0x3C, snapshot.frost_max) &&
           Game::ReadValue(resistance_base + 0x24, snapshot.sleep) &&
           Game::ReadValue(resistance_base + 0x40, snapshot.sleep_max) &&
           Game::ReadValue(resistance_base + 0x28, snapshot.madness) &&
           Game::ReadValue(resistance_base + 0x44, snapshot.madness_max);
}

void ApplyTargetedResistances(const NpcResistanceSnapshot& snapshot) {
    Main::g_FeatureStatus.targeted_npc_poison = snapshot.poison;
    Main::g_FeatureStatus.targeted_npc_poison_max = snapshot.poison_max;
    Main::g_FeatureStatus.targeted_npc_rot = snapshot.rot;
    Main::g_FeatureStatus.targeted_npc_rot_max = snapshot.rot_max;
    Main::g_FeatureStatus.targeted_npc_bleed = snapshot.bleed;
    Main::g_FeatureStatus.targeted_npc_bleed_max = snapshot.bleed_max;
    Main::g_FeatureStatus.targeted_npc_blight = snapshot.blight;
    Main::g_FeatureStatus.targeted_npc_blight_max = snapshot.blight_max;
    Main::g_FeatureStatus.targeted_npc_frost = snapshot.frost;
    Main::g_FeatureStatus.targeted_npc_frost_max = snapshot.frost_max;
    Main::g_FeatureStatus.targeted_npc_sleep = snapshot.sleep;
    Main::g_FeatureStatus.targeted_npc_sleep_max = snapshot.sleep_max;
    Main::g_FeatureStatus.targeted_npc_madness = snapshot.madness;
    Main::g_FeatureStatus.targeted_npc_madness_max = snapshot.madness_max;
}

void ApplyLastHitResistances(const NpcResistanceSnapshot& snapshot) {
    Main::g_FeatureStatus.last_hit_npc_poison = snapshot.poison;
    Main::g_FeatureStatus.last_hit_npc_poison_max = snapshot.poison_max;
    Main::g_FeatureStatus.last_hit_npc_rot = snapshot.rot;
    Main::g_FeatureStatus.last_hit_npc_rot_max = snapshot.rot_max;
    Main::g_FeatureStatus.last_hit_npc_bleed = snapshot.bleed;
    Main::g_FeatureStatus.last_hit_npc_bleed_max = snapshot.bleed_max;
    Main::g_FeatureStatus.last_hit_npc_blight = snapshot.blight;
    Main::g_FeatureStatus.last_hit_npc_blight_max = snapshot.blight_max;
    Main::g_FeatureStatus.last_hit_npc_frost = snapshot.frost;
    Main::g_FeatureStatus.last_hit_npc_frost_max = snapshot.frost_max;
    Main::g_FeatureStatus.last_hit_npc_sleep = snapshot.sleep;
    Main::g_FeatureStatus.last_hit_npc_sleep_max = snapshot.sleep_max;
    Main::g_FeatureStatus.last_hit_npc_madness = snapshot.madness;
    Main::g_FeatureStatus.last_hit_npc_madness_max = snapshot.madness_max;
}

void ClearTargetedResistances() {
    Main::g_FeatureStatus.targeted_npc_poison = 0;
    Main::g_FeatureStatus.targeted_npc_poison_max = 0;
    Main::g_FeatureStatus.targeted_npc_rot = 0;
    Main::g_FeatureStatus.targeted_npc_rot_max = 0;
    Main::g_FeatureStatus.targeted_npc_bleed = 0;
    Main::g_FeatureStatus.targeted_npc_bleed_max = 0;
    Main::g_FeatureStatus.targeted_npc_blight = 0;
    Main::g_FeatureStatus.targeted_npc_blight_max = 0;
    Main::g_FeatureStatus.targeted_npc_frost = 0;
    Main::g_FeatureStatus.targeted_npc_frost_max = 0;
    Main::g_FeatureStatus.targeted_npc_sleep = 0;
    Main::g_FeatureStatus.targeted_npc_sleep_max = 0;
    Main::g_FeatureStatus.targeted_npc_madness = 0;
    Main::g_FeatureStatus.targeted_npc_madness_max = 0;
}

void ClearLastHitResistances() {
    Main::g_FeatureStatus.last_hit_npc_poison = 0;
    Main::g_FeatureStatus.last_hit_npc_poison_max = 0;
    Main::g_FeatureStatus.last_hit_npc_rot = 0;
    Main::g_FeatureStatus.last_hit_npc_rot_max = 0;
    Main::g_FeatureStatus.last_hit_npc_bleed = 0;
    Main::g_FeatureStatus.last_hit_npc_bleed_max = 0;
    Main::g_FeatureStatus.last_hit_npc_blight = 0;
    Main::g_FeatureStatus.last_hit_npc_blight_max = 0;
    Main::g_FeatureStatus.last_hit_npc_frost = 0;
    Main::g_FeatureStatus.last_hit_npc_frost_max = 0;
    Main::g_FeatureStatus.last_hit_npc_sleep = 0;
    Main::g_FeatureStatus.last_hit_npc_sleep_max = 0;
    Main::g_FeatureStatus.last_hit_npc_madness = 0;
    Main::g_FeatureStatus.last_hit_npc_madness_max = 0;
}

void ClearTargetedStatus() {
    Main::g_FeatureStatus.targeted_npc_valid = false;
    Main::g_FeatureStatus.targeted_npc_address = 0;
    Main::g_FeatureStatus.targeted_npc_handle = 0;
    Main::g_FeatureStatus.targeted_npc_id = -1;
    Main::g_FeatureStatus.targeted_npc_character_type = 0;
    Main::g_FeatureStatus.targeted_npc_team_type = 0;
    Main::g_FeatureStatus.targeted_npc_hp = 0;
    Main::g_FeatureStatus.targeted_npc_max_hp = 0;
    Main::g_FeatureStatus.targeted_npc_x = 0.0f;
    Main::g_FeatureStatus.targeted_npc_y = 0.0f;
    Main::g_FeatureStatus.targeted_npc_z = 0.0f;
    ClearTargetedResistances();
    SetTargetedNpcName({});
}

void ClearLastHitStatus() {
    Main::g_FeatureStatus.last_hit_npc_valid = false;
    Main::g_FeatureStatus.last_hit_npc_address = 0;
    Main::g_FeatureStatus.last_hit_npc_handle = 0;
    Main::g_FeatureStatus.last_hit_npc_id = -1;
    Main::g_FeatureStatus.last_hit_npc_character_type = 0;
    Main::g_FeatureStatus.last_hit_npc_team_type = 0;
    Main::g_FeatureStatus.last_hit_npc_hp = 0;
    Main::g_FeatureStatus.last_hit_npc_max_hp = 0;
    Main::g_FeatureStatus.last_hit_npc_x = 0.0f;
    Main::g_FeatureStatus.last_hit_npc_y = 0.0f;
    Main::g_FeatureStatus.last_hit_npc_z = 0.0f;
    ClearLastHitResistances();
    SetLastHitNpcName({});
}

bool GetPlayerPositionAddress(const Game::SingletonRegistry& singletons, uintptr_t& address) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    return world_chr_man != 0 &&
           Game::ResolvePointerChain(world_chr_man, {kWorldChrNetPlayersOffset, 0x0, 0x190, 0x68}, 0x70, address);
}

bool GetNpcPositionAddress(uintptr_t base, uintptr_t& address) {
    return Game::ResolvePointerChain(base, {0x190, 0x68}, 0x70, address);
}

bool GetNpcSecondaryPositionAddress(uintptr_t base, uintptr_t& address) {
    return Game::ResolvePointerChain(base, {0x190, 0x68, 0xA8, 0x18}, 0x80, address);
}

std::string WideToUtf8(const std::wstring& value) {
    if (value.empty()) {
        return {};
    }

    const int required = WideCharToMultiByte(
        CP_UTF8,
        0,
        value.c_str(),
        static_cast<int>(value.size()),
        nullptr,
        0,
        nullptr,
        nullptr
    );
    if (required <= 0) {
        return {};
    }

    std::string utf8(static_cast<std::size_t>(required), '\0');
    WideCharToMultiByte(
        CP_UTF8,
        0,
        value.c_str(),
        static_cast<int>(value.size()),
        utf8.data(),
        required,
        nullptr,
        nullptr
    );
    return utf8;
}

void SetTargetedNpcName(std::string value) {
    std::scoped_lock lock(g_TargetedNpcNameMutex);
    g_TargetedNpcNameUtf8 = std::move(value);
}

void SetLastHitNpcName(std::string value) {
    std::scoped_lock lock(g_LastHitNpcNameMutex);
    g_LastHitNpcNameUtf8 = std::move(value);
}

bool ReadNpcName(uintptr_t base, std::string& name_utf8) {
    uintptr_t name_address = 0;
    if (!Game::ResolvePointerChain(base, {0x580}, 0x9C, name_address)) {
        return false;
    }

    wchar_t buffer[33]{};
    if (!Game::ReadMemory(name_address, buffer, sizeof(buffer) - sizeof(wchar_t))) {
        return false;
    }
    buffer[32] = L'\0';

    std::wstring name(buffer);
    const std::size_t zero_position = name.find(L'\0');
    if (zero_position != std::wstring::npos) {
        name.resize(zero_position);
    }

    name_utf8 = WideToUtf8(name);
    return true;
}

bool EnsureGetChrInsFromHandleResolver() {
    if (g_GetChrInsFromHandle == nullptr) {
        const uintptr_t target = FindPatternInText(kGetChrInsFromHandlePattern);
        g_GetChrInsFromHandle = reinterpret_cast<GetChrInsFromHandleFn>(target);
    }
    return g_GetChrInsFromHandle != nullptr;
}

uintptr_t ResolveNpcBaseFromHandle(const Game::SingletonRegistry& singletons, std::uint64_t handle) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    if (world_chr_man == 0 ||
        !EnsureGetChrInsFromHandleResolver() ||
        handle == 0 ||
        handle == std::numeric_limits<std::uint64_t>::max()) {
        return 0;
    }

    return g_GetChrInsFromHandle(world_chr_man, &handle);
}

uintptr_t ResolveTargetedNpcBase(const Game::SingletonRegistry& singletons) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    if (world_chr_man == 0 || !EnsureGetChrInsFromHandleResolver()) {
        return 0;
    }

    uintptr_t player_base = 0;
    if (!Game::ResolvePointerChain(world_chr_man, {kWorldChrNetPlayersOffset, 0x0}, 0x0, player_base) ||
        player_base == 0) {
        return 0;
    }

    std::uint64_t target_handle = 0;
    if (!Game::ReadValue(player_base + kTargetHandleOffset, target_handle) ||
        target_handle == 0 ||
        target_handle == std::numeric_limits<std::uint64_t>::max()) {
        return 0;
    }

    return g_GetChrInsFromHandle(world_chr_man, &target_handle);
}

bool TeleportSelfToNpc(const Game::SingletonRegistry& singletons, uintptr_t npc_base) {
    uintptr_t player_position = 0;
    uintptr_t npc_position = 0;
    std::array<std::uint8_t, 12> coordinates{};
    return GetPlayerPositionAddress(singletons, player_position) &&
           GetNpcPositionAddress(npc_base, npc_position) &&
           Game::ReadMemory(npc_position, coordinates.data(), coordinates.size()) &&
           Game::WriteMemory(player_position, coordinates.data(), coordinates.size());
}

bool TeleportNpcToSelf(const Game::SingletonRegistry& singletons, uintptr_t npc_base) {
    uintptr_t player_position = 0;
    uintptr_t npc_position = 0;
    uintptr_t npc_secondary_position = 0;
    std::array<std::uint8_t, 12> coordinates{};
    if (!GetPlayerPositionAddress(singletons, player_position) ||
        !GetNpcPositionAddress(npc_base, npc_position) ||
        !Game::ReadMemory(player_position, coordinates.data(), coordinates.size()) ||
        !Game::WriteMemory(npc_position, coordinates.data(), coordinates.size())) {
        return false;
    }

    if (GetNpcSecondaryPositionAddress(npc_base, npc_secondary_position)) {
        Game::WriteMemory(npc_secondary_position, coordinates.data(), coordinates.size());
    }
    return true;
}

}  // namespace

void NpcDebug::Tick(const Game::SingletonRegistry& singletons) {
    ERD_PROTECTED_STEP("NpcDebug.EnsureHooksInstalled", EnsureHooksInstalled());
    ERD_PROTECTED_STEP("NpcDebug.SyncTrackedNpcStatus", SyncTrackedNpcStatus(singletons));
    ERD_PROTECTED_STEP("NpcDebug.SyncTeleportActions", SyncTeleportActions(singletons));
}

void NpcDebug::EnsureHooksInstalled() {
    if (!last_hit_npc_hook_.active && !InstallLastHitNpcHook(last_hit_npc_hook_) && !last_hit_hook_error_logged_) {
        Main::Logger::Instance().Error("Failed to install Last Hit Npc Info hook.");
        last_hit_hook_error_logged_ = true;
    }
}

void NpcDebug::SyncTrackedNpcStatus(const Game::SingletonRegistry& singletons) const {
    {
        uintptr_t base = 0;
        std::uint64_t handle_from_posture_bars = 0;
        if (!ResolveCurrentTargetFromPostureBars(base, handle_from_posture_bars)) {
            base = ResolveTargetedNpcBase(singletons);
        }
        uintptr_t handle = 0;
        int npc_id = -1;
        int character_type = 0;
        int team_type = 0;
        int hp = 0;
        int max_hp = 0;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        std::string name_utf8;
        NpcResistanceSnapshot resistance_snapshot{};
        if (ReadNpcSnapshot(base, handle, npc_id, character_type, team_type, hp, max_hp, x, y, z)) {
            const uintptr_t resolved_base = ResolveNpcBaseFromHandle(singletons, static_cast<std::uint64_t>(handle));
            const uintptr_t stable_base = resolved_base != 0 ? resolved_base : base;
            if (stable_base != base) {
                ReadNpcSnapshot(stable_base, handle, npc_id, character_type, team_type, hp, max_hp, x, y, z);
            }
            Main::g_FeatureStatus.targeted_npc_valid = true;
            Main::g_FeatureStatus.targeted_npc_address = stable_base;
            Main::g_FeatureStatus.targeted_npc_handle = handle;
            Main::g_FeatureStatus.targeted_npc_id = npc_id;
            Main::g_FeatureStatus.targeted_npc_character_type = character_type;
            Main::g_FeatureStatus.targeted_npc_team_type = team_type;
            Main::g_FeatureStatus.targeted_npc_hp = hp;
            Main::g_FeatureStatus.targeted_npc_max_hp = max_hp;
            Main::g_FeatureStatus.targeted_npc_x = x;
            Main::g_FeatureStatus.targeted_npc_y = y;
            Main::g_FeatureStatus.targeted_npc_z = z;
            if (ReadNpcResistances(stable_base, resistance_snapshot)) {
                ApplyTargetedResistances(resistance_snapshot);
            } else {
                ClearTargetedResistances();
            }
            if (ReadNpcName(stable_base, name_utf8)) {
                SetTargetedNpcName(std::move(name_utf8));
            } else {
                SetTargetedNpcName({});
            }
        } else {
            ClearTargetedStatus();
        }
    }

    {
        const uintptr_t captured_base = g_LastHitNpcBase.load();
        uintptr_t base = captured_base;
        uintptr_t handle = 0;
        int npc_id = -1;
        int character_type = 0;
        int team_type = 0;
        int hp = 0;
        int max_hp = 0;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        std::string name_utf8;
        NpcResistanceSnapshot resistance_snapshot{};
        if (ReadNpcSnapshot(base, handle, npc_id, character_type, team_type, hp, max_hp, x, y, z)) {
            const uintptr_t resolved_base = ResolveNpcBaseFromHandle(singletons, static_cast<std::uint64_t>(handle));
            const uintptr_t stable_base = resolved_base != 0 ? resolved_base : captured_base;
            if (stable_base != 0 && stable_base != base) {
                base = stable_base;
                ReadNpcSnapshot(base, handle, npc_id, character_type, team_type, hp, max_hp, x, y, z);
            }
            Main::g_FeatureStatus.last_hit_npc_valid = true;
            Main::g_FeatureStatus.last_hit_npc_address = base;
            Main::g_FeatureStatus.last_hit_npc_handle = handle;
            Main::g_FeatureStatus.last_hit_npc_id = npc_id;
            Main::g_FeatureStatus.last_hit_npc_character_type = character_type;
            Main::g_FeatureStatus.last_hit_npc_team_type = team_type;
            Main::g_FeatureStatus.last_hit_npc_hp = hp;
            Main::g_FeatureStatus.last_hit_npc_max_hp = max_hp;
            Main::g_FeatureStatus.last_hit_npc_x = x;
            Main::g_FeatureStatus.last_hit_npc_y = y;
            Main::g_FeatureStatus.last_hit_npc_z = z;
            if (ReadNpcResistances(base, resistance_snapshot)) {
                ApplyLastHitResistances(resistance_snapshot);
            } else {
                ClearLastHitResistances();
            }
            if (ReadNpcName(base, name_utf8)) {
                SetLastHitNpcName(std::move(name_utf8));
            } else {
                SetLastHitNpcName({});
            }
        } else {
            ClearLastHitStatus();
        }
    }
}

void NpcDebug::SyncTeleportActions(const Game::SingletonRegistry& singletons) const {
    if (Main::g_FeatureStatus.teleport_self_to_targeted_npc_requested.exchange(false)) {
        const uintptr_t base = Main::g_FeatureStatus.targeted_npc_address.load();
        if (TeleportSelfToNpc(singletons, base)) {
            Main::Logger::Instance().Info("Teleported player to targeted NPC.");
        } else {
            Main::Logger::Instance().Error("Failed to teleport player to targeted NPC.");
        }
    }

    if (Main::g_FeatureStatus.teleport_targeted_npc_to_self_requested.exchange(false)) {
        const uintptr_t base = Main::g_FeatureStatus.targeted_npc_address.load();
        if (TeleportNpcToSelf(singletons, base)) {
            Main::Logger::Instance().Info("Teleported targeted NPC to player.");
        } else {
            Main::Logger::Instance().Error("Failed to teleport targeted NPC to player.");
        }
    }

    if (Main::g_FeatureStatus.teleport_self_to_last_hit_npc_requested.exchange(false)) {
        const uintptr_t base = Main::g_FeatureStatus.last_hit_npc_address.load();
        if (TeleportSelfToNpc(singletons, base)) {
            Main::Logger::Instance().Info("Teleported player to last-hit NPC.");
        } else {
            Main::Logger::Instance().Error("Failed to teleport player to last-hit NPC.");
        }
    }

    if (Main::g_FeatureStatus.teleport_last_hit_npc_to_self_requested.exchange(false)) {
        const uintptr_t base = Main::g_FeatureStatus.last_hit_npc_address.load();
        if (TeleportNpcToSelf(singletons, base)) {
            Main::Logger::Instance().Info("Teleported last-hit NPC to player.");
        } else {
            Main::Logger::Instance().Error("Failed to teleport last-hit NPC to player.");
        }
    }
}

std::string GetTargetedNpcNameUtf8() {
    std::scoped_lock lock(g_TargetedNpcNameMutex);
    return g_TargetedNpcNameUtf8;
}

std::string GetLastHitNpcNameUtf8() {
    std::scoped_lock lock(g_LastHitNpcNameMutex);
    return g_LastHitNpcNameUtf8;
}

}  // namespace ERD::Features
