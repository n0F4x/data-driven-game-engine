module;

#include <utility>

export module ddge.modules.execution.v2.primitives.force_on_main;

import ddge.modules.execution.raw_task_c;
import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.ExecPolicy;
import ddge.modules.execution.v2.primitives.as_task;
import ddge.modules.execution.v2.TaskBlueprint;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::exec::v2 {

export template <raw_task_c F>
[[nodiscard]]
auto force_on_main(F&& func)
    -> TaskBlueprint<util::meta::result_of_t<F>, Cardinality::eSingle>
{
    return as_task<ExecPolicy::eForceOnMain>(std::forward<F>(func));
}

}   // namespace ddge::exec::v2
