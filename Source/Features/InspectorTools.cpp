#include "InspectorTools.hpp"

#include "../Game/Memory.hpp"
#include "../Game/Params.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"

#include <charconv>
#include <cstdlib>
#include <fstream>
#include <unordered_set>

namespace ERD::Features {
namespace {

constexpr uintptr_t kPlayerGameDataOffset = 0x8;
constexpr uintptr_t kEquipInventoryDataOffset = 0x5D0;
constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kPlayerLevelOffset = 0x68;
constexpr uintptr_t kCharacterNameOffset = 0x9C;
constexpr uintptr_t kPlayerCurrentWeaponLeftOffset = 0x328;
constexpr uintptr_t kPlayerCurrentWeaponRightOffset = 0x32C;
constexpr uintptr_t kQuickItemBaseOffset = 0x650;
constexpr uintptr_t kPouchBaseOffset = 0x678;
constexpr uintptr_t kCurrentHpOffset = 0x10;
constexpr uintptr_t kCurrentMaxHpOffset = 0x14;
constexpr uintptr_t kCurrentLevelOffset = 0x68;
constexpr uintptr_t kCurrentCharacterTypeOffset = 0x68;
constexpr uintptr_t kCurrentTeamTypeOffset = 0x6C;
constexpr uintptr_t kCurrentReinforceLevelOffset = 0x70;
constexpr uintptr_t kCurrentNameRootOffset = 0x580;
constexpr uintptr_t kSaveSlotOffset = 0xAC0;
constexpr uintptr_t kPendingLoadSaveSlotOffset = 0xB78;
constexpr uintptr_t kLastGraceOffset = 0xB60;
constexpr uintptr_t kSaveRequestOffset = 0xB72;
constexpr uintptr_t kPartyInfoRootOffset = 0xD60;
constexpr uintptr_t kDisableAutoSaveOffset = 0x13C;
constexpr uintptr_t kCharacterClearCountOffset = 0x120;
constexpr uintptr_t kRuneArcOffset = 0xFF;
constexpr uintptr_t kScadutreeBlessingOffset = 0xFC;
constexpr uintptr_t kReveredSpiritAshBlessingOffset = 0xFD;
constexpr uintptr_t kGreatRuneRootOffset = 0x2B0 + 0x288 + 0x88;
constexpr uintptr_t kPlayerResourceBlockOffset = 0x190;
constexpr uintptr_t kPlayerCurrentHpOffset = 0x138;
constexpr uintptr_t kPlayerMaxHpOffset = 0x13C;
constexpr uintptr_t kPlayerCurrentFpOffset = 0x148;
constexpr uintptr_t kPlayerMaxFpOffset = 0x150;
constexpr uintptr_t kPlayerCurrentSpOffset = 0x154;
constexpr uintptr_t kPlayerMaxSpOffset = 0x158;
constexpr uintptr_t kPartySlotStride = 0x30;
constexpr uintptr_t kPartySlotBaseOffset = 0x28;
constexpr int kPartySlotCount = 6;
constexpr int kSessionNetPlayerCount = 4;

struct AobAbsoluteCandidate {
    const char* pattern = nullptr;
    std::ptrdiff_t offset_adjust = 0;
};

struct AobRelCallCandidate {
    const char* pattern = nullptr;
    std::size_t rel32_offset = 0;
    std::size_t instruction_length = 0;
};

constexpr std::array<AobAbsoluteCandidate, 1> kGetQuantityAbsoluteCandidates{{
    // 旧版 CE 风格签名：命中点在函数体内部，需回退到函数起始。
    {"?? 8B F9 ?? 8D 44 ?? 48 ?? 89 44 ?? ?? 8B 01", -0x14},
}};

constexpr std::array<AobRelCallCandidate, 1> kGetQuantityRelCallCandidates{{
    // 来自 AOB 特征表：GetPlayerItemQuantityById（call rel32）。
    {"E8 ? ? ? ? 3B C6 7D 07", 1, 5},
}};

constexpr std::array<AobAbsoluteCandidate, 2> kEzStateDispatcherCandidates{{
    // 现有实现特征。
    {"48 8B DA 4C 8B F1 33 FF 89 7D", -0x47},
    // 来自 AOB 特征表：ExecuteTalkCommand。
    {"89 7D 80 48 8B 02 48 8B CA", -0x4F},
}};

enum class ParamValueType {
    S8 = 0,
    U8 = 1,
    S16 = 2,
    U16 = 3,
    S32 = 4,
    U32 = 5,
    F32 = 6,
};

struct Region {
    uintptr_t base = 0;
    std::size_t size = 0;
};

struct Pattern {
    std::vector<std::uint8_t> bytes;
    std::vector<std::uint8_t> masks;
};

struct EzStateFuncArg {
    union {
        float as_float;
        std::int32_t as_int;
        std::uint64_t as_u64;
    } value{};
    std::int32_t type = 0;
    std::int32_t padding = 0;
};

struct UnkTalkEventField08 {
    std::byte reserved[0x30]{};
    std::uint64_t chr_ins_handle = 0;
};

struct MenuJob {
    std::uint32_t reserved[4]{};
};

struct CSNpcTalkIns;

struct CSTalkInsActiveMenuJob {
    MenuJob menu_job{};
    std::uint32_t maybe_job_id = 0;
    CSNpcTalkIns* parent_talk_ins = nullptr;
};

struct CSNpcTalkIns {
    std::byte reserved0[0x0C]{};
    std::uint32_t field1 = 0;
    std::uint32_t field2 = 0;
    std::byte reserved1[0x40 - 0x14]{};
    std::uint64_t chr_ins_handle = 0;
    std::byte reserved2[0x98 - 0x48]{};
    CSTalkInsActiveMenuJob* active_menu_job = nullptr;
};

struct EzStateTalkEvent {
    void** vtable = nullptr;
    UnkTalkEventField08* unk08 = nullptr;
    CSNpcTalkIns* npc_talk_ins = nullptr;
    std::uint32_t popup_menu_related = 0;
};

struct FakeEzStateExternalEvent;

using EzStateTalkDispatchFn = void (*)(EzStateTalkEvent*, void*);
using FakeExtEventDtorFn = void (*)(FakeEzStateExternalEvent*);
using FakeExtEventUnkFn = void (*)(FakeEzStateExternalEvent*);
using FakeExtEventEventIdFn = int (*)(FakeEzStateExternalEvent*);
using FakeExtEventArgCountFn = int (*)(FakeEzStateExternalEvent*);
using FakeExtEventArgAtFn = EzStateFuncArg* (*)(FakeEzStateExternalEvent*, int);

struct FakeEzStateExternalEventVtable {
    FakeExtEventDtorFn dtor = nullptr;
    FakeExtEventUnkFn unk08 = nullptr;
    FakeExtEventEventIdFn event_id = nullptr;
    FakeExtEventArgCountFn arg_count = nullptr;
    FakeExtEventArgAtFn arg_at = nullptr;
};

struct FakeEzStateExternalEvent {
    FakeEzStateExternalEventVtable* vtable = nullptr;
    EzStateFuncArg* args = nullptr;
    int arg_count = 0;
};

EzStateTalkDispatchFn g_EzStateDispatch = nullptr;

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

uintptr_t ResolveRel32Target(uintptr_t instruction_address,
                             std::size_t rel32_offset,
                             std::size_t instruction_length) {
    if (instruction_address == 0) {
        return 0;
    }

    std::int32_t rel32 = 0;
    if (!Game::ReadValue(instruction_address + rel32_offset, rel32)) {
        return 0;
    }

    const uintptr_t next_ip = instruction_address + instruction_length;
    const auto signed_next_ip = static_cast<std::int64_t>(next_ip);
    const auto target_signed = signed_next_ip + static_cast<std::int64_t>(rel32);
    if (target_signed <= 0) {
        return 0;
    }

    return static_cast<uintptr_t>(target_signed);
}

template <std::size_t N>
uintptr_t FindByAbsoluteCandidates(const std::array<AobAbsoluteCandidate, N>& candidates) {
    for (const AobAbsoluteCandidate& candidate : candidates) {
        if (candidate.pattern == nullptr) {
            continue;
        }

        const uintptr_t match = FindPatternInText(candidate.pattern);
        if (match == 0) {
            continue;
        }

        const auto adjusted_signed =
            static_cast<std::int64_t>(match) + static_cast<std::int64_t>(candidate.offset_adjust);
        if (adjusted_signed <= 0) {
            continue;
        }

        return static_cast<uintptr_t>(adjusted_signed);
    }

    return 0;
}

template <std::size_t N>
uintptr_t FindByRelCallCandidates(const std::array<AobRelCallCandidate, N>& candidates) {
    for (const AobRelCallCandidate& candidate : candidates) {
        if (candidate.pattern == nullptr) {
            continue;
        }

        const uintptr_t match = FindPatternInText(candidate.pattern);
        if (match == 0) {
            continue;
        }

        const uintptr_t target =
            ResolveRel32Target(match, candidate.rel32_offset, candidate.instruction_length);
        if (target != 0) {
            return target;
        }
    }

    return 0;
}

std::string TrimCopy(std::string value) {
    auto not_space = [](unsigned char ch) { return !std::isspace(ch); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

std::wstring Utf8ToWide(const std::string& value) {
    if (value.empty()) {
        return {};
    }

    const int required = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0);
    if (required <= 0) {
        return {};
    }

    std::wstring wide(static_cast<std::size_t>(required), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), wide.data(), required);
    return wide;
}

std::string WideToUtf8(const std::wstring& value) {
    if (value.empty()) {
        return {};
    }

    const int required = WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    if (required <= 0) {
        return {};
    }

    std::string utf8(static_cast<std::size_t>(required), '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), utf8.data(), required, nullptr, nullptr);
    return utf8;
}

bool ReadUnicodeString(uintptr_t address, std::size_t max_chars, std::string& output) {
    std::wstring buffer(max_chars, L'\0');
    if (!Game::ReadMemory(address, buffer.data(), max_chars * sizeof(wchar_t))) {
        return false;
    }

    const std::size_t zero = buffer.find(L'\0');
    if (zero != std::wstring::npos) {
        buffer.resize(zero);
    }

    output = WideToUtf8(buffer);
    return true;
}

uintptr_t GetPlayerGameData(const Game::SingletonRegistry& singletons) {
    const uintptr_t game_data_man = singletons.GetObjectPointer("GameDataMan");
    if (game_data_man == 0) {
        return 0;
    }

    uintptr_t player_game_data = 0;
    return Game::ReadValue(game_data_man + kPlayerGameDataOffset, player_game_data) ? player_game_data : 0;
}

uintptr_t GetPlayerBase(const Game::SingletonRegistry& singletons) {
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

uintptr_t GetResourceBlock(const Game::SingletonRegistry& singletons) {
    uintptr_t address = 0;
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    return (world_chr_man != 0 &&
            Game::ResolvePointerChain(world_chr_man, {kNetPlayersOffset, 0x0, kPlayerResourceBlockOffset}, 0x0, address))
               ? address
               : 0;
}

int GetPartyInfoValue(const Game::SingletonRegistry& singletons, uintptr_t offset) {
    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    uintptr_t party_root = 0;
    int value = 0;
    if (game_man == 0 ||
        !Game::ReadValue(game_man + kPartyInfoRootOffset, party_root) ||
        party_root == 0 ||
        !Game::ReadValue(party_root + offset, value)) {
        return 0;
    }
    return value;
}

bool ReadInventoryList(const Game::SingletonRegistry& singletons,
                       int inventory_kind,
                       uintptr_t& list_address,
                       int& count,
                       std::size_t& slot_limit) {
    list_address = 0;
    count = 0;
    slot_limit = 0;

    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return false;
    }

    uintptr_t inventory_data = 0;
    uintptr_t inventory_offset = kEquipInventoryDataOffset;
    uintptr_t key_offset = 0;
    if (inventory_kind == 1) {
        inventory_offset = 0x8D0;
        slot_limit = 1920;
    } else if (inventory_kind == 2) {
        inventory_offset = kEquipInventoryDataOffset;
        key_offset = 0x10;
        slot_limit = 384;
    } else if (inventory_kind == 3) {
        inventory_offset = 0x8D0;
        key_offset = 0x10;
        slot_limit = 128;
    } else {
        slot_limit = 2688;
    }

    return Game::ReadValue(player_game_data + inventory_offset, inventory_data) &&
           inventory_data != 0 &&
           Game::ReadValue(inventory_data + 0x10 + key_offset, list_address) &&
           list_address != 0 &&
           Game::ReadValue(inventory_data + 0x18 + key_offset, count);
}

using GetQuantityFn = int (*)(uintptr_t item_id_memory);

uintptr_t FindGetQuantityFunction() {
    static uintptr_t address = 0;
    if (address == 0) {
        address = FindByAbsoluteCandidates(kGetQuantityAbsoluteCandidates);
    }
    if (address == 0) {
        address = FindByRelCallCandidates(kGetQuantityRelCallCandidates);
    }
    return address;
}

int QueryHeldQuantity(std::int32_t item_id) {
    const uintptr_t function = FindGetQuantityFunction();
    if (function == 0) {
        return 0;
    }

    alignas(16) std::array<std::uint8_t, 0x100> scratch{};
    Game::WriteValue(reinterpret_cast<uintptr_t>(scratch.data()), item_id);

    int quantity = 0;
    __try {
        quantity = reinterpret_cast<GetQuantityFn>(function)(reinterpret_cast<uintptr_t>(scratch.data()));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        quantity = 0;
    }
    return std::max(quantity, 0);
}

void AppendInventoryEntries(const Game::SingletonRegistry& singletons,
                            int inventory_kind,
                            bool repository,
                            std::unordered_set<std::uint64_t>& seen,
                            std::vector<InventoryEntrySnapshot>& weapons,
                            std::vector<InventoryEntrySnapshot>& protectors,
                            std::vector<InventoryEntrySnapshot>& accessories,
                            std::vector<InventoryEntrySnapshot>& goods) {
    uintptr_t list_address = 0;
    int count = 0;
    std::size_t slot_limit = 0;
    if (!ReadInventoryList(singletons, inventory_kind, list_address, count, slot_limit)) {
        return;
    }

    int populated = 0;
    for (std::size_t index = 0; index <= slot_limit; ++index) {
        std::int32_t item_id = -1;
        if (!Game::ReadValue(list_address + index * 0x18 + 0x4, item_id)) {
            break;
        }
        if (item_id == -1) {
            continue;
        }

        ++populated;
        const std::uint64_t dedupe_key =
            (static_cast<std::uint64_t>(static_cast<std::uint32_t>(item_id)) << 1) | (repository ? 1ull : 0ull);
        if (!seen.insert(dedupe_key).second) {
            if (populated >= count) {
                break;
            }
            continue;
        }

        int quantity = 0;
        if (!Game::ReadValue(list_address + index * 0x18 + 0x8, quantity) || quantity < 0 || quantity > 9999) {
            quantity = QueryHeldQuantity(item_id);
        }
        if (quantity <= 0) {
            quantity = 1;
        }

        InventoryEntrySnapshot entry{item_id, quantity, repository};
        const std::uint32_t prefix = static_cast<std::uint32_t>(item_id) & 0xF0000000u;
        if (prefix == 0x00000000u) {
            weapons.push_back(entry);
        } else if (prefix == 0x10000000u) {
            protectors.push_back(entry);
        } else if (prefix == 0x20000000u) {
            accessories.push_back(entry);
        } else {
            goods.push_back(entry);
        }

        if (populated >= count) {
            break;
        }
    }
}

bool ResolveParamTableAndRow(const Game::SingletonRegistry& singletons,
                             const std::wstring& param_name,
                             std::uint64_t row_id,
                             uintptr_t& row_address,
                             int& row_count) {
    row_address = 0;
    row_count = 0;

    Game::ParamTable* table = Game::FindParamTable<std::byte>(singletons, param_name.c_str());
    if (table == nullptr) {
        return false;
    }

    row_count = table->num_rows;
    for (int row_index = 0; row_index < table->num_rows; ++row_index) {
        if (Game::detail::GetRowId(table, row_index) == row_id) {
            row_address = reinterpret_cast<uintptr_t>(Game::detail::GetRowData(table, row_index));
            return row_address != 0;
        }
    }
    return false;
}

bool ReadParamValuePreview(uintptr_t row_address,
                           std::size_t field_offset,
                           ParamValueType type,
                           std::string& value_text) {
    if (row_address == 0) {
        return false;
    }

    const uintptr_t address = row_address + field_offset;
    switch (type) {
    case ParamValueType::S8: {
        std::int8_t value = 0;
        if (!Game::ReadValue(address, value)) return false;
        value_text = std::to_string(static_cast<int>(value));
        return true;
    }
    case ParamValueType::U8: {
        std::uint8_t value = 0;
        if (!Game::ReadValue(address, value)) return false;
        value_text = std::to_string(static_cast<unsigned>(value));
        return true;
    }
    case ParamValueType::S16: {
        std::int16_t value = 0;
        if (!Game::ReadValue(address, value)) return false;
        value_text = std::to_string(static_cast<int>(value));
        return true;
    }
    case ParamValueType::U16: {
        std::uint16_t value = 0;
        if (!Game::ReadValue(address, value)) return false;
        value_text = std::to_string(static_cast<unsigned>(value));
        return true;
    }
    case ParamValueType::S32: {
        std::int32_t value = 0;
        if (!Game::ReadValue(address, value)) return false;
        value_text = std::to_string(value);
        return true;
    }
    case ParamValueType::U32: {
        std::uint32_t value = 0;
        if (!Game::ReadValue(address, value)) return false;
        value_text = std::to_string(value);
        return true;
    }
    case ParamValueType::F32: {
        float value = 0.0f;
        if (!Game::ReadValue(address, value)) return false;
        char buffer[64]{};
        std::snprintf(buffer, sizeof(buffer), "%.4f", value);
        value_text = buffer;
        return true;
    }
    }

    return false;
}

bool WriteParamValue(uintptr_t row_address,
                     std::size_t field_offset,
                     ParamValueType type,
                     int int_value,
                     float float_value) {
    if (row_address == 0) {
        return false;
    }

    const uintptr_t address = row_address + field_offset;
    switch (type) {
    case ParamValueType::S8: {
        const auto value = static_cast<std::int8_t>(std::clamp(int_value, -128, 127));
        return Game::WriteValue(address, value);
    }
    case ParamValueType::U8: {
        const auto value = static_cast<std::uint8_t>(std::clamp(int_value, 0, 255));
        return Game::WriteValue(address, value);
    }
    case ParamValueType::S16: {
        const auto value = static_cast<std::int16_t>(std::clamp(int_value, -32768, 32767));
        return Game::WriteValue(address, value);
    }
    case ParamValueType::U16: {
        const auto value = static_cast<std::uint16_t>(std::clamp(int_value, 0, 65535));
        return Game::WriteValue(address, value);
    }
    case ParamValueType::S32: {
        const auto value = static_cast<std::int32_t>(int_value);
        return Game::WriteValue(address, value);
    }
    case ParamValueType::U32: {
        const auto value = static_cast<std::uint32_t>(std::max(int_value, 0));
        return Game::WriteValue(address, value);
    }
    case ParamValueType::F32:
        return Game::WriteValue(address, float_value);
    }

    return false;
}

ParamValueType ParseParamValueType(const std::string& token) {
    std::string lower = token;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });

