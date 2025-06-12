export module core.measurement.refresh_rate;

import mp_units.core;

import core.measurement.tick_rate;

namespace core::measurement {

export constexpr inline struct refresh_rate final   //
    : mp_units::quantity_spec<tick_rate> {
} refresh_rate;

}   // namespace core::measurement
