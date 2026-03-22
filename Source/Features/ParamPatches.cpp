#include "ParamPatches.hpp"

#include "../Game/Memory.hpp"
#include "../Game/Params.hpp"
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
constexpr std::size_t kShopLineupValueOffset = 0x4;
constexpr std::size_t kShopLineupValueAddOffset = 0x1C;
constexpr std::size_t kShopLineupValueMagnificationOffset = 0x20;
constexpr std::size_t kWeaponSellValueOffset = 0x20;
constexpr std::size_t kProtectorSellValueOffset = 0x20;
constexpr std::size_t kAccessorySellValueOffset = 0x18;
constexpr std::size_t kGoodsSellValueOffset = 0x14;
constexpr std::size_t kGemSellValueOffset = 0x24;
constexpr std::size_t kProtectorHeadFlagOffset = 0xDC;
constexpr std::size_t kProtectorHeadAndInvisibleFlagsSize = 7;
constexpr std::uint8_t kProtectorHeadEquipBit = 1u << 1;
constexpr std::size_t kMagicArcaneRequirementOffset = 0x0E;
constexpr std::size_t kMagicSlotLengthOffset = 0x21;
constexpr std::size_t kMagicIntelligenceRequirementOffset = 0x22;
constexpr std::size_t kMagicFaithRequirementOffset = 0x23;
constexpr std::uint8_t kMagicSingleSlotValue = 1;
constexpr std::size_t kWeaponWeightOffset = 0x10;
constexpr std::size_t kProtectorWeightOffset = 0x24;
constexpr std::size_t kAccessoryWeightOffset = 0x0C;
constexpr const char* kCraftingMaterialCostPatternPrimary = "0F 4F F8 48 83 C3";
constexpr const char* kCraftingMaterialCostPatternSecondary = "0F B7 50 ? 66 3B CA 77";
constexpr std::uint8_t kCraftingMaterialCostPrimaryPatch[] = {0x90, 0x90, 0x90};
constexpr const char* kMountAnywherePatternPrimary =
    "80xxxxxxxxxxxx48xxxxxxxx48xxxxxxxxxxxxxxxx40xxxxxxxxxx44xxxxxxxxxxxxxx48xxxxxxxxxxxxxxxxE8xxxxxxxx48";
constexpr const char* kMountAnywherePatternSecondary = "48 8B 40 68 80 78 36 00";
constexpr const char* kSpiritAshesAnywherePatternPrimary =
    "48 8B 47 ? F3 0F 10 90 ? ? ? ? 0F 2F D0";
constexpr const char* kSpiritAshesAnywherePatternSecondary =
    "48 8B 45 98 48 85 C0 0F 84 ? ? ? ? 8B 40 20";
constexpr std::uint8_t kMountAnywherePrimaryPatch[] = {0xC6, 0x41, 0x36, 0x00, 0xB0, 0x00, 0x90};
constexpr float kSpiritAshesAnywhereRadius = 1000.0f;

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

