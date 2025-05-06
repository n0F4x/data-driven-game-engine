module;

#include <type_traits>

export module core.scheduler.as_task_builder;

import core.scheduler.concepts.decays_to_task_builder_c;
import core.scheduler.task_builders.SimpleTaskBuilder;

import utility.containers.FunctionWrapper;

namespace core::scheduler {

template <typename T>
    requires requires {
        std::type_identity<util::FunctionWrapper<std::remove_cvref_t<T>>>{};
    } || decays_to_task_builder_c<T>
struct AsTaskBuilder {
    using type = std::conditional_t<
        decays_to_task_builder_c<T>,
        std::remove_cvref_t<T>,
        SimpleTaskBuilder<std::remove_cvref_t<T>>>;
};

export template <typename T>
using as_task_builder_t = typename AsTaskBuilder<T>::type;

}   // namespace core::scheduler
