module;

#include <type_traits>

export module snake.game.AppleSpawnTimer;

import ddge.modules.time.FixedTimer;

import snake.game.apple_spawn_duration;

namespace game {

export class AppleSpawnTimer : public ddge::time::FixedTimer<
                                   std::remove_cvref_t<decltype(apple_spawn_duration)>,
                                   apple_spawn_duration.count()> {};

}   // namespace game
