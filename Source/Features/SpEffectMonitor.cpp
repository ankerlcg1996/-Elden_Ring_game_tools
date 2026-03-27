#include "SpEffectMonitor.hpp"

#include "../Game/Memory.hpp"
#include "../Main/Logger.hpp"

#include <unordered_set>

namespace ERD::Features {
namespace {

constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kChrInsSpecialEffectOffset = 0x178;
constexpr uintptr_t kSpecialEffectHeadOffset = 0x8;

constexpr uintptr_t kSpEffectEntryParamDataOffset = 0x0;
constexpr uintptr_t kSpEffectEntryIdOffset = 0x8;
constexpr uintptr_t kSpEffectEntryNextOffset = 0x30;
constexpr uintptr_t kSpEffectEntryTimeLeftOffset = 0x40;
constexpr uintptr_t kSpEffectEntryDurationOffset = 0x48;
constexpr uintptr_t kSpEffectParamStateInfoOffset = 0x156;

constexpr int kMaxSpEffectEntries = 1024;
constexpr std::array<const char*, 1> kSetSpEffectPatternCandidates{{
    "85 D2 78 09 48 8B",
}};
constexpr std::array<const char*, 1> kFindAndRemoveSpEffectPatternCandidates{{
    "0F B6 D0 42",
}};

struct Region {
    uintptr_t base = 0;
    std::size_t size = 0;
};

struct Pattern {
    std::vector<std::uint8_t> bytes;
    std::vector<std::uint8_t> masks;
};

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
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<uintptr_t>(module) + dos->e_lfanew);
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

uintptr_t ResolveRel32Target(uintptr_t instruction_address, std::size_t rel32_offset, std::size_t instruction_length) {
    if (instruction_address == 0) {
        return 0;
    }

    std::int32_t rel32 = 0;
    if (!Game::ReadValue(instruction_address + rel32_offset, rel32)) {
        return 0;
    }

    const auto next_ip = static_cast<std::int64_t>(instruction_address + instruction_length);
    const auto target = next_ip + static_cast<std::int64_t>(rel32);
    return target > 0 ? static_cast<uintptr_t>(target) : 0;
}

}  // namespace

SpEffectMonitor* SpEffectMonitor::instance_ = nullptr;

void SpEffectMonitor::Tick(const Game::SingletonRegistry& singletons) {
    if (instance_ == nullptr) {
        instance_ = this;
    }

    const uintptr_t player_base = GetPlayerBase(singletons);
    if (player_base == 0) {
        Clear();
        return;
    }

    RefreshSnapshot(player_base);
}

void SpEffectMonitor::Clear() {
    std::scoped_lock lock(snapshot_mutex_);
    snapshot_ = {};
}

SpEffectMonitorSnapshot SpEffectMonitor::Snapshot() const {
    std::scoped_lock lock(snapshot_mutex_);
    return snapshot_;
}

bool SpEffectMonitor::ApplySpEffect(uintptr_t chr_ins, std::uint32_t sp_effect_id) {
    if (chr_ins == 0 || sp_effect_id == 0 || !EnsureFunctionAddresses()) {
        return false;
    }

    using SetSpEffectFn = void (*)(uintptr_t, std::uint32_t);
    auto fn = reinterpret_cast<SetSpEffectFn>(set_sp_effect_fn_);
    fn(chr_ins, sp_effect_id);
    return true;
}

bool SpEffectMonitor::RemoveSpEffect(uintptr_t chr_ins, std::uint32_t sp_effect_id) {
    if (chr_ins == 0 || sp_effect_id == 0 || !EnsureFunctionAddresses()) {
        return false;
    }

    uintptr_t special_effect = 0;
    if (!Game::ReadValue(chr_ins + kChrInsSpecialEffectOffset, special_effect) || special_effect == 0) {
        return false;
    }

    using RemoveSpEffectFn = void (*)(uintptr_t, std::uint32_t);
    auto fn = reinterpret_cast<RemoveSpEffectFn>(find_and_remove_sp_effect_fn_);
    fn(special_effect, sp_effect_id);
    return true;
}

