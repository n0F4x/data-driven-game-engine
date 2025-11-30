module;

#include <utility>

export module ddge.modules.scheduler.primitives.force_on_main;

import ddge.modules.scheduler.raw_task_c;
import ddge.modules.scheduler.ExecPolicy;
import ddge.modules.scheduler.primitives.as_task;
import ddge.modules.scheduler.TaskBuilder;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::scheduler {

export template <raw_task_c F>
[[nodiscard]]
auto force_on_main(F&& func) -> TaskBuilder<util::meta::result_of_t<F>>
{
    return as_task<ExecPolicy::eForceOnMain>(std::forward<F>(func));
}

}   // namespace ddge::scheduler
