module snake.game.tasks.trigger_world_update_message;

import ddge.scheduler.accessors.messages;
import ddge.scheduler.primitives.as_task;

import snake.game.WorldUpdate;

using namespace ddge::scheduler::accessors;

auto game::tasks::trigger_world_update_message() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::as_task(
        +[](const messages::Sender<WorldUpdate>& sender) -> void {   //
            sender.send();
        }
    );
}
