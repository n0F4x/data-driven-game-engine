export module snake.game.game_is_running;

import core.scheduler.accessors.events.Reader;

import snake.game.GameOver;

using namespace core::scheduler::accessors;

namespace game {

export auto game_is_running(const ::events::Reader<GameOver>& game_over_reader) -> bool;

}   // namespace game

module :private;

auto game::game_is_running(const events::Reader<GameOver>& game_over_reader) -> bool
{
    return game_over_reader.read().size() == 0;
}
