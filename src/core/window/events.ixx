module;

#include <chrono>

export module core.window.events;

namespace core::window {

export auto poll_events() -> void;

export auto wait_for_events() -> void;

export auto wait_for_events(std::chrono::duration<float> timeout) -> void;

}   // namespace core::window
