#include "ParamPatches.hpp"

#include "../Game/Memory.hpp"
#include "../Game/Params.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"
#include "../Param/BEHAVIOR_PARAM_ST.hpp"
#include "../Param/EQUIP_PARAM_GOODS_ST.hpp"
#include "../Param/EQUIP_PARAM_WEAPON_ST.hpp"
#include "../Param/MAGIC_PARAM_ST.hpp"
#include "../Param/NPC_PARAM_ST.hpp"
#include "../Param/SP_EFFECT_PARAM_ST.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

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
constexpr std::size_t kMagicArcaneRequirementOffset = ERD_OFFSET_MAGIC_PARAM_ST_requirementLuck;
constexpr std::size_t kMagicSlotLengthOffset = ERD_OFFSET_MAGIC_PARAM_ST_slotLength;
constexpr std::size_t kMagicIntelligenceRequirementOffset = ERD_OFFSET_MAGIC_PARAM_ST_requirementIntellect;
constexpr std::size_t kMagicFaithRequirementOffset = ERD_OFFSET_MAGIC_PARAM_ST_requirementFaith;
constexpr std::size_t kMagicMpOffset = ERD_OFFSET_MAGIC_PARAM_ST_mp;
constexpr std::size_t kMagicMpChargeOffset = ERD_OFFSET_MAGIC_PARAM_ST_mp_charge;
constexpr std::uint8_t kMagicSingleSlotValue = 1;
constexpr std::size_t kGoodsConsumeMpOffset = ERD_OFFSET_EQUIP_PARAM_GOODS_ST_consumeMP;
constexpr std::size_t kBehaviorHeroPointOffset = ERD_OFFSET_BEHAVIOR_PARAM_ST_heroPoint;
constexpr std::size_t kWeaponWeightOffset = 0x10;
constexpr std::size_t kWeaponIsEnhanceOffset = 0x106;
constexpr std::size_t kWeaponGemMountTypeOffset = ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_gemMountType;
constexpr std::size_t kWeaponParryDamageLifeOffset = ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_parryDamageLife;
constexpr std::size_t kWeaponEnableParryByteOffset = ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_BF_BYTE1;
constexpr std::uint8_t kWeaponEnableParryMask = ERD_MASK_EQUIP_PARAM_WEAPON_ST_enableParry;
constexpr std::size_t kProtectorWeightOffset = 0x24;
constexpr std::size_t kAccessoryWeightOffset = 0x0C;
constexpr std::uint64_t kPlayerDamageMultiplierSpEffectRowId = 416;
constexpr std::uint64_t kBuffDurationExtendSpEffectRowId = 330600;
constexpr std::uint64_t kNoFallDeathSpEffectRowId = 360400;
constexpr std::uint64_t kNoFallDeathMiddleSpEffectRowId = 185;
constexpr std::uint64_t kNoFallDeathTriggerSpEffectRowId = 184;
constexpr std::array<std::uint64_t, 8> kAttackLifeStealOnHitSpEffectRowIds{{650, 651, 652, 653, 654, 655, 656, 657}};
constexpr std::size_t kBuffDurationExtendOffset = ERD_OFFSET_SP_EFFECT_PARAM_ST_extendLifeRate;
constexpr std::size_t kEffectEnduranceOffset = ERD_OFFSET_SP_EFFECT_PARAM_ST_effectEndurance;
constexpr std::size_t kChangeMagicSlotOffset = ERD_OFFSET_SP_EFFECT_PARAM_ST_changeMagicSlot;
constexpr std::size_t kConsumeStaminaRateOffset = ERD_OFFSET_SP_EFFECT_PARAM_ST_consumeStaminaRate;
constexpr std::size_t kNoFallDeathCycleOccurrenceOffset = ERD_OFFSET_SP_EFFECT_PARAM_ST_cycleOccurrenceSpEffectId;
constexpr std::size_t kAttackLifeStealOnHitCorrectRateOffset = ERD_OFFSET_SP_EFFECT_PARAM_ST_changeHpEstusFlaskCorrectRate;
constexpr std::int8_t kAutoAppliedSpEffectChangeMagicSlotValue = 3;
constexpr std::array<std::size_t, 5> kPlayerDamageMultiplierOffsets{{
    ERD_OFFSET_SP_EFFECT_PARAM_ST_physicsAttackPowerRate,
    ERD_OFFSET_SP_EFFECT_PARAM_ST_magicAttackPowerRate,
    ERD_OFFSET_SP_EFFECT_PARAM_ST_fireAttackPowerRate,
    ERD_OFFSET_SP_EFFECT_PARAM_ST_thunderAttackPowerRate,
    ERD_OFFSET_SP_EFFECT_PARAM_ST_darkAttackPowerRate,
}};
constexpr std::array<std::size_t, 5> kPlayerDamageCutMultiplierOffsets{{
    ERD_OFFSET_SP_EFFECT_PARAM_ST_neutralDamageCutRate,
    ERD_OFFSET_SP_EFFECT_PARAM_ST_magicDamageCutRate,
    ERD_OFFSET_SP_EFFECT_PARAM_ST_fireDamageCutRate,
    ERD_OFFSET_SP_EFFECT_PARAM_ST_thunderDamageCutRate,
    ERD_OFFSET_SP_EFFECT_PARAM_ST_darkDamageCutRate,
}};
constexpr std::size_t kEnemyHpOffset = ERD_OFFSET_NPC_PARAM_ST_hp;
constexpr std::array<std::size_t, 10> kWeaponRequirementOffsets{{
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_attainmentWepStatusStr,
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_attainmentWepStatusDex,
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_attainmentWepStatusMag,
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_attainmentWepStatusFai,
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_attainmentWepStatusLuc,
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_properStrength,
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_properAgility,
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_properMagic,
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_properFaith,
    ERD_OFFSET_EQUIP_PARAM_WEAPON_ST_properLuck,
}};
constexpr std::array<const char*, 1> kCraftingMaterialCostPatternPrimaryCandidates{{
    "0F 4F F8 48 83 C3",
}};
constexpr std::array<const char*, 1> kCraftingMaterialCostPatternSecondaryCandidates{{
    "0F B7 50 ? 66 3B CA 77",
}};
constexpr std::array<const char*, 1> kNoRuneArcLossPatternCandidates{{
    "74 09 66 C7 81 FF",
}};
constexpr std::array<const char*, 1> kNoTimePassOnDeathPatternCandidates{{
    "4C 8B 74 24 70 48 8B 6C 24 78 4D 39 6F 08 74 40",
}};
constexpr std::uint8_t kCraftingMaterialCostPrimaryPatch[] = {0x90, 0x90, 0x90};
constexpr std::uint8_t kNoRuneArcLossPatchByte = 0xEB;
constexpr uintptr_t kGameManStoredTimeOffset = 0x20;
constexpr std::array<const char*, 1> kSpiritAshesAnywherePatternPrimaryCandidates{{
    "48 8B 47 ? F3 0F 10 90 ? ? ? ? 0F 2F D0",
}};
constexpr std::array<const char*, 1> kSpiritAshesAnywherePatternSecondaryCandidates{{
    "48 8B 45 98 48 85 C0 0F 84 ? ? ? ? 8B 40 20",
}};
constexpr float kSpiritAshesAnywhereRadius = 1000.0f;
constexpr std::array<const char*, 1> kTorrentDisabledInUnderworldPatternCandidates{{
    "80 78 36 00 0F",
}};
constexpr std::array<const char*, 1> kWhistleDisabledPatternCandidates{{
    "80 79 36 00 0F 95 C0 48 83 C4 28 C3",
}};
constexpr std::array<std::uint8_t, 3> kTorrentAnywherePatchBytes{0x30, 0xC0, 0x90};
constexpr ULONGLONG kTorrentAnywhereValidationIntervalMs = 1000;
constexpr std::array<const char*, 1> kOpenMapInCombatPatternCandidates{{
    "84 C0 74 2E C7",
}};
constexpr std::array<const char*, 1> kCloseMapInCombatPatternCandidates{{
    "48 8B 03 48 8B CB 48 8B 94 24 ? ? ? ? FF 50 ? 48 8D 8B ? 27 00 00",
}};
constexpr std::uint8_t kOpenMapInCombatPatchByte = 0xEB;
constexpr std::array<std::uint8_t, 3> kCloseMapInCombatPatchBytes{0x90, 0x90, 0x90};
constexpr std::array<const char*, 2> kInfiniteJumpHorsePrimaryPatternCandidates{{
    // Elden Ring 1.16.1 CE script context for Torrent jump state writeback at +46B41C.
    "88 87 ? ? ? ? 48 8B 83 ? ? ? ? 48 8B 48 ? 80 B9 D1 01 00 00 00 74 ? 48 81 C1 60 02 00 00 E8 ? ? ? ? 84 C0 75 ? B8 01 00 00 00 EB ?",
    "88 87 ? ? ? ? 48 8B 83 ? ? ? ? 48 8B 48 ? 80 B9",
}};
constexpr std::array<const char*, 2> kInfiniteJumpHorseSecondaryPatternCandidates{{
    // Elden Ring 1.16.1 CE script context for secondary Torrent jump flag writeback at +46C56F.
    "88 87 ? ? ? ? 48 8B 83 ? ? ? ? 48 8B 48 ? E8 ? ? ? ? 88 87 ? ? ? ? 48 8B 83 ? ? ? ? 48 8B 48 ? E8 ? ? ? ? 48 8B CF 88 87",
    "88 87 ? ? ? ? 48 8B 83 ? ? ? ? 48 8B 48 ? E8 ? ? ? ? 88 87",
}};
constexpr std::array<const char*, 3> kInfiniteJumpCmpPatternCandidates{{
    // Elden Ring 1.16.1 CE script context for the jump-state compare at +407D4B.
    "83 EC ?? 80 B9 D1 01 00 00 00 74 ?? 48 81 C1 60 02 00 00 E8 ? ? ? ? 84 C0 75 ?? B8 01 00 00 00 EB ??",
    // Elden Ring 1.16.2 emits the success return as mov al,1 instead of mov eax,1.
    "83 EC ?? 80 B9 D1 01 00 00 00 74 ?? 48 81 C1 60 02 00 00 E8 ? ? ? ? 84 C0 75 ?? B0 01 48 83 C4 ?? C3 32 C0",
    "83 EC ?? 80 B9 D1 01 00 00 00 74 ?? 48",
}};
constexpr std::uint8_t kInfiniteJumpCmpImmediate = 0x92;

