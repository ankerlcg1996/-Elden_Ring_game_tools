#pragma once

#include "Game/SingletonRegistry.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace grace_test::runtime {

enum class UnlockMapsMode {
    All = 0,
    BaseGame = 1,
    Dlc = 2,
};

void run(const std::filesystem::path& folder);
void request_stop();
const ERD::Game::SingletonRegistry* get_singletons();
bool unlock_all_whetblades();
bool unlock_all_gestures();
bool unlock_maps(UnlockMapsMode mode);

struct BossReviveMenuItem {
    int id = -1;
    std::string area;
    std::string boss_name;
};

struct QuickAccessShopMenuItem {
    int id = -1;
    std::string name;
};

std::vector<BossReviveMenuItem> get_boss_revive_menu_items();
std::vector<QuickAccessShopMenuItem> get_quick_access_shop_menu_items();
bool revive_boss_by_menu_id(int menu_id);
bool warp_boss_by_menu_id(int menu_id);
bool open_quick_access_upgrade();
bool open_quick_access_spirit_tuning();
bool open_quick_access_dragon_communion_church();
bool open_quick_access_dragon_communion_cathedral();
bool open_quick_access_rebirth();
bool open_quick_access_mirror();
bool open_quick_access_sell();
bool open_quick_access_shop_by_menu_id(int menu_id);

}  // namespace grace_test::runtime