SpEffectMonitor* SpEffectMonitor::Instance() {
    return instance_;
}

bool SpEffectMonitor::EnsureFunctionAddresses() {
    if (function_addresses_initialized_) {
        return set_sp_effect_fn_ != 0 && find_and_remove_sp_effect_fn_ != 0;
    }

    const uintptr_t set_match = FindFirstPatternInText(kSetSpEffectPatternCandidates);
    const uintptr_t remove_match = FindFirstPatternInText(kFindAndRemoveSpEffectPatternCandidates);
    set_sp_effect_fn_ = ResolveRel32Target(set_match + 0x8, 1, 5);
    find_and_remove_sp_effect_fn_ = ResolveRel32Target(remove_match + 0xC, 1, 5);
    function_addresses_initialized_ = true;

    if (set_sp_effect_fn_ == 0 || find_and_remove_sp_effect_fn_ == 0) {
        Main::Logger::Instance().Error("SpEffect function addresses not resolved.");
    } else {
        Main::Logger::Instance().Info("SpEffect function addresses resolved.");
    }

    return set_sp_effect_fn_ != 0 && find_and_remove_sp_effect_fn_ != 0;
}

uintptr_t SpEffectMonitor::GetPlayerBase(const Game::SingletonRegistry& singletons) const {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    uintptr_t slots_root = 0;
    uintptr_t player_base = 0;
    return (world_chr_man != 0 &&
            Game::ReadValue(world_chr_man + kNetPlayersOffset, slots_root) &&
            slots_root != 0 &&
            Game::ReadValue(slots_root + 0x0, player_base))
               ? player_base
               : 0;
}

void SpEffectMonitor::RefreshSnapshot(uintptr_t player_base) {
    SpEffectMonitorSnapshot next{};
    next.valid = true;
    next.player_base = player_base;

    uintptr_t special_effect = 0;
    if (!Game::ReadValue(player_base + kChrInsSpecialEffectOffset, special_effect) || special_effect == 0) {
        std::scoped_lock lock(snapshot_mutex_);
        snapshot_ = std::move(next);
        return;
    }

    uintptr_t current = 0;
    if (!Game::ReadValue(special_effect + kSpecialEffectHeadOffset, current)) {
        std::scoped_lock lock(snapshot_mutex_);
        snapshot_ = std::move(next);
        return;
    }

    std::unordered_set<uintptr_t> visited;
    visited.reserve(128);
    for (int i = 0; i < kMaxSpEffectEntries && current != 0; ++i) {
        if (!visited.insert(current).second) {
            break;
        }

        uintptr_t param_data = 0;
        std::int32_t effect_id = 0;
        float time_left = 0.0f;
        float duration = 0.0f;
        uintptr_t next_ptr = 0;
        std::uint16_t state_info = 0;

        Game::ReadValue(current + kSpEffectEntryParamDataOffset, param_data);
        Game::ReadValue(current + kSpEffectEntryIdOffset, effect_id);
        Game::ReadValue(current + kSpEffectEntryTimeLeftOffset, time_left);
        Game::ReadValue(current + kSpEffectEntryDurationOffset, duration);
        Game::ReadValue(current + kSpEffectEntryNextOffset, next_ptr);
        if (param_data != 0) {
            Game::ReadValue(param_data + kSpEffectParamStateInfoOffset, state_info);
        }

        next.active_effects.push_back(SpEffectEntrySnapshot{
            effect_id,
            time_left,
            duration,
            state_info,
        });

        current = next_ptr;
    }

    std::scoped_lock lock(snapshot_mutex_);
    snapshot_ = std::move(next);
}

SpEffectMonitorSnapshot GetSpEffectMonitorSnapshot() {
    SpEffectMonitor* monitor = SpEffectMonitor::Instance();
    return monitor == nullptr ? SpEffectMonitorSnapshot{} : monitor->Snapshot();
}

}  // namespace ERD::Features