template <typename Fn>
bool ForEachMagicParamRowWithFallback(
    const Game::SingletonRegistry& singletons,
    Fn&& fn,
    const wchar_t** used_table_name = nullptr) {
    constexpr std::array<const wchar_t*, 4> kMagicParamTableCandidates{{
        L"Magic",
        L"MagicParam",
        L"MAGIC_PARAM_ST",
        L"MagicParam_PC",
    }};

    for (const wchar_t* table_name : kMagicParamTableCandidates) {
        if (Game::ForEachParamRow<MagicParam>(singletons, table_name, fn)) {
            if (used_table_name != nullptr) {
                *used_table_name = table_name;
            }
            return true;
        }
    }
    return false;
}

template <typename Fn>
bool ForEachBehaviorParamRowWithFallback(
    const Game::SingletonRegistry& singletons,
    Fn&& fn,
    const wchar_t** used_table_name = nullptr) {
    constexpr std::array<const wchar_t*, 2> kBehaviorParamTableCandidates{{
        L"BehaviorParam_PC",
        L"BehaviorParam",
    }};

    for (const wchar_t* table_name : kBehaviorParamTableCandidates) {
        if (Game::ForEachParamRow<BehaviorParam>(singletons, table_name, fn)) {
            if (used_table_name != nullptr) {
                *used_table_name = table_name;
            }
            return true;
        }
    }
    return false;
}

void LogProtectedException(const char* scope, const char* detail) {
    Main::Logger::Instance().Error((std::string(scope) + " failed: " + detail).c_str());
}

std::string NarrowAscii(const wchar_t* text) {
    if (text == nullptr) {
        return "(unknown)";
    }

    std::string result;
    while (*text != L'\0') {
        const wchar_t ch = *text++;
        result.push_back(ch >= 0 && ch <= 0x7F ? static_cast<char>(ch) : '?');
    }
    return result;
}