bool ParseCompactPattern(const char* aob, Pattern& pattern) {
    pattern.bytes.clear();
    pattern.masks.clear();

    std::string compact;
    for (const char* cursor = aob; *cursor != '\0'; ++cursor) {
        if (*cursor != ' ') {
            compact.push_back(*cursor);
        }
    }

    if (compact.empty() || (compact.size() % 2) != 0) {
        return false;
    }

    for (std::size_t i = 0; i < compact.size(); i += 2) {
        const char hi_ch = compact[i];
        const char lo_ch = compact[i + 1];

        std::uint8_t byte = 0;
        std::uint8_t mask = 0;

        const int hi = HexValue(hi_ch);
        const int lo = HexValue(lo_ch);
        if (hi >= 0) {
            byte |= static_cast<std::uint8_t>(hi << 4);
            mask |= 0xF0;
        } else if (hi_ch != '?' && hi_ch != 'x' && hi_ch != 'X') {
            return false;
        }

        if (lo >= 0) {
            byte |= static_cast<std::uint8_t>(lo);
            mask |= 0x0F;
        } else if (lo_ch != '?' && lo_ch != 'x' && lo_ch != 'X') {
            return false;
        }

        pattern.bytes.push_back(byte);
        pattern.masks.push_back(mask);
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

uintptr_t FindPatternInTextCompact(const char* aob) {
    Pattern pattern{};
    Region text{};
    if (!ParseCompactPattern(aob, pattern) || !RegionFromMainModuleText(text) || text.size < pattern.bytes.size()) {
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

bool CapturePatchTarget(ParamPatches::CodePatchState& state, uintptr_t target, std::size_t patch_size) {
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

bool InstallDirectPatch(ParamPatches::CodePatchState& state, uintptr_t target, const void* patch, std::size_t patch_size) {
    if (!CapturePatchTarget(state, target, patch_size)) {
        return false;
    }

    if (state.active) {
        return true;
    }

    if (!WriteProtectedMemory(state.target, patch, state.patch_size)) {
        return false;
    }

    state.active = true;
    return true;
}

template <typename T>
T ReadOffsetValue(const std::uint8_t* row, std::size_t offset) {
    T value{};
    std::memcpy(&value, row + offset, sizeof(T));
    return value;
}

template <typename T>
void WriteOffsetValue(std::uint8_t* row, std::size_t offset, const T& value) {
    std::memcpy(row + offset, &value, sizeof(T));
}

bool InstallMountAnywhereSecondaryPatch(ParamPatches::CodePatchState& state) {
    if (!state.captured) {
        if (!CapturePatchTarget(state, FindPatternInText(kMountAnywherePatternSecondary), 8)) {
            return false;
        }
    }

    if (state.active) {
        return true;
    }

    state.cave = AllocateNearbyExecutableMemory(state.target, 0x40);
    if (state.cave == nullptr) {
        return false;
    }

    auto* cave = reinterpret_cast<std::uint8_t*>(state.cave);
    std::size_t cursor = 0;
    const std::uint8_t code[] = {
        0x48, 0x8B, 0x40, 0x68,
        0xC6, 0x40, 0x36, 0x00,
        0x80, 0x78, 0x36, 0x00,
    };
    std::memcpy(cave + cursor, code, sizeof(code));
    cursor += sizeof(code);

    std::array<std::uint8_t, kInlinePatchSize> cave_jump{};
    if (!MakeRelativeJump(cave_jump, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, cave_jump.data(), cave_jump.size());

    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, state.target, reinterpret_cast<uintptr_t>(state.cave))) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    std::array<std::uint8_t, 8> target_patch{};
    std::memcpy(target_patch.data(), target_jump.data(), target_jump.size());
    target_patch[5] = 0x90;
    target_patch[6] = 0x90;
    target_patch[7] = 0x90;

    if (!WriteProtectedMemory(state.target, target_patch.data(), state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    state.active = true;
    return true;
}

bool InstallSpiritAshesPrimaryPatch(ParamPatches::CodePatchState& state) {
    if (!state.captured) {
        if (!CapturePatchTarget(state, FindPatternInText(kSpiritAshesAnywherePatternPrimary), 12)) {
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
        0x48, 0x8B, 0x47, 0x28,
        0x81, 0x38, 0xD0, 0x07, 0x00, 0x00,
        0x75, 0x0E,
        0xB8, 0x00, 0x00, 0x7A, 0x44,
        0x66, 0x0F, 0x6E, 0xD0,
    };
    std::memcpy(cave + cursor, prefix, sizeof(prefix));
    cursor += sizeof(prefix);

    std::array<std::uint8_t, kInlinePatchSize> jump_after_constant{};
    if (!MakeRelativeJump(jump_after_constant, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, jump_after_constant.data(), jump_after_constant.size());
    cursor += jump_after_constant.size();

    const std::uint8_t code_path[] = {0xF3, 0x0F, 0x10, 0x90, 0x84, 0x00, 0x00, 0x00};
    std::memcpy(cave + cursor, code_path, sizeof(code_path));
    cursor += sizeof(code_path);

    std::array<std::uint8_t, kInlinePatchSize> jump_after_code{};
    if (!MakeRelativeJump(jump_after_code, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, jump_after_code.data(), jump_after_code.size());

    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, state.target, reinterpret_cast<uintptr_t>(state.cave))) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    std::array<std::uint8_t, 12> target_patch{};
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

bool InstallSpiritAshesSecondaryPatch(ParamPatches::CodePatchState& state) {
    if (!state.captured) {
        if (!CapturePatchTarget(state, FindPatternInText(kSpiritAshesAnywherePatternSecondary), 7)) {
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
    const std::uint8_t code[] = {
        0x48, 0x8B, 0x45, 0x98,
        0x48, 0x85, 0xC0,
        0x74, 0x0D,
        0xC7, 0x40, 0x20, 0x00, 0x00, 0x00, 0x00,
        0x66, 0xC7, 0x40, 0x1C, 0xFF, 0xFF,
        0x48, 0x85, 0xC0,
    };
    std::memcpy(cave + cursor, code, sizeof(code));
    cursor += sizeof(code);

    std::array<std::uint8_t, kInlinePatchSize> cave_jump{};
    if (!MakeRelativeJump(cave_jump, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, cave_jump.data(), cave_jump.size());

    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, state.target, reinterpret_cast<uintptr_t>(state.cave))) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    std::array<std::uint8_t, 7> target_patch{};
    std::memcpy(target_patch.data(), target_jump.data(), target_jump.size());
    target_patch[5] = 0x90;
    target_patch[6] = 0x90;

    if (!WriteProtectedMemory(state.target, target_patch.data(), state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    state.active = true;
    return true;
}

bool InstallCraftingMaterialCostSecondaryPatch(ParamPatches::CodePatchState& state) {
    if (!state.captured) {
        if (!CapturePatchTarget(state, FindPatternInText(kCraftingMaterialCostPatternSecondary), 7)) {
            return false;
        }
    }

    if (state.active) {
        return true;
    }

    state.cave = AllocateNearbyExecutableMemory(state.target, 0x40);
    if (state.cave == nullptr) {
        return false;
    }

    auto* cave = reinterpret_cast<std::uint8_t*>(state.cave);
    std::size_t cursor = 0;
    const std::uint8_t material_count_offset = state.original_bytes[3];
    const std::uint8_t code[] = {
        0x66, 0xC7, 0x40, material_count_offset, 0x00, 0x00,
        0x0F, 0xB7, 0x50, material_count_offset,
        0x66, 0x3B, 0xCA,
    };
    std::memcpy(cave + cursor, code, sizeof(code));
    cursor += sizeof(code);

    std::array<std::uint8_t, kInlinePatchSize> cave_jump{};
    if (!MakeRelativeJump(cave_jump, reinterpret_cast<uintptr_t>(cave + cursor), state.target + state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }
    std::memcpy(cave + cursor, cave_jump.data(), cave_jump.size());

    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, state.target, reinterpret_cast<uintptr_t>(state.cave))) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    std::array<std::uint8_t, 7> target_patch{};
    std::memcpy(target_patch.data(), target_jump.data(), target_jump.size());
    target_patch[5] = 0x90;
    target_patch[6] = 0x90;

    if (!WriteProtectedMemory(state.target, target_patch.data(), state.patch_size)) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
        return false;
    }

    state.active = true;
    return true;
}

bool RestoreCodePatch(ParamPatches::CodePatchState& state) {
    if (!state.active) {
        return true;
    }

    if (!WriteProtectedMemory(state.target, state.original_bytes.data(), state.patch_size)) {
        return false;
    }

    if (state.cave != nullptr) {
        VirtualFree(state.cave, 0, MEM_RELEASE);
        state.cave = nullptr;
    }

    state.active = false;
    return true;
}

}  // namespace

void ParamPatches::Tick(const Game::SingletonRegistry& singletons) {
    ERD_PROTECTED_STEP("ParamPatches.FasterRespawn", SyncFasterRespawn(singletons));
    ERD_PROTECTED_STEP("ParamPatches.MiniDungeonWarp", SyncMiniDungeonWarp(singletons));
    ERD_PROTECTED_STEP("ParamPatches.FreePurchase", SyncFreePurchase(singletons));
    ERD_PROTECTED_STEP("ParamPatches.NoCraftingMaterialCost", SyncNoCraftingMaterialCost(singletons));
    ERD_PROTECTED_STEP("ParamPatches.NoUpgradeMaterialCost", SyncNoUpgradeMaterialCost(singletons));
    ERD_PROTECTED_STEP("ParamPatches.NoMagicRequirements", SyncNoMagicRequirements(singletons));
    ERD_PROTECTED_STEP("ParamPatches.AllMagicOneSlot", SyncAllMagicOneSlot(singletons));
    ERD_PROTECTED_STEP("ParamPatches.WeightlessEquipment", SyncWeightlessEquipment(singletons));
    ERD_PROTECTED_STEP("ParamPatches.CustomFov", SyncCustomFov(singletons));
    ERD_PROTECTED_STEP("ParamPatches.CustomCameraDistance", SyncCustomCameraDistance(singletons));
    ERD_PROTECTED_STEP("ParamPatches.MountAnywhere", SyncMountAnywhere(singletons));
    ERD_PROTECTED_STEP("ParamPatches.SpiritAshesAnywhere", SyncSpiritAshesAnywhere(singletons));
    ERD_PROTECTED_STEP("ParamPatches.ItemDiscovery", SyncItemDiscovery(singletons));
    ERD_PROTECTED_STEP("ParamPatches.PermanentLantern", SyncPermanentLantern(singletons));
    ERD_PROTECTED_STEP("ParamPatches.InvisibleHelmets", SyncInvisibleHelmets(singletons));
}

void ParamPatches::SyncFasterRespawn(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.faster_respawn.load();
    if (!faster_respawn_.captured) {
        faster_respawn_.row = Game::FindParamRow<MenuCommonParam>(singletons, L"MenuCommonParam", 0);
        if (faster_respawn_.row == nullptr) {
            return;
        }

        faster_respawn_.solo_fade_out_time = faster_respawn_.row->soloPlayDeath_ToFadeOutTime;
        faster_respawn_.party_fade_out_time = faster_respawn_.row->partyGhostDeath_ToFadeOutTime;
        faster_respawn_.captured = true;
    }

    if (desired && !faster_respawn_.active) {
        faster_respawn_.row->soloPlayDeath_ToFadeOutTime = 0.0f;
        faster_respawn_.row->partyGhostDeath_ToFadeOutTime = 0.0f;
        faster_respawn_.active = true;
        Main::Logger::Instance().Info("Faster respawn patch applied.");
    } else if (!desired && faster_respawn_.active) {
        faster_respawn_.row->soloPlayDeath_ToFadeOutTime = faster_respawn_.solo_fade_out_time;
        faster_respawn_.row->partyGhostDeath_ToFadeOutTime = faster_respawn_.party_fade_out_time;
        faster_respawn_.active = false;
        Main::Logger::Instance().Info("Faster respawn patch restored.");
    }
}

void ParamPatches::SyncMiniDungeonWarp(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.warp_out_of_uncleared_minidungeons.load();
    if (!mini_dungeon_warp_.captured) {
        mini_dungeon_warp_.row = Game::FindParamRow<SpEffectParam>(singletons, L"SpEffectParam", 4270);
        if (mini_dungeon_warp_.row == nullptr) {
            return;
        }

        mini_dungeon_warp_.state_info = mini_dungeon_warp_.row->stateInfo;
        mini_dungeon_warp_.captured = true;
    }

    if (desired && !mini_dungeon_warp_.active) {
        mini_dungeon_warp_.row->stateInfo = 0;
        mini_dungeon_warp_.active = true;
        Main::Logger::Instance().Info("Mini-dungeon warp patch applied.");
    } else if (!desired && mini_dungeon_warp_.active) {
        mini_dungeon_warp_.row->stateInfo = mini_dungeon_warp_.state_info;
        mini_dungeon_warp_.active = false;
        Main::Logger::Instance().Info("Mini-dungeon warp patch restored.");
    }
}

void ParamPatches::SyncFreePurchase(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.free_purchase.load();
    if (!shop_free_captured_ && desired) {
        shop_free_lineup_rows_.clear();
        shop_free_sell_rows_.clear();

        Game::ForEachParamRow<ShopLineupParam>(
            singletons,
            L"ShopLineupParam",
            [this](std::uint64_t, ShopLineupParam* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                shop_free_lineup_rows_.push_back(ShopFreeLineupState{
                    bytes,
                    ReadOffsetValue<std::int32_t>(bytes, kShopLineupValueOffset),
                    ReadOffsetValue<std::int32_t>(bytes, kShopLineupValueAddOffset),
                    ReadOffsetValue<float>(bytes, kShopLineupValueMagnificationOffset),
                });
            }
        );

        Game::ForEachParamRow<EquipParamWeapon>(
            singletons,
            L"EquipParamWeapon",
            [this](std::uint64_t, EquipParamWeapon* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                shop_free_sell_rows_.push_back(SellValueState{
                    bytes,
                    kWeaponSellValueOffset,
                    ReadOffsetValue<std::int32_t>(bytes, kWeaponSellValueOffset),
                });
            }
        );

        Game::ForEachParamRow<EquipParamProtector>(
            singletons,
            L"EquipParamProtector",
            [this](std::uint64_t, EquipParamProtector* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                shop_free_sell_rows_.push_back(SellValueState{
                    bytes,
                    kProtectorSellValueOffset,
                    ReadOffsetValue<std::int32_t>(bytes, kProtectorSellValueOffset),
                });
            }
        );

        Game::ForEachParamRow<EquipParamAccessory>(
            singletons,
            L"EquipParamAccessory",
            [this](std::uint64_t, EquipParamAccessory* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                shop_free_sell_rows_.push_back(SellValueState{
                    bytes,
                    kAccessorySellValueOffset,
                    ReadOffsetValue<std::int32_t>(bytes, kAccessorySellValueOffset),
                });
            }
        );

        Game::ForEachParamRow<EquipParamGoods>(
            singletons,
            L"EquipParamGoods",
            [this](std::uint64_t, EquipParamGoods* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                shop_free_sell_rows_.push_back(SellValueState{
                    bytes,
                    kGoodsSellValueOffset,
                    ReadOffsetValue<std::int32_t>(bytes, kGoodsSellValueOffset),
                });
            }
        );

        Game::ForEachParamRow<EquipParamGem>(
            singletons,
            L"EquipParamGem",
            [this](std::uint64_t, EquipParamGem* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                shop_free_sell_rows_.push_back(SellValueState{
                    bytes,
                    kGemSellValueOffset,
                    ReadOffsetValue<std::int32_t>(bytes, kGemSellValueOffset),
                });
            }
        );

        if (shop_free_lineup_rows_.empty()) {
            return;
        }

        shop_free_captured_ = true;
    }

    if (desired && shop_free_captured_ && !shop_free_active_) {
        for (ShopFreeLineupState& entry : shop_free_lineup_rows_) {
            WriteOffsetValue<std::int32_t>(entry.row, kShopLineupValueOffset, 0);
            WriteOffsetValue<std::int32_t>(entry.row, kShopLineupValueAddOffset, 0);
            WriteOffsetValue<float>(entry.row, kShopLineupValueMagnificationOffset, 0.0f);
        }

        for (SellValueState& entry : shop_free_sell_rows_) {
            WriteOffsetValue<std::int32_t>(entry.row, entry.offset, 0);
        }

        shop_free_active_ = true;
        Main::Logger::Instance().Info("Free purchase patch applied.");
    } else if (!desired && shop_free_active_) {
        for (const ShopFreeLineupState& entry : shop_free_lineup_rows_) {
            WriteOffsetValue<std::int32_t>(entry.row, kShopLineupValueOffset, entry.value);
            WriteOffsetValue<std::int32_t>(entry.row, kShopLineupValueAddOffset, entry.value_add);
            WriteOffsetValue<float>(entry.row, kShopLineupValueMagnificationOffset, entry.value_magnification);
        }

        for (const SellValueState& entry : shop_free_sell_rows_) {
            WriteOffsetValue<std::int32_t>(entry.row, entry.offset, entry.sell_value);
        }

        shop_free_active_ = false;
        Main::Logger::Instance().Info("Free purchase patch restored.");
    }
}

void ParamPatches::SyncNoCraftingMaterialCost(const Game::SingletonRegistry&) {
    const bool desired = Main::g_FeatureStatus.no_crafting_material_cost.load();
    if (desired && !crafting_material_cost_patch_primary_.active && !crafting_material_cost_patch_secondary_.active) {
        const uintptr_t primary_target = FindPatternInText(kCraftingMaterialCostPatternPrimary);
        if (InstallDirectPatch(
                crafting_material_cost_patch_primary_,
                primary_target,
                kCraftingMaterialCostPrimaryPatch,
                sizeof(kCraftingMaterialCostPrimaryPatch)) &&
            InstallCraftingMaterialCostSecondaryPatch(crafting_material_cost_patch_secondary_)) {
            Main::Logger::Instance().Info("No crafting material cost patch applied.");
        } else {
            RestoreCodePatch(crafting_material_cost_patch_primary_);
            RestoreCodePatch(crafting_material_cost_patch_secondary_);
        }
    } else if (!desired &&
               (crafting_material_cost_patch_primary_.active || crafting_material_cost_patch_secondary_.active)) {
        const bool primary_restored = RestoreCodePatch(crafting_material_cost_patch_primary_);
        const bool secondary_restored = RestoreCodePatch(crafting_material_cost_patch_secondary_);
        if (primary_restored && secondary_restored) {
            Main::Logger::Instance().Info("No crafting material cost patch restored.");
        }
    }
}

void ParamPatches::SyncNoUpgradeMaterialCost(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.no_upgrade_material_cost.load();
    if (!upgrade_cost_captured_ && desired) {
        upgrade_cost_rows_.clear();
        const bool found_rows = Game::ForEachParamRow<EquipMtrlSetParam>(
            singletons,
            L"EquipMtrlSetParam",
            [this](std::uint64_t, EquipMtrlSetParam* row) {
                upgrade_cost_rows_.push_back(UpgradeCostState{
                    row,
                    row->materialId01,
                    row->itemNum01,
                });
            }
        );

        if (!found_rows || upgrade_cost_rows_.empty()) {
            return;
        }

        upgrade_cost_captured_ = true;
    }

    if (desired && upgrade_cost_captured_ && !upgrade_cost_active_) {
        for (UpgradeCostState& entry : upgrade_cost_rows_) {
            entry.row->materialId01 = -1;
            entry.row->itemNum01 = -1;
        }
        upgrade_cost_active_ = true;
        Main::Logger::Instance().Info("No upgrade material cost patch applied.");
    } else if (!desired && upgrade_cost_active_) {
        for (const UpgradeCostState& entry : upgrade_cost_rows_) {
            entry.row->materialId01 = entry.material_id01;
            entry.row->itemNum01 = entry.item_num01;
        }
        upgrade_cost_active_ = false;
        Main::Logger::Instance().Info("No upgrade material cost patch restored.");
    }
}

void ParamPatches::SyncNoMagicRequirements(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.no_magic_requirements.load();
    if (!magic_requirement_rows_captured_ && desired) {
        magic_requirement_rows_.clear();
        const bool found_rows = Game::ForEachParamRow<MagicParam>(
            singletons,
            L"MagicParam",
            [this](std::uint64_t, MagicParam* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                magic_requirement_rows_.push_back(MagicRequirementState{
                    bytes,
                    ReadOffsetValue<std::uint8_t>(bytes, kMagicArcaneRequirementOffset),
                    ReadOffsetValue<std::uint8_t>(bytes, kMagicIntelligenceRequirementOffset),
                    ReadOffsetValue<std::uint8_t>(bytes, kMagicFaithRequirementOffset),
                });
            }
        );

        if (!found_rows || magic_requirement_rows_.empty()) {
            return;
        }

        magic_requirement_rows_captured_ = true;
    }

    if (desired && magic_requirement_rows_captured_ && !magic_requirement_active_) {
        for (MagicRequirementState& entry : magic_requirement_rows_) {
            WriteOffsetValue<std::uint8_t>(entry.row, kMagicArcaneRequirementOffset, 0);
            WriteOffsetValue<std::uint8_t>(entry.row, kMagicIntelligenceRequirementOffset, 0);
            WriteOffsetValue<std::uint8_t>(entry.row, kMagicFaithRequirementOffset, 0);
        }
        magic_requirement_active_ = true;
        Main::Logger::Instance().Info("No magic requirements patch applied.");
    } else if (!desired && magic_requirement_active_) {
        for (const MagicRequirementState& entry : magic_requirement_rows_) {
            WriteOffsetValue<std::uint8_t>(entry.row, kMagicArcaneRequirementOffset, entry.arcane_requirement);
            WriteOffsetValue<std::uint8_t>(entry.row, kMagicIntelligenceRequirementOffset, entry.intelligence_requirement);
            WriteOffsetValue<std::uint8_t>(entry.row, kMagicFaithRequirementOffset, entry.faith_requirement);
        }
        magic_requirement_active_ = false;
        Main::Logger::Instance().Info("No magic requirements patch restored.");
    }
}

void ParamPatches::SyncAllMagicOneSlot(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.all_magic_one_slot.load();
    if (!magic_slot_rows_captured_ && desired) {
        magic_slot_rows_.clear();
        const bool found_rows = Game::ForEachParamRow<MagicParam>(
            singletons,
            L"MagicParam",
            [this](std::uint64_t, MagicParam* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                magic_slot_rows_.push_back(MagicSlotState{
                    bytes,
                    ReadOffsetValue<std::uint8_t>(bytes, kMagicSlotLengthOffset),
                });
            }
        );

        if (!found_rows || magic_slot_rows_.empty()) {
            return;
        }

        magic_slot_rows_captured_ = true;
    }

    if (desired && magic_slot_rows_captured_ && !magic_slot_active_) {
        for (MagicSlotState& entry : magic_slot_rows_) {
            WriteOffsetValue<std::uint8_t>(entry.row, kMagicSlotLengthOffset, kMagicSingleSlotValue);
        }
        magic_slot_active_ = true;
        Main::Logger::Instance().Info("All magic one slot patch applied.");
    } else if (!desired && magic_slot_active_) {
        for (const MagicSlotState& entry : magic_slot_rows_) {
            WriteOffsetValue<std::uint8_t>(entry.row, kMagicSlotLengthOffset, entry.slot_length);
        }
        magic_slot_active_ = false;
        Main::Logger::Instance().Info("All magic one slot patch restored.");
    }
}

void ParamPatches::SyncWeightlessEquipment(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.weightless_equipment.load();

    if (!equipment_weight_rows_captured_ && desired) {
        equipment_weight_rows_.clear();

        // 这些偏移直接来自 TGA 仓库里的 ID Helper：
        // EquipParamWeapon +0x10
        // EquipParamProtector +0x24
        // EquipParamAccessory +0x0C
        Game::ForEachParamRow<EquipParamWeapon>(
            singletons,
            L"EquipParamWeapon",
            [this](std::uint64_t, EquipParamWeapon* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                equipment_weight_rows_.push_back(EquipmentWeightState{
                    bytes,
                    kWeaponWeightOffset,
                    ReadOffsetValue<float>(bytes, kWeaponWeightOffset),
                });
            }
        );

        Game::ForEachParamRow<EquipParamProtector>(
            singletons,
            L"EquipParamProtector",
            [this](std::uint64_t, EquipParamProtector* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                equipment_weight_rows_.push_back(EquipmentWeightState{
                    bytes,
                    kProtectorWeightOffset,
                    ReadOffsetValue<float>(bytes, kProtectorWeightOffset),
                });
            }
        );

        Game::ForEachParamRow<EquipParamAccessory>(
            singletons,
            L"EquipParamAccessory",
            [this](std::uint64_t, EquipParamAccessory* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                equipment_weight_rows_.push_back(EquipmentWeightState{
                    bytes,
                    kAccessoryWeightOffset,
                    ReadOffsetValue<float>(bytes, kAccessoryWeightOffset),
                });
            }
        );

        if (equipment_weight_rows_.empty()) {
            return;
        }

        equipment_weight_rows_captured_ = true;
    }

    if (desired && equipment_weight_rows_captured_ && !equipment_weight_active_) {
        for (const EquipmentWeightState& entry : equipment_weight_rows_) {
            WriteOffsetValue<float>(entry.row, entry.offset, 0.0f);
        }
        equipment_weight_active_ = true;
        Main::Logger::Instance().Info("Weightless equipment patch applied via param weights.");
    } else if (!desired && equipment_weight_active_) {
        for (const EquipmentWeightState& entry : equipment_weight_rows_) {
            WriteOffsetValue<float>(entry.row, entry.offset, entry.weight);
        }
        equipment_weight_active_ = false;
        Main::Logger::Instance().Info("Weightless equipment patch restored.");
    }
}

void ParamPatches::SyncCustomFov(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.custom_fov_enabled.load();
    const float requested_value = std::clamp(Main::g_FeatureStatus.custom_fov_value.load(), 20.0f, 120.0f);

    if (!lock_cam_rows_captured_ && (desired || Main::g_FeatureStatus.custom_camera_distance_enabled.load())) {
        lock_cam_rows_.clear();
        const bool found_rows = Game::ForEachParamRow<LockCamParam>(
            singletons,
            L"LockCamParam",
            [this](std::uint64_t, LockCamParam* row) {
                lock_cam_rows_.push_back(LockCamState{
                    row,
                    row->camDistTarget,
                    row->camFovY,
                });
            }
        );

        if (!found_rows || lock_cam_rows_.empty()) {
            return;
        }

        lock_cam_rows_captured_ = true;
    }

    if (!lock_cam_rows_captured_) {
        return;
    }

    if (desired) {
        for (LockCamState& entry : lock_cam_rows_) {
            entry.row->camFovY = requested_value;
        }

        if (!custom_fov_active_) {
            Main::Logger::Instance().Info("Custom FOV patch applied.");
        }
        custom_fov_active_ = true;
    } else if (custom_fov_active_) {
        for (const LockCamState& entry : lock_cam_rows_) {
            entry.row->camFovY = entry.cam_fov_y;
        }
        custom_fov_active_ = false;
        Main::Logger::Instance().Info("Custom FOV patch restored.");
    }
}

void ParamPatches::SyncCustomCameraDistance(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.custom_camera_distance_enabled.load();
    const float requested_value =
        std::clamp(Main::g_FeatureStatus.custom_camera_distance_value.load(), 1.0f, 20.0f);

    if (!lock_cam_rows_captured_ && (desired || Main::g_FeatureStatus.custom_fov_enabled.load())) {
        lock_cam_rows_.clear();
        const bool found_rows = Game::ForEachParamRow<LockCamParam>(
            singletons,
            L"LockCamParam",
            [this](std::uint64_t, LockCamParam* row) {
                lock_cam_rows_.push_back(LockCamState{
                    row,
                    row->camDistTarget,
                    row->camFovY,
                });
            }
        );

        if (!found_rows || lock_cam_rows_.empty()) {
            return;
        }

        lock_cam_rows_captured_ = true;
    }

    if (!lock_cam_rows_captured_) {
        return;
    }

    if (desired) {
        for (LockCamState& entry : lock_cam_rows_) {
            entry.row->camDistTarget = requested_value;
        }

        if (!custom_camera_distance_active_) {
            Main::Logger::Instance().Info("Custom camera distance patch applied.");
        }
        custom_camera_distance_active_ = true;
    } else if (custom_camera_distance_active_) {
        for (const LockCamState& entry : lock_cam_rows_) {
            entry.row->camDistTarget = entry.cam_dist_target;
        }
        custom_camera_distance_active_ = false;
        Main::Logger::Instance().Info("Custom camera distance patch restored.");
    }
}

void ParamPatches::SyncMountAnywhere(const Game::SingletonRegistry&) {
    const bool desired = Main::g_FeatureStatus.mount_anywhere.load();
    if (desired && !mount_anywhere_patch_primary_.active && !mount_anywhere_patch_secondary_.active) {
        const uintptr_t primary_target = FindPatternInTextCompact(kMountAnywherePatternPrimary);
        if (InstallDirectPatch(
                mount_anywhere_patch_primary_,
                primary_target,
                kMountAnywherePrimaryPatch,
                sizeof(kMountAnywherePrimaryPatch)
            ) &&
            InstallMountAnywhereSecondaryPatch(mount_anywhere_patch_secondary_)) {
            Main::Logger::Instance().Info("Mount anywhere patch applied.");
        } else {
            RestoreCodePatch(mount_anywhere_patch_primary_);
            RestoreCodePatch(mount_anywhere_patch_secondary_);
        }
    } else if (!desired &&
               (mount_anywhere_patch_primary_.active || mount_anywhere_patch_secondary_.active)) {
        const bool primary_restored = RestoreCodePatch(mount_anywhere_patch_primary_);
        const bool secondary_restored = RestoreCodePatch(mount_anywhere_patch_secondary_);
        if (primary_restored && secondary_restored) {
            Main::Logger::Instance().Info("Mount anywhere patch restored.");
        }
    }
}

void ParamPatches::SyncSpiritAshesAnywhere(const Game::SingletonRegistry&) {
    const bool desired = Main::g_FeatureStatus.spirit_ashes_anywhere.load();
    if (desired && !spirit_ashes_anywhere_patch_primary_.active &&
        !spirit_ashes_anywhere_patch_secondary_.active) {
        if (InstallSpiritAshesPrimaryPatch(spirit_ashes_anywhere_patch_primary_) &&
            InstallSpiritAshesSecondaryPatch(spirit_ashes_anywhere_patch_secondary_)) {
            Main::Logger::Instance().Info("Spirit ashes anywhere patch applied.");
        } else {
            RestoreCodePatch(spirit_ashes_anywhere_patch_primary_);
            RestoreCodePatch(spirit_ashes_anywhere_patch_secondary_);
        }
    } else if (!desired &&
               (spirit_ashes_anywhere_patch_primary_.active ||
                spirit_ashes_anywhere_patch_secondary_.active)) {
        const bool primary_restored = RestoreCodePatch(spirit_ashes_anywhere_patch_primary_);
        const bool secondary_restored = RestoreCodePatch(spirit_ashes_anywhere_patch_secondary_);
        if (primary_restored && secondary_restored) {
            Main::Logger::Instance().Info("Spirit ashes anywhere patch restored.");
        }
    }
}

void ParamPatches::SyncItemDiscovery(const Game::SingletonRegistry& singletons) {
    const int desired_multiplier = std::clamp(Main::g_FeatureStatus.item_discovery_multiplier.load(), 1, 100);
    if (!item_discovery_.captured) {
        item_discovery_.row = Game::FindParamRow<CalcCorrectGraph>(singletons, L"CalcCorrectGraph", 140);
        if (item_discovery_.row == nullptr) {
            return;
        }

        item_discovery_.original_values = {
            item_discovery_.row->stageMaxGrowVal0,
            item_discovery_.row->stageMaxGrowVal1,
            item_discovery_.row->stageMaxGrowVal2,
            item_discovery_.row->stageMaxGrowVal3,
            item_discovery_.row->stageMaxGrowVal4,
        };
        item_discovery_.captured = true;
    }

    if (desired_multiplier == item_discovery_.applied_multiplier) {
        return;
    }

    item_discovery_.row->stageMaxGrowVal0 = item_discovery_.original_values[0] * desired_multiplier;
    item_discovery_.row->stageMaxGrowVal1 = item_discovery_.original_values[1] * desired_multiplier;
    item_discovery_.row->stageMaxGrowVal2 = item_discovery_.original_values[2] * desired_multiplier;
    item_discovery_.row->stageMaxGrowVal3 = item_discovery_.original_values[3] * desired_multiplier;
    item_discovery_.row->stageMaxGrowVal4 = item_discovery_.original_values[4] * desired_multiplier;
    item_discovery_.applied_multiplier = desired_multiplier;

    if (desired_multiplier == 1) {
        Main::Logger::Instance().Info("Item discovery multiplier restored to x1.");
    } else {
        Main::Logger::Instance().Info(
            ("Item discovery multiplier applied: x" + std::to_string(desired_multiplier)).c_str()
        );
    }
}

void ParamPatches::SyncPermanentLantern(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.permanent_lantern.load();
    if (!permanent_lantern_.captured) {
        permanent_lantern_.row = Game::FindParamRow<SpEffectParam>(singletons, L"SpEffectParam", 3245);
        if (permanent_lantern_.row == nullptr) {
            return;
        }

        permanent_lantern_.save_category = permanent_lantern_.row->saveCategory;
        permanent_lantern_.captured = true;
    }

    if (desired && !permanent_lantern_.active) {
        permanent_lantern_.row->saveCategory = 0;
        permanent_lantern_.active = true;
        Main::Logger::Instance().Info("Permanent lantern patch applied.");
    } else if (!desired && permanent_lantern_.active) {
        permanent_lantern_.row->saveCategory = permanent_lantern_.save_category;
        permanent_lantern_.active = false;
        Main::Logger::Instance().Info("Permanent lantern patch restored.");
    }
}

void ParamPatches::SyncInvisibleHelmets(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.invisible_helmets.load();
    if (!helmet_rows_captured_ && desired) {
        helmet_rows_.clear();
        const bool found_rows = Game::ForEachParamRow<EquipParamProtector>(
            singletons,
            L"EquipParamProtector",
            [this](std::uint64_t, EquipParamProtector* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                if ((bytes[kProtectorHeadFlagOffset] & kProtectorHeadEquipBit) == 0) {
                    return;
                }

                HelmetState state{};
                state.row = bytes;
                std::memcpy(
                    state.head_and_invisible_flags.data(),
                    bytes + kProtectorHeadFlagOffset,
                    state.head_and_invisible_flags.size()
                );
                helmet_rows_.push_back(std::move(state));
            }
        );

        if (!found_rows || helmet_rows_.empty()) {
            return;
        }

        helmet_rows_captured_ = true;
    }

    if (desired && helmet_rows_captured_ && !invisible_helmets_active_) {
        for (HelmetState& state : helmet_rows_) {
            std::memset(state.row + kProtectorHeadFlagOffset, 0, state.head_and_invisible_flags.size());
        }
        invisible_helmets_active_ = true;
        Main::Logger::Instance().Info("Invisible helmets patch applied.");
    } else if (!desired && invisible_helmets_active_) {
        for (const HelmetState& state : helmet_rows_) {
            std::memcpy(
                state.row + kProtectorHeadFlagOffset,
                state.head_and_invisible_flags.data(),
                state.head_and_invisible_flags.size()
            );
        }
        invisible_helmets_active_ = false;
        Main::Logger::Instance().Info("Invisible helmets patch restored.");
    }
}

}  // namespace ERD::Features
