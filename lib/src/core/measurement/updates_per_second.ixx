module;

#include <mp-units/framework.h>
#include <mp-units/systems/si.h>

export module core.measurement.updates_per_second;

import core.measurement.tick_rate;

namespace core::measurement {

export constexpr inline struct updates_per_second final
    : mp_units::named_unit<
          "ups",
          mp_units::one / mp_units::si::second,
          mp_units::get_kind(tick_rate)> {
} updates_per_second;

namespace symbols {

export constexpr inline auto ups = updates_per_second;

}   // namespace symbols

namespace literals {

export [[nodiscard]]
constexpr auto operator""_ups(unsigned long long int amount)
    -> mp_units::quantity<updates_per_second>
{
    return amount * updates_per_second;
}

}   // namespace literals

}   // namespace core::measurement
