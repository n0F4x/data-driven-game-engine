export module core.measurement.tick_rate;

import mp_units;

namespace core::measurement {

export constexpr inline struct tick_rate final
    : mp_units::quantity_spec<mp_units::isq::frequency> {
} tick_rate;

}   // namespace core::measurement
