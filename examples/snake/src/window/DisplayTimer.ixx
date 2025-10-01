module;

#include <type_traits>

export module snake.window.DisplayTimer;

import ddge.modules.time.FixedTimer;

import snake.window.display_refresh_duration;

namespace window {

export class DisplayTimer : public ddge::time::FixedTimer<
                                std::remove_cvref_t<decltype(display_refresh_duration)>,
                                display_refresh_duration.count()> {};

}   // namespace window
