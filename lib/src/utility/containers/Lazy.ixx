module;

#include <concepts>
#include <type_traits>

export module ddge.utility.containers.Lazy;

import ddge.utility.meta.concepts.naked;

namespace ddge::util {

export template <meta::naked_c F>
    requires std::invocable<F&&> && (!std::is_void_v<std::invoke_result_t<F &&>>)
class Lazy {
public:
    constexpr explicit Lazy(F&& create) : m_create{ std::move(create) } {}

    [[nodiscard]]
    explicit(false) operator std::invoke_result_t<F&&>() &&
    {
        return std::move(m_create)();
    }

private:
    F m_create;
};

}   // namespace ddge::util
