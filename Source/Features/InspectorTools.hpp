#pragma once

#include "../Common.hpp"

#include "../Game/SingletonRegistry.hpp"

namespace ERD::Features {

struct SessionPlayerSnapshot {
    bool valid = false;
    std::string name;
    int hp = 0;
    int max_hp = 0;
    int level = 0;
    int reinforce_level = 0;
    int character_type = 0;
    int team_type = 0;
};

struct PartyMemberSlotSnapshot {
    std::uint64_t handle = 0;
    int type = 0;
    int state = 0;
    int init_flag = 0;
    int end_flag = 0;
};

struct InventoryEntrySnapshot {
    std::int32_t item_id = -1;
    int quantity = 0;
    bool repository = false;
};

struct ParamTableSnapshot {
    std::string name_utf8;
    std::wstring name_wide;
    int row_count = 0;
};

struct ParamValuePreviewSnapshot {
    bool valid = false;
    std::string value_text;
    std::string type_text;
    uintptr_t row_address = 0;
};

struct ParamScriptOperation {
    std::wstring param_name;
    std::uint64_t row_id = 0;
    std::size_t offset = 0;
    std::string type;
    std::string value;
    std::string description;
};

struct ParamScriptSnapshot {
    int id = 0;
    std::string group;
    std::string label;
    std::string description;
    std::vector<ParamScriptOperation> operations;
};

struct InspectorSnapshot {
    bool game_ready = false;
    uintptr_t world_chr_man = 0;
    uintptr_t game_man = 0;
    uintptr_t game_data_man = 0;
    uintptr_t cs_menu_man = 0;
    uintptr_t cs_regulation_manager = 0;

    std::string character_name;
    int level = 0;
    int current_hp = 0;
    int max_hp = 0;
    int current_fp = 0;
    int max_fp = 0;
    int current_sp = 0;
    int max_sp = 0;
    int character_type = 0;
    int team_type = 0;
    int clear_count = 0;
    int rune_arc = 0;
    int scadutree_blessing = 0;
    int revered_spirit_ash_blessing = 0;
    int save_slot = -1;
    int pending_load_save_slot = -1;
    int last_grace = -1;
    int great_rune = 0;
    bool disable_auto_save = false;

    int party_white_ghosts = 0;
    int party_black_ghosts = 0;
    int party_connected_members = 0;
    int party_npc_members = 0;
    int party_members_created = 0;
    int party_detective_ghosts = 0;

    int current_weapon_slot_left = 0;
    int current_weapon_slot_right = 0;
    std::array<std::int32_t, 6> equipped_weapons{};
    std::array<std::int32_t, 10> quick_items{};
    std::array<std::int32_t, 6> pouch_items{};

    std::array<PartyMemberSlotSnapshot, 6> party_slots{};
    SessionPlayerSnapshot local_player;
    std::array<SessionPlayerSnapshot, 4> net_players{};

    std::vector<InventoryEntrySnapshot> weapon_inventory;
    std::vector<InventoryEntrySnapshot> protector_inventory;
    std::vector<InventoryEntrySnapshot> accessory_inventory;
    std::vector<InventoryEntrySnapshot> goods_inventory;

    std::vector<ParamTableSnapshot> param_tables;
    ParamValuePreviewSnapshot param_preview;
    std::vector<ParamScriptSnapshot> param_scripts;
    std::string param_script_error;
};

class InspectorTools {
public:
    void Tick(const Game::SingletonRegistry& singletons);
    InspectorSnapshot Snapshot() const;
    static InspectorTools* Instance();

private:
    void RefreshSnapshot(const Game::SingletonRegistry& singletons);
    void RefreshHeroAndStatistics(const Game::SingletonRegistry& singletons, InspectorSnapshot& next);
    void RefreshInventory(const Game::SingletonRegistry& singletons, InspectorSnapshot& next);
    void RefreshParamTables(const Game::SingletonRegistry& singletons, InspectorSnapshot& next);
    void LoadParamScripts();
    void SyncSaveLoadTools(const Game::SingletonRegistry& singletons);
    void SyncParamTools(const Game::SingletonRegistry& singletons);

    mutable std::mutex snapshot_mutex_;
    InspectorSnapshot snapshot_;
    ULONGLONG next_refresh_tick_ = 0;
    fs::path loaded_param_script_path_;
    fs::file_time_type loaded_param_script_time_{};

    static InspectorTools* instance_;
};

InspectorSnapshot GetInspectorSnapshot();

}  // namespace ERD::Features
