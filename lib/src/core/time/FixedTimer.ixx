module;

#include <chrono>
#include <cmath>

export module core.time.FixedTimer;

import mp_units;

import core.measurement.tick_rate;

namespace core::time {

export template <mp_units::QuantityOf<measurement::tick_rate> auto tick_rate>
class FixedTimer {
public:
    using Clock = std::chrono::steady_clock;
    using Delta = Clock::duration;

    [[nodiscard]]
    auto delta_ticks() const -> uint32_t;
    [[nodiscard]]
    auto current() const -> Clock::time_point;

    auto update(Clock::time_point current = Clock::now()) -> void;
    auto reset() -> void;

private:
    Clock::time_point m_current{ Clock::now() };
    Delta             m_delta_time{};
};

}   // namespace core::time

template <mp_units::QuantityOf<core::measurement::tick_rate> auto tick_rate>
auto core::time::FixedTimer<tick_rate>::delta_ticks() const -> uint32_t
{
    return static_cast<uint32_t>(std::floor(
        (tick_rate * mp_units::quantity{ m_delta_time }).numerical_value_in(mp_units::one)
    ));
}

template <mp_units::QuantityOf<core::measurement::tick_rate> auto tick_rate>
auto core::time::FixedTimer<tick_rate>::current() const -> Clock::time_point
{
    return m_current;
}

template <mp_units::QuantityOf<core::measurement::tick_rate> auto tick_rate>
auto core::time::FixedTimer<tick_rate>::update(const Clock::time_point current) -> void
{
    if (delta_ticks() >= 1) {
        m_delta_time -= std::chrono::duration_cast<Delta>(
            mp_units::to_chrono_duration(delta_ticks() / tick_rate)
        );
    }

    const auto elapsed_time{ current - m_current };
    const auto elapsed_ticks{ mp_units::quantity{ elapsed_time } * tick_rate };
    if (elapsed_ticks >= 1 * mp_units::one) {
        const auto elapsed_delta{
            std::chrono::duration_cast<Delta>(mp_units::to_chrono_duration(
                std::floor(elapsed_ticks.numerical_value_in(mp_units::one)) / tick_rate
            ))
        };
        m_current += elapsed_delta;
        m_delta_time += elapsed_delta;
    }
}

template <mp_units::QuantityOf<core::measurement::tick_rate> auto tick_rate>
auto core::time::FixedTimer<tick_rate>::reset() -> void
{
    m_delta_time = Delta{};
    m_current    = Clock::now();
}
