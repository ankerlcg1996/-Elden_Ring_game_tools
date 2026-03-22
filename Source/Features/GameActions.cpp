#include "GameActions.hpp"

#include "../Game/EventFlags.hpp"
#include "../Game/Memory.hpp"
#include "../Game/Params.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"
#include "MassItemGibData.hpp"

#include <cmath>

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

constexpr uintptr_t kStartNextCycleOffset = 0xB7D;
constexpr uintptr_t kSaveRequestOffset = 0xB72;
constexpr uintptr_t kPlayerGameDataOffset = 0x8;
constexpr uintptr_t kEquipGameDataOffset = 0x2B0;
constexpr uintptr_t kEquipInventoryTailDataOffset = 0x408;
constexpr uintptr_t kEquipInventoryDataOffset = 0x5D0;
constexpr uintptr_t kEquipMagicDataOffset = 0x530;
constexpr uintptr_t kEquipItemDataOffset = 0x538;
constexpr uintptr_t kHpFlaskAllocationOffset = 0x101;
constexpr uintptr_t kMpFlaskAllocationOffset = 0x102;
constexpr uintptr_t kPlayerReinforceLevelOffset = 0xE2;
constexpr uintptr_t kPlayerVigorOffset = 0x3C;
constexpr uintptr_t kPlayerMindOffset = 0x40;
constexpr uintptr_t kPlayerEnduranceOffset = 0x44;
constexpr uintptr_t kPlayerStrengthOffset = 0x48;
constexpr uintptr_t kPlayerDexterityOffset = 0x4C;
constexpr uintptr_t kPlayerIntelligenceOffset = 0x50;
constexpr uintptr_t kPlayerFaithOffset = 0x54;
constexpr uintptr_t kPlayerArcaneOffset = 0x58;
constexpr uintptr_t kPlayerLevelOffset = 0x68;
constexpr uintptr_t kPlayerMagicSlotCountOffset = 0xA74;
constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kPlayerPositionRootOffset = 0x190;
constexpr uintptr_t kPlayerPositionNodeOffset = 0x68;
constexpr uintptr_t kPlayerPositionValueOffset = 0x70;
constexpr uintptr_t kPlayerGlobalCoordsOffset = 0x6B0;
constexpr uintptr_t kPlayerRadOffset = 0x6BC;
constexpr uintptr_t kPlayerMapIdOffset = 0x6C0;
constexpr uintptr_t kLastGoodGlobalCoordsOffset = 0x6C4;
constexpr uintptr_t kChunkCoordsOffset = 0xD0;
constexpr uintptr_t kGameManInitPosOffset = 0xAA0;
constexpr uintptr_t kPlayerHpOffset = 0x138;
constexpr uintptr_t kPlayerMaxHpOffset = 0x13C;
constexpr uintptr_t kPlayerMpOffset = 0x148;
constexpr uintptr_t kPlayerMaxMpOffset = 0x150;
constexpr uintptr_t kPlayerSpOffset = 0x154;
constexpr uintptr_t kPlayerMaxSpOffset = 0x158;
constexpr uintptr_t kGestureEquipDataOffset = 0x5E0;

constexpr const char* kAddSoulPattern = "44 8B ? ? 45 33 ? 44 89 5C 24";
constexpr const char* kEstusAllocationUpdatePattern =
    "? 8B ? ? ? ? ? ? 8B C1 ? 8B ? ? ? 85 C9 74 ? ? 85 C0 74 ? ? 83 F8 01 75 ? 0F B6";
constexpr const char* kActivateBonfirePattern =
    "88 4C ? ? 53 ? 83 EC ? C6 44 ? ? 63 C6 44 ? ? 01 80 F9 01";
constexpr const char* kReplaceToolPattern =
    "? 0F B6 F1 ? 8B D8 ? 8B F9 81 E2 FF FF FF 0F 0F BA EA ? 89 54 ? ? ? 81 C1";
constexpr const char* kItemGivePattern = "8B 02 83 F8 0A";
constexpr const char* kEquipGearPattern = "?? 8b f1 ?? 8b d8 ?? 63 ea ?? 8b f9";
constexpr const char* kEquipGoodsPattern = "?? fa ?? ?? 0f ?? 81 c1 ?? ?? ?? ?? ?? 8b c1 e9";
constexpr const char* kChangeMagicPattern =
    "?? 89 5c ?? ?? ?? 89 74 ?? ?? 57 ?? 83 ec ?? ?? 8b c2 8b f9 ?? 8b c8";
constexpr const char* kGetWeaponAddrPattern = "8B EA 48 8B F1 33 DB 41 83 CE FF 85 D2";
constexpr const char* kGetGoodsAddrPattern =
    "41 8D 50 03 E8 ?? ?? ?? ?? ?? 85 C0 0F 84 ?? ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? 48";
constexpr const char* kGetQuantityPattern = "?? 8B F9 ?? 8D 44 ?? 48 ?? 89 44 ?? ?? 8B 01";

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

uintptr_t GetPlayerGameData(const Game::SingletonRegistry& singletons) {
    const uintptr_t game_data_man = singletons.GetObjectPointer("GameDataMan");
    if (game_data_man == 0) {
        return 0;
    }

    uintptr_t player_game_data = 0;
    return Game::ReadValue(game_data_man + kPlayerGameDataOffset, player_game_data) ? player_game_data : 0;
}

uintptr_t GetEquipInventoryData(const Game::SingletonRegistry& singletons) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return 0;
    }

    uintptr_t equip_inventory_data = 0;
    return Game::ReadValue(player_game_data + kEquipInventoryDataOffset, equip_inventory_data) ? equip_inventory_data : 0;
}

struct ItemTypeInfo {
    std::uint32_t prefix = 0xF0000000u;
    int type = -1;
};

struct ItemGiveEntry {
    std::int32_t item = 0;
    std::int32_t quantity = 0;
    std::int32_t unknown = -1;
    std::int32_t gem = -1;
};

struct ItemGiveBatch {
    std::int32_t count = 0;
    ItemGiveEntry entries[10]{};
};

struct CharacterEditorValues {
    int level = 1;
    int vigor = 10;
    int mind = 10;
    int endurance = 10;
    int strength = 10;
    int dexterity = 10;
    int intelligence = 10;
    int faith = 10;
    int arcane = 10;
};

struct EquipSlotInfo {
    int slot = -1;
    uintptr_t player_game_data_offset = 0;
};

bool GetLocalPlayerSessionDataAddress(const Game::SingletonRegistry& singletons, uintptr_t& address);

constexpr std::array<EquipSlotInfo, 22> kEquipSlotInfos{{
    {0, 0x398}, {1, 0x39C}, {2, 0x3A0}, {3, 0x3A4}, {4, 0x3A8}, {5, 0x3AC},
    {6, 0x3B0}, {7, 0x3B4}, {8, 0x3B8}, {9, 0x3BC}, {10, 0x3C0}, {11, 0x3C4},
    {12, 0x3C8}, {13, 0x3CC}, {14, 0x3D0}, {15, 0x3D4}, {16, 0x3D8},
    {17, 0x3DC}, {18, 0x3E0}, {19, 0x3E4}, {20, 0x3E8}, {21, 0x3EC},
}};

ItemTypeInfo ResolveItemType(std::uint32_t item_id) {
    constexpr std::array<ItemTypeInfo, 5> kTypes{{
        {0x00000000u, 0},
        {0x10000000u, 1},
        {0x20000000u, 2},
        {0x40000000u, 4},
        {0x80000000u, 8},
    }};

    constexpr std::uint32_t kTypeMask = 0xF0000000u;
    const std::uint32_t prefix = item_id & kTypeMask;
    for (const ItemTypeInfo& candidate : kTypes) {
        if (static_cast<std::uint32_t>(candidate.prefix) == prefix) {
            return candidate;
        }
    }

    return {};
}

std::int32_t ResolveParamId(std::uint32_t item_id, std::uint32_t prefix) {
    return static_cast<std::int32_t>(item_id - prefix);
}

uintptr_t FindItemGiveFunction() {
    static uintptr_t address = 0;
    if (address == 0) {
        const uintptr_t pattern_address = FindPatternInText(kItemGivePattern);
        if (pattern_address != 0) {
            address = pattern_address - 0x52;
        }
    }
    return address;
}

uintptr_t FindEquipGearFunction() {
    static uintptr_t address = 0;
    if (address == 0) {
        const uintptr_t pattern_address = FindPatternInText(kEquipGearPattern);
        if (pattern_address != 0) {
            address = pattern_address - 0x17;
        }
    }
    return address;
}

uintptr_t FindEquipGoodsFunction() {
    static uintptr_t address = 0;
    if (address == 0) {
        address = FindPatternInText(kEquipGoodsPattern);
    }
    return address;
}

uintptr_t FindChangeMagicFunction() {
    static uintptr_t address = 0;
    if (address == 0) {
        address = FindPatternInText(kChangeMagicPattern);
    }
    return address;
}

