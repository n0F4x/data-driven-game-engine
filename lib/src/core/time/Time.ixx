module;

#include <chrono>

export module core.time.Time;

namespace core::time {

export class Time {
public:
    using Clock = std::chrono::steady_clock;
    using Delta = Clock::duration;

    [[nodiscard]]
    auto delta() const -> Delta;
    [[nodiscard]]
    auto current() const -> Clock::time_point;

    auto update(Clock::time_point current) -> void;
    auto reset() -> void;

private:
    Clock::time_point m_current{ Clock::now() };
    Delta             m_delta{};
};

}   // namespace core::time

auto core::time::Time::delta() const -> Delta
{
    return m_delta;
}

auto core::time::Time::current() const -> Clock::time_point
{
    return m_current;
}

auto core::time::Time::update(const Clock::time_point current) -> void
{
    m_delta   = current - m_current;
    m_current = current;
}

auto core::time::Time::reset() -> void
{
    m_delta   = Delta{ 0 };
    m_current = Clock::now();
}
