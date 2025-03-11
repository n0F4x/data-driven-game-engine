module;

#include <array>
#include <exception>
#include <type_traits>

export module utility.ScopeGuard;

import utility.meta.concepts.decayed;

namespace util {

export template <::util::meta::decayed_c Rollback_T>
    requires(std::is_nothrow_invocable_v<Rollback_T>)
class [[nodiscard]]
ScopeGuard {
public:
    constexpr explicit(false) ScopeGuard(const Rollback_T& rollback) noexcept
        requires(std::is_nothrow_constructible_v<Rollback_T, const Rollback_T&>);
    constexpr explicit(false) ScopeGuard(Rollback_T&& rollback) noexcept
        requires(std::is_nothrow_constructible_v<Rollback_T, Rollback_T &&>);
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&)      = default;
    constexpr ~ScopeGuard() noexcept;

private:
    Rollback_T m_rollback;
    int        m_uncaught_exceptions{ [] {
#ifdef __cpp_constexpr_exceptions
        static_assert(false, "FIXME: Exceptions are now constexpr");
#endif
        return std::is_constant_evaluated() ? 0 : std::uncaught_exceptions();
    }() };
};

export template <::util::meta::decayed_c... Rollbacks_T>
using ScopeGuards = std::tuple<ScopeGuard<Rollbacks_T>...>;

}   // namespace util

template <util::meta::decayed_c Rollback_T>
    requires(std::is_nothrow_invocable_v<Rollback_T>)
constexpr util::ScopeGuard<Rollback_T>::~ScopeGuard<Rollback_T>() noexcept
{
#ifdef __cpp_constexpr_exceptions
    static_assert(false, "FIXME: Exceptions are now constexpr");
#endif
    if !consteval {
        if (m_uncaught_exceptions > std::uncaught_exceptions()) {
            std::invoke(m_rollback);
        }
    }
}

template <util::meta::decayed_c Rollback_T>
    requires(std::is_nothrow_invocable_v<Rollback_T>)
constexpr util::ScopeGuard<Rollback_T>::ScopeGuard(const Rollback_T& rollback) noexcept
    requires(std::is_nothrow_constructible_v<Rollback_T, const Rollback_T&>)
    : m_rollback{ rollback }
{}

template <::util::meta::decayed_c Rollback_T>
    requires(std::is_nothrow_invocable_v<Rollback_T>)
constexpr util::ScopeGuard<Rollback_T>::ScopeGuard(Rollback_T&& rollback) noexcept
    requires(std::is_nothrow_constructible_v<Rollback_T, Rollback_T &&>)
    : m_rollback{ std::move(rollback) }
{}