uintptr_t FindGetWeaponAddrFunction() {
    static uintptr_t address = 0;
    if (address == 0) {
        const uintptr_t pattern_address = FindPatternInText(kGetWeaponAddrPattern);
        if (pattern_address != 0) {
            address = pattern_address - 0x22;
        }
    }
    return address;
}

uintptr_t FindGetGoodsAddrFunction() {
    static uintptr_t address = 0;
    if (address == 0) {
        const uintptr_t pattern_address = FindPatternInText(kGetGoodsAddrPattern);
        if (pattern_address != 0) {
            address = pattern_address - 0x6A;
        }
    }
    return address;
}

uintptr_t FindGetQuantityFunction() {
    static uintptr_t address = 0;
    if (address == 0) {
        const uintptr_t pattern_address = FindPatternInText(kGetQuantityPattern);
        if (pattern_address != 0) {
            address = pattern_address - 0x14;
        }
    }
    return address;
}

using ItemGiveFn = void (*)(uintptr_t map_item_man, uintptr_t item_table, uintptr_t item_give_data, int unknown);
using GetParamAddrFn = void (*)(uintptr_t output_memory, int id, int arg3, int arg4);
using GetQuantityFn = int (*)(uintptr_t item_id_memory);
using EquipGearFn = void (*)(uintptr_t equip_game_data, int slot, uintptr_t item_data, int inventory_index, int arg5, int arg6, int arg7);
using EquipGoodsFn = void (*)(uintptr_t equip_game_data, int slot, uintptr_t item_data, int inventory_index);
using ChangeMagicFn = int (*)(int magic_slot, uintptr_t scratch_memory);

uintptr_t GetEquipItemDataBase(const Game::SingletonRegistry& singletons) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    return player_game_data != 0 ? player_game_data + kEquipItemDataOffset : 0;
}

int GetTailDataIndexForEquip(const Game::SingletonRegistry& singletons) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return 0;
    }

    int tail_data_index = 0;
    return Game::ReadValue(player_game_data + kEquipInventoryTailDataOffset + 0x1C, tail_data_index)
               ? tail_data_index
               : 0;
}

bool GetInventoryList(const Game::SingletonRegistry& singletons,
                      int inventory_kind,
                      uintptr_t& inventory_list,
                      int& inventory_num,
                      std::size_t& slot_limit) {
    inventory_list = 0;
    inventory_num = 0;
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

    if (!Game::ReadValue(player_game_data + inventory_offset, inventory_data) || inventory_data == 0) {
        return false;
    }

    return Game::ReadValue(inventory_data + 0x10 + key_offset, inventory_list) &&
           inventory_list != 0 &&
           Game::ReadValue(inventory_data + 0x18 + key_offset, inventory_num);
}

int FindItemIndexInInventory(const Game::SingletonRegistry& singletons, std::int32_t item_id, int inventory_kind = 0) {
    uintptr_t inventory_list = 0;
    int inventory_num = 0;
    std::size_t slot_limit = 0;
    if (!GetInventoryList(singletons, inventory_kind, inventory_list, inventory_num, slot_limit)) {
        return -1;
    }

    int populated = 0;
    for (std::size_t i = 0; i <= slot_limit; ++i) {
        std::int32_t row_item_id = -1;
        if (!Game::ReadValue(inventory_list + i * 0x18 + 0x4, row_item_id)) {
            break;
        }

        if (row_item_id == item_id) {
            return static_cast<int>(i);
        }
        if (row_item_id != -1) {
            ++populated;
        }
        if (populated >= inventory_num) {
            break;
        }
    }

    return -1;
}

std::int32_t GetItemIdByInventoryIndex(const Game::SingletonRegistry& singletons, int index, int inventory_kind = 0) {
    if (index < 0) {
        return -1;
    }

    uintptr_t inventory_list = 0;
    int inventory_num = 0;
    std::size_t slot_limit = 0;
    if (!GetInventoryList(singletons, inventory_kind, inventory_list, inventory_num, slot_limit) ||
        static_cast<std::size_t>(index) > slot_limit) {
        return -1;
    }

    std::int32_t item_id = -1;
    if (!Game::ReadValue(inventory_list + static_cast<uintptr_t>(index) * 0x18, item_id)) {
        return -1;
    }
    return item_id;
}

uintptr_t ResolveItemParamAddress(const Game::SingletonRegistry& singletons, std::int32_t param_id, int item_type) {
    uintptr_t function = 0;
    if (item_type == 0) {
        function = FindGetWeaponAddrFunction();
    } else if (item_type == 4) {
        function = FindGetGoodsAddrFunction();
    }

    if (function != 0) {
        alignas(16) std::array<std::uint8_t, 0x20> scratch{};
        __try {
            reinterpret_cast<GetParamAddrFn>(function)(
                reinterpret_cast<uintptr_t>(scratch.data()),
                param_id,
                0,
                0
            );
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            function = 0;
        }

        if (function != 0) {
            uintptr_t param_address = 0;
            if (Game::ReadValue(reinterpret_cast<uintptr_t>(scratch.data()) + 0x8, param_address) && param_address != 0) {
                return param_address;
            }
        }
    }

    if (item_type == 0) {
        return reinterpret_cast<uintptr_t>(
            Game::FindParamRow<EquipParamWeapon>(singletons, L"EquipParamWeapon", static_cast<std::uint64_t>(param_id))
        );
    }
    if (item_type == 4) {
        return reinterpret_cast<uintptr_t>(
            Game::FindParamRow<EquipParamGoods>(singletons, L"EquipParamGoods", static_cast<std::uint64_t>(param_id))
        );
    }
    return 0;
}

