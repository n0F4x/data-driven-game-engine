module;

#include <chrono>
#include <cmath>
#include <type_traits>

export module core.time.FixedTimer;

import utility.meta.concepts.specialization_of;

namespace core::time {

// TODO: take only tick_duration_T when it becomes a valid non-type parameter
export template <
    util::meta::specialization_of_c<std::chrono::duration> Duration_T,
    typename Duration_T::rep                               tick_duration_T>
class FixedTimer {
public:
    using Clock = std::chrono::steady_clock;
    using Delta = Clock::duration;

    [[nodiscard]]
    constexpr static auto tick_duration() -> Duration_T;

    [[nodiscard]]
    auto delta_ticks() const -> uint32_t;
    [[nodiscard]]
    auto current() const -> Clock::time_point;

    auto update(Clock::time_point current = Clock::now()) -> void;
    auto reset(Clock::time_point current = Clock::now()) -> void;

private:
    Clock::time_point m_current{ Clock::now() };
    Delta             m_delta_time{};
};

template <typename T>
struct IsSpecializationOfFixedTimer;

template <
    util::meta::specialization_of_c<std::chrono::duration> Duration_T,
    typename Duration_T::rep                               tick_duration_T>
struct IsSpecializationOfFixedTimer<FixedTimer<Duration_T, tick_duration_T>>
    : std::true_type {};

export template <typename T>
concept specialization_of_FixedTimer_c = IsSpecializationOfFixedTimer<T>::value;

}   // namespace core::time

template <
    util::meta::specialization_of_c<std::chrono::duration> Duration_T,
    typename Duration_T::rep                               tick_duration_T>
constexpr auto core::time::FixedTimer<Duration_T, tick_duration_T>::tick_duration()
    -> Duration_T
{
    return Duration_T{ tick_duration_T };
}

template <
    util::meta::specialization_of_c<std::chrono::duration> Duration_T,
    typename Duration_T::rep                               tick_duration_T>
auto core::time::FixedTimer<Duration_T, tick_duration_T>::delta_ticks() const -> uint32_t
{
    return static_cast<uint32_t>(std::floor(m_delta_time / tick_duration()));
}

template <
    util::meta::specialization_of_c<std::chrono::duration> Duration_T,
    typename Duration_T::rep                               tick_duration_T>
auto core::time::FixedTimer<Duration_T, tick_duration_T>::current() const
    -> Clock::time_point
{
    return m_current;
}

template <
    util::meta::specialization_of_c<std::chrono::duration> Duration_T,
    typename Duration_T::rep                               tick_duration_T>
auto core::time::FixedTimer<Duration_T, tick_duration_T>::update(
    const Clock::time_point current
) -> void
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

template <
    util::meta::specialization_of_c<std::chrono::duration> Duration_T,
    typename Duration_T::rep                               tick_duration_T>
auto core::time::FixedTimer<Duration_T, tick_duration_T>::reset(
    const Clock::time_point current
) -> void
{
    m_current    = current;
    m_delta_time = Delta{};
}
