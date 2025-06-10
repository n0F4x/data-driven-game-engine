export module snake.game.game_is_running;

import extensions.scheduler.accessors.events.Reader;

import snake.game.GameOver;

using namespace extensions::scheduler::accessors;

namespace game {

export constexpr inline auto game_is_running =
    [](const ::events::Reader<GameOver>& game_over_reader) -> bool   //
{                                                                    //
    return game_over_reader.read().size() == 0;
};

}   // namespace game
