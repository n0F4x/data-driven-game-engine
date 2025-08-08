export module snake.game.trigger_world_update_message;

import core.scheduler.accessors.messages.Sender;

import snake.game.WorldUpdate;

using namespace core::scheduler::accessors;

namespace game {

export auto trigger_world_update_message(const ::messages::Sender<WorldUpdate>& sender)
    -> void;

}   // namespace game

module :private;

auto game::trigger_world_update_message(const messages::Sender<WorldUpdate>& sender)
    -> void
{
    sender.send();
}
