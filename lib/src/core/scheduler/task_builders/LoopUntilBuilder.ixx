module;

#include <concepts>
#include <utility>

export module core.scheduler.task_builders.LoopUntilBuilder;

import core.scheduler.build;
import core.scheduler.concepts.task_builder_c;
import core.scheduler.concepts.predicate_task_builder_c;
import core.scheduler.task_builders.TaskBuilderBase;

import utility.meta.algorithms.apply;
import utility.meta.type_traits.type_list.type_list_union;

namespace core::scheduler {

export template <
    task_builder_c           MainTaskBuilder_T,
    predicate_task_builder_c PredicateTaskBuilder_T>
class LoopUntilBuilder : public TaskBuilderBase {
public:
    using Result          = void;
    using UniqueAccessors = util::meta::type_list_union_t<
        typename MainTaskBuilder_T::UniqueAccessors,
        typename PredicateTaskBuilder_T::UniqueAccessors>;

    template <typename UMainTaskBuilder_T, typename UPredicateTaskBuilder_T>
        requires std::constructible_from<MainTaskBuilder_T, UMainTaskBuilder_T&&>
              && std::constructible_from<PredicateTaskBuilder_T, UPredicateTaskBuilder_T&&>
    constexpr explicit LoopUntilBuilder(
        UMainTaskBuilder_T&&      main_task_builder,
        UPredicateTaskBuilder_T&& predicate_task_builder
    );

    template <typename Self_T, typename... Providers_T>
    [[nodiscard]]
    constexpr auto operator()(this Self_T&&, Providers_T... providers);

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
constexpr core::scheduler::LoopUntilBuilder<MainTaskBuilder_T, PredicateTaskBuilder_T>::
    LoopUntilBuilder(
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
template <typename Self_T, typename... Providers_T>
constexpr auto
    core::scheduler::LoopUntilBuilder<MainTaskBuilder_T, PredicateTaskBuilder_T>::
        operator()(this Self_T&& self, Providers_T... providers)
{
    return [main_task = build(self.m_main_task_builder, providers...),
            predicate_task =
                build(self.m_predicate_task_builder, providers...)] mutable -> Result {
        while (std::invoke(predicate_task)) {
            std::invoke(main_task);
        }
    };
}
