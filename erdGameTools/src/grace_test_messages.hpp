#pragma once

#include <string>
#include <string_view>

namespace grace_test::msg {

static constexpr int event_text_for_talk_root = 69010000;
static constexpr int event_text_for_talk_game_mods = 69010001;
static constexpr int event_text_for_talk_quick_access = 69010002;
static constexpr int event_text_for_talk_back = 69010003;
static constexpr int event_text_for_talk_no_entries = 69010004;
static constexpr int event_text_for_talk_unlocks = 69010005;
static constexpr int event_text_for_talk_toggle_posture_root = 69010006;
static constexpr int event_text_for_talk_toggle_damage_root = 69010007;

static constexpr int event_text_for_talk_toggle_weightless = 69010100;
static constexpr int event_text_for_talk_toggle_no_item_consume = 69010101;
static constexpr int event_text_for_talk_toggle_no_damage = 69010102;
static constexpr int event_text_for_talk_toggle_no_stamina = 69010103;
static constexpr int event_text_for_talk_toggle_free_purchase = 69010104;
static constexpr int event_text_for_talk_toggle_no_craft_cost = 69010105;
static constexpr int event_text_for_talk_toggle_no_upgrade_cost = 69010106;
static constexpr int event_text_for_talk_toggle_no_magic_req = 69010107;
static constexpr int event_text_for_talk_toggle_one_slot_magic = 69010108;
static constexpr int event_text_for_talk_toggle_mount_anywhere = 69010109;
static constexpr int event_text_for_talk_toggle_spirit_anywhere = 69010110;
static constexpr int event_text_for_talk_toggle_posture_bars = 69010111;
static constexpr int event_text_for_talk_unlock_maps = 69010120;
static constexpr int event_text_for_talk_unlock_cookbooks = 69010121;
static constexpr int event_text_for_talk_unlock_whetblades = 69010122;
static constexpr int event_text_for_talk_unlock_graces = 69010123;
static constexpr int event_text_for_talk_unlock_summoning_pools = 69010124;
static constexpr int event_text_for_talk_unlock_colosseums = 69010125;
static constexpr int event_text_for_talk_unlock_gestures = 69010126;

static constexpr int event_text_for_talk_quick_access_base = 69011000;
static constexpr int event_text_for_talk_sort_chest = 15000395;

void initialize();
std::wstring get_vanilla_message(unsigned int bnd_id, int msg_id);
std::string detect_game_text_language();

}  // namespace grace_test::msg
