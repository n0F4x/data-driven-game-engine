module;

#include <gsl/gsl-lite.hpp>

#include "core/utility/Size.hpp"

namespace core::window {

class Window;

}   // namespace core::window

export module plugins.window.Window;

namespace plugins {

namespace window {

// NOLINTNEXTLINE(*-member-init)
export struct WindowPlugin {
    core::Size2i       size;
    gsl_lite::czstring title;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()() const -> core::window::Window;
};

}   // namespace window

export using Window = window::WindowPlugin;

}   // namespace plugins
