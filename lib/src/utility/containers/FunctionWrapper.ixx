module;

#include <concepts>
#include <functional>
#include <utility>

export module utility.containers.FunctionWrapper;

import utility.meta.concepts.decayed;
import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.member_function_pointer;
import utility.meta.concepts.functional.unambiguous_functor;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.meta.type_traits.type_list.type_list_push_front;
import utility.meta.type_traits.type_list.type_list_to;

template <typename>
class FunctionWrapperInterface;

template <typename Result_T, typename... Args_T>
class FunctionWrapperInterface<Result_T(Args_T...)> {
public:
    template <typename Self_T>
    constexpr auto operator()(this Self_T&&, Args_T... args) -> Result_T;
};

template <typename, typename>
struct FunctionWrapperInterfaceFromImpl;

template <typename Result_T, template <typename...> typename TypeList_T, typename... Args_T>
struct FunctionWrapperInterfaceFromImpl<Result_T, TypeList_T<Args_T...>> {
    using type = FunctionWrapperInterface<Result_T(Args_T...)>;
};

template <typename Invocable_T>
using function_wrapper_interface_from_t = typename FunctionWrapperInterfaceFromImpl<
    util::meta::result_of_t<Invocable_T>,
    util::meta::arguments_of_t<Invocable_T>>::type;

namespace util {

export template <typename Invocable_T>
class FunctionWrapper;

template <util::meta::function_c Invocable_T>
class FunctionWrapper<Invocable_T>
    : public function_wrapper_interface_from_t<Invocable_T> {
    using Base = function_wrapper_interface_from_t<Invocable_T>;

public:
    constexpr explicit FunctionWrapper(Invocable_T& invocable);

private:
    friend Base;

    std::reference_wrapper<Invocable_T> m_invocable;
};

template <util::meta::member_function_pointer_c Invocable_T>
class FunctionWrapper<Invocable_T>
    : public function_wrapper_interface_from_t<Invocable_T> {
    using Base = function_wrapper_interface_from_t<Invocable_T>;

public:
    constexpr explicit FunctionWrapper(Invocable_T invocable);

private:
    friend Base;

    Invocable_T m_invocable;
};

template <util::meta::unambiguous_functor_c Invocable_T>
    requires util::meta::decayed_c<Invocable_T>
class FunctionWrapper<Invocable_T>
    : public function_wrapper_interface_from_t<Invocable_T> {
    using Base = function_wrapper_interface_from_t<Invocable_T>;

public:
    FunctionWrapper()
        requires std::default_initializable<Invocable_T>
    = default;

    template <typename UInvocable_T>
        requires std::constructible_from<Invocable_T, UInvocable_T&&>
    constexpr explicit FunctionWrapper(UInvocable_T&& invocable);

private:
    friend Base;

    Invocable_T m_invocable;
};

}   // namespace util

template <typename Result_T, typename... Args_T>
template <typename Self_T>
constexpr auto FunctionWrapperInterface<Result_T(Args_T...)>::operator()(
    this Self_T&& self,
    Args_T... args
) -> Result_T
{
    return std::invoke(
        std::forward_like<Self_T>(self.m_invocable), std::forward<Args_T>(args)...
    );
}

template <util::meta::function_c Invocable_T>
constexpr util::FunctionWrapper<Invocable_T>::FunctionWrapper(Invocable_T& invocable)
    : m_invocable{ invocable }
{}

template <util::meta::member_function_pointer_c Invocable_T>
constexpr util::FunctionWrapper<Invocable_T>::FunctionWrapper(Invocable_T invocable)
    : m_invocable{ invocable }
{}

template <util::meta::unambiguous_functor_c Invocable_T>
    requires util::meta::decayed_c<Invocable_T>
template <typename UInvocable_T>
    requires std::constructible_from<Invocable_T, UInvocable_T&&>
constexpr util::FunctionWrapper<Invocable_T>::FunctionWrapper(UInvocable_T&& invocable)
    : m_invocable{ std::forward<UInvocable_T>(invocable) }
{}
