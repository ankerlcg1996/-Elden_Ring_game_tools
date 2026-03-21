#include "CharacterFlags.hpp"

#include "../Game/Memory.hpp"
#include "../Main/FeatureStatus.hpp"

namespace ERD::Features {
namespace {

constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kCharacterFlagsBlockOffset = 0x190;
constexpr uintptr_t kCommonFlagsOffset = 0x19B;
constexpr uintptr_t kNoGoodsConsumeOffset = 0x532;

}  // namespace

void CharacterFlags::Tick(const Game::SingletonRegistry& singletons) const {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    if (world_chr_man == 0) {
        return;
    }

    uintptr_t common_flags_address = 0;
    if (Game::ResolvePointerChain(
            world_chr_man,
            {kNetPlayersOffset, 0x0, kCharacterFlagsBlockOffset, 0x0},
            kCommonFlagsOffset,
            common_flags_address)) {
        Game::SetBitFlag(common_flags_address, 2, Main::g_FeatureStatus.infinite_fp.load());
        Game::SetBitFlag(common_flags_address, 3, Main::g_FeatureStatus.no_stamina_consumption.load());
    }

    uintptr_t no_goods_address = 0;
    if (Game::ResolvePointerChain(
            world_chr_man,
            {kNetPlayersOffset, 0x0},
            kNoGoodsConsumeOffset,
            no_goods_address)) {
        Game::SetBitFlag(no_goods_address, 0, Main::g_FeatureStatus.infinite_items.load());
    }
}

}  // namespace ERD::Features
