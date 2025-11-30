module;

#include <utility>

export module ddge.modules.scheduler.primitives.force_on_main;

import ddge.modules.scheduler.raw_task_c;
import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.ExecPolicy;
import ddge.modules.scheduler.primitives.as_task;
import ddge.modules.scheduler.TaskBlueprint;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::scheduler {

export template <raw_task_c F>
[[nodiscard]]
auto force_on_main(F&& func)
    -> TaskBlueprint<util::meta::result_of_t<F>, Cardinality::eSingle>
{
    return as_task<ExecPolicy::eForceOnMain>(std::forward<F>(func));
}

}   // namespace ddge::scheduler