int GetCurrentHeldQuantity(std::int32_t item_id) {
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

bool IsProjectileWeapon(uintptr_t param_address) {
    std::uint8_t weapon_category = 0;
    return Game::ReadValue(param_address + 0xE6, weapon_category) &&
           (weapon_category == 13 || weapon_category == 14);
}

int GetMaxQuantityForItem(const Game::SingletonRegistry& singletons, uintptr_t param_address, int item_type) {
    if (param_address == 0) {
        return 0;
    }

    if (item_type == 0) {
        std::uint16_t weapon_type = 0;
        if (!Game::ReadValue(param_address + 0x1A6, weapon_type)) {
            return 0;
        }

        if (weapon_type == 81 || weapon_type == 85) {
            return 99 + 600;
        }
        if (weapon_type == 83) {
            return 30 + 600;
        }
        if (weapon_type == 86) {
            return 20 + 600;
        }
        return 1;
    }

    if (item_type == 4) {
        std::uint16_t max_num = 0;
        std::uint16_t max_repository_num = 0;
        std::uint8_t pot_group_id = 0;
        std::uint8_t goods_type = 0;
        if (!Game::ReadValue(param_address + 0x3A, max_num) ||
            !Game::ReadValue(param_address + 0x70, max_repository_num) ||
            !Game::ReadValue(param_address + 0x2E, pot_group_id) ||
            !Game::ReadValue(param_address + 0x3E, goods_type)) {
            return 0;
        }

        if (pot_group_id > 0 && pot_group_id <= 4 && goods_type != 11) {
            const uintptr_t equip_inventory_data = GetEquipInventoryData(singletons);
            int dynamic_cap = 0;
            if (equip_inventory_data != 0 &&
                Game::ReadValue(equip_inventory_data + 0xC8 + 4 * static_cast<uintptr_t>(pot_group_id), dynamic_cap)) {
                max_num = static_cast<std::uint16_t>(std::max(dynamic_cap, 0));
            }
        }

        return static_cast<int>(max_num) + static_cast<int>(max_repository_num);
    }

    return 0;
}

int AdjustRequestedQuantity(const Game::SingletonRegistry& singletons,
                            const MassItemGibItem& item,
                            int item_type,
                            std::int32_t param_id,
                            uintptr_t param_address) {
    if (item_type == 0 && !IsProjectileWeapon(param_address)) {
        return 1;
    }

    if (item_type == 0 || item_type == 4) {
        const int max_quantity = GetMaxQuantityForItem(singletons, param_address, item_type);
        const int held_quantity = GetCurrentHeldQuantity(static_cast<std::int32_t>(param_id + static_cast<std::int32_t>(ResolveItemType(item.item_id).prefix)));
        return std::max(std::min(item.quantity, max_quantity - held_quantity), 0);
    }

    if (item_type == 1 || item_type == 2) {
        return 1;
    }

    if (item_type == 8) {
        return std::max(item.quantity, 0);
    }

    return 0;
}

std::int32_t ResolveWeaponSpawnId(const Game::SingletonRegistry& singletons,
                                  uintptr_t param_address,
                                  const MassItemGibItem& item) {
    if (param_address == 0) {
        return static_cast<std::int32_t>(item.item_id);
    }

    std::int32_t equip_wep1 = -1;
    std::int32_t equip_wep16 = -1;
    if (!Game::ReadValue(param_address + 0x64, equip_wep1) ||
        !Game::ReadValue(param_address + 0x250, equip_wep16)) {
        return static_cast<std::int32_t>(item.item_id);
    }

    int reinforce_level = 0;
    if (equip_wep1 != -1) {
        std::uint8_t player_reinforce_level = 0;
        const uintptr_t player_game_data = GetPlayerGameData(singletons);
        if (player_game_data != 0) {
            Game::ReadValue(player_game_data + kPlayerReinforceLevelOffset, player_reinforce_level);
        }

        reinforce_level = item.reinforce_level;
        if (reinforce_level > 25 || reinforce_level < 0) {
            reinforce_level = static_cast<int>(player_reinforce_level);
        }
        if (equip_wep16 == -1) {
            reinforce_level = static_cast<int>(std::floor((static_cast<float>(reinforce_level) + 0.5f) / 2.5f));
        }
    }

    std::int32_t final_item_id = static_cast<std::int32_t>(item.item_id) + reinforce_level;
    if (item.upgrade >= 0 && item.upgrade <= 1200) {
        final_item_id += item.upgrade;
    }
    return final_item_id;
}

bool FlushItemBatch(const Game::SingletonRegistry& singletons,
                    const ItemGiveBatch& batch,
                    std::array<std::uint8_t, 128>& item_give_data) {
    if (batch.count <= 0) {
        return true;
    }

    const uintptr_t map_item_man = singletons.GetObjectPointer("MapItemMan");
    const uintptr_t function = FindItemGiveFunction();
    if (map_item_man == 0 || function == 0) {
        return false;
    }

    __try {
        reinterpret_cast<ItemGiveFn>(function)(
            map_item_man,
            reinterpret_cast<uintptr_t>(&batch),
            reinterpret_cast<uintptr_t>(item_give_data.data()),
            0
        );
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool GiveItemPackage(const Game::SingletonRegistry& singletons, const MassItemGibPackage& package) {
    if (package.requires_dlc && !Game::IsDlcOwned(singletons, 1)) {
        Main::Logger::Instance().Error(("MassItemGib requires DLC ownership: " + std::string(package.label)).c_str());
        return false;
    }

    std::array<std::uint8_t, 128> item_give_data{};
    ItemGiveBatch batch{};

    for (std::size_t index = 0; index < package.item_count; ++index) {
        const MassItemGibItem& source_item = package.items[index];
        const ItemTypeInfo item_type_info = ResolveItemType(source_item.item_id);
        if (item_type_info.type < 0) {
            continue;
        }

        const std::int32_t param_id = ResolveParamId(source_item.item_id, item_type_info.prefix);
        const uintptr_t param_address = ResolveItemParamAddress(singletons, param_id, item_type_info.type);
        int quantity = AdjustRequestedQuantity(singletons, source_item, item_type_info.type, param_id, param_address);
        if (quantity <= 0) {
            continue;
        }

        std::int32_t spawn_item_id = static_cast<std::int32_t>(source_item.item_id);
        std::int32_t gem = source_item.gem;
        if (item_type_info.type == 0) {
            spawn_item_id = ResolveWeaponSpawnId(singletons, param_address, source_item);
        } else if (item_type_info.type != 0 && source_item.gem < 0) {
            gem = -1;
        }

        ItemGiveEntry& target = batch.entries[batch.count];
        target.item = spawn_item_id;
        target.quantity = quantity;
        target.unknown = -1;
        target.gem = gem;
        ++batch.count;

        if (batch.count >= static_cast<int>(std::size(batch.entries))) {
            if (!FlushItemBatch(singletons, batch, item_give_data)) {
                return false;
            }
            batch = {};
        }
    }

    return FlushItemBatch(singletons, batch, item_give_data);
}

bool GiveSingleCustomItem(const Game::SingletonRegistry& singletons,
                          std::int32_t item_id,
                          int quantity,
                          int reinforce_level,
                          int upgrade_value,
                          int gem_id) {
    const MassItemGibItem item{
        static_cast<std::uint32_t>(item_id),
        quantity,
        reinforce_level,
        upgrade_value,
        gem_id,
    };

    const ItemTypeInfo item_type_info = ResolveItemType(item.item_id);
    if (item_type_info.type < 0) {
        return false;
    }

    const std::int32_t param_id = ResolveParamId(item.item_id, item_type_info.prefix);
    const uintptr_t param_address = ResolveItemParamAddress(singletons, param_id, item_type_info.type);
    int adjusted_quantity = AdjustRequestedQuantity(singletons, item, item_type_info.type, param_id, param_address);
    if (adjusted_quantity <= 0) {
        adjusted_quantity = std::max(quantity, 0);
    }
    if (adjusted_quantity <= 0) {
        return false;
    }

    std::int32_t spawn_item_id = item_id;
    int adjusted_gem = gem_id;
    if (item_type_info.type == 0) {
        spawn_item_id = ResolveWeaponSpawnId(singletons, param_address, item);
    } else {
        adjusted_gem = -1;
    }

    ItemGiveBatch batch{};
    batch.count = 1;
    batch.entries[0].item = spawn_item_id;
    batch.entries[0].quantity = adjusted_quantity;
    batch.entries[0].unknown = -1;
    batch.entries[0].gem = adjusted_gem;

    std::array<std::uint8_t, 128> item_give_data{};
    return FlushItemBatch(singletons, batch, item_give_data);
}

bool ReadCharacterEditorValues(const Game::SingletonRegistry& singletons, CharacterEditorValues& values) {
    uintptr_t local_player_data = 0;
    if (GetLocalPlayerSessionDataAddress(singletons, local_player_data) &&
        Game::ReadValue(local_player_data + kPlayerLevelOffset, values.level) &&
        Game::ReadValue(local_player_data + kPlayerVigorOffset, values.vigor) &&
        Game::ReadValue(local_player_data + kPlayerMindOffset, values.mind) &&
        Game::ReadValue(local_player_data + kPlayerEnduranceOffset, values.endurance) &&
        Game::ReadValue(local_player_data + kPlayerStrengthOffset, values.strength) &&
        Game::ReadValue(local_player_data + kPlayerDexterityOffset, values.dexterity) &&
        Game::ReadValue(local_player_data + kPlayerIntelligenceOffset, values.intelligence) &&
        Game::ReadValue(local_player_data + kPlayerFaithOffset, values.faith) &&
        Game::ReadValue(local_player_data + kPlayerArcaneOffset, values.arcane)) {
        return true;
    }

    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    return player_game_data != 0 &&
           Game::ReadValue(player_game_data + kPlayerLevelOffset, values.level) &&
           Game::ReadValue(player_game_data + kPlayerVigorOffset, values.vigor) &&
           Game::ReadValue(player_game_data + kPlayerMindOffset, values.mind) &&
           Game::ReadValue(player_game_data + kPlayerEnduranceOffset, values.endurance) &&
           Game::ReadValue(player_game_data + kPlayerStrengthOffset, values.strength) &&
           Game::ReadValue(player_game_data + kPlayerDexterityOffset, values.dexterity) &&
           Game::ReadValue(player_game_data + kPlayerIntelligenceOffset, values.intelligence) &&
           Game::ReadValue(player_game_data + kPlayerFaithOffset, values.faith) &&
           Game::ReadValue(player_game_data + kPlayerArcaneOffset, values.arcane);
}

bool WriteCharacterEditorValues(const Game::SingletonRegistry& singletons, const CharacterEditorValues& values) {
    bool wrote_any = false;

    uintptr_t local_player_data = 0;
    if (GetLocalPlayerSessionDataAddress(singletons, local_player_data)) {
        const bool local_ok =
            Game::WriteValue(local_player_data + kPlayerLevelOffset, values.level) &&
            Game::WriteValue(local_player_data + kPlayerVigorOffset, values.vigor) &&
            Game::WriteValue(local_player_data + kPlayerMindOffset, values.mind) &&
            Game::WriteValue(local_player_data + kPlayerEnduranceOffset, values.endurance) &&
            Game::WriteValue(local_player_data + kPlayerStrengthOffset, values.strength) &&
            Game::WriteValue(local_player_data + kPlayerDexterityOffset, values.dexterity) &&
            Game::WriteValue(local_player_data + kPlayerIntelligenceOffset, values.intelligence) &&
            Game::WriteValue(local_player_data + kPlayerFaithOffset, values.faith) &&
            Game::WriteValue(local_player_data + kPlayerArcaneOffset, values.arcane);
        wrote_any = wrote_any || local_ok;
    }

    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data != 0) {
        const bool save_ok =
            Game::WriteValue(player_game_data + kPlayerLevelOffset, values.level) &&
            Game::WriteValue(player_game_data + kPlayerVigorOffset, values.vigor) &&
            Game::WriteValue(player_game_data + kPlayerMindOffset, values.mind) &&
            Game::WriteValue(player_game_data + kPlayerEnduranceOffset, values.endurance) &&
            Game::WriteValue(player_game_data + kPlayerStrengthOffset, values.strength) &&
            Game::WriteValue(player_game_data + kPlayerDexterityOffset, values.dexterity) &&
            Game::WriteValue(player_game_data + kPlayerIntelligenceOffset, values.intelligence) &&
            Game::WriteValue(player_game_data + kPlayerFaithOffset, values.faith) &&
            Game::WriteValue(player_game_data + kPlayerArcaneOffset, values.arcane);
        wrote_any = wrote_any || save_ok;
    }

    return wrote_any;
}

std::int32_t NormalizeGoodsId(std::int32_t item_id) {
    if (item_id < 0) {
        return -1;
    }
    const std::uint32_t raw = static_cast<std::uint32_t>(item_id);
    if ((raw & 0xF0000000u) == 0x40000000u) {
        return item_id;
    }
    return static_cast<std::int32_t>(0x40000000u | (raw & 0x0FFFFFFFu));
}

std::int32_t ReadCurrentEquipSlotItemId(const Game::SingletonRegistry& singletons, int slot) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return -1;
    }

    if (slot >= 0 && slot <= 21) {
        for (const EquipSlotInfo& info : kEquipSlotInfos) {
            if (info.slot == slot) {
                std::int32_t item_id = -1;
                return Game::ReadValue(player_game_data + info.player_game_data_offset, item_id) ? item_id : -1;
            }
        }
        return -1;
    }

    const uintptr_t equip_item_data = GetEquipItemDataBase(singletons);
    if (equip_item_data == 0) {
        return -1;
    }

    uintptr_t item_address = 0;
    if (slot >= 22 && slot <= 31) {
        item_address = equip_item_data + static_cast<uintptr_t>(slot - 21) * 8;
    } else if (slot >= 32 && slot <= 37) {
        item_address = equip_item_data + 0x50 + static_cast<uintptr_t>(slot - 31) * 8;
    } else if (slot == 38) {
        item_address = equip_item_data + 0x88;
    } else {
        return -1;
    }

    std::uint32_t raw_item_id = 0x0FFFFFFFu;
    if (!Game::ReadValue(item_address, raw_item_id)) {
        return -1;
    }
    if ((raw_item_id & 0x0FFFFFFFu) == 0x0FFFFFFFu) {
        return -1;
    }
    return static_cast<std::int32_t>(0x40000000u | (raw_item_id & 0x0FFFFFFFu));
}

