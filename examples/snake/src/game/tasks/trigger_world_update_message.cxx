module snake.game.tasks.trigger_world_update_message;

import ddge.modules.exec.accessors.messages;
import ddge.modules.exec.primitives.as_task;

import snake.game.WorldUpdate;

using namespace ddge::exec::accessors;

auto game::tasks::trigger_world_update_message()
    -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::as_task(
        +[](const messages::Sender<WorldUpdate>& sender) -> void {   //
            sender.send();
        }
    );
}
