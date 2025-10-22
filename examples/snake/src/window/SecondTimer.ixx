module;

#include <chrono>
#include <type_traits>

export module snake.window.SecondTimer;

import ddge.modules.time.FixedTimer;

using namespace std::chrono_literals;

namespace window {

export class SecondTimer
    : public ddge::time::FixedTimer<std::remove_cvref_t<decltype(1s)>, (1s).count()> {};

}   // namespace window