bool EquipItemById(const Game::SingletonRegistry& singletons, int slot, std::int32_t requested_item_id) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0 || slot < 0 || slot > 38) {
        return false;
    }

    if (slot >= 22 && requested_item_id >= 0) {
        requested_item_id = NormalizeGoodsId(requested_item_id);
    }

    const uintptr_t equip_game_data = player_game_data + kEquipGameDataOffset;
    int inventory_index = -1;
    int tail_data_index = 0;
    alignas(16) std::array<std::uint8_t, 0x20> scratch{};
    std::int32_t item_to_write = requested_item_id;

    if (requested_item_id >= 0) {
        inventory_index = FindItemIndexInInventory(singletons, requested_item_id);
        if (inventory_index < 0) {
            if (!GiveSingleCustomItem(singletons, requested_item_id, 1, 0, 0, -1)) {
                return false;
            }
            inventory_index = FindItemIndexInInventory(singletons, requested_item_id);
        }
        if (inventory_index < 0) {
            return false;
        }

        if (slot <= 21) {
            item_to_write = GetItemIdByInventoryIndex(singletons, inventory_index);
            tail_data_index = GetTailDataIndexForEquip(singletons);
        } else {
            item_to_write = NormalizeGoodsId(requested_item_id);
            const uintptr_t equip_inventory_data = GetEquipInventoryData(singletons);
            if (equip_inventory_data == 0 || !Game::ReadValue(equip_inventory_data + 0x1C, tail_data_index)) {
                return false;
            }
        }
    } else {
        if (slot == 10 || slot == 11 || slot == 16) {
            inventory_index = -1;
        } else if (slot <= 5) {
            item_to_write = 110000;
        } else if (slot == 12) {
            item_to_write = static_cast<std::int32_t>(0x10000000u | 10000u);
        } else if (slot == 13) {
            item_to_write = static_cast<std::int32_t>(0x10000000u | 10100u);
        } else if (slot == 14) {
            item_to_write = static_cast<std::int32_t>(0x10000000u | 10200u);
        } else if (slot == 15) {
            item_to_write = static_cast<std::int32_t>(0x10000000u | 10300u);
        } else {
            inventory_index = -1;
            tail_data_index = 0;
        }

        if (inventory_index != -1 && requested_item_id < 0) {
            inventory_index = FindItemIndexInInventory(singletons, item_to_write);
            if (inventory_index < 0) {
                if (!GiveSingleCustomItem(singletons, item_to_write, 1, 0, 0, -1)) {
                    return false;
                }
                inventory_index = FindItemIndexInInventory(singletons, item_to_write);
            }
            if (inventory_index < 0) {
                return false;
            }
            tail_data_index = slot <= 21 ? GetTailDataIndexForEquip(singletons) : 0;
        }
    }

    Game::WriteValue(reinterpret_cast<uintptr_t>(scratch.data()) + 0x10, item_to_write);

    __try {
        if (slot <= 21) {
            const uintptr_t equip_gear_fn = FindEquipGearFunction();
            if (equip_gear_fn == 0) {
                return false;
            }
            reinterpret_cast<EquipGearFn>(equip_gear_fn)(
                equip_game_data,
                slot,
                reinterpret_cast<uintptr_t>(scratch.data()) + 0x10,
                inventory_index >= 0 ? inventory_index + tail_data_index : -1,
                1,
                1,
                0
            );
        } else {
            const uintptr_t equip_goods_fn = FindEquipGoodsFunction();
            if (equip_goods_fn == 0) {
                return false;
            }
            reinterpret_cast<EquipGoodsFn>(equip_goods_fn)(
                equip_game_data,
                slot - 22,
                reinterpret_cast<uintptr_t>(scratch.data()) + 0x10,
                inventory_index >= 0 ? inventory_index + tail_data_index : 0xFFFFFFFF
            );
        }
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool ReadMagicStatus(const Game::SingletonRegistry& singletons, int slot, int& slot_count, int& current_magic_id) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return false;
    }

    int unlocked_slots = 0;
    uintptr_t equip_magic_data = 0;
    if (!Game::ReadValue(player_game_data + kPlayerMagicSlotCountOffset, unlocked_slots) ||
        !Game::ReadValue(player_game_data + kEquipMagicDataOffset, equip_magic_data) ||
        equip_magic_data == 0) {
        return false;
    }

    slot_count = std::clamp(unlocked_slots, 0, 14);
    slot = std::clamp(slot, 0, std::max(slot_count - 1, 0));
    current_magic_id = -1;
    return Game::ReadValue(equip_magic_data + 0x10 + static_cast<uintptr_t>(slot) * 8, current_magic_id);
}

