module;

#include <concepts>
#include <ranges>
#include <utility>

export module core.scheduler.task_builders.RepeatedTaskBuilder;

import core.scheduler.build;
import core.scheduler.concepts.task_builder_c;
import core.scheduler.concepts.repetition_specifier_task_builder_c;
import core.scheduler.task_builders.TaskBuilderBase;

import utility.meta.algorithms.apply;
import utility.meta.type_traits.type_list.type_list_union;

namespace core::scheduler {

export template <
    task_builder_c                      MainTaskBuilder_T,
    repetition_specifier_task_builder_c RepetitionSpecifierTaskBuilder_T>
class RepeatedTaskBuilder : public TaskBuilderBase {
public:
    using Result          = void;
    using UniqueArguments = util::meta::type_list_union_t<
        typename MainTaskBuilder_T::UniqueArguments,
        typename RepetitionSpecifierTaskBuilder_T::UniqueArguments>;

    template <typename UMainTaskBuilder_T, typename URepetitionSpecifierTaskBuilder_T>
        requires std::constructible_from<MainTaskBuilder_T, UMainTaskBuilder_T&&>
              && std::constructible_from<
                     RepetitionSpecifierTaskBuilder_T,
                     URepetitionSpecifierTaskBuilder_T&&>
    constexpr explicit RepeatedTaskBuilder(
        UMainTaskBuilder_T&&                main_task_builder,
        URepetitionSpecifierTaskBuilder_T&& repetition_specifier_task_builder
    );

    template <typename Self_T, typename... ArgumentProviders_T>
    [[nodiscard]]
    constexpr auto operator()(this Self_T&&, ArgumentProviders_T... argument_providers);

private:
    MainTaskBuilder_T                m_main_task_builder;
    RepetitionSpecifierTaskBuilder_T m_repetition_specifier_task_builder;
};

}   // namespace core::scheduler

template <
    core::scheduler::task_builder_c                      MainTaskBuilder_T,
    core::scheduler::repetition_specifier_task_builder_c RepetitionSpecifierTaskBuilder_T>
template <typename UMainTaskBuilder_T, typename URepetitionSpecifierTaskBuilder_T>
    requires std::constructible_from<MainTaskBuilder_T, UMainTaskBuilder_T&&>
              && std::constructible_from<
                  RepetitionSpecifierTaskBuilder_T,
                  URepetitionSpecifierTaskBuilder_T&&>
constexpr core::scheduler::
    RepeatedTaskBuilder<MainTaskBuilder_T, RepetitionSpecifierTaskBuilder_T>::
        RepeatedTaskBuilder(
            UMainTaskBuilder_T&&                main_task_builder,
            URepetitionSpecifierTaskBuilder_T&& repetition_specifier_task_builder
        )
    : m_main_task_builder{ std::forward<UMainTaskBuilder_T>(main_task_builder) },
      m_repetition_specifier_task_builder{ std::forward<URepetitionSpecifierTaskBuilder_T>(
          repetition_specifier_task_builder
      ) }
{}

template <
    core::scheduler::task_builder_c                      MainTaskBuilder_T,
    core::scheduler::repetition_specifier_task_builder_c RepetitionSpecifierTaskBuilder_T>
template <typename Self_T, typename... ArgumentProviders_T>
constexpr auto core::scheduler::
    RepeatedTaskBuilder<MainTaskBuilder_T, RepetitionSpecifierTaskBuilder_T>::operator()(
        this Self_T&& self,
        ArgumentProviders_T... argument_providers
    )
{
    return [main_task = build(self.m_main_task_builder, argument_providers...),
            repetition_specifier_task = build(
                self.m_repetition_specifier_task_builder, argument_providers...
            )] mutable -> Result {
        for (const auto _ :
             std::views::repeat(std::ignore, std::invoke(repetition_specifier_task)))
        {
            std::invoke(main_task);
        }
    };
}
