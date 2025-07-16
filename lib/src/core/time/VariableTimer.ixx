module;

#include <chrono>

export module core.time.VariableTimer;

import mp_units;

import core.measurement.tick_rate;

namespace core::time {

export template <mp_units::UnitOf<measurement::tick_rate> auto tick_rate_unit_T>
class VariableTimer {
public:
    using Clock = std::chrono::steady_clock;
    using Delta = Clock::duration;

    explicit VariableTimer(mp_units::quantity<tick_rate_unit_T> tick_rate);

    [[nodiscard]]
    auto delta_ticks() const -> uint32_t;
    [[nodiscard]]
    auto current() const -> Clock::time_point;
    [[nodiscard]]
    auto tick_rate() const -> mp_units::quantity<tick_rate_unit_T>;

    auto adjust_tick_rate(mp_units::quantity<tick_rate_unit_T> tick_rate) -> void;
    auto update(Clock::time_point current = Clock::now()) -> void;
    auto reset(Clock::time_point current = Clock::now()) -> void;

private:
    mp_units::quantity<tick_rate_unit_T> m_tick_rate;
    Clock::time_point                    m_current{ Clock::now() };
    Delta                                m_delta_time{};
};

}   // namespace core::time

template <mp_units::UnitOf<core::measurement::tick_rate> auto tick_rate_unit_T>
core::time::VariableTimer<tick_rate_unit_T>::VariableTimer(
    mp_units::quantity<tick_rate_unit_T> tick_rate
)
    : m_tick_rate{ tick_rate }
{}

template <mp_units::UnitOf<core::measurement::tick_rate> auto tick_rate_unit_T>
auto core::time::VariableTimer<tick_rate_unit_T>::delta_ticks() const -> uint32_t
{
    return static_cast<uint32_t>(std::floor(
        (m_tick_rate * mp_units::quantity{ m_delta_time }).numerical_value_in(mp_units::one)
    ));
}

template <mp_units::UnitOf<core::measurement::tick_rate> auto tick_rate_unit_T>
auto core::time::VariableTimer<tick_rate_unit_T>::current() const -> Clock::time_point
{
    return m_current;
}

template <mp_units::UnitOf<core::measurement::tick_rate> auto tick_rate_unit_T>
auto core::time::VariableTimer<tick_rate_unit_T>::tick_rate() const
    -> mp_units::quantity<tick_rate_unit_T>
{
    return m_tick_rate;
}

template <mp_units::UnitOf<core::measurement::tick_rate> auto tick_rate_unit_T>
auto core::time::VariableTimer<tick_rate_unit_T>::adjust_tick_rate(
    mp_units::quantity<tick_rate_unit_T> tick_rate
) -> void
{
    m_tick_rate = tick_rate;
}

template <mp_units::UnitOf<core::measurement::tick_rate> auto tick_rate_unit_T>
auto core::time::VariableTimer<tick_rate_unit_T>::update(Clock::time_point current)
    -> void
{
    if (delta_ticks() >= 1) {
        m_delta_time -= std::chrono::duration_cast<Delta>(
            mp_units::to_chrono_duration(delta_ticks() / m_tick_rate)
        );
    }

    const auto elapsed_time{ current - m_current };
    const auto elapsed_ticks{ mp_units::quantity{ elapsed_time } * m_tick_rate };
    if (elapsed_ticks >= 1 * mp_units::one) {
        const auto elapsed_delta{
            std::chrono::duration_cast<Delta>(mp_units::to_chrono_duration(
                std::floor(elapsed_ticks.numerical_value_in(mp_units::one)) / m_tick_rate
            ))
        };
        m_current += elapsed_delta;
        m_delta_time += elapsed_delta;
    }
}

template <mp_units::UnitOf<core::measurement::tick_rate> auto tick_rate_unit_T>
auto core::time::VariableTimer<tick_rate_unit_T>::reset(Clock::time_point current) -> void
{
    m_current    = current;
    m_delta_time = Delta{};
}