bool ChangeMagicSlot(const Game::SingletonRegistry& singletons, int magic_slot, std::int32_t requested_magic_id) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return false;
    }

    uintptr_t equip_inventory_data = 0;
    uintptr_t equip_magic_data = 0;
    int tail_data_index = 0;
    int unlocked_slots = 0;
    if (!Game::ReadValue(player_game_data + kEquipInventoryDataOffset, equip_inventory_data) ||
        !Game::ReadValue(player_game_data + kEquipMagicDataOffset, equip_magic_data) ||
        equip_inventory_data == 0 ||
        equip_magic_data == 0 ||
        !Game::ReadValue(equip_inventory_data + 0x1C, tail_data_index) ||
        !Game::ReadValue(player_game_data + kPlayerMagicSlotCountOffset, unlocked_slots)) {
        return false;
    }

    magic_slot = std::clamp(magic_slot, 0, std::clamp(unlocked_slots - 1, 0, 13));

    std::int32_t normalized_magic_id = requested_magic_id;
    if (normalized_magic_id >= 0) {
        normalized_magic_id = NormalizeGoodsId(normalized_magic_id);
    }

    for (int slot = 0; slot < 14; ++slot) {
        std::int32_t attuned_magic_id = -1;
        if (Game::ReadValue(equip_magic_data + 0x10 + static_cast<uintptr_t>(slot) * 8, attuned_magic_id) &&
            attuned_magic_id == normalized_magic_id) {
            return false;
        }
    }

    alignas(16) std::array<std::uint8_t, 128> scratch{};
    Game::WriteValue(reinterpret_cast<uintptr_t>(scratch.data()) + 0x8, magic_slot);

    if (normalized_magic_id >= 0) {
        int inventory_index = FindItemIndexInInventory(singletons, normalized_magic_id);
        if (inventory_index < 0) {
            if (!GiveSingleCustomItem(singletons, normalized_magic_id, 1, 0, 0, -1)) {
                return false;
            }
            inventory_index = FindItemIndexInInventory(singletons, normalized_magic_id);
        }
        if (inventory_index < 0) {
            return false;
        }

        const std::int32_t magic_param_id = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(normalized_magic_id) & 0x0FFFFFFFu
        );
        Game::WriteValue(reinterpret_cast<uintptr_t>(scratch.data()) + 0x48, inventory_index + tail_data_index - 1);
        Game::WriteValue(reinterpret_cast<uintptr_t>(scratch.data()) + 0x4C, normalized_magic_id);
        Game::WriteValue(
            reinterpret_cast<uintptr_t>(scratch.data()) + 0x50,
            static_cast<std::int32_t>(0xB0000000u | static_cast<std::uint32_t>(magic_param_id))
        );
    } else {
        Game::WriteValue(reinterpret_cast<uintptr_t>(scratch.data()) + 0x48, static_cast<std::int32_t>(0xFFFFFFFF));
        Game::WriteValue(reinterpret_cast<uintptr_t>(scratch.data()) + 0x4C, static_cast<std::int32_t>(0x4FFFFFFF));
        Game::WriteValue(reinterpret_cast<uintptr_t>(scratch.data()) + 0x50, static_cast<std::int32_t>(0xBFFFFFFF));
    }

    const uintptr_t change_magic_fn = FindChangeMagicFunction();
    if (change_magic_fn == 0) {
        return false;
    }

    __try {
        reinterpret_cast<ChangeMagicFn>(change_magic_fn)(magic_slot, reinterpret_cast<uintptr_t>(scratch.data()));
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool GetPlayerPositionAddress(const Game::SingletonRegistry& singletons, uintptr_t& address) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    return world_chr_man != 0 &&
           Game::ResolvePointerChain(
               world_chr_man,
               {kNetPlayersOffset, 0x0, kPlayerPositionRootOffset, kPlayerPositionNodeOffset},
               kPlayerPositionValueOffset,
               address);
}

bool ReadPlayerCoordinates(const Game::SingletonRegistry& singletons, float& x, float& y, float& z);

bool GetPlayerBaseAddress(const Game::SingletonRegistry& singletons, uintptr_t& address) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    return world_chr_man != 0 &&
           Game::ResolvePointerChain(world_chr_man, {kNetPlayersOffset, 0x0}, 0x0, address);
}

bool GetLocalPlayerSessionDataAddress(const Game::SingletonRegistry& singletons, uintptr_t& address) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    return world_chr_man != 0 &&
           Game::ResolvePointerChain(world_chr_man, {kNetPlayersOffset, 0x0, 0x580}, 0x0, address);
}

bool GetPlayerResourceBlockAddress(const Game::SingletonRegistry& singletons, uintptr_t& address) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    return world_chr_man != 0 &&
           Game::ResolvePointerChain(world_chr_man, {kNetPlayersOffset, 0x0, kPlayerPositionRootOffset}, 0x0, address);
}

bool ReadPlayerResourceValues(const Game::SingletonRegistry& singletons,
                              int& current_hp,
                              int& max_hp,
                              int& current_mp,
                              int& max_mp,
                              int& current_sp,
                              int& max_sp) {
    uintptr_t resource_block = 0;
    return GetPlayerResourceBlockAddress(singletons, resource_block) &&
           Game::ReadValue(resource_block + kPlayerHpOffset, current_hp) &&
           Game::ReadValue(resource_block + kPlayerMaxHpOffset, max_hp) &&
           Game::ReadValue(resource_block + kPlayerMpOffset, current_mp) &&
           Game::ReadValue(resource_block + kPlayerMaxMpOffset, max_mp) &&
           Game::ReadValue(resource_block + kPlayerSpOffset, current_sp) &&
           Game::ReadValue(resource_block + kPlayerMaxSpOffset, max_sp);
}

bool WritePlayerResourceValues(const Game::SingletonRegistry& singletons, int hp, int mp, int sp) {
    uintptr_t resource_block = 0;
    int max_hp = 0;
    int max_mp = 0;
    int max_sp = 0;
    if (!GetPlayerResourceBlockAddress(singletons, resource_block) ||
        !Game::ReadValue(resource_block + kPlayerMaxHpOffset, max_hp) ||
        !Game::ReadValue(resource_block + kPlayerMaxMpOffset, max_mp) ||
        !Game::ReadValue(resource_block + kPlayerMaxSpOffset, max_sp)) {
        return false;
    }

    hp = std::clamp(hp, 0, std::max(max_hp, 0));
    mp = std::clamp(mp, 0, std::max(max_mp, 0));
    sp = std::clamp(sp, 0, std::max(max_sp, 0));

    return Game::WriteValue(resource_block + kPlayerHpOffset, hp) &&
           Game::WriteValue(resource_block + kPlayerMpOffset, mp) &&
           Game::WriteValue(resource_block + kPlayerSpOffset, sp);
}

uintptr_t GetGestureEquipData(const Game::SingletonRegistry& singletons) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return 0;
    }

    uintptr_t gesture_equip_data = 0;
    return Game::ReadValue(player_game_data + kGestureEquipDataOffset, gesture_equip_data) ? gesture_equip_data : 0;
}

int ReadGestureSlot(const Game::SingletonRegistry& singletons, int slot) {
    const uintptr_t gesture_equip_data = GetGestureEquipData(singletons);
    if (gesture_equip_data == 0) {
        return -1;
    }

    slot = std::clamp(slot, 0, 6);
    int gesture_id = -1;
    return Game::ReadValue(gesture_equip_data + 0x8 + static_cast<uintptr_t>(slot) * 4, gesture_id)
               ? gesture_id
               : -1;
}

bool WriteGestureSlot(const Game::SingletonRegistry& singletons, int slot, int gesture_id) {
    const uintptr_t gesture_equip_data = GetGestureEquipData(singletons);
    if (gesture_equip_data == 0) {
        return false;
    }

    slot = std::clamp(slot, 0, 6);
    return Game::WriteValue(gesture_equip_data + 0x8 + static_cast<uintptr_t>(slot) * 4, gesture_id);
}

bool ReadVec3(uintptr_t address, float& x, float& y, float& z) {
    return Game::ReadValue(address + 0x0, x) &&
           Game::ReadValue(address + 0x4, z) &&
           Game::ReadValue(address + 0x8, y);
}

bool ReadPlayerGlobalCoordinates(const Game::SingletonRegistry& singletons, float& x, float& y, float& z) {
    uintptr_t player_base = 0;
    return GetPlayerBaseAddress(singletons, player_base) &&
           ReadVec3(player_base + kPlayerGlobalCoordsOffset, x, y, z);
}

bool ReadPlayerLastGoodGlobalCoordinates(const Game::SingletonRegistry& singletons, float& x, float& y, float& z) {
    uintptr_t player_base = 0;
    return GetPlayerBaseAddress(singletons, player_base) &&
           ReadVec3(player_base + kLastGoodGlobalCoordsOffset, x, y, z);
}

bool ReadPlayerChunkCoordinates(const Game::SingletonRegistry& singletons, float& x, float& y, float& z) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    uintptr_t chunk_address = 0;
    return world_chr_man != 0 &&
           Game::ResolvePointerChain(
               world_chr_man,
               {kNetPlayersOffset, 0x0, kPlayerPositionRootOffset, kPlayerPositionNodeOffset, 0xA8, 0x18},
               kChunkCoordsOffset,
               chunk_address) &&
           ReadVec3(chunk_address, x, y, z);
}

bool ReadPlayerInitPosition(const Game::SingletonRegistry& singletons, float& x, float& y, float& z) {
    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    return game_man != 0 && ReadVec3(game_man + kGameManInitPosOffset, x, y, z);
}

bool ReadPlayerMapContext(const Game::SingletonRegistry& singletons, float& player_rad, int& map_id) {
    uintptr_t player_base = 0;
    return GetPlayerBaseAddress(singletons, player_base) &&
           Game::ReadValue(player_base + kPlayerRadOffset, player_rad) &&
           Game::ReadValue(player_base + kPlayerMapIdOffset, map_id);
}