    if (lower == "s8") return ParamValueType::S8;
    if (lower == "u8") return ParamValueType::U8;
    if (lower == "s16") return ParamValueType::S16;
    if (lower == "u16") return ParamValueType::U16;
    if (lower == "u32") return ParamValueType::U32;
    if (lower == "f32" || lower == "float") return ParamValueType::F32;
    return ParamValueType::S32;
}

const char* ParamValueTypeLabel(ParamValueType type) {
    switch (type) {
    case ParamValueType::S8: return "s8";
    case ParamValueType::U8: return "u8";
    case ParamValueType::S16: return "s16";
    case ParamValueType::U16: return "u16";
    case ParamValueType::S32: return "s32";
    case ParamValueType::U32: return "u32";
    case ParamValueType::F32: return "f32";
    }
    return "unknown";
}

void FakeExtEventStub(FakeEzStateExternalEvent*) {}

int FakeExtEventArgCount(FakeEzStateExternalEvent* event) {
    return event->arg_count;
}

int FakeExtEventId(FakeEzStateExternalEvent* event) {
    return event->arg_count > 0 ? event->args[0].value.as_int : 0;
}

EzStateFuncArg* FakeExtEventArgAt(FakeEzStateExternalEvent* event, int index) {
    return event->args + index;
}

FakeEzStateExternalEventVtable g_FakeExtEventVtable{
    &FakeExtEventStub,
    &FakeExtEventStub,
    &FakeExtEventId,
    &FakeExtEventArgCount,
    &FakeExtEventArgAt,
};

bool EnsureEzStateDispatcher() {
    if (g_EzStateDispatch != nullptr) {
        return true;
    }

    const uintptr_t target = FindByAbsoluteCandidates(kEzStateDispatcherCandidates);
    if (target == 0) {
        return false;
    }

    g_EzStateDispatch = reinterpret_cast<EzStateTalkDispatchFn>(target);
    return g_EzStateDispatch != nullptr;
}

bool ExecuteEzStateEvent(std::uint64_t chr_ins_handle,
                         int event_id,
                         const std::array<std::int64_t, 4>& params,
                         int param_count) {
    if (!EnsureEzStateDispatcher()) {
        return false;
    }

    std::array<EzStateFuncArg, 5> args{};
    args[0].value.as_int = event_id;
    args[0].type = 2;
    for (int index = 0; index < param_count && index < static_cast<int>(params.size()); ++index) {
        args[static_cast<std::size_t>(index + 1)].value.as_int = static_cast<std::int32_t>(params[static_cast<std::size_t>(index)]);
        args[static_cast<std::size_t>(index + 1)].type = 2;
    }

    EzStateTalkEvent talk_event{};
    UnkTalkEventField08 talk_field{};
    CSNpcTalkIns talk_ins{};
    CSTalkInsActiveMenuJob menu_job{};
    talk_field.chr_ins_handle = chr_ins_handle;
    talk_ins.chr_ins_handle = chr_ins_handle;
    menu_job.parent_talk_ins = &talk_ins;
    talk_ins.active_menu_job = &menu_job;
    talk_event.unk08 = &talk_field;
    talk_event.npc_talk_ins = &talk_ins;

    FakeEzStateExternalEvent event{};
    event.vtable = &g_FakeExtEventVtable;
    event.args = args.data();
    event.arg_count = std::clamp(param_count + 1, 1, static_cast<int>(args.size()));

    __try {
        g_EzStateDispatch(&talk_event, &event);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
    return true;
}

std::uint64_t GetPlayerHandle(const Game::SingletonRegistry& singletons) {
    const uintptr_t player_base = GetPlayerBase(singletons);
    std::uint64_t handle = 0;
    return (player_base != 0 && Game::ReadValue(player_base + 0x8, handle))
               ? handle
               : static_cast<std::uint64_t>(-1);
}

fs::path GetResourcePath(const wchar_t* relative) {
    wchar_t buffer[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(g_Module, buffer, MAX_PATH);
    if (length == 0) {
        return {};
    }

    const fs::path dll_path(std::wstring(buffer, buffer + length));
    return dll_path.parent_path() / L"Resources" / relative;
}

int ParseIntSafe(const std::string& value, int fallback) {
    try {
        return std::stoi(value, nullptr, 0);
    } catch (...) {
        return fallback;
    }
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

InspectorTools* InspectorTools::instance_ = nullptr;

InspectorSnapshot GetInspectorSnapshot() {
    return InspectorTools::Instance() != nullptr ? InspectorTools::Instance()->Snapshot() : InspectorSnapshot{};
}

InspectorTools* InspectorTools::Instance() {
    return instance_;
}

void InspectorTools::Tick(const Game::SingletonRegistry& singletons) {
    if (instance_ == nullptr) {
        instance_ = this;
    }

    ERD_PROTECTED_STEP("InspectorTools.LoadParamScripts", LoadParamScripts());
    ERD_PROTECTED_STEP("InspectorTools.SyncSaveLoadTools", SyncSaveLoadTools(singletons));
    ERD_PROTECTED_STEP("InspectorTools.SyncParamTools", SyncParamTools(singletons));

    const ULONGLONG now = GetTickCount64();
    if (now >= next_refresh_tick_) {
        ERD_PROTECTED_STEP("InspectorTools.RefreshSnapshot", RefreshSnapshot(singletons));
        next_refresh_tick_ = now + 750;
    }
}

InspectorSnapshot InspectorTools::Snapshot() const {
    std::scoped_lock lock(snapshot_mutex_);
    return snapshot_;
}

void InspectorTools::RefreshSnapshot(const Game::SingletonRegistry& singletons) {
    InspectorSnapshot next{};
    next.game_ready = Main::g_FeatureStatus.game_ready.load();
    next.world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    next.game_man = singletons.GetObjectPointer("GameMan");
    next.game_data_man = singletons.GetObjectPointer("GameDataMan");
    next.cs_menu_man = singletons.GetObjectPointer("CSMenuMan");
    next.cs_regulation_manager = singletons.GetObjectPointer("CSRegulationManager");

    next.param_scripts = snapshot_.param_scripts;
    next.param_script_error = snapshot_.param_script_error;

    RefreshHeroAndStatistics(singletons, next);
    RefreshInventory(singletons, next);
    RefreshParamTables(singletons, next);

    std::scoped_lock lock(snapshot_mutex_);
    snapshot_ = std::move(next);
}

void InspectorTools::RefreshHeroAndStatistics(const Game::SingletonRegistry& singletons, InspectorSnapshot& next) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    const uintptr_t player_base = GetPlayerBase(singletons);
    const uintptr_t resource_block = GetResourceBlock(singletons);
    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    const uintptr_t game_data_man = singletons.GetObjectPointer("GameDataMan");
    const uintptr_t menu_man = singletons.GetObjectPointer("CSMenuMan");

    if (player_game_data != 0) {
        ReadUnicodeString(player_game_data + kCharacterNameOffset, 32, next.character_name);
        Game::ReadValue(player_game_data + kPlayerLevelOffset, next.level);
        Game::ReadValue(player_game_data + kCharacterClearCountOffset, next.clear_count);
        Game::ReadValue(player_game_data + kRuneArcOffset, next.rune_arc);
        Game::ReadValue(player_game_data + kScadutreeBlessingOffset, next.scadutree_blessing);
        Game::ReadValue(player_game_data + kReveredSpiritAshBlessingOffset, next.revered_spirit_ash_blessing);
        Game::ReadValue(player_game_data + kPlayerCurrentWeaponLeftOffset, next.current_weapon_slot_left);
        Game::ReadValue(player_game_data + kPlayerCurrentWeaponRightOffset, next.current_weapon_slot_right);

        for (std::size_t index = 0; index < next.quick_items.size(); ++index) {
            Game::ReadValue(player_game_data + kQuickItemBaseOffset + static_cast<uintptr_t>(index) * 4, next.quick_items[index]);
        }
        for (std::size_t index = 0; index < next.pouch_items.size(); ++index) {
            Game::ReadValue(player_game_data + kPouchBaseOffset + static_cast<uintptr_t>(index) * 4, next.pouch_items[index]);
        }

        constexpr std::array<uintptr_t, 6> kWeaponOffsets{0x398, 0x39C, 0x3A0, 0x3A4, 0x3A8, 0x3AC};
        for (std::size_t index = 0; index < kWeaponOffsets.size(); ++index) {
            Game::ReadValue(player_game_data + kWeaponOffsets[index], next.equipped_weapons[index]);
        }
    }

    if (game_data_man != 0) {
        uintptr_t great_rune_root = 0;
        if (Game::ReadValue(game_data_man + 0x8, great_rune_root) && great_rune_root != 0) {
            Game::ReadValue(great_rune_root + kGreatRuneRootOffset, next.great_rune);
        }
    }

    if (player_base != 0) {
        Game::ReadValue(player_base + kCurrentCharacterTypeOffset, next.character_type);
        std::uint8_t team_type = 0;
        if (Game::ReadValue(player_base + kCurrentTeamTypeOffset, team_type)) {
            next.team_type = static_cast<int>(team_type);
        }
    }

    if (resource_block != 0) {
        Game::ReadValue(resource_block + kPlayerCurrentHpOffset, next.current_hp);
        Game::ReadValue(resource_block + kPlayerMaxHpOffset, next.max_hp);
        Game::ReadValue(resource_block + kPlayerCurrentFpOffset, next.current_fp);
        Game::ReadValue(resource_block + kPlayerMaxFpOffset, next.max_fp);
        Game::ReadValue(resource_block + kPlayerCurrentSpOffset, next.current_sp);
        Game::ReadValue(resource_block + kPlayerMaxSpOffset, next.max_sp);
    }

    if (game_man != 0) {
        Game::ReadValue(game_man + kSaveSlotOffset, next.save_slot);
        Game::ReadValue(game_man + kPendingLoadSaveSlotOffset, next.pending_load_save_slot);
        Game::ReadValue(game_man + kLastGraceOffset, next.last_grace);
    }

    if (menu_man != 0) {
        int auto_save = 0;
        if (Game::ReadValue(menu_man + kDisableAutoSaveOffset, auto_save)) {
            next.disable_auto_save = auto_save != 0;
        }
    }

    next.party_white_ghosts = GetPartyInfoValue(singletons, 0x8);
    next.party_black_ghosts = GetPartyInfoValue(singletons, 0xC);
    next.party_detective_ghosts = GetPartyInfoValue(singletons, 0x10);
    next.party_npc_members = GetPartyInfoValue(singletons, 0x14);
    next.party_members_created = GetPartyInfoValue(singletons, 0x1C);
    next.party_connected_members = GetPartyInfoValue(singletons, 0x24);

    uintptr_t party_root = 0;
    if (game_man != 0 && Game::ReadValue(game_man + kPartyInfoRootOffset, party_root) && party_root != 0) {
        for (int slot = 0; slot < kPartySlotCount; ++slot) {
            PartyMemberSlotSnapshot& entry = next.party_slots[static_cast<std::size_t>(slot)];
            Game::ReadValue(party_root + kPartySlotBaseOffset + kPartySlotStride * slot + 0x0, entry.handle);
            Game::ReadValue(party_root + kPartySlotBaseOffset + kPartySlotStride * slot + 0x8, entry.type);
            Game::ReadValue(party_root + kPartySlotBaseOffset + kPartySlotStride * slot + 0xC, entry.state);
            Game::ReadValue(party_root + kPartySlotBaseOffset + kPartySlotStride * slot + 0x20, entry.init_flag);
            Game::ReadValue(party_root + kPartySlotBaseOffset + kPartySlotStride * slot + 0x24, entry.end_flag);
        }
    }

    auto read_session_player = [&singletons](int slot_index, SessionPlayerSnapshot& out) {
        const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
        if (world_chr_man == 0) {
            return;
        }

        uintptr_t slots_root = 0;
        uintptr_t player_slot = 0;
        uintptr_t name_root = 0;
        if (!Game::ReadValue(world_chr_man + kNetPlayersOffset, slots_root) ||
            slots_root == 0 ||
            !Game::ReadValue(slots_root + static_cast<uintptr_t>(slot_index) * 0x10, player_slot) ||
            player_slot == 0) {
            return;
        }

        out.valid = true;
        Game::ReadValue(player_slot + kCurrentHpOffset, out.hp);
        Game::ReadValue(player_slot + kCurrentMaxHpOffset, out.max_hp);
        Game::ReadValue(player_slot + kCurrentLevelOffset, out.level);
        Game::ReadValue(player_slot + kCurrentCharacterTypeOffset, out.character_type);
        std::uint8_t team_type = 0;
        std::uint8_t reinforce_level = 0;
        if (Game::ReadValue(player_slot + kCurrentTeamTypeOffset, team_type)) {
            out.team_type = static_cast<int>(team_type);
        }
        if (Game::ReadValue(player_slot + kCurrentReinforceLevelOffset, reinforce_level)) {
            out.reinforce_level = static_cast<int>(reinforce_level);
        }
        if (Game::ReadValue(player_slot + kCurrentNameRootOffset, name_root) && name_root != 0) {
            ReadUnicodeString(name_root + 0x9C, 32, out.name);
        }
    };

    read_session_player(0, next.local_player);
    for (int slot = 0; slot < kSessionNetPlayerCount; ++slot) {
        read_session_player(slot + 1, next.net_players[static_cast<std::size_t>(slot)]);
    }

    if (next.character_name.empty() && !next.local_player.name.empty()) {
        next.character_name = next.local_player.name;
    }
    if (next.level <= 0 && next.local_player.level > 0) {
        next.level = next.local_player.level;
    }
    if (next.current_hp <= 0 && next.local_player.hp > 0) {
        next.current_hp = next.local_player.hp;
    }
    if (next.max_hp <= 0 && next.local_player.max_hp > 0) {
        next.max_hp = next.local_player.max_hp;
    }
    if (next.character_type == 0 && next.local_player.character_type != 0) {
        next.character_type = next.local_player.character_type;
    }
    if (next.team_type == 0 && next.local_player.team_type != 0) {
        next.team_type = next.local_player.team_type;
    }
}

void InspectorTools::RefreshInventory(const Game::SingletonRegistry& singletons, InspectorSnapshot& next) {
    std::unordered_set<std::uint64_t> seen;
    AppendInventoryEntries(singletons, 0, false, seen, next.weapon_inventory, next.protector_inventory, next.accessory_inventory, next.goods_inventory);
    AppendInventoryEntries(singletons, 2, false, seen, next.weapon_inventory, next.protector_inventory, next.accessory_inventory, next.goods_inventory);
    AppendInventoryEntries(singletons, 1, true, seen, next.weapon_inventory, next.protector_inventory, next.accessory_inventory, next.goods_inventory);
    AppendInventoryEntries(singletons, 3, true, seen, next.weapon_inventory, next.protector_inventory, next.accessory_inventory, next.goods_inventory);

    auto sorter = [](const InventoryEntrySnapshot& left, const InventoryEntrySnapshot& right) {
        if (left.repository != right.repository) {
            return left.repository < right.repository;
        }
        return static_cast<std::uint32_t>(left.item_id) < static_cast<std::uint32_t>(right.item_id);
    };

    std::sort(next.weapon_inventory.begin(), next.weapon_inventory.end(), sorter);
    std::sort(next.protector_inventory.begin(), next.protector_inventory.end(), sorter);
    std::sort(next.accessory_inventory.begin(), next.accessory_inventory.end(), sorter);
    std::sort(next.goods_inventory.begin(), next.goods_inventory.end(), sorter);
}

void InspectorTools::RefreshParamTables(const Game::SingletonRegistry& singletons, InspectorSnapshot& next) {
    next.param_preview = {};

    const uintptr_t regulation_manager = singletons.GetObjectPointer("CSRegulationManager");
    if (regulation_manager == 0) {
        return;
    }

    const auto* regulation_bytes = reinterpret_cast<const std::uint8_t*>(regulation_manager);
    const auto start = *reinterpret_cast<const std::uint8_t* const*>(regulation_bytes + Game::detail::kRegulationParamListBeginOffset);
    const auto end = *reinterpret_cast<const std::uint8_t* const*>(regulation_bytes + Game::detail::kRegulationParamListEndOffset);
    if (start == nullptr || end == nullptr || end < start) {
        return;
    }

    const std::size_t num_params = static_cast<std::size_t>(end - start) / sizeof(std::uint64_t);
    next.param_tables.reserve(num_params);
    for (std::size_t index = 0; index < num_params; ++index) {
        const auto* param_entry = *reinterpret_cast<const std::uint8_t* const*>(start + index * sizeof(std::uint64_t));
        if (param_entry == nullptr) {
            continue;
        }

        const wchar_t* name = Game::detail::ResolveParamName(param_entry);
        Game::ParamTable* table = reinterpret_cast<Game::ParamTable*>(Game::detail::ResolveParamTable(param_entry));
        if (name == nullptr || table == nullptr) {
            continue;
        }

        next.param_tables.push_back(ParamTableSnapshot{WideToUtf8(name), std::wstring(name), table->num_rows});
    }

    std::sort(next.param_tables.begin(), next.param_tables.end(), [](const ParamTableSnapshot& left, const ParamTableSnapshot& right) {
        return left.name_utf8 < right.name_utf8;
    });

    if (next.param_tables.empty()) {
        return;
    }

    const int selected_table = std::clamp(Main::g_FeatureStatus.param_tool_selected_table.load(), 0, static_cast<int>(next.param_tables.size()) - 1);
    const std::uint64_t row_id = static_cast<std::uint64_t>(std::max(Main::g_FeatureStatus.param_tool_row_id.load(), 0));
    const std::size_t field_offset = static_cast<std::size_t>(std::max(Main::g_FeatureStatus.param_tool_field_offset.load(), 0));
    const ParamValueType value_type = static_cast<ParamValueType>(std::clamp(Main::g_FeatureStatus.param_tool_value_type.load(), 0, 6));
    uintptr_t row_address = 0;
    int row_count = 0;
    if (!ResolveParamTableAndRow(singletons, next.param_tables[static_cast<std::size_t>(selected_table)].name_wide, row_id, row_address, row_count)) {
        return;
    }

    std::string value_text;
    if (ReadParamValuePreview(row_address, field_offset, value_type, value_text)) {
        next.param_preview.valid = true;
        next.param_preview.value_text = std::move(value_text);
        next.param_preview.type_text = ParamValueTypeLabel(value_type);
        next.param_preview.row_address = row_address;
    }
}

void InspectorTools::LoadParamScripts() {
    const fs::path path = GetResourcePath(L"ParamScripts.zh.txt");
    if (path.empty() || !fs::exists(path)) {
        std::scoped_lock lock(snapshot_mutex_);
        snapshot_.param_scripts.clear();
        snapshot_.param_script_error = "未找到外置参数脚本表: Resources/ParamScripts.zh.txt";
        return;
    }

    const auto write_time = fs::last_write_time(path);
    if (loaded_param_script_path_ == path && loaded_param_script_time_ == write_time) {
        return;
    }

    std::ifstream input(path);
    std::unordered_map<std::string, std::size_t> group_to_index;
    std::vector<ParamScriptSnapshot> scripts;
    std::string line;
    int next_id = 1;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        line = TrimCopy(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::vector<std::string> parts;
        std::size_t begin = 0;
        while (begin <= line.size()) {
            const std::size_t separator = line.find('|', begin);
            if (separator == std::string::npos) {
                parts.push_back(TrimCopy(line.substr(begin)));
                break;
            }
            parts.push_back(TrimCopy(line.substr(begin, separator - begin)));
            begin = separator + 1;
        }

        if (parts.size() < 7) {
            continue;
        }

        const std::string key = parts[0] + "|" + parts[1];
        std::size_t script_index = 0;
        auto found = group_to_index.find(key);
        if (found == group_to_index.end()) {
            script_index = scripts.size();
            group_to_index.emplace(key, script_index);
            scripts.push_back(ParamScriptSnapshot{next_id++, parts[0], parts[1], parts.size() >= 8 ? parts[7] : "", {}});
        } else {
            script_index = found->second;
        }

        ParamScriptOperation op{};
        op.param_name = Utf8ToWide(parts[2]);
        op.row_id = static_cast<std::uint64_t>(std::stoull(parts[3], nullptr, 0));
        op.offset = static_cast<std::size_t>(std::stoull(parts[4], nullptr, 0));
        op.type = parts[5];
        op.value = parts[6];
        op.description = parts.size() >= 8 ? parts[7] : "";
        scripts[script_index].operations.push_back(std::move(op));
    }

    {
        std::scoped_lock lock(snapshot_mutex_);
        snapshot_.param_scripts = std::move(scripts);
        snapshot_.param_script_error.clear();
    }
    loaded_param_script_path_ = path;
    loaded_param_script_time_ = write_time;
}

void InspectorTools::SyncSaveLoadTools(const Game::SingletonRegistry& singletons) {
    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    const uintptr_t menu_man = singletons.GetObjectPointer("CSMenuMan");

    if (Main::g_FeatureStatus.statistics_save_game_requested.exchange(false)) {
        const std::uint8_t value = 1;
        if (game_man != 0 && Game::WriteValue(game_man + kSaveRequestOffset, value)) {
            Main::Logger::Instance().Info("Statistics: save request triggered.");
        } else {
            Main::Logger::Instance().Error("Statistics: save request failed.");
        }
    }

    if (Main::g_FeatureStatus.statistics_load_save_slot_requested.exchange(false)) {
        const int slot = std::max(Main::g_FeatureStatus.statistics_load_save_slot.load(), 0);
        if (game_man != 0 && Game::WriteValue(game_man + kPendingLoadSaveSlotOffset, slot)) {
            Main::Logger::Instance().Info(("Statistics: load save slot request set to " + std::to_string(slot) + ".").c_str());
        } else {
            Main::Logger::Instance().Error("Statistics: load save slot request failed.");
        }
    }

    if (Main::g_FeatureStatus.statistics_auto_save_dirty.exchange(false)) {
        const int desired = Main::g_FeatureStatus.statistics_disable_auto_save.load() ? 1 : 0;
        if (menu_man != 0 && Game::WriteValue(menu_man + kDisableAutoSaveOffset, desired)) {
            Main::Logger::Instance().Info(desired ? "Statistics: auto-save disabled." : "Statistics: auto-save enabled.");
        } else {
            Main::Logger::Instance().Error("Statistics: auto-save toggle failed.");
        }
    }
}

void InspectorTools::SyncParamTools(const Game::SingletonRegistry& singletons) {
    ParamTableSnapshot table{};
    {
        std::scoped_lock lock(snapshot_mutex_);
        const int selected_table = Main::g_FeatureStatus.param_tool_selected_table.load();
        if (selected_table >= 0 && selected_table < static_cast<int>(snapshot_.param_tables.size())) {
            table = snapshot_.param_tables[static_cast<std::size_t>(selected_table)];
        }
    }

    if (table.name_wide.empty()) {
        return;
    }

    if (Main::g_FeatureStatus.param_tool_write_requested.exchange(false)) {
        const std::uint64_t row_id = static_cast<std::uint64_t>(std::max(Main::g_FeatureStatus.param_tool_row_id.load(), 0));
        const std::size_t offset = static_cast<std::size_t>(std::max(Main::g_FeatureStatus.param_tool_field_offset.load(), 0));
        const ParamValueType type = static_cast<ParamValueType>(std::clamp(Main::g_FeatureStatus.param_tool_value_type.load(), 0, 6));
        uintptr_t row_address = 0;
        int row_count = 0;
        if (ResolveParamTableAndRow(singletons, table.name_wide, row_id, row_address, row_count) &&
            WriteParamValue(row_address, offset, type, Main::g_FeatureStatus.param_tool_int_value.load(), Main::g_FeatureStatus.param_tool_float_value.load())) {
            Main::Logger::Instance().Info(("Param write applied: " + table.name_utf8).c_str());
        } else {
            Main::Logger::Instance().Error("Param write failed.");
        }
    }

    const int script_id = Main::g_FeatureStatus.param_tool_script_request_id.exchange(0);
    if (script_id <= 0) {
        return;
    }

    ParamScriptSnapshot script{};
    {
        std::scoped_lock lock(snapshot_mutex_);
        auto found = std::find_if(snapshot_.param_scripts.begin(), snapshot_.param_scripts.end(), [script_id](const ParamScriptSnapshot& entry) {
            return entry.id == script_id;
        });
        if (found == snapshot_.param_scripts.end()) {
            Main::Logger::Instance().Error(("Param script id not found: " + std::to_string(script_id)).c_str());
            return;
        }
        script = *found;
    }

    bool success = true;
    for (const ParamScriptOperation& operation : script.operations) {
        uintptr_t row_address = 0;
        int row_count = 0;
        if (!ResolveParamTableAndRow(singletons, operation.param_name, operation.row_id, row_address, row_count)) {
            success = false;
            break;
        }

        const ParamValueType type = ParseParamValueType(operation.type);
        const int int_value = ParseIntSafe(operation.value, 0);
        const float float_value = static_cast<float>(std::strtod(operation.value.c_str(), nullptr));
        if (!WriteParamValue(row_address, operation.offset, type, int_value, float_value)) {
            success = false;
            break;
        }
    }

    if (success) {
        Main::Logger::Instance().Info(("Param script applied: " + script.label).c_str());
    } else {
        Main::Logger::Instance().Error(("Param script failed: " + script.label).c_str());
    }
}

}  // namespace ERD::Features
