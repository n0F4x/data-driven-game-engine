export module snake.game.trigger_world_update_message;

import extensions.scheduler.accessors.messages.Sender;

import snake.game.WorldUpdate;

using namespace extensions::scheduler::accessors;

namespace game {

export constexpr inline auto trigger_world_update_message =
    [](const ::messages::Sender<WorldUpdate>& sender)   //
{                                                           //
    sender.send();
};

}   // namespace game
