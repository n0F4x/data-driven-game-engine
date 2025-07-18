module;

#include <type_traits>

export module snake.window.DisplayTimer;

import core.time.FixedTimer;

import snake.window.display_time;

namespace window {

export using DisplayTimer = core::time::
    FixedTimer<std::remove_cvref_t<decltype(display_time)>, display_time.count()>;

}   // namespace window
