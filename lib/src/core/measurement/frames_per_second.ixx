module;

#include <mp-units/framework.h>
#include <mp-units/systems/si.h>

export module core.measurement.frames_per_second;

import core.measurement.refresh_rate;

namespace core::measurement {

export constexpr inline struct frames_per_second final
    : mp_units::named_unit<
          "fps",
          mp_units::one / mp_units::si::second,
          mp_units::get_kind(refresh_rate)> {
} frames_per_second;

namespace symbols {

export constexpr inline auto fps = frames_per_second;

}   // namespace symbols

}   // namespace core::measurement
