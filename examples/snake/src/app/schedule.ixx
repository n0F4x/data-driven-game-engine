export module snake.app.schedule;

import ddge.scheduler.ScheduleBuilder;

namespace app {

export [[nodiscard]]
auto schedule() -> ddge::scheduler::ScheduleBuilder;

}   // namespace app
