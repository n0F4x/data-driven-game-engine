module;

#include <concepts>
#include <functional>
#include <utility>

export module ddge.utility.containers.FunctionWrapper;

import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.functional.function;
import ddge.utility.meta.concepts.functional.member_function_pointer;
import ddge.utility.meta.concepts.functional.unambiguous_functor;
import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.functional.result_of;
import ddge.utility.meta.type_traits.type_list.type_list_push_front;
import ddge.utility.meta.type_traits.type_list.type_list_to;

template <typename Derived_T, typename Signature_T>
class FunctionWrapperInterface;

template <typename Derived_T, typename Result_T, typename... Args_T>
class FunctionWrapperInterface<Derived_T, Result_T(Args_T...)> {
public:
    template <typename Self_T>
    constexpr auto operator()(this Self_T&&, Args_T... args) -> Result_T;
};

template <typename Derived_T, typename Result_T, typename ArgList_T>
struct FunctionWrapperInterfaceFromImpl;

template <
    typename Derived_T,
    typename Result_T,
    template <typename...> typename TypeList_T,
    typename... Args_T>
struct FunctionWrapperInterfaceFromImpl<Derived_T, Result_T, TypeList_T<Args_T...>> {
    using type = FunctionWrapperInterface<Derived_T, Result_T(Args_T...)>;
};

template <typename Derived_T, typename Invocable_T>
using function_wrapper_interface_from_t = FunctionWrapperInterfaceFromImpl<
    Derived_T,
    ddge::util::meta::result_of_t<Invocable_T>,
    ddge::util::meta::arguments_of_t<Invocable_T>>::type;

namespace ddge::util {

export template <util::meta::unambiguously_invocable_c Invocable_T>
class FunctionWrapper;

export template <util::meta::function_c Invocable_T>
class FunctionWrapper<Invocable_T>
    : public function_wrapper_interface_from_t<FunctionWrapper<Invocable_T>, Invocable_T> {
    using Base = function_wrapper_interface_from_t<FunctionWrapper, Invocable_T>;

public:
    constexpr explicit FunctionWrapper(Invocable_T& invocable);

private:
    friend Base;

    std::reference_wrapper<Invocable_T> m_invocable;
};

export template <util::meta::member_function_pointer_c Invocable_T>
class FunctionWrapper<Invocable_T>
    : public function_wrapper_interface_from_t<FunctionWrapper<Invocable_T>, Invocable_T> {
    using Base = function_wrapper_interface_from_t<FunctionWrapper, Invocable_T>;

public:
    constexpr explicit FunctionWrapper(Invocable_T invocable);

private:
    friend Base;

    Invocable_T m_invocable;
};

export template <util::meta::unambiguous_functor_c Invocable_T>
    requires util::meta::naked_c<Invocable_T>
class FunctionWrapper<Invocable_T>
    : public function_wrapper_interface_from_t<FunctionWrapper<Invocable_T>, Invocable_T> {
    using Base = function_wrapper_interface_from_t<FunctionWrapper, Invocable_T>;

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

}   // namespace ddge::util

template <typename Derived_T, typename Result_T, typename... Args_T>
template <typename Self_T>
constexpr auto FunctionWrapperInterface<Derived_T, Result_T(Args_T...)>::operator()(
    this Self_T&& self,
    Args_T... args
) -> Result_T
{
    return std::invoke(
        std::forward_like<Self_T>(self.Derived_T::m_invocable),
        std::forward<Args_T>(args)...
    );
}

template <ddge::util::meta::function_c Invocable_T>
constexpr ddge::util::FunctionWrapper<Invocable_T>::FunctionWrapper(Invocable_T& invocable)
    : m_invocable{ invocable }
{}

template <ddge::util::meta::member_function_pointer_c Invocable_T>
constexpr ddge::util::FunctionWrapper<Invocable_T>::FunctionWrapper(Invocable_T invocable)
    : m_invocable{ invocable }
{}

template <ddge::util::meta::unambiguous_functor_c Invocable_T>
    requires ddge::util::meta::naked_c<Invocable_T>
template <typename UInvocable_T>
    requires std::constructible_from<Invocable_T, UInvocable_T&&>
constexpr ddge::util::FunctionWrapper<Invocable_T>::FunctionWrapper(
    UInvocable_T&& invocable
)
    : m_invocable{ std::forward<UInvocable_T>(invocable) }
{}
