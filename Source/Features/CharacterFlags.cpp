#include "CharacterFlags.hpp"

#include "../Game/Memory.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"

#include <string>
#include <vector>

namespace ERD::Features {
namespace {

constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kCharacterFlagsBlockOffset = 0x190;
constexpr uintptr_t kCommonFlagsOffset = 0x19B;
constexpr uintptr_t kChrModulesOffset = 0x190;
constexpr uintptr_t kChrRideModuleOffset = 0xE8;
constexpr uintptr_t kRideNodeOffset = 0x10;
constexpr uintptr_t kRideNodeHorseHandleOffset = 0x18;
constexpr uintptr_t kChrSetEntriesOffset = 0x18;
constexpr uintptr_t kChrSetEntryStride = 0x10;
constexpr uintptr_t kChrDbgFlagsSilentOffset = 0x9;
constexpr const char* kChrDbgFlagsPattern = "80 3D ? ? ? ? 00 0F 85 6C";
constexpr std::array<uintptr_t, 2> kChrSetPoolOffsets{0x1DED8, 0x18038};

struct Region {
    uintptr_t base = 0;
    std::size_t size = 0;
};

struct Pattern {
    std::vector<std::uint8_t> bytes;
    std::vector<std::uint8_t> masks;
};

uintptr_t g_chr_dbg_flags_base = 0;
bool g_chr_dbg_flags_scanned = false;
bool g_chr_dbg_flags_log_once = false;

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

    const auto* begin = reinterpret_cast<const std::uint8_t*>(text.base);
    const auto* end = begin + text.size - pattern.bytes.size();
    for (const auto* cursor = begin; cursor <= end; ++cursor) {
        if (MatchesPattern(cursor, pattern)) {
            return reinterpret_cast<uintptr_t>(cursor);
        }
    }
    return 0;
}

bool ResolveChrDbgFlagsBase(uintptr_t& out_base) {
    if (!g_chr_dbg_flags_scanned) {
        g_chr_dbg_flags_scanned = true;
        const uintptr_t pattern_match = FindPatternInText(kChrDbgFlagsPattern);
        if (pattern_match != 0) {
            std::int32_t rip_rel = 0;
            if (Game::ReadValue(pattern_match + 2, rip_rel)) {
                g_chr_dbg_flags_base = pattern_match + 7 + static_cast<std::int64_t>(rip_rel);
            }
        }
    }

    if (g_chr_dbg_flags_base == 0) {
        return false;
    }

    std::uint8_t probe = 0;
    if (!Game::ReadValue(g_chr_dbg_flags_base + kChrDbgFlagsSilentOffset, probe)) {
        return false;
    }

    out_base = g_chr_dbg_flags_base;
    return true;
}

uintptr_t ResolveChrInsByHandle(uintptr_t world_chr_man, std::uint32_t handle) {
    const std::uint32_t pool_index = (handle >> 20) & 0xFFu;
    const std::uint32_t slot_index = handle & 0xFFFFFu;

    for (const uintptr_t chr_set_pool_offset : kChrSetPoolOffsets) {
        uintptr_t chr_set = 0;
        if (!Game::ReadValue(world_chr_man + chr_set_pool_offset + static_cast<uintptr_t>(pool_index) * sizeof(uintptr_t), chr_set) ||
            chr_set == 0) {
            continue;
        }

        uintptr_t entries_base = 0;
        if (!Game::ReadValue(chr_set + kChrSetEntriesOffset, entries_base) || entries_base == 0) {
            continue;
        }

        uintptr_t chr_ins = 0;
        if (!Game::ReadValue(entries_base + static_cast<uintptr_t>(slot_index) * kChrSetEntryStride, chr_ins) ||
            chr_ins == 0) {
            continue;
        }

        std::uint8_t flags = 0;
        if (Game::ReadValue(chr_ins + kCommonFlagsOffset, flags)) {
            return chr_ins;
        }
    }

    return 0;
}

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

}  // namespace

void CharacterFlags::Tick(const Game::SingletonRegistry& singletons) const {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    if (world_chr_man == 0) {
        return;
    }

    uintptr_t player_slots_root = 0;
    uintptr_t player_chr = 0;
    if (Game::ReadValue(world_chr_man + kNetPlayersOffset, player_slots_root) && player_slots_root != 0) {
        Game::ReadValue(player_slots_root + 0x0, player_chr);
    }

    uintptr_t common_flags_address = 0;
    ERD_PROTECTED_STEP(
        "CharacterFlags.CommonFlags",
        if (Game::ResolvePointerChain(
                world_chr_man,
                {kNetPlayersOffset, 0x0, kCharacterFlagsBlockOffset, 0x0},
                kCommonFlagsOffset,
                common_flags_address)) {
            Game::SetBitFlag(common_flags_address, 0, Main::g_FeatureStatus.no_dead.load());
            Game::SetBitFlag(common_flags_address, 2, Main::g_FeatureStatus.infinite_fp.load());
        }
    );

    ERD_PROTECTED_STEP(
        "CharacterFlags.Torrent",
        if (player_chr != 0 &&
            (Main::g_FeatureStatus.torrent_no_death.load() || Main::g_FeatureStatus.torrent_anywhere.load())) {
            uintptr_t modules = 0;
            if (Game::ReadValue(player_chr + kChrModulesOffset, modules) && modules != 0) {
                uintptr_t chr_ride = 0;
                if (Game::ReadValue(modules + kChrRideModuleOffset, chr_ride) && chr_ride != 0) {
                    if (Main::g_FeatureStatus.torrent_anywhere.load()) {
                        Game::WriteValue<std::uint8_t>(chr_ride + 0x164, 0);
                    }

                    if (Main::g_FeatureStatus.torrent_no_death.load()) {
                        uintptr_t ride_node = 0;
                        if (Game::ReadValue(chr_ride + kRideNodeOffset, ride_node) && ride_node != 0) {
                            std::uint32_t horse_handle = 0;
                            if (Game::ReadValue(ride_node + kRideNodeHorseHandleOffset, horse_handle) &&
                                horse_handle != 0) {
                                const uintptr_t horse_chr = ResolveChrInsByHandle(world_chr_man, horse_handle);
                                if (horse_chr != 0) {
                                    uintptr_t horse_modules = 0;
                                    if (Game::ReadValue(horse_chr + kChrModulesOffset, horse_modules) &&
                                        horse_modules != 0) {
                                        uintptr_t horse_chr_data = 0;
                                        if (Game::ReadValue(horse_modules + 0x0, horse_chr_data) &&
                                            horse_chr_data != 0) {
                                            Game::SetBitFlag(horse_chr_data + kCommonFlagsOffset, 0, true);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    );

    ERD_PROTECTED_STEP(
        "CharacterFlags.SilentFootsteps",
        if (player_chr != 0) {
            uintptr_t chr_dbg_flags_base = 0;
            if (ResolveChrDbgFlagsBase(chr_dbg_flags_base)) {
                const std::uint8_t desired =
                    Main::g_FeatureStatus.silent_footsteps.load() ? static_cast<std::uint8_t>(1) : static_cast<std::uint8_t>(0);
                Game::WriteValue<std::uint8_t>(chr_dbg_flags_base + kChrDbgFlagsSilentOffset, desired);
            } else if (!g_chr_dbg_flags_log_once) {
                g_chr_dbg_flags_log_once = true;
                Main::Logger::Instance().Error("Silent footsteps: ChrDbgFlags base not found.");
            }
        }
    );
}

}  // namespace ERD::Features
