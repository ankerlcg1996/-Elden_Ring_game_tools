#include "Unlocks.hpp"

#include "UnlockData.hpp"

#include "../Game/EventFlags.hpp"
#include "../Game/Memory.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"

namespace ERD::Features {
namespace {

template <std::size_t BaseCount, std::size_t DlcCount>
bool ApplyBaseAndOptionalDlcFlags(
    const Game::SingletonRegistry& singletons,
    const std::array<std::uint32_t, BaseCount>& base_flags,
    const std::array<std::uint32_t, DlcCount>& dlc1_flags) {
    if (!Game::ApplyFlagList(singletons, base_flags)) {
        return false;
    }

    if (!Game::IsDlcOwned(singletons, 1)) {
        return true;
    }

    return Game::ApplyFlagList(singletons, dlc1_flags);
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

bool Unlocks::UnlockAllGestures(const Game::SingletonRegistry& singletons) const {
    const uintptr_t game_data_man = singletons.GetObjectPointer("GameDataMan");
    if (game_data_man == 0) {
        return false;
    }

    uintptr_t player_game_data = 0;
    if (!Game::ReadValue(game_data_man + 0x8, player_game_data) || player_game_data == 0) {
        return false;
    }

    uintptr_t gesture_game_data = 0;
    if (!Game::ReadValue(player_game_data + 0x8D8, gesture_game_data) || gesture_game_data == 0) {
        return false;
    }

    std::array<std::int32_t, UnlockData::kGestureIdsBase.size()> base_gestures = UnlockData::kGestureIdsBase;
    base_gestures.back() = Game::IsDlcOwned(singletons, 0)
                               ? UnlockData::kTheRingGestureIdOwned
                               : UnlockData::kTheRingGestureIdFallback;

    std::array<std::int32_t, UnlockData::kGestureIdsDlc1.size()> dlc_gestures = UnlockData::kGestureIdsDlc1;
    dlc_gestures[2] = Game::IsDlcOwned(singletons, 2)
                          ? UnlockData::kRingOfMiquellaGestureIdOwned
                          : UnlockData::kRingOfMiquellaGestureIdFallback;

    std::size_t slot = 0;
    for (std::int32_t gesture_id : base_gestures) {
        if (!Game::WriteValue(gesture_game_data + 0x8 + slot * sizeof(std::int32_t), gesture_id)) {
            return false;
        }
        ++slot;
    }

    if (Game::IsDlcOwned(singletons, 1)) {
        for (std::int32_t gesture_id : dlc_gestures) {
            if (!Game::WriteValue(gesture_game_data + 0x8 + slot * sizeof(std::int32_t), gesture_id)) {
                return false;
            }
            ++slot;
        }
    }

    return true;
}

void Unlocks::ResetSessionState() {
    maps_applied_ = false;
    cookbooks_applied_ = false;
    whetblades_applied_ = false;
    graces_applied_ = false;
    summoning_pools_applied_ = false;
    colosseums_applied_ = false;
    gestures_applied_ = false;
}

void Unlocks::Tick(const Game::SingletonRegistry& singletons) {
    ERD_PROTECTED_STEP(
        "Unlocks.Maps",
        if (!maps_applied_ &&
            Main::g_FeatureStatus.unlock_all_maps.load() &&
            ApplyBaseAndOptionalDlcFlags(singletons, UnlockData::kMapFlagsBase, UnlockData::kMapFlagsDlc1)) {
            Main::Logger::Instance().Info("Unlock all maps applied.");
            maps_applied_ = true;
        }
    );

    ERD_PROTECTED_STEP(
        "Unlocks.Cookbooks",
        if (!cookbooks_applied_ &&
            Main::g_FeatureStatus.unlock_all_cookbooks.load() &&
            ApplyBaseAndOptionalDlcFlags(singletons, UnlockData::kCookbookFlagsBase, UnlockData::kCookbookFlagsDlc1)) {
            Main::Logger::Instance().Info("Unlock all cookbooks applied.");
            cookbooks_applied_ = true;
        }
    );

    ERD_PROTECTED_STEP(
        "Unlocks.Whetblades",
        if (!whetblades_applied_ &&
            Main::g_FeatureStatus.unlock_all_whetblades.load() &&
            Game::ApplyFlagList(singletons, UnlockData::kWhetbladeFlagsBase)) {
            Main::Logger::Instance().Info("Unlock all whetblades applied.");
            whetblades_applied_ = true;
        }
    );

    ERD_PROTECTED_STEP(
        "Unlocks.Graces",
        if (!graces_applied_ &&
            Main::g_FeatureStatus.unlock_all_graces.load() &&
            ApplyBaseAndOptionalDlcFlags(singletons, UnlockData::kGraceFlagsBase, UnlockData::kGraceFlagsDlc1)) {
            Main::Logger::Instance().Info("Unlock all graces applied.");
            graces_applied_ = true;
        }
    );

    ERD_PROTECTED_STEP(
        "Unlocks.SummoningPools",
        if (!summoning_pools_applied_ &&
            Main::g_FeatureStatus.unlock_all_summoning_pools.load() &&
            ApplyBaseAndOptionalDlcFlags(
                singletons, UnlockData::kSummoningPoolFlagsBase, UnlockData::kSummoningPoolFlagsDlc1)) {
            Main::Logger::Instance().Info("Unlock all summoning pools applied.");
            summoning_pools_applied_ = true;
        }
    );

    ERD_PROTECTED_STEP(
        "Unlocks.Colosseums",
        if (!colosseums_applied_ &&
            Main::g_FeatureStatus.unlock_all_colosseums.load() &&
            Game::ApplyFlagList(singletons, UnlockData::kColosseumFlagsBase)) {
            Main::Logger::Instance().Info("Unlock all colosseums applied.");
            colosseums_applied_ = true;
        }
    );

    ERD_PROTECTED_STEP(
        "Unlocks.Gestures",
        if (!gestures_applied_ &&
            Main::g_FeatureStatus.unlock_all_gestures.load() &&
            UnlockAllGestures(singletons)) {
            Main::Logger::Instance().Info("Unlock all gestures applied.");
            gestures_applied_ = true;
        }
    );
}

}  // namespace ERD::Features
