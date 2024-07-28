#pragma once

// reference: https://stackoverflow.com/a/70954691

namespace core::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <typename>
struct signature;

template <typename Result, typename... Args>
struct signature<Result(Args...)> {
    using type = std::tuple<Args...>;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...)> {
    using type = std::tuple<Args...>;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const> {
    using type = std::tuple<Args...>;
};

template <typename F>
concept function = std::is_function_v<F>;

template <typename F>
concept member_function = std::is_member_function_pointer_v<std::decay_t<F>>;

template <typename F>
concept functor = std::is_class_v<std::decay_t<F>>
               && requires(F&& t) { &std::decay_t<F>::operator(); };

template <typename Callable>
concept callable = function<Callable> || member_function<Callable> || functor<Callable>;

template <typename F>
struct arguments;

template <function F>
struct arguments<F> {
    using type = signature<F>::type;
};

template <member_function F>
struct arguments<F> {
    using type = signature<std::decay_t<F>>::type;
};

template <functor F>
struct arguments<F> {
    using type = signature<decltype(&std::decay_t<F>::operator())>::type;
};

template <typename F>
using arguments_t = arguments<F>::type;

// NOLINTEND(readability-identifier-naming)
}   // namespace core::meta
