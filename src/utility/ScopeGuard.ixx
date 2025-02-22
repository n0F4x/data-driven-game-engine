module;

#include <exception>
#include <type_traits>

export module utility.ScopeGuard;

namespace util {

export template <typename Rollback_T>
    requires(std::is_nothrow_invocable_v<Rollback_T>)
class [[nodiscard]] ScopeGuard {
public:
    template <typename F>
    constexpr explicit ScopeGuard(F&& rollback);
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&)      = default;
    constexpr ~ScopeGuard() noexcept;

private:
    Rollback_T m_rollback;
    int        m_uncaught_exceptions;
};

export template <typename Rollback_T>
[[nodiscard]]
constexpr auto make_scope_guard(Rollback_T&& rollback)
    -> ScopeGuard<std::decay_t<Rollback_T>>;

}   // namespace util

template <typename Rollback_T>
    requires(std::is_nothrow_invocable_v<Rollback_T>)
template <typename F>
constexpr util::ScopeGuard<Rollback_T>::ScopeGuard(F&& rollback)
    : m_rollback{ std::forward<F>(rollback) },
      m_uncaught_exceptions{ [] {
#ifdef __cpp_constexpr_exceptions
          static_assert(false, "FIXME: Exceptions are now constexpr");
#endif
          return std::is_constant_evaluated() ? 0 : std::uncaught_exceptions();
      }() }
{}

template <typename Rollback_T>
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

template <typename Rollback_T>
constexpr auto util::make_scope_guard(Rollback_T&& rollback)
    -> ScopeGuard<std::decay_t<Rollback_T>>
{
    return ScopeGuard<std::decay_t<Rollback_T>>{ std::forward<Rollback_T>(rollback) };
}
