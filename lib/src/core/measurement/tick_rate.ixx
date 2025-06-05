module;

#include <mp-units/framework.h>
#include <mp-units/systems/si.h>

export module core.measurement.tick_rate;

namespace core::measurement {

export constexpr inline struct tick_rate final
    : mp_units::quantity_spec<mp_units::isq::frequency> {
} tick_rate;

}   // namespace core::measurement
