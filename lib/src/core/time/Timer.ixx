module;

#include <chrono>

export module core.time.Timer;

namespace core::time {

export class Timer {
public:
    using Clock = std::chrono::steady_clock;
    using Delta = Clock::duration;

    [[nodiscard]]
    auto delta() const -> Delta;
    [[nodiscard]]
    auto current() const -> Clock::time_point;

    auto update(Clock::time_point current = Clock::now()) -> void;
    auto reset(Clock::time_point current = Clock::now()) -> void;

private:
    Clock::time_point m_current{ Clock::now() };
    Delta             m_delta{};
};

}   // namespace core::time

module :private;

auto core::time::Timer::delta() const -> Delta
{
    return m_delta;
}

auto core::time::Timer::current() const -> Clock::time_point
{
    return m_current;
}

auto core::time::Timer::update(const Clock::time_point current) -> void
{
    m_delta   = current - m_current;
    m_current = current;
}

auto core::time::Timer::reset(const Clock::time_point current) -> void
{
    m_current = current;
    m_delta   = Delta{};
}
