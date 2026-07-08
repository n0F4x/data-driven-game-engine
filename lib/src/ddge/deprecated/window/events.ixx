module;

#include <chrono>

export module ddge.deprecated.window.events;

namespace ddge::window {

export auto poll_events() -> void;

export auto wait_for_events() -> void;

export auto wait_for_events(std::chrono::duration<float> timeout) -> void;

}   // namespace ddge::window
