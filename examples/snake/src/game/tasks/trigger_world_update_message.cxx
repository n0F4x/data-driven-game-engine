module snake.game.tasks.trigger_world_update_message;

import ddge.modules.execution.accessors.messages.Sender;
import ddge.modules.execution.providers.MessageProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.game.WorldUpdate;

using namespace ddge::exec::accessors;

auto game::trigger_world_update_message()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        +[](const messages::Sender<WorldUpdate>& sender) -> void {   //
            sender.send();
        }
    );
}
