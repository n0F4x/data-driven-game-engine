#pragma once

#include <chrono>

namespace core::window {

auto poll_events() -> void;

auto wait_for_events() -> void;

auto wait_for_events(std::chrono::duration<float> timeout) -> void;

}   // namespace core::window
