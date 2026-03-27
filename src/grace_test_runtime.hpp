#pragma once

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
bool unlock_all_whetblades();
bool unlock_all_gestures();
bool unlock_maps(UnlockMapsMode mode);
bool open_portable_upgrade();
bool open_portable_rebirth();
bool open_portable_mirror();

struct BossReviveMenuItem {
    int id = -1;
    std::string area;
    std::string boss_name;
};

std::vector<BossReviveMenuItem> get_boss_revive_menu_items();
bool revive_boss_by_menu_id(int menu_id);
bool warp_boss_by_menu_id(int menu_id);

}  // namespace grace_test::runtime
