#pragma once

#include <string>
#include <string_view>

namespace grace_test::msg {

static constexpr int event_text_for_talk_root = 69010000;
static constexpr int event_text_for_talk_game_mods = 69010001;
static constexpr int event_text_for_talk_player_mods = 69010002;
static constexpr int event_text_for_talk_back = 69010003;
static constexpr int event_text_for_talk_unlocks = 69010005;
static constexpr int event_text_for_talk_toggle_posture_root = 69010006;
static constexpr int event_text_for_talk_portable_grace = 69010011;
static constexpr int event_text_for_talk_damage_multiplier_up = 69010012;
static constexpr int event_text_for_talk_damage_multiplier_cut = 69010013;
static constexpr int event_text_for_talk_damage_multiplier_reduce = 69010014;
static constexpr int event_text_for_talk_damage_multiplier_taken = 69010015;
static constexpr int event_text_for_talk_player_speed_increase = 69010016;
static constexpr int event_text_for_talk_enemy_mods = 69010017;
static constexpr int event_text_for_talk_enemy_mods_empty = 69010018;
static constexpr int event_text_for_talk_enemy_hp_increase = 69010019;
static constexpr int event_text_for_talk_enemy_hp_decrease = 69010020;
static constexpr int event_text_for_talk_boss_revive = 69010021;
static constexpr int event_text_for_talk_boss_revive_action = 69010022;
static constexpr int event_text_for_talk_boss_revive_do_revive = 69010023;
static constexpr int event_text_for_talk_boss_revive_do_warp = 69010024;
static constexpr int event_text_for_talk_buff_duration_extend = 69010025;
static constexpr int event_text_for_talk_equipment_weight_reduction = 69010008;
static constexpr int event_text_for_talk_easier_parry = 69010009;
static constexpr int event_text_for_talk_weapon_requirement_reduction = 69010010;

static constexpr int event_text_for_talk_toggle_weightless = 69010100;
static constexpr int event_text_for_talk_toggle_no_item_consume = 69010101;
static constexpr int event_text_for_talk_toggle_no_damage = 69010102;
static constexpr int event_text_for_talk_toggle_free_purchase = 69010104;
static constexpr int event_text_for_talk_toggle_no_craft_cost = 69010105;
static constexpr int event_text_for_talk_toggle_no_upgrade_cost = 69010106;
static constexpr int event_text_for_talk_toggle_no_magic_req = 69010107;
static constexpr int event_text_for_talk_toggle_one_slot_magic = 69010108;
static constexpr int event_text_for_talk_toggle_mount_anywhere = 69010109;
static constexpr int event_text_for_talk_toggle_spirit_anywhere = 69010110;
static constexpr int event_text_for_talk_toggle_posture_bars = 69010111;
static constexpr int event_text_for_talk_toggle_torrent_no_death = 69010112;
static constexpr int event_text_for_talk_toggle_torrent_anywhere = 69010113;
static constexpr int event_text_for_talk_toggle_silent_footsteps = 69010114;
static constexpr int event_text_for_talk_toggle_all_weapons_enchantable = 69010115;
static constexpr int event_text_for_talk_toggle_infinite_jump = 69010116;
static constexpr int event_text_for_talk_toggle_beast_not_hostile = 69010117;
static constexpr int event_text_for_talk_toggle_infinite_consumables = 69010133;
static constexpr int event_text_for_talk_toggle_infinite_arrows = 69010134;
static constexpr int event_text_for_talk_toggle_infinite_stamina = 69010135;
static constexpr int event_text_for_talk_toggle_infinite_fp = 69010136;
static constexpr int event_text_for_talk_toggle_no_rune_loss_on_death = 69010137;
static constexpr int event_text_for_talk_toggle_all_weapons_ash_of_war_changeable = 69010138;
static constexpr int event_text_for_talk_unlock_maps = 69010120;
static constexpr int event_text_for_talk_unlock_cookbooks = 69010121;
static constexpr int event_text_for_talk_unlock_whetblades = 69010122;
static constexpr int event_text_for_talk_unlock_graces = 69010123;
static constexpr int event_text_for_talk_unlock_summoning_pools = 69010124;
static constexpr int event_text_for_talk_unlock_colosseums = 69010125;
static constexpr int event_text_for_talk_unlock_gestures = 69010126;
static constexpr int event_text_for_talk_unlock_maps_all = 69010127;
static constexpr int event_text_for_talk_unlock_maps_base = 69010128;
static constexpr int event_text_for_talk_unlock_maps_dlc = 69010129;
static constexpr int event_text_for_talk_portable_upgrade = 69010130;
static constexpr int event_text_for_talk_portable_rebirth = 69010131;
static constexpr int event_text_for_talk_portable_mirror = 69010132;
static constexpr int event_text_for_talk_equipment_weight_reduction_base = 69011000;
static constexpr int event_text_for_talk_easier_parry_base = 69011100;
static constexpr int event_text_for_talk_weapon_requirement_reduction_base = 69011200;
static constexpr int event_text_for_talk_damage_multiplier_base = 69011300;
static constexpr int event_text_for_talk_damage_cut_multiplier_base = 69011400;
static constexpr int event_text_for_talk_damage_reduce_multiplier_base = 69011500;
static constexpr int event_text_for_talk_damage_taken_multiplier_base = 69011600;
static constexpr int event_text_for_talk_player_speed_increase_base = 69011700;
static constexpr int event_text_for_talk_enemy_hp_increase_base = 69011800;
static constexpr int event_text_for_talk_enemy_hp_decrease_base = 69011900;
static constexpr int event_text_for_talk_buff_duration_extend_base = 69014000;

static constexpr int event_text_for_talk_sort_chest = 15000395;

void initialize();
std::wstring get_vanilla_message(unsigned int bnd_id, int msg_id);
std::string detect_game_text_language();

}  // namespace grace_test::msg
