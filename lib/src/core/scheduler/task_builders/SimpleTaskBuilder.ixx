module;

#include <functional>
#include <string>
#include <string_view>
#include <utility>

export module core.scheduler.task_builders.SimpleTaskBuilder;

import core.scheduler.provider_index_for_argument;
import core.scheduler.task_builders.TaskBuilderBase;

import utility.meta.algorithms.apply;
import utility.containers.FunctionWrapper;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.meta.type_traits.integer_sequence.integer_sequence_at;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_unique;
import utility.meta.type_traits.type_list.type_list_size;
import utility.wrap_ref;

namespace core::scheduler {

export template <typename Invocable_T>
    requires requires { std::type_identity<util::FunctionWrapper<Invocable_T>>{}; }
class SimpleTaskBuilder : public TaskBuilderBase {
public:
    using Result = util::meta::result_of_t<Invocable_T>;
    using UniqueArguments =
        util::meta::type_list_unique_t<util::meta::arguments_of_t<Invocable_T>>;

    template <typename UInvocable>
        requires std::constructible_from<util::FunctionWrapper<Invocable_T>, UInvocable&&>
    constexpr explicit SimpleTaskBuilder(UInvocable&& invocable);

    template <typename Self_T, typename... ArgumentProviders_T>
    [[nodiscard]]
    constexpr auto operator()(this Self_T&&, ArgumentProviders_T&&... argument_providers);

private:
    util::FunctionWrapper<Invocable_T> m_invocable;
};

}   // namespace core::scheduler

template <typename Invocable_T>
    requires requires { std::type_identity<util::FunctionWrapper<Invocable_T>>{}; }
template <typename UInvocable>
    requires std::constructible_from<util::FunctionWrapper<Invocable_T>, UInvocable&&>
constexpr core::scheduler::SimpleTaskBuilder<Invocable_T>::SimpleTaskBuilder(
    UInvocable&& invocable
)
    : m_invocable{ std::forward<UInvocable>(invocable) }
{}

template <typename Invocable_T>
    requires requires { std::type_identity<util::FunctionWrapper<Invocable_T>>{}; }
template <typename Self_T, typename... ArgumentProviders_T>
constexpr auto core::scheduler::SimpleTaskBuilder<Invocable_T>::operator()(
    this Self_T&& self,
    ArgumentProviders_T&&... argument_providers
)
{
    const auto make_arguments = [&argument_providers...]<typename... Arguments_T> {
        return std::make_tuple(
            util::wrap_ref(
                argument_providers...
                    [provider_index_for_argument<Arguments_T, ArgumentProviders_T...>]
                        .template provide<Arguments_T>()
            )...
        );
    };

    return util::meta::apply<util::meta::arguments_of_t<Invocable_T>>(
        [&self, &make_arguments]<typename... Arguments_T> {
            return [arguments = make_arguments.template operator()<Arguments_T...>(),
                    invocable = std::forward_like<Self_T>(self.m_invocable)] mutable {
                return std::apply(invocable, arguments);
            };
        }
    );
}
