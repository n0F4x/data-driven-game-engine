module;

#include <concepts>
#include <functional>
#include <utility>

export module ddge.utility.containers.FunctionWrapper;

import ddge.utility.FunctionInterface;
import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.functional.function;
import ddge.utility.meta.concepts.functional.member_function_pointer;
import ddge.utility.meta.concepts.functional.unambiguous_functor;
import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.functional.result_of;
import ddge.utility.meta.type_traits.type_list.type_list_push_front;
import ddge.utility.meta.type_traits.type_list.type_list_to;

template <typename Implementation_T>
struct Projection {
    template <typename T>
    [[nodiscard]]
    constexpr static auto operator()(T&& that) noexcept -> decltype(auto)
    {
        return std::forward_like<T>(that.Implementation_T::m_invocable);
    }
};

template <typename Implementation_T, typename Invocable_T>
using function_wrapper_interface_from_t = ddge::util::FunctionInterface<
    ddge::util::meta::result_of_t<Invocable_T>,
    ddge::util::meta::arguments_of_t<Invocable_T>,
    Projection<Implementation_T>{}>;

namespace ddge::util {

export template <
    util::meta::unambiguously_invocable_c Invocable_T,
    auto                                  transformation_T = std::identity{}>
class BasicFunctionWrapper;

export template <util::meta::function_c Invocable_T, auto transformation_T>
class BasicFunctionWrapper<Invocable_T, transformation_T>
    : public function_wrapper_interface_from_t<
          BasicFunctionWrapper<Invocable_T, transformation_T>,
          Invocable_T> {
public:
    constexpr explicit BasicFunctionWrapper(Invocable_T& invocable);

private:
    friend ::Projection<BasicFunctionWrapper>;

    std::reference_wrapper<Invocable_T> m_invocable;
};

export template <util::meta::member_function_pointer_c Invocable_T, auto transformation_T>
class BasicFunctionWrapper<Invocable_T, transformation_T>
    : public function_wrapper_interface_from_t<
          BasicFunctionWrapper<Invocable_T, transformation_T>,
          Invocable_T> {
public:
    constexpr explicit BasicFunctionWrapper(Invocable_T invocable);

private:
    friend ::Projection<BasicFunctionWrapper>;

    Invocable_T m_invocable;
};

export template <util::meta::unambiguous_functor_c Invocable_T, auto transformation_T>
    requires util::meta::naked_c<Invocable_T>
class BasicFunctionWrapper<Invocable_T, transformation_T>
    : public function_wrapper_interface_from_t<
          BasicFunctionWrapper<Invocable_T, transformation_T>,
          Invocable_T> {
public:
    BasicFunctionWrapper()
        requires std::default_initializable<Invocable_T>
    = default;

    template <typename UInvocable_T>
        requires std::constructible_from<Invocable_T, UInvocable_T&&>
    constexpr explicit BasicFunctionWrapper(UInvocable_T&& invocable);

private:
    friend ::Projection<BasicFunctionWrapper>;

    Invocable_T m_invocable;
};

export template <typename F>
using FunctionWrapper = BasicFunctionWrapper<F>;

}   // namespace ddge::util

template <ddge::util::meta::function_c Invocable_T, auto transformation_T>
constexpr ddge::util::BasicFunctionWrapper<Invocable_T, transformation_T>::
    BasicFunctionWrapper(Invocable_T& invocable)
    : m_invocable{ invocable }
{}

template <ddge::util::meta::member_function_pointer_c Invocable_T, auto transformation_T>
constexpr ddge::util::BasicFunctionWrapper<Invocable_T, transformation_T>::
    BasicFunctionWrapper(Invocable_T invocable)
    : m_invocable{ invocable }
{}

template <ddge::util::meta::unambiguous_functor_c Invocable_T, auto transformation_T>
    requires ddge::util::meta::naked_c<Invocable_T>
template <typename UInvocable_T>
    requires std::constructible_from<Invocable_T, UInvocable_T&&>
constexpr ddge::util::BasicFunctionWrapper<Invocable_T, transformation_T>::
    BasicFunctionWrapper(UInvocable_T&& invocable)
    : m_invocable{ std::forward<UInvocable_T>(invocable) }
{}
