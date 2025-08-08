module;

#include <type_traits>

export module snake.game.AppleSpawnTimer;

import modules.time.FixedTimer;

import snake.game.apple_spawn_time;

namespace game {

export class AppleSpawnTimer : public modules::time::FixedTimer<
                                   std::remove_cvref_t<decltype(apple_spawn_time)>,
                                   apple_spawn_time.count()> {};

}   // namespace game
