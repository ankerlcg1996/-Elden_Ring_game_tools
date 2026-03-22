#include "CharacterFlags.hpp"

#include "../Game/Memory.hpp"
#include "../Main/FeatureStatus.hpp"
#include "../Main/Logger.hpp"

namespace ERD::Features {
namespace {

constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kCharacterFlagsBlockOffset = 0x190;
constexpr uintptr_t kCommonFlagsOffset = 0x19B;
constexpr uintptr_t kNoGoodsConsumeOffset = 0x532;

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

void CharacterFlags::Tick(const Game::SingletonRegistry& singletons) const {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    if (world_chr_man == 0) {
        return;
    }

    uintptr_t common_flags_address = 0;
    ERD_PROTECTED_STEP(
        "CharacterFlags.CommonFlags",
        if (Game::ResolvePointerChain(
                world_chr_man,
                {kNetPlayersOffset, 0x0, kCharacterFlagsBlockOffset, 0x0},
                kCommonFlagsOffset,
                common_flags_address)) {
            Game::SetBitFlag(common_flags_address, 0, Main::g_FeatureStatus.no_dead.load());
            Game::SetBitFlag(common_flags_address, 1, Main::g_FeatureStatus.no_damage.load());
            Game::SetBitFlag(common_flags_address, 2, Main::g_FeatureStatus.infinite_fp.load());
            Game::SetBitFlag(common_flags_address, 3, Main::g_FeatureStatus.no_stamina_consumption.load());
        }
    );

    uintptr_t no_goods_address = 0;
    ERD_PROTECTED_STEP(
        "CharacterFlags.NoGoodsConsume",
        if (Game::ResolvePointerChain(
                world_chr_man,
                {kNetPlayersOffset, 0x0},
                kNoGoodsConsumeOffset,
                no_goods_address)) {
            Game::SetBitFlag(no_goods_address, 0, Main::g_FeatureStatus.infinite_items.load());
        }
    );
}

}  // namespace ERD::Features
