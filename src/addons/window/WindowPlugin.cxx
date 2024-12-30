module;

module addons.window.WindowPlugin;

import core.window.Window;

namespace addons::window {

auto WindowPlugin::operator()() const -> core::window::Window
{
    return core::window::Window{size, title};
}

}   // namespace addons::window
