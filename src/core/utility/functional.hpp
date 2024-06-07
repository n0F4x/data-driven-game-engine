#pragma once

// source: https://stackoverflow.com/a/70954691

namespace core::utils {

template <typename Sig>
struct signature;

template <typename Ret, typename... Args>
struct signature<Ret(Args...)> {
    using type = std::tuple<Args...>;
};

template <typename Ret, typename Obj, typename... Args>
struct signature<Ret (Obj::*)(Args...)> {
    using type = std::tuple<Args...>;
};

template <typename Ret, typename Obj, typename... Args>
struct signature<Ret (Obj::*)(Args...) const> {
    using type = std::tuple<Args...>;
};

template <typename Fun>
concept is_fun = std::is_function_v<Fun>;

template <typename Fun>
concept is_mem_fun = std::is_member_function_pointer_v<std::decay_t<Fun>>;

template <typename Fun>
concept is_functor = std::is_class_v<std::decay_t<Fun>>
                  && requires(Fun&& t) { &std::decay_t<Fun>::operator(); };

template <is_functor T>
auto arguments(T&& t) -> signature<decltype(&std::decay_t<T>::operator())>::type;

template <is_functor T>
auto arguments(const T& t) -> signature<decltype(&std::decay_t<T>::operator())>::type;

// template<is_fun T>
// auto arguments(T&& t)->signature<T>::type;

template <is_fun T>
auto arguments(const T& t) -> signature<T>::type;

template <is_mem_fun T>
auto arguments(T&& t) -> signature<std::decay_t<T>>::type;

template <is_mem_fun T>
auto arguments(const T& t) -> signature<std::decay_t<T>>::type;

}   // namespace core::utils
