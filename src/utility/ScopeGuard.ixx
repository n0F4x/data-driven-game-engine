module;

#include <functional>
#include <type_traits>

export module utility.ScopeGuard;

namespace util {

export class [[nodiscard]] ScopeGuard {
public:
    template <typename F>
        requires(std::is_nothrow_invocable_v<F>)
    explicit ScopeGuard(F&& callback);

    ~ScopeGuard() noexcept;

private:
    std::function<void()> m_callback;
};

template <typename F>
    requires(std::is_nothrow_invocable_v<F>)
ScopeGuard::ScopeGuard(F&& callback) : m_callback{ std::forward<F>(callback) }
{}

}   // namespace util

module :private;

util::ScopeGuard::~ScopeGuard() noexcept
{
    if (m_callback) {
        m_callback();
    }
}
