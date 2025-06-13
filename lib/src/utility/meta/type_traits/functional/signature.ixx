module;

#include <tuple>

export module utility.meta.type_traits.functional.signature;

namespace util::meta {

export template <typename>
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
struct signature<Result (Class::*)(Args...) noexcept> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...)&> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) & noexcept> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) &&> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) && noexcept> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const noexcept> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const&> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const & noexcept> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const&&> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const && noexcept> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

}   // namespace util::meta
