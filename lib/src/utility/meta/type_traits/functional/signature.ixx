export module ddge.utility.meta.type_traits.functional.signature;

import ddge.utility.TypeList;

namespace ddge::util::meta {

export template <typename>
struct signature;

template <typename Result, typename... Args>
struct signature<Result(Args...)> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...)> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) noexcept> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...)&> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) & noexcept> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) &&> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) && noexcept> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const noexcept> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const&> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const & noexcept> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const&&> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const && noexcept> {
    using arguments_t = TypeList<Args...>;
    using result_t    = Result;
};

}   // namespace ddge::util::meta
