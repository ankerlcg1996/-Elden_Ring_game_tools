#pragma once

#include "../Common.hpp"

#include "../Game/SingletonRegistry.hpp"

namespace ERD::Features {

class CharacterFlags {
public:
    void Tick(const Game::SingletonRegistry& singletons) const;
};

}  // namespace ERD::Features
