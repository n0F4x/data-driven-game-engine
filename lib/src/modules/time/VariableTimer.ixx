module;

#include <chrono>
#include <cmath>
#include <concepts>

export module modules.time.VariableTimer;

import utility.meta.concepts.specialization_of;

namespace modules::time {

class VariableTimerBase {};

export template <util::meta::specialization_of_c<std::chrono::duration> Duration_T>
class VariableTimer : public VariableTimerBase {
public:
    using Clock = std::chrono::steady_clock;
    using Delta = Clock::duration;

    explicit VariableTimer(Duration_T tick_duration);

    [[nodiscard]]
    auto delta_ticks() const -> uint32_t;
    [[nodiscard]]
    auto current() const -> Clock::time_point;
    [[nodiscard]]
    auto tick_duration() const -> Duration_T;

    auto adjust_tick_duration(Duration_T tick_duration) -> void;
    auto update(Clock::time_point current = Clock::now()) -> void;
    auto reset(Clock::time_point current = Clock::now()) -> void;

private:
    Duration_T        m_tick_duration;
    Clock::time_point m_current{ Clock::now() };
    Delta             m_delta_time{};
};

export template <typename T>
concept is_VariableTimer_c = std::derived_from<T, VariableTimerBase>;

}   // namespace modules::time

template <util::meta::specialization_of_c<std::chrono::duration> Duration_T>
modules::time::VariableTimer<Duration_T>::VariableTimer(const Duration_T tick_duration)
    : m_tick_duration{ tick_duration }
{}

template <util::meta::specialization_of_c<std::chrono::duration> Duration_T>
auto modules::time::VariableTimer<Duration_T>::delta_ticks() const -> uint32_t
{
    return static_cast<uint32_t>(std::floor(m_delta_time / m_tick_duration));
}

template <util::meta::specialization_of_c<std::chrono::duration> Duration_T>
auto modules::time::VariableTimer<Duration_T>::current() const -> Clock::time_point
{
    return m_current;
}

template <util::meta::specialization_of_c<std::chrono::duration> Duration_T>
auto modules::time::VariableTimer<Duration_T>::tick_duration() const -> Duration_T
{
    return m_tick_duration;
}

template <util::meta::specialization_of_c<std::chrono::duration> Duration_T>
auto modules::time::VariableTimer<Duration_T>::adjust_tick_duration(
    const Duration_T tick_duration
) -> void
{
    m_tick_duration = tick_duration;
}

template <util::meta::specialization_of_c<std::chrono::duration> Duration_T>
auto modules::time::VariableTimer<Duration_T>::update(const Clock::time_point current)
    -> void
{
    if (delta_ticks() >= 1) {
        m_delta_time -= std::chrono::duration_cast<Delta>(delta_ticks() * tick_duration());
    }

    const auto elapsed_time{ current - m_current };
    const auto elapsed_ticks{
        static_cast<uint32_t>(std::floor(elapsed_time / tick_duration()))
    };
    if (elapsed_ticks >= 1) {
        const auto elapsed_delta{
            std::chrono::duration_cast<Delta>(elapsed_ticks * tick_duration())
        };
        m_current += elapsed_delta;
        m_delta_time += elapsed_delta;
    }
}

template <util::meta::specialization_of_c<std::chrono::duration> Duration_T>
auto modules::time::VariableTimer<Duration_T>::reset(const Clock::time_point current) -> void
{
    m_current    = current;
    m_delta_time = Delta{};
}
