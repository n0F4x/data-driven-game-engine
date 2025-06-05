module;

#include <concepts>
#include <utility>

export module core.scheduler.task_builders.ConditionalTaskBuilder;

import core.scheduler.build;
import core.scheduler.concepts.task_builder_c;
import core.scheduler.concepts.predicate_task_builder_c;
import core.scheduler.task_builders.TaskBuilderBase;

import utility.meta.algorithms.apply;
import utility.meta.type_traits.type_list.type_list_concat;
import utility.meta.type_traits.type_list.type_list_unique;

namespace core::scheduler {

export template <
    task_builder_c           MainTaskBuilder_T,
    predicate_task_builder_c PredicateTaskBuilder_T>
class ConditionalTaskBuilder : public TaskBuilderBase {
public:
    using Result          = void;
    using UniqueArguments = util::meta::type_list_unique_t<util::meta::type_list_concat_t<
        typename MainTaskBuilder_T::UniqueArguments,
        typename PredicateTaskBuilder_T::UniqueArguments>>;

    template <typename UMainTaskBuilder_T, typename UPredicateTaskBuilder_T>
        requires std::constructible_from<MainTaskBuilder_T, UMainTaskBuilder_T&&>
              && std::constructible_from<PredicateTaskBuilder_T, UPredicateTaskBuilder_T&&>
    constexpr explicit ConditionalTaskBuilder(
        UMainTaskBuilder_T&&      main_task_builder,
        UPredicateTaskBuilder_T&& predicate_task_builder
    );

    template <typename Self_T, typename... ArgumentProviders_T>
    constexpr auto operator()(this Self_T&&, ArgumentProviders_T... argument_providers);

private:
    MainTaskBuilder_T      m_main_task_builder;
    PredicateTaskBuilder_T m_predicate_task_builder;
};

}   // namespace core::scheduler

template <
    core::scheduler::task_builder_c           MainTaskBuilder_T,
    core::scheduler::predicate_task_builder_c PredicateTaskBuilder_T>
template <typename UMainTaskBuilder_T, typename UPredicateTaskBuilder_T>
    requires std::constructible_from<MainTaskBuilder_T, UMainTaskBuilder_T&&>
              && std::constructible_from<PredicateTaskBuilder_T, UPredicateTaskBuilder_T&&>
constexpr core::scheduler::
    ConditionalTaskBuilder<MainTaskBuilder_T, PredicateTaskBuilder_T>::
        ConditionalTaskBuilder(
            UMainTaskBuilder_T&&      main_task_builder,
            UPredicateTaskBuilder_T&& predicate_task_builder
        )
    : m_main_task_builder{ std::forward<UMainTaskBuilder_T>(main_task_builder) },
      m_predicate_task_builder{
          std::forward<UPredicateTaskBuilder_T>(predicate_task_builder)
      }
{}

template <
    core::scheduler::task_builder_c           MainTaskBuilder_T,
    core::scheduler::predicate_task_builder_c PredicateTaskBuilder_T>
template <typename Self_T, typename... ArgumentProviders_T>
constexpr auto
    core::scheduler::ConditionalTaskBuilder<MainTaskBuilder_T, PredicateTaskBuilder_T>::
        operator()(this Self_T&& self, ArgumentProviders_T... argument_providers)
{
    return [main_task = build(self.m_main_task_builder, argument_providers...),
            predicate_task =
                build(self.m_predicate_task_builder, argument_providers...)] mutable {
        if (std::invoke(predicate_task)) {
            std::invoke(main_task);
        }
    };
}