std::string HexAddress(uintptr_t value) {
    std::ostringstream stream;
    stream << "0x" << std::hex << std::uppercase << value;
    return stream.str();
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

std::vector<uintptr_t> FindAllPatternsInText(const char* aob) {
    std::vector<uintptr_t> matches;
    Pattern pattern{};
    Region text{};
    if (!ParsePattern(aob, pattern) || !RegionFromMainModuleText(text) || text.size < pattern.bytes.size()) {
        return matches;
    }

    const auto* begin = reinterpret_cast<const std::uint8_t*>(text.base);
    const auto* end = begin + text.size - pattern.bytes.size();
    for (const auto* cursor = begin; cursor <= end; ++cursor) {
        if (MatchesPattern(cursor, pattern)) {
            matches.push_back(reinterpret_cast<uintptr_t>(cursor));
        }
    }

    return matches;
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

template <std::size_t N>
uintptr_t FindFirstPatternInText(const std::array<const char*, N>& candidates) {
    for (const char* pattern : candidates) {
        if (pattern == nullptr) {
            continue;
        }
        const uintptr_t address = FindPatternInText(pattern);
        if (address != 0) {
            return address;
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

bool WriteRipRelativeDisplacement(std::uint8_t* instruction,
                                  uintptr_t instruction_address,
                                  uintptr_t target_address,
                                  std::size_t instruction_size,
                                  std::size_t displacement_offset) {
    if (instruction == nullptr || instruction_size < displacement_offset + sizeof(std::int32_t)) {
        return false;
    }

    const std::int64_t delta =
        static_cast<std::int64_t>(target_address) -
        static_cast<std::int64_t>(instruction_address + instruction_size);
    if (delta < INT32_MIN || delta > INT32_MAX) {
        return false;
    }

    const auto rel32 = static_cast<std::int32_t>(delta);
    std::memcpy(instruction + displacement_offset, &rel32, sizeof(rel32));
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

bool PatchBytesMatch(const ParamPatches::CodePatchState& state, const void* expected, std::size_t size) {
    if (!state.captured || state.target == 0 || size == 0 || size > state.patch_size) {
        return false;
    }

    std::array<std::uint8_t, 16> current_bytes{};
    if (!Game::ReadMemory(state.target, current_bytes.data(), size)) {
        return false;
    }

    return std::memcmp(current_bytes.data(), expected, size) == 0;
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

std::uint8_t ScaleRequirementValue(std::uint8_t value, int reduction_percent) {
    const int clamped_percent = std::clamp(reduction_percent, 0, 100);
    if (clamped_percent >= 100) {
        return 0;
    }
    const float ratio = static_cast<float>(100 - clamped_percent) / 100.0f;
    const int scaled = static_cast<int>(std::lround(static_cast<float>(value) * ratio));
    return static_cast<std::uint8_t>(std::clamp(scaled, 0, 255));
}

bool InstallSpiritAshesPrimaryPatch(ParamPatches::CodePatchState& state) {
    if (!state.captured) {
        if (!CapturePatchTarget(state, FindFirstPatternInText(kSpiritAshesAnywherePatternPrimaryCandidates), 12)) {
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
        if (!CapturePatchTarget(state, FindFirstPatternInText(kSpiritAshesAnywherePatternSecondaryCandidates), 7)) {
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
        if (!CapturePatchTarget(state, FindFirstPatternInText(kCraftingMaterialCostPatternSecondaryCandidates), 7)) {
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

bool InstallForceAlThenReplayPatch(ParamPatches::CodePatchState& state, uintptr_t target) {
    if (!state.captured) {
        if (!CapturePatchTarget(state, target, 6)) {
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
    const std::uint8_t set_al_true[] = {0xB0, 0x01};
    std::memcpy(cave + cursor, set_al_true, sizeof(set_al_true));
    cursor += sizeof(set_al_true);

    std::memcpy(cave + cursor, state.original_bytes.data(), state.patch_size);
    cursor += state.patch_size;

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

    std::array<std::uint8_t, 6> target_patch{};
    std::memcpy(target_patch.data(), target_jump.data(), target_jump.size());
    target_patch[5] = 0x90;

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
    ERD_PROTECTED_STEP("ParamPatches.AllWeaponsEnchantable", SyncAllWeaponsEnchantable(singletons));
    ERD_PROTECTED_STEP("ParamPatches.AllWeaponsAshOfWarChangeable", SyncAllWeaponsAshOfWarChangeable(singletons));
    ERD_PROTECTED_STEP("ParamPatches.NoMagicRequirements", SyncNoMagicRequirements(singletons));
    ERD_PROTECTED_STEP("ParamPatches.AllMagicOneSlot", SyncAllMagicOneSlot(singletons));
    ERD_PROTECTED_STEP("ParamPatches.WeightlessEquipment", SyncWeightlessEquipment(singletons));
    ERD_PROTECTED_STEP("ParamPatches.WeaponRequirementReduction", SyncWeaponRequirementReduction(singletons));
    ERD_PROTECTED_STEP("ParamPatches.WeaponParry", SyncWeaponParry(singletons));
    ERD_PROTECTED_STEP("ParamPatches.CustomFov", SyncCustomFov(singletons));
    ERD_PROTECTED_STEP("ParamPatches.CustomCameraDistance", SyncCustomCameraDistance(singletons));
    ERD_PROTECTED_STEP("ParamPatches.SpiritAshesAnywhere", SyncSpiritAshesAnywhere(singletons));
    ERD_PROTECTED_STEP("ParamPatches.TorrentAnywhere", SyncTorrentAnywhere(singletons));
    ERD_PROTECTED_STEP("ParamPatches.OpenMapInCombat", SyncOpenMapInCombat(singletons));
    ERD_PROTECTED_STEP("ParamPatches.InfiniteJump", SyncInfiniteJump(singletons));
    ERD_PROTECTED_STEP("ParamPatches.ItemDiscovery", SyncItemDiscovery(singletons));
    ERD_PROTECTED_STEP("ParamPatches.RevealInvisible", SyncRevealInvisible(singletons));
    ERD_PROTECTED_STEP("ParamPatches.PlayerDamageMultiplier", SyncPlayerDamageMultiplier(singletons));
    ERD_PROTECTED_STEP("ParamPatches.PlayerDamageCutMultiplier", SyncPlayerDamageCutMultiplier(singletons));
    ERD_PROTECTED_STEP("ParamPatches.StaminaCostReduction", SyncStaminaCostReduction(singletons));
    ERD_PROTECTED_STEP("ParamPatches.FpCostReduction", SyncFpCostReduction(singletons));
    ERD_PROTECTED_STEP("ParamPatches.BuffDurationExtend", SyncBuffDurationExtend(singletons));
    ERD_PROTECTED_STEP("ParamPatches.NoFallDeath", SyncNoFallDeath(singletons));
    ERD_PROTECTED_STEP("ParamPatches.NoRuneArcLossOnDeath", SyncNoRuneArcLossOnDeath(singletons));
    ERD_PROTECTED_STEP("ParamPatches.NoTimePassOnDeath", SyncNoTimePassOnDeath(singletons));
    ERD_PROTECTED_STEP("ParamPatches.EnemyHpMultiplier", SyncEnemyHpMultiplier(singletons));
    ERD_PROTECTED_STEP("ParamPatches.PermanentLantern", SyncPermanentLantern(singletons));
    ERD_PROTECTED_STEP("ParamPatches.InvisibleHelmets", SyncInvisibleHelmets(singletons));
}

void ParamPatches::Reset(const Game::SingletonRegistry& singletons) {
    auto& status = Main::g_FeatureStatus;

    const bool free_purchase = status.free_purchase.load();
    const bool no_crafting_material_cost = status.no_crafting_material_cost.load();
    const bool no_upgrade_material_cost = status.no_upgrade_material_cost.load();
    const bool all_weapons_enchantable = status.all_weapons_enchantable.load();
    const bool all_weapons_ash_of_war_changeable = status.all_weapons_ash_of_war_changeable.load();
    const bool no_magic_requirements = status.no_magic_requirements.load();
    const bool all_magic_one_slot = status.all_magic_one_slot.load();
    const int equipment_weight_reduction_percent = status.equipment_weight_reduction_percent.load();
    const int weapon_requirement_reduction_percent = status.weapon_requirement_reduction_percent.load();
    const int weapon_parry_mode = status.weapon_parry_mode.load();
    const bool custom_fov_enabled = status.custom_fov_enabled.load();
    const bool custom_camera_distance_enabled = status.custom_camera_distance_enabled.load();
    const bool spirit_ashes_anywhere = status.spirit_ashes_anywhere.load();
    const bool torrent_anywhere = status.torrent_anywhere.load();
    const bool open_map_in_combat = status.open_map_in_combat.load();
    const bool infinite_jump = status.infinite_jump.load();
    const int damage_multiplier_percent = status.damage_multiplier_percent.load();
    const int damage_cut_multiplier_percent = status.damage_cut_multiplier_percent.load();
    const int damage_reduce_multiplier_percent = status.damage_reduce_multiplier_percent.load();
    const int damage_taken_multiplier_percent = status.damage_taken_multiplier_percent.load();
    const int stamina_cost_reduction_percent = status.stamina_cost_reduction_percent.load();
    const int fp_cost_reduction_percent = status.fp_cost_reduction_percent.load();
    const int buff_duration_extend_mode = status.buff_duration_extend_mode.load();
    const bool no_fall_death = status.no_fall_death.load();
    const bool no_rune_arc_loss_on_death = status.no_rune_arc_loss_on_death.load();
    const bool no_time_pass_on_death = status.no_time_pass_on_death.load();
    const int enemy_hp_increase_percent = status.enemy_hp_increase_percent.load();
    const int enemy_hp_decrease_percent = status.enemy_hp_decrease_percent.load();
    const bool permanent_lantern = status.permanent_lantern.load();
    const bool invisible_helmets = status.invisible_helmets.load();

    status.free_purchase = false;
    status.no_crafting_material_cost = false;
    status.no_upgrade_material_cost = false;
    status.all_weapons_enchantable = false;
    status.all_weapons_ash_of_war_changeable = false;
    status.no_magic_requirements = false;
    status.all_magic_one_slot = false;
    status.equipment_weight_reduction_percent = 0;
    status.weapon_requirement_reduction_percent = 0;
    status.weapon_parry_mode = 0;
    status.custom_fov_enabled = false;
    status.custom_camera_distance_enabled = false;
    status.spirit_ashes_anywhere = false;
    status.torrent_anywhere = false;
    status.open_map_in_combat = false;
    status.infinite_jump = false;
    status.damage_multiplier_percent = 0;
    status.damage_cut_multiplier_percent = 0;
    status.damage_reduce_multiplier_percent = 0;
    status.damage_taken_multiplier_percent = 0;
    status.stamina_cost_reduction_percent = 0;
    status.fp_cost_reduction_percent = 0;
    status.buff_duration_extend_mode = 0;
    status.no_fall_death = false;
    status.no_rune_arc_loss_on_death = false;
    status.no_time_pass_on_death = false;
    status.enemy_hp_increase_percent = 0;
    status.enemy_hp_decrease_percent = 0;
    status.permanent_lantern = false;
    status.invisible_helmets = false;

    SyncFreePurchase(singletons);
    SyncNoCraftingMaterialCost(singletons);
    SyncNoUpgradeMaterialCost(singletons);
    SyncAllWeaponsEnchantable(singletons);
    SyncAllWeaponsAshOfWarChangeable(singletons);
    SyncNoMagicRequirements(singletons);
    SyncAllMagicOneSlot(singletons);
    SyncWeightlessEquipment(singletons);
    SyncWeaponRequirementReduction(singletons);
    SyncWeaponParry(singletons);
    SyncCustomFov(singletons);
    SyncCustomCameraDistance(singletons);
    SyncSpiritAshesAnywhere(singletons);
    SyncTorrentAnywhere(singletons);
    SyncOpenMapInCombat(singletons);
    SyncInfiniteJump(singletons);
    SyncPlayerDamageMultiplier(singletons);
    SyncPlayerDamageCutMultiplier(singletons);
    SyncStaminaCostReduction(singletons);
    SyncFpCostReduction(singletons);
    SyncBuffDurationExtend(singletons);
    SyncNoFallDeath(singletons);
    SyncNoRuneArcLossOnDeath(singletons);
    SyncNoTimePassOnDeath(singletons);
    SyncEnemyHpMultiplier(singletons);
    SyncPermanentLantern(singletons);
    SyncInvisibleHelmets(singletons);

    status.free_purchase = free_purchase;
    status.no_crafting_material_cost = no_crafting_material_cost;
    status.no_upgrade_material_cost = no_upgrade_material_cost;
    status.all_weapons_enchantable = all_weapons_enchantable;
    status.all_weapons_ash_of_war_changeable = all_weapons_ash_of_war_changeable;
    status.no_magic_requirements = no_magic_requirements;
    status.all_magic_one_slot = all_magic_one_slot;
    status.equipment_weight_reduction_percent = equipment_weight_reduction_percent;
    status.weapon_requirement_reduction_percent = weapon_requirement_reduction_percent;
    status.weapon_parry_mode = weapon_parry_mode;
    status.custom_fov_enabled = custom_fov_enabled;
    status.custom_camera_distance_enabled = custom_camera_distance_enabled;
    status.spirit_ashes_anywhere = spirit_ashes_anywhere;
    status.torrent_anywhere = torrent_anywhere;
    status.open_map_in_combat = open_map_in_combat;
    status.infinite_jump = infinite_jump;
    status.damage_multiplier_percent = damage_multiplier_percent;
    status.damage_cut_multiplier_percent = damage_cut_multiplier_percent;
    status.damage_reduce_multiplier_percent = damage_reduce_multiplier_percent;
    status.damage_taken_multiplier_percent = damage_taken_multiplier_percent;
    status.stamina_cost_reduction_percent = stamina_cost_reduction_percent;
    status.fp_cost_reduction_percent = fp_cost_reduction_percent;
    status.buff_duration_extend_mode = buff_duration_extend_mode;
    status.no_fall_death = no_fall_death;
    status.no_rune_arc_loss_on_death = no_rune_arc_loss_on_death;
    status.no_time_pass_on_death = no_time_pass_on_death;
    status.enemy_hp_increase_percent = enemy_hp_increase_percent;
    status.enemy_hp_decrease_percent = enemy_hp_decrease_percent;
    status.permanent_lantern = permanent_lantern;
    status.invisible_helmets = invisible_helmets;
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
        const uintptr_t primary_target = FindFirstPatternInText(kCraftingMaterialCostPatternPrimaryCandidates);
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

void ParamPatches::SyncAllWeaponsEnchantable(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.all_weapons_enchantable.load();

    if (!weapon_enhance_rows_captured_ && desired) {
        weapon_enhance_rows_.clear();
        const bool found_rows = Game::ForEachParamRow<EquipParamWeapon>(
            singletons,
            L"EquipParamWeapon",
            [this](std::uint64_t, EquipParamWeapon* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                weapon_enhance_rows_.push_back(WeaponEnhanceState{
                    bytes,
                    ReadOffsetValue<std::uint8_t>(bytes, kWeaponIsEnhanceOffset),
                });
            }
        );

        if (!found_rows || weapon_enhance_rows_.empty()) {
            return;
        }

        weapon_enhance_rows_captured_ = true;
    }

    if (!weapon_enhance_rows_captured_) {
        return;
    }

    if (desired && !weapon_enhance_active_) {
        for (const WeaponEnhanceState& entry : weapon_enhance_rows_) {
            WriteOffsetValue<std::uint8_t>(entry.row, kWeaponIsEnhanceOffset, 0xFF);
        }
        weapon_enhance_active_ = true;
        Main::Logger::Instance().Info("All weapons enchantable patch applied.");
    } else if (!desired && weapon_enhance_active_) {
        for (const WeaponEnhanceState& entry : weapon_enhance_rows_) {
            WriteOffsetValue<std::uint8_t>(entry.row, kWeaponIsEnhanceOffset, entry.is_enhance);
        }
        weapon_enhance_active_ = false;
        Main::Logger::Instance().Info("All weapons enchantable patch restored.");
    }
}

void ParamPatches::SyncAllWeaponsAshOfWarChangeable(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.all_weapons_ash_of_war_changeable.load();

    if (!weapon_ash_of_war_rows_captured_ && desired) {
        weapon_ash_of_war_rows_.clear();
        const bool found_rows = Game::ForEachParamRow<EquipParamWeapon>(
            singletons,
            L"EquipParamWeapon",
            [this](std::uint64_t, EquipParamWeapon* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                weapon_ash_of_war_rows_.push_back(WeaponEnhanceState{
                    bytes,
                    ReadOffsetValue<std::uint8_t>(bytes, kWeaponGemMountTypeOffset),
                });
            }
        );

        if (!found_rows || weapon_ash_of_war_rows_.empty()) {
            return;
        }

        weapon_ash_of_war_rows_captured_ = true;
        const std::size_t patchable_rows = std::count_if(
            weapon_ash_of_war_rows_.begin(),
            weapon_ash_of_war_rows_.end(),
            [](const WeaponEnhanceState& entry) { return entry.is_enhance == 0; });
        Main::Logger::Instance().Info(
            ("All weapons Ash of War changeable captured rows=" +
             std::to_string(weapon_ash_of_war_rows_.size()) +
             ", patchable gemMountType=0 rows=" +
             std::to_string(patchable_rows) +
             ".")
                .c_str());
    }

    if (!weapon_ash_of_war_rows_captured_) {
        return;
    }

    if (desired && !weapon_ash_of_war_active_) {
        std::size_t patched_rows = 0;
        for (const WeaponEnhanceState& entry : weapon_ash_of_war_rows_) {
            if (entry.is_enhance != 0) {
                continue;
            }
            WriteOffsetValue<std::uint8_t>(entry.row, kWeaponGemMountTypeOffset, 1);
            ++patched_rows;
        }
        weapon_ash_of_war_active_ = true;
        Main::Logger::Instance().Info(
            ("All weapons Ash of War changeable patch applied. rows=" +
             std::to_string(patched_rows) +
             ".")
                .c_str());
    } else if (!desired && weapon_ash_of_war_active_) {
        for (const WeaponEnhanceState& entry : weapon_ash_of_war_rows_) {
            WriteOffsetValue<std::uint8_t>(entry.row, kWeaponGemMountTypeOffset, entry.is_enhance);
        }
        weapon_ash_of_war_active_ = false;
        Main::Logger::Instance().Info("All weapons Ash of War changeable patch restored.");
    }
}

void ParamPatches::SyncNoMagicRequirements(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.no_magic_requirements.load();
    static bool s_logged_missing_magic_table = false;
    static bool s_logged_capture_once = false;
    if (!magic_requirement_rows_captured_ && desired) {
        magic_requirement_rows_.clear();
        const wchar_t* used_table_name = nullptr;
        const bool found_rows = ForEachMagicParamRowWithFallback(
            singletons,
            [this](std::uint64_t row_id, MagicParam* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                magic_requirement_rows_.push_back(MagicRequirementState{
                    row_id,
                    ReadOffsetValue<std::uint8_t>(bytes, kMagicArcaneRequirementOffset),
                    ReadOffsetValue<std::uint8_t>(bytes, kMagicIntelligenceRequirementOffset),
                    ReadOffsetValue<std::uint8_t>(bytes, kMagicFaithRequirementOffset),
                });
            },
            &used_table_name
        );

        if (!found_rows || magic_requirement_rows_.empty()) {
            if (!s_logged_missing_magic_table) {
                Main::Logger::Instance().Error("No magic requirements patch failed: MagicParam table not found.");
                s_logged_missing_magic_table = true;
            }
            return;
        }

        s_logged_missing_magic_table = false;
        magic_requirement_rows_captured_ = true;
        if (!s_logged_capture_once) {
            const std::string info =
                "No magic requirements captured rows from table: " +
                NarrowAscii(used_table_name) +
                ", rows=" + std::to_string(magic_requirement_rows_.size());
            Main::Logger::Instance().Info(info.c_str());
            s_logged_capture_once = true;
        }
    }

    if (desired && magic_requirement_rows_captured_ && !magic_requirement_active_) {
        ForEachMagicParamRowWithFallback(singletons, [this](std::uint64_t row_id, MagicParam* row) {
            auto* bytes = reinterpret_cast<std::uint8_t*>(row);
            WriteOffsetValue<std::uint8_t>(bytes, kMagicArcaneRequirementOffset, 0);
            WriteOffsetValue<std::uint8_t>(bytes, kMagicIntelligenceRequirementOffset, 0);
            WriteOffsetValue<std::uint8_t>(bytes, kMagicFaithRequirementOffset, 0);
        });
        magic_requirement_active_ = true;
        Main::Logger::Instance().Info("No magic requirements patch applied.");
    } else if (!desired && magic_requirement_active_) {
        ForEachMagicParamRowWithFallback(singletons, [this](std::uint64_t row_id, MagicParam* row) {
            auto* bytes = reinterpret_cast<std::uint8_t*>(row);
            const auto it = std::find_if(
                magic_requirement_rows_.begin(),
                magic_requirement_rows_.end(),
                [row_id](const MagicRequirementState& state) { return state.row_id == row_id; });
            if (it == magic_requirement_rows_.end()) {
                return;
            }
            WriteOffsetValue<std::uint8_t>(bytes, kMagicArcaneRequirementOffset, it->arcane_requirement);
            WriteOffsetValue<std::uint8_t>(bytes, kMagicIntelligenceRequirementOffset, it->intelligence_requirement);
            WriteOffsetValue<std::uint8_t>(bytes, kMagicFaithRequirementOffset, it->faith_requirement);
        });
        magic_requirement_active_ = false;
        Main::Logger::Instance().Info("No magic requirements patch restored.");
    }
}

void ParamPatches::SyncAllMagicOneSlot(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.all_magic_one_slot.load();
    static bool s_logged_missing_magic_table = false;
    static bool s_logged_capture_once = false;
    if (!magic_slot_rows_captured_ && desired) {
        magic_slot_rows_.clear();
        const wchar_t* used_table_name = nullptr;
        const bool found_rows = ForEachMagicParamRowWithFallback(
            singletons,
            [this](std::uint64_t row_id, MagicParam* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                magic_slot_rows_.push_back(MagicSlotState{
                    row_id,
                    ReadOffsetValue<std::uint8_t>(bytes, kMagicSlotLengthOffset),
                });
            },
            &used_table_name
        );

        if (!found_rows || magic_slot_rows_.empty()) {
            if (!s_logged_missing_magic_table) {
                Main::Logger::Instance().Error("All magic one slot patch failed: MagicParam table not found.");
                s_logged_missing_magic_table = true;
            }
            return;
        }

        s_logged_missing_magic_table = false;
        magic_slot_rows_captured_ = true;
        if (!s_logged_capture_once) {
            const std::string info =
                "All magic one slot captured rows from table: " +
                NarrowAscii(used_table_name) +
                ", rows=" + std::to_string(magic_slot_rows_.size());
            Main::Logger::Instance().Info(info.c_str());
            s_logged_capture_once = true;
        }
    }

    if (desired && magic_slot_rows_captured_ && !magic_slot_active_) {
        ForEachMagicParamRowWithFallback(singletons, [](std::uint64_t, MagicParam* row) {
            auto* bytes = reinterpret_cast<std::uint8_t*>(row);
            WriteOffsetValue<std::uint8_t>(bytes, kMagicSlotLengthOffset, kMagicSingleSlotValue);
        });
        magic_slot_active_ = true;
        Main::Logger::Instance().Info("All magic one slot patch applied.");
    } else if (!desired && magic_slot_active_) {
        ForEachMagicParamRowWithFallback(singletons, [this](std::uint64_t row_id, MagicParam* row) {
            auto* bytes = reinterpret_cast<std::uint8_t*>(row);
            const auto it = std::find_if(
                magic_slot_rows_.begin(),
                magic_slot_rows_.end(),
                [row_id](const MagicSlotState& state) { return state.row_id == row_id; });
            if (it == magic_slot_rows_.end()) {
                return;
            }
            WriteOffsetValue<std::uint8_t>(bytes, kMagicSlotLengthOffset, it->slot_length);
        });
        magic_slot_active_ = false;
        Main::Logger::Instance().Info("All magic one slot patch restored.");
    }
}

void ParamPatches::SyncWeightlessEquipment(const Game::SingletonRegistry& singletons) {
    int desired_percent = std::clamp(Main::g_FeatureStatus.equipment_weight_reduction_percent.load(), 0, 100);
    if (Main::g_FeatureStatus.weightless_equipment.load()) {
        desired_percent = 100;  // 兼容旧配置开关。
    }

    if (!equipment_weight_rows_captured_ && desired_percent > 0) {
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

    if (!equipment_weight_rows_captured_) {
        return;
    }

    if (desired_percent > 0 && desired_percent != equipment_weight_applied_percent_) {
        const float reduction_ratio = static_cast<float>(100 - desired_percent) / 100.0f;
        for (const EquipmentWeightState& entry : equipment_weight_rows_) {
            WriteOffsetValue<float>(entry.row, entry.offset, entry.weight * reduction_ratio);
        }
        equipment_weight_active_ = desired_percent > 0;
        equipment_weight_applied_percent_ = desired_percent;
        Main::Logger::Instance().Info(
            ("Equipment weight reduction applied: " + std::to_string(desired_percent) + "%").c_str());
    } else if (desired_percent == 0 && equipment_weight_active_) {
        for (const EquipmentWeightState& entry : equipment_weight_rows_) {
            WriteOffsetValue<float>(entry.row, entry.offset, entry.weight);
        }
        equipment_weight_active_ = false;
        equipment_weight_applied_percent_ = 0;
        Main::Logger::Instance().Info("Equipment weight reduction restored.");
    }
}

void ParamPatches::SyncWeaponRequirementReduction(const Game::SingletonRegistry& singletons) {
    const int desired_percent = std::clamp(Main::g_FeatureStatus.weapon_requirement_reduction_percent.load(), 0, 100);

    if (!weapon_requirement_rows_captured_ && desired_percent > 0) {
        weapon_requirement_rows_.clear();
        Game::ForEachParamRow<EquipParamWeapon>(
            singletons,
            L"EquipParamWeapon",
            [this](std::uint64_t, EquipParamWeapon* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                WeaponRequirementState state{};
                state.row = bytes;
                for (std::size_t i = 0; i < kWeaponRequirementOffsets.size(); ++i) {
                    state.requirements[i] = ReadOffsetValue<std::uint8_t>(bytes, kWeaponRequirementOffsets[i]);
                }
                weapon_requirement_rows_.push_back(std::move(state));
            }
        );

        if (weapon_requirement_rows_.empty()) {
            return;
        }

        weapon_requirement_rows_captured_ = true;
    }

    if (!weapon_requirement_rows_captured_) {
        return;
    }

    if (desired_percent > 0 && desired_percent != weapon_requirement_applied_percent_) {
        for (const WeaponRequirementState& entry : weapon_requirement_rows_) {
            for (std::size_t i = 0; i < kWeaponRequirementOffsets.size(); ++i) {
                WriteOffsetValue<std::uint8_t>(
                    entry.row,
                    kWeaponRequirementOffsets[i],
                    ScaleRequirementValue(entry.requirements[i], desired_percent));
            }
        }
        weapon_requirement_active_ = true;
        weapon_requirement_applied_percent_ = desired_percent;
        Main::Logger::Instance().Info(
            ("Weapon requirement reduction applied: " + std::to_string(desired_percent) + "%").c_str());
    } else if (desired_percent == 0 && weapon_requirement_active_) {
        for (const WeaponRequirementState& entry : weapon_requirement_rows_) {
            for (std::size_t i = 0; i < kWeaponRequirementOffsets.size(); ++i) {
                WriteOffsetValue<std::uint8_t>(entry.row, kWeaponRequirementOffsets[i], entry.requirements[i]);
            }
        }
        weapon_requirement_active_ = false;
        weapon_requirement_applied_percent_ = 0;
        Main::Logger::Instance().Info("Weapon requirement reduction restored.");
    }
}

void ParamPatches::SyncWeaponParry(const Game::SingletonRegistry& singletons) {
    int desired_mode = Main::g_FeatureStatus.weapon_parry_mode.load();
    switch (desired_mode) {
    case 0:
    case 60:
    case 40:
    case 20:
    case 10:
        break;
    default:
        desired_mode = 0;
        break;
    }

    if (!weapon_parry_rows_captured_ && desired_mode > 0) {
        weapon_parry_rows_.clear();
        Game::ForEachParamRow<EquipParamWeapon>(
            singletons,
            L"EquipParamWeapon",
            [this](std::uint64_t, EquipParamWeapon* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                weapon_parry_rows_.push_back(WeaponParryState{
                    bytes,
                    ReadOffsetValue<std::int16_t>(bytes, kWeaponParryDamageLifeOffset),
                    ReadOffsetValue<std::uint8_t>(bytes, kWeaponEnableParryByteOffset),
                });
            }
        );

        if (weapon_parry_rows_.empty()) {
            return;
        }

        weapon_parry_rows_captured_ = true;
    }

    if (!weapon_parry_rows_captured_) {
        return;
    }

    if (desired_mode > 0 && desired_mode != weapon_parry_applied_mode_) {
        for (const WeaponParryState& entry : weapon_parry_rows_) {
            WriteOffsetValue<std::int16_t>(entry.row, kWeaponParryDamageLifeOffset, static_cast<std::int16_t>(desired_mode));
            const std::uint8_t flags = ReadOffsetValue<std::uint8_t>(entry.row, kWeaponEnableParryByteOffset);
            WriteOffsetValue<std::uint8_t>(entry.row, kWeaponEnableParryByteOffset, static_cast<std::uint8_t>(flags | kWeaponEnableParryMask));
        }
        weapon_parry_active_ = true;
        weapon_parry_applied_mode_ = desired_mode;
        Main::Logger::Instance().Info(
            ("Weapon parry applied: parryDamageLife=" + std::to_string(desired_mode)).c_str());
    } else if (desired_mode == 0 && weapon_parry_active_) {
        for (const WeaponParryState& entry : weapon_parry_rows_) {
            WriteOffsetValue<std::int16_t>(entry.row, kWeaponParryDamageLifeOffset, entry.parry_damage_life);
            WriteOffsetValue<std::uint8_t>(entry.row, kWeaponEnableParryByteOffset, entry.enable_parry_flags);
        }
        weapon_parry_active_ = false;
        weapon_parry_applied_mode_ = 0;
        Main::Logger::Instance().Info("Weapon parry restored.");
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

void ParamPatches::SyncTorrentAnywhere(const Game::SingletonRegistry&) {
    const bool desired = Main::g_FeatureStatus.torrent_anywhere.load();
    const bool was_active =
        torrent_anywhere_patch_underworld_.active || torrent_anywhere_patch_whistle_.active;

    const uintptr_t underworld_target = FindFirstPatternInText(kTorrentDisabledInUnderworldPatternCandidates);
    const uintptr_t whistle_target = FindFirstPatternInText(kWhistleDisabledPatternCandidates);

    bool ok = true;
    if (desired) {
        ok = InstallDirectPatch(
            torrent_anywhere_patch_underworld_,
            underworld_target == 0 ? 0 : (underworld_target + 4),
            kTorrentAnywherePatchBytes.data(),
            kTorrentAnywherePatchBytes.size());
        if (!ok) {
            return;
        }

        ok = InstallDirectPatch(
            torrent_anywhere_patch_whistle_,
            whistle_target == 0 ? 0 : (whistle_target + 4),
            kTorrentAnywherePatchBytes.data(),
            kTorrentAnywherePatchBytes.size());
        if (!ok) {
            RestoreCodePatch(torrent_anywhere_patch_underworld_);
            return;
        }

        const ULONGLONG now = GetTickCount64();
        if (was_active && now >= torrent_anywhere_next_validation_tick_) {
            torrent_anywhere_next_validation_tick_ = now + kTorrentAnywhereValidationIntervalMs;

            const bool underworld_ok = PatchBytesMatch(
                torrent_anywhere_patch_underworld_,
                kTorrentAnywherePatchBytes.data(),
                kTorrentAnywherePatchBytes.size());
            const bool whistle_ok = PatchBytesMatch(
                torrent_anywhere_patch_whistle_,
                kTorrentAnywherePatchBytes.data(),
                kTorrentAnywherePatchBytes.size());

            if (!underworld_ok) {
                torrent_anywhere_patch_underworld_.active = false;
                const bool reapplied = InstallDirectPatch(
                    torrent_anywhere_patch_underworld_,
                    torrent_anywhere_patch_underworld_.target,
                    kTorrentAnywherePatchBytes.data(),
                    kTorrentAnywherePatchBytes.size());
                if (reapplied) {
                    Main::Logger::Instance().Info("Torrent anywhere underworld patch reapplied after runtime reset.");
                }
            }

            if (!whistle_ok) {
                torrent_anywhere_patch_whistle_.active = false;
                const bool reapplied = InstallDirectPatch(
                    torrent_anywhere_patch_whistle_,
                    torrent_anywhere_patch_whistle_.target,
                    kTorrentAnywherePatchBytes.data(),
                    kTorrentAnywherePatchBytes.size());
                if (reapplied) {
                    Main::Logger::Instance().Info("Torrent anywhere whistle patch reapplied after runtime reset.");
                }
            }
        }

        if (!was_active) {
            torrent_anywhere_next_validation_tick_ = GetTickCount64() + kTorrentAnywhereValidationIntervalMs;
            Main::Logger::Instance().Info("Torrent anywhere patch applied.");
        }
    } else {
        const bool restored_a = RestoreCodePatch(torrent_anywhere_patch_underworld_);
        const bool restored_b = RestoreCodePatch(torrent_anywhere_patch_whistle_);
        torrent_anywhere_next_validation_tick_ = 0;
        if (was_active && restored_a && restored_b) {
            Main::Logger::Instance().Info("Torrent anywhere patch restored.");
        }
    }
}

void ParamPatches::SyncOpenMapInCombat(const Game::SingletonRegistry&) {
    const bool desired = Main::g_FeatureStatus.open_map_in_combat.load();
    const bool was_active =
        open_map_in_combat_patch_open_map_.active ||
        open_map_in_combat_patch_close_map_.active;

    const uintptr_t open_map_target = FindFirstPatternInText(kOpenMapInCombatPatternCandidates);
    const uintptr_t close_map_target = FindFirstPatternInText(kCloseMapInCombatPatternCandidates);

    if (desired) {
        const bool open_ok = InstallDirectPatch(
            open_map_in_combat_patch_open_map_,
            open_map_target == 0 ? 0 : (open_map_target + 2),
            &kOpenMapInCombatPatchByte,
            1);
        if (!open_ok) {
            return;
        }

        const bool close_ok = InstallDirectPatch(
            open_map_in_combat_patch_close_map_,
            close_map_target == 0 ? 0 : (close_map_target + 14),
            kCloseMapInCombatPatchBytes.data(),
            kCloseMapInCombatPatchBytes.size());
        if (!close_ok) {
            RestoreCodePatch(open_map_in_combat_patch_open_map_);
            return;
        }

        if (!was_active) {
            Main::Logger::Instance().Info("Open map in combat patch applied.");
        }
    } else {
        const bool restored_a = RestoreCodePatch(open_map_in_combat_patch_open_map_);
        const bool restored_b = RestoreCodePatch(open_map_in_combat_patch_close_map_);
        if (was_active && restored_a && restored_b) {
            Main::Logger::Instance().Info("Open map in combat patch restored.");
        }
    }
}

void ParamPatches::SyncInfiniteJump(const Game::SingletonRegistry&) {
    const bool desired = Main::g_FeatureStatus.infinite_jump.load();
    static bool s_logged_missing_pattern = false;
    const bool was_active =
        infinite_jump_horse_primary_patch_.active ||
        infinite_jump_horse_secondary_patch_.active ||
        infinite_jump_cmp_patch_.active;

    const uintptr_t horse_primary_target = FindFirstPatternInText(kInfiniteJumpHorsePrimaryPatternCandidates);
    const uintptr_t horse_secondary_target = FindFirstPatternInText(kInfiniteJumpHorseSecondaryPatternCandidates);
    const uintptr_t cmp_target = FindFirstPatternInText(kInfiniteJumpCmpPatternCandidates);

    if (desired) {
        if ((horse_primary_target == 0 || horse_secondary_target == 0 || cmp_target == 0) &&
            !s_logged_missing_pattern) {
            Main::Logger::Instance().Error(
                ("Infinite jump patch failed to resolve patterns: primary=" +
                 HexAddress(horse_primary_target) +
                 ", secondary=" +
                 HexAddress(horse_secondary_target) +
                 ", cmp=" +
                 HexAddress(cmp_target) +
                 ".")
                    .c_str());
            s_logged_missing_pattern = true;
        }

        bool ok = InstallForceAlThenReplayPatch(infinite_jump_horse_primary_patch_, horse_primary_target);
        if (!ok) {
            return;
        }

        ok = InstallForceAlThenReplayPatch(infinite_jump_horse_secondary_patch_, horse_secondary_target);
        if (!ok) {
            RestoreCodePatch(infinite_jump_horse_primary_patch_);
            return;
        }

        ok = InstallDirectPatch(infinite_jump_cmp_patch_, cmp_target == 0 ? 0 : (cmp_target + 9), &kInfiniteJumpCmpImmediate, 1);
        if (!ok) {
            RestoreCodePatch(infinite_jump_horse_primary_patch_);
            RestoreCodePatch(infinite_jump_horse_secondary_patch_);
            return;
        }

        if (!was_active) {
            Main::Logger::Instance().Info("Infinite jump patch applied.");
        }
        s_logged_missing_pattern = false;
    } else {
        const bool restored_a = RestoreCodePatch(infinite_jump_horse_primary_patch_);
        const bool restored_b = RestoreCodePatch(infinite_jump_horse_secondary_patch_);
        const bool restored_c = RestoreCodePatch(infinite_jump_cmp_patch_);
        if (was_active && restored_a && restored_b && restored_c) {
            Main::Logger::Instance().Info("Infinite jump patch restored.");
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

void ParamPatches::SyncPlayerDamageMultiplier(const Game::SingletonRegistry& singletons) {
    const int desired_increase_percent = std::clamp(Main::g_FeatureStatus.damage_multiplier_percent.load(), 0, 500);
    const int desired_reduce_percent =
        std::clamp(Main::g_FeatureStatus.damage_reduce_multiplier_percent.load(), 0, 500);
    if (!player_damage_multiplier_.captured) {
        auto* row = Game::FindParamRow<SpEffectParam>(
            singletons,
            L"SpEffectParam",
            kPlayerDamageMultiplierSpEffectRowId);
        if (row == nullptr) {
            return;
        }

        auto* bytes = reinterpret_cast<std::uint8_t*>(row);
        player_damage_multiplier_.row = bytes;
        for (std::size_t i = 0; i < kPlayerDamageMultiplierOffsets.size(); ++i) {
            player_damage_multiplier_.original_values[i] =
                ReadOffsetValue<float>(bytes, kPlayerDamageMultiplierOffsets[i]);
        }
        player_damage_multiplier_.captured = true;
    }

    const int desired_signature = desired_increase_percent > 0 ? desired_increase_percent : -desired_reduce_percent;
    if (player_damage_multiplier_.row == nullptr ||
        desired_signature == player_damage_multiplier_.applied_percent) {
        return;
    }

    if (desired_signature == 0) {
        for (std::size_t i = 0; i < kPlayerDamageMultiplierOffsets.size(); ++i) {
            WriteOffsetValue<float>(
                player_damage_multiplier_.row,
                kPlayerDamageMultiplierOffsets[i],
                player_damage_multiplier_.original_values[i]);
        }
        player_damage_multiplier_.applied_percent = 0;
        Main::Logger::Instance().Info("Player damage multiplier restored.");
        return;
    }

    float rate = 1.0f;
    std::string mode_text;
    if (desired_signature > 0) {
        rate = 1.0f + static_cast<float>(desired_increase_percent) / 100.0f;
        mode_text = "increase";
    } else {
        rate = 1.0f / (1.0f + static_cast<float>(desired_reduce_percent) / 100.0f);
        mode_text = "reduce-hard";
    }
    for (const std::size_t offset : kPlayerDamageMultiplierOffsets) {
        WriteOffsetValue<float>(player_damage_multiplier_.row, offset, rate);
    }
    player_damage_multiplier_.applied_percent = desired_signature;
    Main::Logger::Instance().Info(
        ("Player damage multiplier applied: mode=" + mode_text +
         ", value=" + std::to_string(desired_signature) +
         ", rate=" + std::to_string(rate)).c_str());
}

void ParamPatches::SyncRevealInvisible(const Game::SingletonRegistry& singletons) {
    if (!reveal_invisible_.captured) {
        auto* row = Game::FindParamRow<SpEffectParam>(
            singletons,
            L"SpEffectParam",
            kPlayerDamageMultiplierSpEffectRowId);
        if (row == nullptr) {
            return;
        }

        reveal_invisible_.row = reinterpret_cast<std::uint8_t*>(row);
        reveal_invisible_.captured = true;
    }

    if (reveal_invisible_.row == nullptr) {
        return;
    }

    const std::int8_t current_change_magic_slot =
        ReadOffsetValue<std::int8_t>(reveal_invisible_.row, kChangeMagicSlotOffset);
    if (current_change_magic_slot != kAutoAppliedSpEffectChangeMagicSlotValue) {
        WriteOffsetValue<std::int8_t>(
            reveal_invisible_.row,
            kChangeMagicSlotOffset,
            kAutoAppliedSpEffectChangeMagicSlotValue);
        Main::Logger::Instance().Info(
            ("Reveal invisible SpEffect updated: row=" + std::to_string(kPlayerDamageMultiplierSpEffectRowId) +
             ", changeMagicSlot=" + std::to_string(kAutoAppliedSpEffectChangeMagicSlotValue)).c_str());
    }
}

void ParamPatches::SyncPlayerDamageCutMultiplier(const Game::SingletonRegistry& singletons) {
    const int desired_cut_percent = std::clamp(Main::g_FeatureStatus.damage_cut_multiplier_percent.load(), 0, 500);
    const int desired_taken_percent =
        std::clamp(Main::g_FeatureStatus.damage_taken_multiplier_percent.load(), 0, 500);
    if (!player_damage_cut_multiplier_.captured) {
        auto* row = Game::FindParamRow<SpEffectParam>(
            singletons,
            L"SpEffectParam",
            kPlayerDamageMultiplierSpEffectRowId);
        if (row == nullptr) {
            return;
        }

        auto* bytes = reinterpret_cast<std::uint8_t*>(row);
        player_damage_cut_multiplier_.row = bytes;
        for (std::size_t i = 0; i < kPlayerDamageCutMultiplierOffsets.size(); ++i) {
            player_damage_cut_multiplier_.original_values[i] =
                ReadOffsetValue<float>(bytes, kPlayerDamageCutMultiplierOffsets[i]);
        }
        player_damage_cut_multiplier_.captured = true;
    }

    const int desired_signature = desired_cut_percent > 0 ? desired_cut_percent : -desired_taken_percent;
    if (player_damage_cut_multiplier_.row == nullptr ||
        desired_signature == player_damage_cut_multiplier_.applied_percent) {
        return;
    }

    if (desired_signature == 0) {
        for (std::size_t i = 0; i < kPlayerDamageCutMultiplierOffsets.size(); ++i) {
            WriteOffsetValue<float>(
                player_damage_cut_multiplier_.row,
                kPlayerDamageCutMultiplierOffsets[i],
                player_damage_cut_multiplier_.original_values[i]);
        }
        player_damage_cut_multiplier_.applied_percent = 0;
        Main::Logger::Instance().Info("Player damage cut multiplier restored.");
        return;
    }

    const float delta = static_cast<float>(std::abs(desired_signature)) / 100.0f;
    const bool is_cut_mode = desired_signature > 0;
    for (std::size_t i = 0; i < kPlayerDamageCutMultiplierOffsets.size(); ++i) {
        const float value = is_cut_mode
                                ? (player_damage_cut_multiplier_.original_values[i] - delta)
                                : (player_damage_cut_multiplier_.original_values[i] + delta);
        WriteOffsetValue<float>(player_damage_cut_multiplier_.row, kPlayerDamageCutMultiplierOffsets[i], value);
    }
    player_damage_cut_multiplier_.applied_percent = desired_signature;
    Main::Logger::Instance().Info(
        ("Player damage cut multiplier applied: mode=" + std::string(is_cut_mode ? "cut" : "taken-hard") +
         ", value=" + std::to_string(desired_signature) +
         ", delta=" + std::to_string(delta)).c_str());
}

void ParamPatches::SyncBuffDurationExtend(const Game::SingletonRegistry& singletons) {
    int desired_mode = Main::g_FeatureStatus.buff_duration_extend_mode.load();
    switch (desired_mode) {
    case 0:
    case 50:
    case 100:
    case 200:
    case 300:
    case 500:
    case 99999:
        break;
    default:
        desired_mode = 0;
        break;
    }

    if (!buff_duration_extend_.captured) {
        auto* row = Game::FindParamRow<SpEffectParam>(
            singletons,
            L"SpEffectParam",
            kBuffDurationExtendSpEffectRowId);
        if (row == nullptr) {
            return;
        }

        auto* bytes = reinterpret_cast<std::uint8_t*>(row);
        buff_duration_extend_.row = bytes;
        buff_duration_extend_.original_extend_life_rate =
            ReadOffsetValue<float>(bytes, kBuffDurationExtendOffset);
        buff_duration_extend_.original_change_magic_slot =
            ReadOffsetValue<std::int8_t>(bytes, kChangeMagicSlotOffset);
        buff_duration_extend_.captured = true;
    }

    if (buff_duration_extend_.row == nullptr) {
        return;
    }

    float value = 1.0f;
    if (desired_mode == 99999) {
        value = 99999.0f;
    } else {
        value = 1.0f + static_cast<float>(desired_mode) / 100.0f;
    }

    const float current_extend_life_rate =
        ReadOffsetValue<float>(buff_duration_extend_.row, kBuffDurationExtendOffset);
    const std::int8_t current_change_magic_slot =
        ReadOffsetValue<std::int8_t>(buff_duration_extend_.row, kChangeMagicSlotOffset);

    if (desired_mode != buff_duration_extend_.applied_mode ||
        current_extend_life_rate != value ||
        current_change_magic_slot != kAutoAppliedSpEffectChangeMagicSlotValue) {
        WriteOffsetValue<float>(buff_duration_extend_.row, kBuffDurationExtendOffset, value);
        WriteOffsetValue<std::int8_t>(
            buff_duration_extend_.row,
            kChangeMagicSlotOffset,
            kAutoAppliedSpEffectChangeMagicSlotValue);
        buff_duration_extend_.applied_mode = desired_mode;
        Main::Logger::Instance().Info(
            ("Buff duration extend applied: mode=" + std::to_string(desired_mode) +
             ", extendLifeRate=" + std::to_string(value) +
             ", changeMagicSlot=" + std::to_string(kAutoAppliedSpEffectChangeMagicSlotValue)).c_str());
    }

    if (desired_mode == 0) {
        const std::int8_t restore_change_magic_slot = buff_duration_extend_.original_change_magic_slot;
        if (current_extend_life_rate != value || current_change_magic_slot != restore_change_magic_slot) {
            WriteOffsetValue<float>(buff_duration_extend_.row, kBuffDurationExtendOffset, value);
            WriteOffsetValue<std::int8_t>(buff_duration_extend_.row, kChangeMagicSlotOffset, restore_change_magic_slot);
        }
    }
}

void ParamPatches::SyncStaminaCostReduction(const Game::SingletonRegistry& singletons) {
    const int desired_percent = std::clamp(Main::g_FeatureStatus.stamina_cost_reduction_percent.load(), 0, 100);

    if (!stamina_cost_reduction_.captured) {
        stamina_cost_reduction_.row = reinterpret_cast<std::uint8_t*>(
            Game::FindParamRow<SpEffectParam>(singletons, L"SpEffectParam", kPlayerDamageMultiplierSpEffectRowId));
        if (stamina_cost_reduction_.row == nullptr) {
            return;
        }

        stamina_cost_reduction_.original_consume_stamina_rate =
            ReadOffsetValue<float>(stamina_cost_reduction_.row, kConsumeStaminaRateOffset);
        stamina_cost_reduction_.captured = true;
    }

    if (desired_percent == stamina_cost_reduction_.applied_percent) {
        return;
    }

    const float multiplier = static_cast<float>(100 - desired_percent) / 100.0f;
    const float applied_rate = stamina_cost_reduction_.original_consume_stamina_rate * multiplier;
    WriteOffsetValue<float>(stamina_cost_reduction_.row, kConsumeStaminaRateOffset, applied_rate);
    stamina_cost_reduction_.applied_percent = desired_percent;
}

void ParamPatches::SyncFpCostReduction(const Game::SingletonRegistry& singletons) {
    const int desired_percent = std::clamp(Main::g_FeatureStatus.fp_cost_reduction_percent.load(), 0, 100);

    if (!fp_cost_reduction_.captured) {
        Game::ForEachParamRow<MagicParam>(
            singletons,
            L"Magic",
            [this](std::uint64_t, MagicParam* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                fp_cost_reduction_.magic_rows.push_back(MagicFpCostState{
                    bytes,
                    ReadOffsetValue<std::int16_t>(bytes, kMagicMpOffset),
                    ReadOffsetValue<std::int16_t>(bytes, kMagicMpChargeOffset),
                });
            });
        if (fp_cost_reduction_.magic_rows.empty()) {
            ForEachMagicParamRowWithFallback(singletons, [this](std::uint64_t, MagicParam* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                fp_cost_reduction_.magic_rows.push_back(MagicFpCostState{
                    bytes,
                    ReadOffsetValue<std::int16_t>(bytes, kMagicMpOffset),
                    ReadOffsetValue<std::int16_t>(bytes, kMagicMpChargeOffset),
                });
            });
        }

        Game::ForEachParamRow<EquipParamGoods>(
            singletons,
            L"EquipParamGoods",
            [this](std::uint64_t, EquipParamGoods* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                fp_cost_reduction_.goods_rows.push_back(GoodsFpCostState{
                    bytes,
                    ReadOffsetValue<std::int16_t>(bytes, kGoodsConsumeMpOffset),
                });
            });

        ForEachBehaviorParamRowWithFallback(singletons, [this](std::uint64_t, BehaviorParam* row) {
            auto* bytes = reinterpret_cast<std::uint8_t*>(row);
            fp_cost_reduction_.behavior_rows.push_back(BehaviorFpCostState{
                bytes,
                ReadOffsetValue<std::uint8_t>(bytes, kBehaviorHeroPointOffset),
            });
        });

        fp_cost_reduction_.captured = !fp_cost_reduction_.magic_rows.empty() ||
                                      !fp_cost_reduction_.goods_rows.empty() ||
                                      !fp_cost_reduction_.behavior_rows.empty();
        if (!fp_cost_reduction_.captured) {
            return;
        }
    }

    if (desired_percent == fp_cost_reduction_.applied_percent) {
        return;
    }

    const float multiplier = static_cast<float>(100 - desired_percent) / 100.0f;
    const auto scale_int16_cost = [multiplier](std::int16_t original) -> std::int16_t {
        if (original <= 0) {
            return original;
        }
        return static_cast<std::int16_t>(std::max(0, static_cast<int>(std::lround(static_cast<float>(original) * multiplier))));
    };
    const auto scale_uint8_cost = [multiplier](std::uint8_t original) -> std::uint8_t {
        if (original == 0) {
            return original;
        }
        return static_cast<std::uint8_t>(std::clamp(
            static_cast<int>(std::lround(static_cast<float>(original) * multiplier)),
            0,
            255));
    };

    for (const MagicFpCostState& entry : fp_cost_reduction_.magic_rows) {
        WriteOffsetValue<std::int16_t>(entry.row, kMagicMpOffset, scale_int16_cost(entry.mp));
        WriteOffsetValue<std::int16_t>(entry.row, kMagicMpChargeOffset, scale_int16_cost(entry.mp_charge));
    }

    for (const GoodsFpCostState& entry : fp_cost_reduction_.goods_rows) {
        WriteOffsetValue<std::int16_t>(entry.row, kGoodsConsumeMpOffset, scale_int16_cost(entry.consume_mp));
    }

    for (const BehaviorFpCostState& entry : fp_cost_reduction_.behavior_rows) {
        WriteOffsetValue<std::uint8_t>(entry.row, kBehaviorHeroPointOffset, scale_uint8_cost(entry.hero_point));
    }

    fp_cost_reduction_.applied_percent = desired_percent;
}

void ParamPatches::SyncNoFallDeath(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.no_fall_death.load();

    if (!no_fall_death_.captured) {
        auto* primary_row = Game::FindParamRow<SpEffectParam>(
            singletons,
            L"SpEffectParam",
            kNoFallDeathSpEffectRowId);
        auto* secondary_row = Game::FindParamRow<SpEffectParam>(
            singletons,
            L"SpEffectParam",
            kNoFallDeathMiddleSpEffectRowId);
        auto* trigger_row = Game::FindParamRow<SpEffectParam>(
            singletons,
            L"SpEffectParam",
            kNoFallDeathTriggerSpEffectRowId);
        if (primary_row == nullptr || secondary_row == nullptr || trigger_row == nullptr) {
            return;
        }

        auto* primary_bytes = reinterpret_cast<std::uint8_t*>(primary_row);
        auto* secondary_bytes = reinterpret_cast<std::uint8_t*>(secondary_row);
        auto* trigger_bytes = reinterpret_cast<std::uint8_t*>(trigger_row);
        no_fall_death_.primary_row = primary_bytes;
        no_fall_death_.primary_original_cycle_occurrence_sp_effect_id =
            ReadOffsetValue<std::int32_t>(primary_bytes, kNoFallDeathCycleOccurrenceOffset);
        no_fall_death_.primary_original_effect_endurance =
            ReadOffsetValue<float>(primary_bytes, kEffectEnduranceOffset);
        no_fall_death_.primary_original_change_magic_slot =
            ReadOffsetValue<std::int8_t>(primary_bytes, kChangeMagicSlotOffset);
        no_fall_death_.secondary_row = secondary_bytes;
        no_fall_death_.secondary_original_cycle_occurrence_sp_effect_id =
            ReadOffsetValue<std::int32_t>(secondary_bytes, kNoFallDeathCycleOccurrenceOffset);
        no_fall_death_.secondary_original_effect_endurance =
            ReadOffsetValue<float>(secondary_bytes, kEffectEnduranceOffset);
        no_fall_death_.secondary_original_change_magic_slot =
            ReadOffsetValue<std::int8_t>(secondary_bytes, kChangeMagicSlotOffset);
        no_fall_death_.trigger_row = trigger_bytes;
        no_fall_death_.trigger_original_effect_endurance =
            ReadOffsetValue<float>(trigger_bytes, kEffectEnduranceOffset);
        no_fall_death_.trigger_original_change_magic_slot =
            ReadOffsetValue<std::int8_t>(trigger_bytes, kChangeMagicSlotOffset);
        no_fall_death_.captured = true;
    }

    if (no_fall_death_.primary_row == nullptr || no_fall_death_.secondary_row == nullptr || no_fall_death_.trigger_row == nullptr) {
        return;
    }

    if (desired && !no_fall_death_.active) {
        WriteOffsetValue<std::int32_t>(no_fall_death_.primary_row, kNoFallDeathCycleOccurrenceOffset, 185);
        WriteOffsetValue<std::int32_t>(no_fall_death_.secondary_row, kNoFallDeathCycleOccurrenceOffset, 184);
        WriteOffsetValue<std::int8_t>(
            no_fall_death_.primary_row,
            kChangeMagicSlotOffset,
            kAutoAppliedSpEffectChangeMagicSlotValue);
        WriteOffsetValue<std::int8_t>(
            no_fall_death_.secondary_row,
            kChangeMagicSlotOffset,
            kAutoAppliedSpEffectChangeMagicSlotValue);
        WriteOffsetValue<std::int8_t>(
            no_fall_death_.trigger_row,
            kChangeMagicSlotOffset,
            kAutoAppliedSpEffectChangeMagicSlotValue);
        WriteOffsetValue<float>(no_fall_death_.secondary_row, kEffectEnduranceOffset, -1.0f);
        WriteOffsetValue<float>(no_fall_death_.trigger_row, kEffectEnduranceOffset, -1.0f);
        no_fall_death_.active = true;
        Main::Logger::Instance().Info("No fall death SpEffect patch applied.");
    } else if (!desired && no_fall_death_.active) {
        WriteOffsetValue<std::int32_t>(
            no_fall_death_.primary_row,
            kNoFallDeathCycleOccurrenceOffset,
            no_fall_death_.primary_original_cycle_occurrence_sp_effect_id);
        WriteOffsetValue<std::int32_t>(
            no_fall_death_.secondary_row,
            kNoFallDeathCycleOccurrenceOffset,
            no_fall_death_.secondary_original_cycle_occurrence_sp_effect_id);
        WriteOffsetValue<float>(
            no_fall_death_.primary_row,
            kEffectEnduranceOffset,
            no_fall_death_.primary_original_effect_endurance);
        WriteOffsetValue<std::int8_t>(
            no_fall_death_.primary_row,
            kChangeMagicSlotOffset,
            no_fall_death_.primary_original_change_magic_slot);
        WriteOffsetValue<float>(
            no_fall_death_.secondary_row,
            kEffectEnduranceOffset,
            no_fall_death_.secondary_original_effect_endurance);
        WriteOffsetValue<std::int8_t>(
            no_fall_death_.secondary_row,
            kChangeMagicSlotOffset,
            no_fall_death_.secondary_original_change_magic_slot);
        WriteOffsetValue<float>(
            no_fall_death_.trigger_row,
            kEffectEnduranceOffset,
            no_fall_death_.trigger_original_effect_endurance);
        WriteOffsetValue<std::int8_t>(
            no_fall_death_.trigger_row,
            kChangeMagicSlotOffset,
            no_fall_death_.trigger_original_change_magic_slot);
        no_fall_death_.active = false;
        Main::Logger::Instance().Info("No fall death SpEffect patch restored.");
    }
}

void ParamPatches::SyncNoRuneArcLossOnDeath(const Game::SingletonRegistry&) {
    const bool desired = Main::g_FeatureStatus.no_rune_arc_loss_on_death.load();

    if (desired) {
        if (InstallDirectPatch(
                no_rune_arc_loss_patch_,
                FindFirstPatternInText(kNoRuneArcLossPatternCandidates),
                &kNoRuneArcLossPatchByte,
                sizeof(kNoRuneArcLossPatchByte))) {
            return;
        }
        Main::Logger::Instance().Error("No rune arc loss on death patch not found.");
        return;
    }

    RestoreCodePatch(no_rune_arc_loss_patch_);
}

void ParamPatches::SyncNoTimePassOnDeath(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.no_time_pass_on_death.load();

    if (!desired) {
        RestoreCodePatch(no_time_pass_on_death_patch_);
        return;
    }

    const uintptr_t world_area_time_storage = singletons.GetStorage("WorldAreaTimeImpl");
    const uintptr_t game_man_storage = singletons.GetStorage("GameMan");
    if (world_area_time_storage == 0 || game_man_storage == 0) {
        Main::Logger::Instance().Error("No time pass on death: singleton storage not found.");
        return;
    }

    if (!no_time_pass_on_death_patch_.captured &&
        !CapturePatchTarget(
            no_time_pass_on_death_patch_,
            FindFirstPatternInText(kNoTimePassOnDeathPatternCandidates),
            5)) {
        Main::Logger::Instance().Error("No time pass on death hook target not found.");
        return;
    }

    if (no_time_pass_on_death_patch_.active) {
        return;
    }

    no_time_pass_on_death_patch_.cave =
        AllocateNearbyExecutableMemory(no_time_pass_on_death_patch_.target, 0x80);
    if (no_time_pass_on_death_patch_.cave == nullptr) {
        Main::Logger::Instance().Error("No time pass on death cave allocation failed.");
        return;
    }

    auto* cave = reinterpret_cast<std::uint8_t*>(no_time_pass_on_death_patch_.cave);
    constexpr std::array<std::uint8_t, 45> kCaveTemplate{{
        0x4C, 0x8B, 0x74, 0x24, 0x70,
        0x50,
        0x51,
        0x52,
        0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00,
        0x48, 0x8B, 0x15, 0x00, 0x00, 0x00, 0x00,
        0x48, 0x8B, 0x08,
        0x48, 0x89, 0x4A, 0x20,
        0x48, 0x8B, 0x48, 0x08,
        0x48, 0x89, 0x4A, 0x28,
        0x5A,
        0x59,
        0x58,
        0xE9, 0x00, 0x00, 0x00, 0x00,
    }};
    std::memcpy(cave, kCaveTemplate.data(), kCaveTemplate.size());

    cave[28] = static_cast<std::uint8_t>(kGameManStoredTimeOffset);
    cave[35] = static_cast<std::uint8_t>(kGameManStoredTimeOffset + 8);

    std::array<std::uint8_t, kInlinePatchSize> cave_return_jump{};
    if (!WriteRipRelativeDisplacement(cave + 8, reinterpret_cast<uintptr_t>(cave + 8), world_area_time_storage, 7, 3) ||
        !WriteRipRelativeDisplacement(cave + 15, reinterpret_cast<uintptr_t>(cave + 15), game_man_storage, 7, 3) ||
        !MakeRelativeJump(
            cave_return_jump,
            reinterpret_cast<uintptr_t>(cave + 40),
            no_time_pass_on_death_patch_.target + no_time_pass_on_death_patch_.patch_size)) {
        VirtualFree(no_time_pass_on_death_patch_.cave, 0, MEM_RELEASE);
        no_time_pass_on_death_patch_.cave = nullptr;
        Main::Logger::Instance().Error("No time pass on death cave patch failed.");
        return;
    }
    std::memcpy(cave + 40, cave_return_jump.data(), cave_return_jump.size());

    std::array<std::uint8_t, kInlinePatchSize> target_jump{};
    if (!MakeRelativeJump(target_jump, no_time_pass_on_death_patch_.target, reinterpret_cast<uintptr_t>(cave))) {
        VirtualFree(no_time_pass_on_death_patch_.cave, 0, MEM_RELEASE);
        no_time_pass_on_death_patch_.cave = nullptr;
        Main::Logger::Instance().Error("No time pass on death hook jump out of range.");
        return;
    }

    if (!WriteProtectedMemory(
            no_time_pass_on_death_patch_.target,
            target_jump.data(),
            no_time_pass_on_death_patch_.patch_size)) {
        VirtualFree(no_time_pass_on_death_patch_.cave, 0, MEM_RELEASE);
        no_time_pass_on_death_patch_.cave = nullptr;
        Main::Logger::Instance().Error("No time pass on death hook install failed.");
        return;
    }

    no_time_pass_on_death_patch_.active = true;
}

void ParamPatches::SyncAttackLifeStealOnHit(const Game::SingletonRegistry& singletons) {
    const bool desired = Main::g_FeatureStatus.attack_life_steal_on_hit.load();

    if (!attack_life_steal_on_hit_.captured) {
        for (std::size_t index = 0; index < kAttackLifeStealOnHitSpEffectRowIds.size(); ++index) {
            auto* row = Game::FindParamRow<SpEffectParam>(
                singletons,
                L"SpEffectParam",
                kAttackLifeStealOnHitSpEffectRowIds[index]);
            if (row == nullptr) {
                return;
            }

            auto* bytes = reinterpret_cast<std::uint8_t*>(row);
            attack_life_steal_on_hit_.rows[index] = bytes;
            attack_life_steal_on_hit_.original_change_hp_estus_flask_correct_rates[index] =
                ReadOffsetValue<float>(bytes, kAttackLifeStealOnHitCorrectRateOffset);
        }
        attack_life_steal_on_hit_.captured = true;
    }

    if (!attack_life_steal_on_hit_.captured) {
        return;
    }

    if (desired) {
        bool changed = false;
        for (std::size_t index = 0; index < attack_life_steal_on_hit_.rows.size(); ++index) {
            std::uint8_t* row = attack_life_steal_on_hit_.rows[index];
            if (row != nullptr) {
                const float current_correct_rate =
                    ReadOffsetValue<float>(row, kAttackLifeStealOnHitCorrectRateOffset);
                if (current_correct_rate != 1.0f) {
                    WriteOffsetValue<float>(row, kAttackLifeStealOnHitCorrectRateOffset, 1.0f);
                    changed = true;
                }
            }
        }
        if (!attack_life_steal_on_hit_.active || changed) {
            attack_life_steal_on_hit_.active = true;
            Main::Logger::Instance().Info("Attack lifesteal on hit SpEffect patch applied.");
        }
    } else if (!desired && attack_life_steal_on_hit_.active) {
        for (std::size_t index = 0; index < attack_life_steal_on_hit_.rows.size(); ++index) {
            std::uint8_t* row = attack_life_steal_on_hit_.rows[index];
            if (row != nullptr) {
                WriteOffsetValue<float>(
                    row,
                    kAttackLifeStealOnHitCorrectRateOffset,
                    attack_life_steal_on_hit_.original_change_hp_estus_flask_correct_rates[index]);
            }
        }
        attack_life_steal_on_hit_.active = false;
        Main::Logger::Instance().Info("Attack lifesteal on hit SpEffect patch restored.");
    }
}

void ParamPatches::SyncEnemyHpMultiplier(const Game::SingletonRegistry& singletons) {
    const int desired_increase_percent = std::clamp(Main::g_FeatureStatus.enemy_hp_increase_percent.load(), 0, 1000);
    const int desired_decrease_percent = std::clamp(Main::g_FeatureStatus.enemy_hp_decrease_percent.load(), 0, 1000);
    const int desired_signature = desired_increase_percent > 0 ? desired_increase_percent : -desired_decrease_percent;

    if (!enemy_hp_multiplier_.captured && desired_signature != 0) {
        enemy_hp_multiplier_.rows.clear();
        const bool found_rows = Game::ForEachParamRow<NpcParam>(
            singletons,
            L"NpcParam",
            [this](std::uint64_t, NpcParam* row) {
                auto* bytes = reinterpret_cast<std::uint8_t*>(row);
                enemy_hp_multiplier_.rows.push_back(EnemyHpState{
                    bytes,
                    ReadOffsetValue<std::int32_t>(bytes, kEnemyHpOffset),
                });
            });

        if (!found_rows || enemy_hp_multiplier_.rows.empty()) {
            return;
        }
        enemy_hp_multiplier_.captured = true;
    }

    if (!enemy_hp_multiplier_.captured || desired_signature == enemy_hp_multiplier_.applied_percent) {
        return;
    }

    if (desired_signature == 0) {
        for (const EnemyHpState& state : enemy_hp_multiplier_.rows) {
            WriteOffsetValue<std::int32_t>(state.row, kEnemyHpOffset, state.hp);
        }
        enemy_hp_multiplier_.applied_percent = 0;
        Main::Logger::Instance().Info("Enemy HP multiplier restored.");
        return;
    }

    const float rate = desired_signature > 0
                           ? (1.0f + static_cast<float>(desired_increase_percent) / 100.0f)
                           : (1.0f / (1.0f + static_cast<float>(desired_decrease_percent) / 100.0f));
    for (const EnemyHpState& state : enemy_hp_multiplier_.rows) {
        if (state.hp <= 0) {
            continue;
        }
        const int scaled = static_cast<int>(std::lround(static_cast<float>(state.hp) * rate));
        WriteOffsetValue<std::int32_t>(state.row, kEnemyHpOffset, std::max(1, scaled));
    }
    enemy_hp_multiplier_.applied_percent = desired_signature;
    Main::Logger::Instance().Info(
        ("Enemy HP multiplier applied: value=" + std::to_string(desired_signature) +
         ", rate=" + std::to_string(rate)).c_str());
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
