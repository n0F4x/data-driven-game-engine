module;

#include <chrono>

export module modules.time.Timer;

namespace modules::time {

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

}   // namespace modules::time

module :private;

auto modules::time::Timer::delta() const -> Delta
{
    return m_delta;
}

auto modules::time::Timer::current() const -> Clock::time_point
{
    return m_current;
}

auto modules::time::Timer::update(const Clock::time_point current) -> void
{
    m_delta   = current - m_current;
    m_current = current;
}

auto modules::time::Timer::reset(const Clock::time_point current) -> void
{
    m_current = current;
    m_delta   = Delta{};
}
