#pragma once

#include "../Common.hpp"

#include "SingletonRegistry.hpp"

namespace ERD::Game {

// 读取 DLC 拥有状态，决定是否需要给 DLC 地图/食谱补 flag。
bool IsDlcOwned(const SingletonRegistry& singletons, int dlc_index);

// 直接写 Event Flag。
bool SetEventFlag(const SingletonRegistry& singletons, std::uint32_t flag_id, bool enabled);
bool GetEventFlag(const SingletonRegistry& singletons, std::uint32_t flag_id, bool& enabled);

// 对一组 flag 批量置位。
template <std::size_t N>
bool ApplyFlagList(const SingletonRegistry& singletons, const std::array<std::uint32_t, N>& flags) {
    bool all_succeeded = true;
    for (std::uint32_t flag_id : flags) {
        all_succeeded = SetEventFlag(singletons, flag_id, true) && all_succeeded;
    }
    return all_succeeded;
}

}  // namespace ERD::Game
