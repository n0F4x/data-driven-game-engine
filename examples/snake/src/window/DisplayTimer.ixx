module;

#include <type_traits>

export module snake.window.DisplayTimer;

import modules.time.FixedTimer;

import snake.window.display_time;

namespace window {

export class DisplayTimer
    : public modules::time::
          FixedTimer<std::remove_cvref_t<decltype(display_time)>, display_time.count()> {
};

}   // namespace window
