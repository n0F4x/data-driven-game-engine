module;

#include "core/window/Window.hpp"

module plugins.window.Window;

namespace plugins::window {

auto WindowPlugin::operator()() const -> core::window::Window
{
    return core::window::Window{size, title};
}

}   // namespace plugins::window
