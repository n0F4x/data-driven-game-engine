module;

#include <concepts>

export module core.scheduler.concepts.task_builder_c;

import core.scheduler.task_builders.TaskBuilderBase;
import utility.meta.concepts.type_list.type_list;

namespace core::scheduler {

template <typename T>
concept has_task_builder_traits_c = util::meta::type_list_c<typename T::UniqueAccessors>
                                 && requires { typename T::Result; };

export template <typename T>
concept task_builder_c = std::derived_from<T, TaskBuilderBase>
                      && has_task_builder_traits_c<T>;

}   // namespace core::scheduler
