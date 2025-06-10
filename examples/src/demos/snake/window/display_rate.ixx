export module demo.window.display_rate;

import core.measurement.updates_per_second;

using namespace core::measurement::literals;

namespace window {

export inline constexpr auto display_rate{ 60_ups };

}   // namespace window
