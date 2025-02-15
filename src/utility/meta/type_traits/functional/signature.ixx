module;

#include <tuple>

export module utility.meta.type_traits.functional.signature;

namespace util::meta {
// NOLINTBEGIN(readability-identifier-naming)

export template <typename>
struct signature;

export template <typename Result, typename... Args>
struct signature<Result(Args...)> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

export template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...)> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

export template <typename Result, typename Class, typename... Args>
struct signature<Result (Class::*)(Args...) const> {
    using arguments_t = std::tuple<Args...>;
    using result_t    = Result;
};

// NOLINTEND(readability-identifier-naming)
}   // namespace util::meta
