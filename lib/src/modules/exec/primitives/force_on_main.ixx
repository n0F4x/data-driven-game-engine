module;

#include <utility>

export module ddge.modules.exec.primitives.force_on_main;

import ddge.modules.exec.raw_task_c;
import ddge.modules.exec.Cardinality;
import ddge.modules.exec.ExecPolicy;
import ddge.modules.exec.primitives.as_task;
import ddge.modules.exec.TaskBlueprint;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::exec {

export template <raw_task_c F>
[[nodiscard]]
auto force_on_main(F&& func)
    -> TaskBlueprint<util::meta::result_of_t<F>, Cardinality::eSingle>
{
    return as_task<ExecPolicy::eForceOnMain>(std::forward<F>(func));
}

}   // namespace ddge::exec
