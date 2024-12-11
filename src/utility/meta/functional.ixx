module;

#include <tuple>
#include <type_traits>

// reference: https://stackoverflow.com/a/70954691
export module utility.meta.functional;

namespace core::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <typename>
struct signature;

template <typename Result, typename... Args>
struct signature<Result(Args...)> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...)> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

export template <typename F>
concept function = std::is_function_v<F>;

export template <typename F>
concept member_function = std::is_member_function_pointer_v<std::decay_t<F>>;

export template <typename F>
concept functor = std::is_class_v<std::decay_t<F>>
               && requires(F&& t) { &std::decay_t<F>::operator(); };

export template <typename Callable>
concept callable = function<Callable> || member_function<Callable> || functor<Callable>;

template <typename F>
struct arguments_of;

template <function F>
struct arguments_of<F> {
    using type = typename signature<F>::arguments_t;
};

template <member_function F>
struct arguments_of<F> {
    using type = typename signature<std::decay_t<F>>::arguments_t;
};

template <functor F>
struct arguments_of<F> {
    using type = typename signature<decltype(&std::decay_t<F>::operator())>::arguments_t;
};

export template <typename F>
using arguments_of_t = typename arguments_of<F>::type;

template <typename F>
struct invoke_result_of;

template <function F>
struct invoke_result_of<F> {
    using type = typename signature<F>::result_t;
};

template <member_function F>
struct invoke_result_of<F> {
    using type = typename signature<std::decay_t<F>>::result_t;
};

template <functor F>
struct invoke_result_of<F> {
    using type = typename signature<decltype(&std::decay_t<F>::operator())>::result_t;
};

export template <typename F>
using invoke_result_of_t = typename invoke_result_of<F>::type;

// NOLINTEND(readability-identifier-naming)
}   // namespace core::meta
