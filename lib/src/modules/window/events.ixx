module;

#include <chrono>

export module modules.window.events;

namespace modules::window {

export auto poll_events() -> void;

export auto wait_for_events() -> void;

export auto wait_for_events(std::chrono::duration<float> timeout) -> void;

}   // namespace modules::window
