#include "Unlocks.hpp"

#include "UnlockData.hpp"

#include "../Game/EventFlags.hpp"
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

}  // namespace

void Unlocks::Tick(const Game::SingletonRegistry& singletons) {
    if (!maps_applied_ &&
        Main::g_FeatureStatus.unlock_all_maps.load() &&
        ApplyBaseAndOptionalDlcFlags(singletons, UnlockData::kMapFlagsBase, UnlockData::kMapFlagsDlc1)) {
        Main::Logger::Instance().Info("Unlock all maps applied.");
        maps_applied_ = true;
    }

    if (!cookbooks_applied_ &&
        Main::g_FeatureStatus.unlock_all_cookbooks.load() &&
        ApplyBaseAndOptionalDlcFlags(singletons, UnlockData::kCookbookFlagsBase, UnlockData::kCookbookFlagsDlc1)) {
        Main::Logger::Instance().Info("Unlock all cookbooks applied.");
        cookbooks_applied_ = true;
    }

    if (!whetblades_applied_ &&
        Main::g_FeatureStatus.unlock_all_whetblades.load() &&
        Game::ApplyFlagList(singletons, UnlockData::kWhetbladeFlagsBase)) {
        Main::Logger::Instance().Info("Unlock all whetblades applied.");
        whetblades_applied_ = true;
    }

    if (!graces_applied_ &&
        Main::g_FeatureStatus.unlock_all_graces.load() &&
        ApplyBaseAndOptionalDlcFlags(singletons, UnlockData::kGraceFlagsBase, UnlockData::kGraceFlagsDlc1)) {
        Main::Logger::Instance().Info("Unlock all graces applied.");
        graces_applied_ = true;
    }

    if (!summoning_pools_applied_ &&
        Main::g_FeatureStatus.unlock_all_summoning_pools.load() &&
        ApplyBaseAndOptionalDlcFlags(
            singletons, UnlockData::kSummoningPoolFlagsBase, UnlockData::kSummoningPoolFlagsDlc1)) {
        Main::Logger::Instance().Info("Unlock all summoning pools applied.");
        summoning_pools_applied_ = true;
    }

    if (!colosseums_applied_ &&
        Main::g_FeatureStatus.unlock_all_colosseums.load() &&
        Game::ApplyFlagList(singletons, UnlockData::kColosseumFlagsBase)) {
        Main::Logger::Instance().Info("Unlock all colosseums applied.");
        colosseums_applied_ = true;
    }
}

}  // namespace ERD::Features