void RefreshCoordinateSnapshots(const Game::SingletonRegistry& singletons) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float player_rad = 0.0f;
    int map_id = 0;

    if (ReadPlayerCoordinates(singletons, x, y, z)) {
        Main::g_FeatureStatus.local_coord_x = x;
        Main::g_FeatureStatus.local_coord_y = y;
        Main::g_FeatureStatus.local_coord_z = z;
    }

    if (ReadPlayerGlobalCoordinates(singletons, x, y, z)) {
        Main::g_FeatureStatus.global_coord_x = x;
        Main::g_FeatureStatus.global_coord_y = y;
        Main::g_FeatureStatus.global_coord_z = z;
    }

    if (ReadPlayerChunkCoordinates(singletons, x, y, z)) {
        Main::g_FeatureStatus.chunk_coord_x = x;
        Main::g_FeatureStatus.chunk_coord_y = y;
        Main::g_FeatureStatus.chunk_coord_z = z;
    }

    if (ReadPlayerInitPosition(singletons, x, y, z)) {
        Main::g_FeatureStatus.init_pos_x = x;
        Main::g_FeatureStatus.init_pos_y = y;
        Main::g_FeatureStatus.init_pos_z = z;
    }

    if (ReadPlayerLastGoodGlobalCoordinates(singletons, x, y, z)) {
        Main::g_FeatureStatus.last_good_global_x = x;
        Main::g_FeatureStatus.last_good_global_y = y;
        Main::g_FeatureStatus.last_good_global_z = z;
    }

    if (ReadPlayerMapContext(singletons, player_rad, map_id)) {
        Main::g_FeatureStatus.player_rad = player_rad;
        Main::g_FeatureStatus.player_map_id = map_id;
    }
}

bool ReadPlayerCoordinates(const Game::SingletonRegistry& singletons, float& x, float& y, float& z) {
    uintptr_t position_address = 0;
    if (!GetPlayerPositionAddress(singletons, position_address)) {
        return false;
    }

    return Game::ReadValue(position_address + 0x0, x) &&
           Game::ReadValue(position_address + 0x4, z) &&
           Game::ReadValue(position_address + 0x8, y);
}

bool WritePlayerCoordinates(const Game::SingletonRegistry& singletons, float x, float y, float z) {
    uintptr_t position_address = 0;
    if (!GetPlayerPositionAddress(singletons, position_address)) {
        return false;
    }

    return Game::WriteValue(position_address + 0x0, x) &&
           Game::WriteValue(position_address + 0x4, z) &&
           Game::WriteValue(position_address + 0x8, y);
}

int GetEstusAllocation(const Game::SingletonRegistry& singletons, int flask_type) {
    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return -1;
    }

    std::uint8_t value = 0;
    const uintptr_t offset = (flask_type == 0) ? kHpFlaskAllocationOffset : kMpFlaskAllocationOffset;
    if (!Game::ReadValue(player_game_data + offset, value)) {
        return -1;
    }
    return static_cast<int>(value);
}

using AddSoulFn = int (*)(uintptr_t player_game_data, int value);
using EstusAllocationUpdateFn = void (*)(int flask_type, int new_quantity);
using ActivateBonfireFn = void (*)(int bonfire_level);
using ReplaceToolFn = void (*)(uintptr_t equip_game_data, int current_id, int replace_id, int quantity);

bool GiveRunes(const Game::SingletonRegistry& singletons, int amount) {
    if (amount <= 0) {
        return false;
    }

    static uintptr_t add_soul = 0;
    if (add_soul == 0) {
        add_soul = FindPatternInText(kAddSoulPattern);
    }
    if (add_soul == 0) {
        return false;
    }

    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return false;
    }

    __try {
        reinterpret_cast<AddSoulFn>(add_soul)(player_game_data, amount);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool AddFlaskCharge(const Game::SingletonRegistry& singletons, int flask_type) {
    static uintptr_t estus_allocation_update = 0;
    if (estus_allocation_update == 0) {
        estus_allocation_update = FindPatternInText(kEstusAllocationUpdatePattern);
    }
    if (estus_allocation_update == 0) {
        return false;
    }

    const int hp = GetEstusAllocation(singletons, 0);
    const int mp = GetEstusAllocation(singletons, 1);
    if (hp < 0 || mp < 0 || hp + mp > 13) {
        return false;
    }

    const int new_quantity = ((flask_type == 0) ? hp : mp) + 1;
    __try {
        reinterpret_cast<EstusAllocationUpdateFn>(estus_allocation_update)(flask_type, new_quantity);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool SaveRequest(const Game::SingletonRegistry& singletons) {
    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    if (game_man == 0) {
        return false;
    }

    const std::uint8_t enabled = 1;
    return Game::WriteValue(game_man + kSaveRequestOffset, enabled);
}

bool SetFlaskLevel(const Game::SingletonRegistry& singletons, int flask_level) {
    static uintptr_t activate_bonfire = 0;
    static uintptr_t replace_tool = 0;

    if (activate_bonfire == 0) {
        activate_bonfire = FindPatternInText(kActivateBonfirePattern);
    }
    if (replace_tool == 0) {
        const uintptr_t pattern_address = FindPatternInText(kReplaceToolPattern);
        if (pattern_address != 0) {
            replace_tool = pattern_address - 0x19;
        }
    }

    if (activate_bonfire == 0 || replace_tool == 0) {
        return false;
    }

    flask_level = std::clamp(flask_level, 0, 12);
    const int bonfire_level = flask_level + 1;

    std::int8_t total_bonfire_level = 0;
    const int32_t rel32 = *reinterpret_cast<const int32_t*>(activate_bonfire + 0x46);
    const uintptr_t total_bonfire_level_address = activate_bonfire + 0x44 + rel32 + 5;
    if (!Game::ReadValue(total_bonfire_level_address, total_bonfire_level)) {
        return false;
    }

    const uintptr_t player_game_data = GetPlayerGameData(singletons);
    if (player_game_data == 0) {
        return false;
    }

    const uintptr_t equip_game_data = player_game_data + kEquipGameDataOffset;
    const int current_flask_level = static_cast<int>(total_bonfire_level) - 1;
    const int flask_ids[] = {1000, 1001, 1050, 1051};

    __try {
        for (int base_id : flask_ids) {
            const int old_item = base_id + current_flask_level * 2;
            const int new_item = base_id + flask_level * 2;
            if (old_item != new_item) {
                reinterpret_cast<ReplaceToolFn>(replace_tool)(equip_game_data, old_item, new_item, 1);
            }
        }

        reinterpret_cast<ActivateBonfireFn>(activate_bonfire)(bonfire_level);
        return SaveRequest(singletons);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

}  // namespace

void GameActions::Tick(const Game::SingletonRegistry& singletons) {
    ERD_PROTECTED_STEP("GameActions.StartNextCycle", SyncStartNextCycle(singletons));
    ERD_PROTECTED_STEP("GameActions.GiveRunes", SyncGiveRunes(singletons));
    ERD_PROTECTED_STEP("GameActions.CustomItemGive", SyncCustomItemGive(singletons));
    ERD_PROTECTED_STEP("GameActions.MassItemGib", SyncMassItemGib(singletons));
    ERD_PROTECTED_STEP("GameActions.AddFlaskCharge", SyncAddFlaskCharge(singletons));
    ERD_PROTECTED_STEP("GameActions.SetFlaskLevel", SyncSetFlaskLevel(singletons));
    ERD_PROTECTED_STEP("GameActions.Coordinates", SyncCoordinates(singletons));
    ERD_PROTECTED_STEP("GameActions.CharacterEditor", SyncCharacterEditor(singletons));
    ERD_PROTECTED_STEP("GameActions.ResourceEditor", SyncResourceEditor(singletons));
    ERD_PROTECTED_STEP("GameActions.EquipmentEditor", SyncEquipmentEditor(singletons));
    ERD_PROTECTED_STEP("GameActions.MagicEditor", SyncMagicEditor(singletons));
    ERD_PROTECTED_STEP("GameActions.GestureEditor", SyncGestureEditor(singletons));
    ERD_PROTECTED_STEP("GameActions.EventFlagDebug", SyncEventFlagDebug(singletons));
}

void GameActions::SyncStartNextCycle(const Game::SingletonRegistry& singletons) {
    if (next_cycle_applied_ || !Main::g_FeatureStatus.start_next_cycle.load()) {
        return;
    }

    const uintptr_t game_man = singletons.GetObjectPointer("GameMan");
    if (game_man == 0) {
        return;
    }

    const std::uint8_t enabled = 1;
    if (Game::WriteValue(game_man + kStartNextCycleOffset, enabled)) {
        Main::Logger::Instance().Info("Start next cycle requested.");
        next_cycle_applied_ = true;
    }
}

void GameActions::SyncGiveRunes(const Game::SingletonRegistry& singletons) {
    if (!Main::g_FeatureStatus.give_runes_requested.exchange(false)) {
        return;
    }

    const int amount = std::max(Main::g_FeatureStatus.give_runes_amount.load(), 1);
    if (GiveRunes(singletons, amount)) {
        Main::Logger::Instance().Info(("Give Runes applied: " + std::to_string(amount)).c_str());
    } else {
        Main::Logger::Instance().Error("Give Runes failed.");
    }
}

void GameActions::SyncCustomItemGive(const Game::SingletonRegistry& singletons) {
    if (!Main::g_FeatureStatus.custom_item_give_requested.exchange(false)) {
        return;
    }

    const int item_id = Main::g_FeatureStatus.custom_item_id.load();
    const int quantity = std::clamp(Main::g_FeatureStatus.custom_item_quantity.load(), 1, 9999);
    const int reinforce_level = std::clamp(Main::g_FeatureStatus.custom_item_reinforce_level.load(), 0, 25);
    const int upgrade_value = std::clamp(Main::g_FeatureStatus.custom_item_upgrade_value.load(), 0, 1200);
    const int gem_id = Main::g_FeatureStatus.custom_item_gem_id.load();

    const bool success = GiveSingleCustomItem(singletons, item_id, quantity, reinforce_level, upgrade_value, gem_id);
    Main::g_FeatureStatus.custom_item_last_result = success ? 1 : -1;

    if (success) {
        Main::Logger::Instance().Info(
            ("Custom item give applied: id " + std::to_string(item_id) +
             " qty " + std::to_string(quantity)).c_str()
        );
    } else {
        Main::Logger::Instance().Error("Custom item give failed.");
    }
}

void GameActions::SyncMassItemGib(const Game::SingletonRegistry& singletons) {
    const int request_id = Main::g_FeatureStatus.mass_item_gib_request_id.exchange(0);
    if (request_id <= 0) {
        return;
    }

    const MassItemGibPackage* package = FindMassItemGibPackage(request_id);
    if (package == nullptr) {
        Main::g_FeatureStatus.mass_item_gib_last_package_id = request_id;
        Main::g_FeatureStatus.mass_item_gib_last_result = -1;
        Main::Logger::Instance().Error(("MassItemGib package id not found: " + std::to_string(request_id)).c_str());
        return;
    }

    const bool success = GiveItemPackage(singletons, *package);
    Main::g_FeatureStatus.mass_item_gib_last_package_id = package->id;
    Main::g_FeatureStatus.mass_item_gib_last_result = success ? 1 : -1;

    if (success) {
        Main::Logger::Instance().Info(
            ("MassItemGib applied: " + std::string(package->label) +
             " (" + std::to_string(package->item_count) + " entries)").c_str()
        );
    } else {
        Main::Logger::Instance().Error(("MassItemGib failed: " + std::string(package->label)).c_str());
    }
}

void GameActions::SyncAddFlaskCharge(const Game::SingletonRegistry& singletons) {
    if (!Main::g_FeatureStatus.add_flask_charge_requested.exchange(false)) {
        return;
    }

    const int flask_type = std::clamp(Main::g_FeatureStatus.add_flask_charge_type.load(), 0, 1);
    if (AddFlaskCharge(singletons, flask_type)) {
        Main::Logger::Instance().Info(flask_type == 0 ? "Added Crimson Tears flask charge." : "Added Cerulean Tears flask charge.");
    } else {
        Main::Logger::Instance().Error("Add flask charge failed.");
    }
}

void GameActions::SyncSetFlaskLevel(const Game::SingletonRegistry& singletons) {
    if (!Main::g_FeatureStatus.set_flask_level_requested.exchange(false)) {
        return;
    }

    const int flask_level = std::clamp(Main::g_FeatureStatus.set_flask_level_value.load(), 0, 12);
    if (SetFlaskLevel(singletons, flask_level)) {
        Main::Logger::Instance().Info(("Set flask level applied: +" + std::to_string(flask_level)).c_str());
    } else {
        Main::Logger::Instance().Error("Set flask level failed.");
    }
}

void GameActions::SyncCoordinates(const Game::SingletonRegistry& singletons) {
    RefreshCoordinateSnapshots(singletons);

    if (Main::g_FeatureStatus.teleport_read_player_coords_requested.exchange(false)) {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        if (ReadPlayerCoordinates(singletons, x, y, z)) {
            Main::g_FeatureStatus.teleport_coord_x = x;
            Main::g_FeatureStatus.teleport_coord_y = y;
            Main::g_FeatureStatus.teleport_coord_z = z;
            Main::Logger::Instance().Info(
                ("Read player coords: X " + std::to_string(x) +
                 " | Y " + std::to_string(y) +
                 " | Z " + std::to_string(z)).c_str()
            );
        } else {
            Main::Logger::Instance().Error("Read player coords failed.");
        }
    }

    if (Main::g_FeatureStatus.teleport_use_targeted_npc_coords_requested.exchange(false)) {
        if (Main::g_FeatureStatus.targeted_npc_valid.load()) {
            Main::g_FeatureStatus.teleport_coord_x = Main::g_FeatureStatus.targeted_npc_x.load();
            Main::g_FeatureStatus.teleport_coord_y = Main::g_FeatureStatus.targeted_npc_y.load();
            Main::g_FeatureStatus.teleport_coord_z = Main::g_FeatureStatus.targeted_npc_z.load();
            Main::Logger::Instance().Info("Loaded targeted NPC coords into teleport inputs.");
        } else {
            Main::Logger::Instance().Error("Targeted NPC coords unavailable.");
        }
    }

    if (Main::g_FeatureStatus.teleport_use_last_hit_npc_coords_requested.exchange(false)) {
        if (Main::g_FeatureStatus.last_hit_npc_valid.load()) {
            Main::g_FeatureStatus.teleport_coord_x = Main::g_FeatureStatus.last_hit_npc_x.load();
            Main::g_FeatureStatus.teleport_coord_y = Main::g_FeatureStatus.last_hit_npc_y.load();
            Main::g_FeatureStatus.teleport_coord_z = Main::g_FeatureStatus.last_hit_npc_z.load();
            Main::Logger::Instance().Info("Loaded last-hit NPC coords into teleport inputs.");
        } else {
            Main::Logger::Instance().Error("Last-hit NPC coords unavailable.");
        }
    }

    if (Main::g_FeatureStatus.teleport_use_global_coords_requested.exchange(false)) {
        Main::g_FeatureStatus.teleport_coord_x = Main::g_FeatureStatus.global_coord_x.load();
        Main::g_FeatureStatus.teleport_coord_y = Main::g_FeatureStatus.global_coord_y.load();
        Main::g_FeatureStatus.teleport_coord_z = Main::g_FeatureStatus.global_coord_z.load();
        Main::Logger::Instance().Info("Loaded current global coords into teleport inputs.");
    }

    if (Main::g_FeatureStatus.teleport_use_last_good_global_coords_requested.exchange(false)) {
        Main::g_FeatureStatus.teleport_coord_x = Main::g_FeatureStatus.last_good_global_x.load();
        Main::g_FeatureStatus.teleport_coord_y = Main::g_FeatureStatus.last_good_global_y.load();
        Main::g_FeatureStatus.teleport_coord_z = Main::g_FeatureStatus.last_good_global_z.load();
        Main::Logger::Instance().Info("Loaded last-good global coords into teleport inputs.");
    }

    if (Main::g_FeatureStatus.teleport_use_init_pos_coords_requested.exchange(false)) {
        Main::g_FeatureStatus.teleport_coord_x = Main::g_FeatureStatus.init_pos_x.load();
        Main::g_FeatureStatus.teleport_coord_y = Main::g_FeatureStatus.init_pos_y.load();
        Main::g_FeatureStatus.teleport_coord_z = Main::g_FeatureStatus.init_pos_z.load();
        Main::Logger::Instance().Info("Loaded init position coords into teleport inputs.");
    }

    if (!Main::g_FeatureStatus.teleport_player_to_input_coords_requested.exchange(false)) {
        return;
    }

    const float x = Main::g_FeatureStatus.teleport_coord_x.load();
    const float y = Main::g_FeatureStatus.teleport_coord_y.load();
    const float z = Main::g_FeatureStatus.teleport_coord_z.load();
    if (WritePlayerCoordinates(singletons, x, y, z)) {
        Main::Logger::Instance().Info(
            ("Teleported player to coords: X " + std::to_string(x) +
             " | Y " + std::to_string(y) +
             " | Z " + std::to_string(z)).c_str()
        );
    } else {
        Main::Logger::Instance().Error("Teleport player to coords failed.");
    }
}

void GameActions::SyncCharacterEditor(const Game::SingletonRegistry& singletons) {
    if (Main::g_FeatureStatus.character_read_requested.exchange(false)) {
        CharacterEditorValues values{};
        if (ReadCharacterEditorValues(singletons, values)) {
            Main::g_FeatureStatus.character_level = values.level;
            Main::g_FeatureStatus.character_vigor = values.vigor;
            Main::g_FeatureStatus.character_mind = values.mind;
            Main::g_FeatureStatus.character_endurance = values.endurance;
            Main::g_FeatureStatus.character_strength = values.strength;
            Main::g_FeatureStatus.character_dexterity = values.dexterity;
            Main::g_FeatureStatus.character_intelligence = values.intelligence;
            Main::g_FeatureStatus.character_faith = values.faith;
            Main::g_FeatureStatus.character_arcane = values.arcane;
            Main::g_FeatureStatus.character_editor_valid = true;
            Main::Logger::Instance().Info("Character editor values refreshed from PlayerGameData.");
        } else {
            Main::g_FeatureStatus.character_editor_valid = false;
            Main::Logger::Instance().Error("Character editor read failed.");
        }
    }

    if (!Main::g_FeatureStatus.character_apply_requested.exchange(false)) {
        return;
    }

    const CharacterEditorValues values{
        std::clamp(Main::g_FeatureStatus.character_level.load(), 1, 713),
        std::clamp(Main::g_FeatureStatus.character_vigor.load(), 1, 99),
        std::clamp(Main::g_FeatureStatus.character_mind.load(), 1, 99),
        std::clamp(Main::g_FeatureStatus.character_endurance.load(), 1, 99),
        std::clamp(Main::g_FeatureStatus.character_strength.load(), 1, 99),
        std::clamp(Main::g_FeatureStatus.character_dexterity.load(), 1, 99),
        std::clamp(Main::g_FeatureStatus.character_intelligence.load(), 1, 99),
        std::clamp(Main::g_FeatureStatus.character_faith.load(), 1, 99),
        std::clamp(Main::g_FeatureStatus.character_arcane.load(), 1, 99),
    };

    if (WriteCharacterEditorValues(singletons, values) && SaveRequest(singletons)) {
        Main::g_FeatureStatus.character_editor_valid = true;
        Main::Logger::Instance().Info("Character editor values written.");
    } else {
        Main::Logger::Instance().Error("Character editor apply failed.");
    }
}

void GameActions::SyncResourceEditor(const Game::SingletonRegistry& singletons) {
    int current_hp = 0;
    int max_hp = 0;
    int current_mp = 0;
    int max_mp = 0;
    int current_sp = 0;
    int max_sp = 0;
    if (ReadPlayerResourceValues(singletons, current_hp, max_hp, current_mp, max_mp, current_sp, max_sp)) {
        Main::g_FeatureStatus.current_hp = current_hp;
        Main::g_FeatureStatus.max_hp = max_hp;
        Main::g_FeatureStatus.current_mp = current_mp;
        Main::g_FeatureStatus.max_mp = max_mp;
        Main::g_FeatureStatus.current_sp = current_sp;
        Main::g_FeatureStatus.max_sp = max_sp;
    }

    if (Main::g_FeatureStatus.resource_read_requested.exchange(false)) {
        Main::g_FeatureStatus.edit_hp = Main::g_FeatureStatus.current_hp.load();
        Main::g_FeatureStatus.edit_mp = Main::g_FeatureStatus.current_mp.load();
        Main::g_FeatureStatus.edit_sp = Main::g_FeatureStatus.current_sp.load();
        Main::Logger::Instance().Info("Resource editor values refreshed.");
    }

    if (!Main::g_FeatureStatus.resource_apply_requested.exchange(false)) {
        return;
    }

    const int hp = Main::g_FeatureStatus.edit_hp.load();
    const int mp = Main::g_FeatureStatus.edit_mp.load();
    const int sp = Main::g_FeatureStatus.edit_sp.load();
    if (WritePlayerResourceValues(singletons, hp, mp, sp) && SaveRequest(singletons)) {
        Main::Logger::Instance().Info("Player HP / FP / SP written.");
    } else {
        Main::Logger::Instance().Error("Player HP / FP / SP write failed.");
    }
}

void GameActions::SyncEquipmentEditor(const Game::SingletonRegistry& singletons) {
    const int slot = std::clamp(Main::g_FeatureStatus.equip_slot.load(), 0, 38);
    Main::g_FeatureStatus.equip_current_item_id = ReadCurrentEquipSlotItemId(singletons, slot);

    if (Main::g_FeatureStatus.equip_unequip_requested.exchange(false)) {
        if (EquipItemById(singletons, slot, -1) && SaveRequest(singletons)) {
            Main::Logger::Instance().Info(("Unequipped slot " + std::to_string(slot) + ".").c_str());
        } else {
            Main::Logger::Instance().Error("Unequip slot failed.");
        }
        Main::g_FeatureStatus.equip_current_item_id = ReadCurrentEquipSlotItemId(singletons, slot);
    }

    if (!Main::g_FeatureStatus.equip_apply_requested.exchange(false)) {
        return;
    }

    const int item_id = Main::g_FeatureStatus.equip_item_id.load();
    if (EquipItemById(singletons, slot, item_id) && SaveRequest(singletons)) {
        Main::Logger::Instance().Info(
            ("Equipped slot " + std::to_string(slot) + " with item " + std::to_string(item_id) + ".").c_str()
        );
    } else {
        Main::Logger::Instance().Error("Equip by item ID failed.");
    }
    Main::g_FeatureStatus.equip_current_item_id = ReadCurrentEquipSlotItemId(singletons, slot);
}

void GameActions::SyncMagicEditor(const Game::SingletonRegistry& singletons) {
    int slot_count = 0;
    int current_magic_id = -1;
    if (ReadMagicStatus(singletons, std::clamp(Main::g_FeatureStatus.magic_slot.load(), 0, 13), slot_count, current_magic_id)) {
        Main::g_FeatureStatus.magic_slot_count = slot_count;
        Main::g_FeatureStatus.magic_current_item_id = current_magic_id;
    } else {
        Main::g_FeatureStatus.magic_slot_count = 0;
        Main::g_FeatureStatus.magic_current_item_id = -1;
    }

    if (Main::g_FeatureStatus.magic_remove_requested.exchange(false)) {
        const int slot = std::clamp(Main::g_FeatureStatus.magic_slot.load(), 0, 13);
        if (ChangeMagicSlot(singletons, slot, -1) && SaveRequest(singletons)) {
            Main::Logger::Instance().Info(("Removed magic from slot " + std::to_string(slot) + ".").c_str());
        } else {
            Main::Logger::Instance().Error("Remove magic failed.");
        }
    }

    if (!Main::g_FeatureStatus.magic_attune_requested.exchange(false)) {
        return;
    }

    const int slot = std::clamp(Main::g_FeatureStatus.magic_slot.load(), 0, 13);
    const int magic_id = Main::g_FeatureStatus.magic_item_id.load();
    if (ChangeMagicSlot(singletons, slot, magic_id) && SaveRequest(singletons)) {
        Main::Logger::Instance().Info(
            ("Attuned magic " + std::to_string(magic_id) + " to slot " + std::to_string(slot) + ".").c_str()
        );
    } else {
        Main::Logger::Instance().Error("Attune magic failed.");
    }
}

void GameActions::SyncGestureEditor(const Game::SingletonRegistry& singletons) {
    const int slot = std::clamp(Main::g_FeatureStatus.gesture_slot.load(), 0, 6);
    Main::g_FeatureStatus.gesture_current_item_id = ReadGestureSlot(singletons, slot);

    if (!Main::g_FeatureStatus.gesture_apply_requested.exchange(false)) {
        return;
    }

    const int gesture_id = Main::g_FeatureStatus.gesture_item_id.load();
    if (WriteGestureSlot(singletons, slot, gesture_id) && SaveRequest(singletons)) {
        Main::Logger::Instance().Info(
            ("Gesture slot " + std::to_string(slot + 1) + " set to " + std::to_string(gesture_id) + ".").c_str()
        );
    } else {
        Main::Logger::Instance().Error("Gesture editor apply failed.");
    }
}

void GameActions::SyncEventFlagDebug(const Game::SingletonRegistry& singletons) {
    const std::uint32_t flag_id = static_cast<std::uint32_t>(std::max(Main::g_FeatureStatus.debug_event_flag_id.load(), 0));

    if (Main::g_FeatureStatus.debug_event_flag_read_requested.exchange(false)) {
        bool enabled = false;
        if (Game::GetEventFlag(singletons, flag_id, enabled)) {
            Main::g_FeatureStatus.debug_event_flag_current_state = enabled ? 1 : 0;
            Main::Logger::Instance().Info(("Event Flag read: " + std::to_string(flag_id) + " = " + (enabled ? "ON" : "OFF")).c_str());
        } else {
            Main::g_FeatureStatus.debug_event_flag_current_state = -1;
            Main::Logger::Instance().Error("Event Flag read failed.");
        }
    }

    if (Main::g_FeatureStatus.debug_event_flag_write_requested.exchange(false)) {
        const bool desired_state = Main::g_FeatureStatus.debug_event_flag_desired_state.load();
        if (Game::SetEventFlag(singletons, flag_id, desired_state)) {
            Main::g_FeatureStatus.debug_event_flag_current_state = desired_state ? 1 : 0;
            Main::Logger::Instance().Info(("Event Flag write: " + std::to_string(flag_id) + " -> " + (desired_state ? "ON" : "OFF")).c_str());
        } else {
            Main::Logger::Instance().Error("Event Flag write failed.");
        }
    }
}

}  // namespace ERD::Features
