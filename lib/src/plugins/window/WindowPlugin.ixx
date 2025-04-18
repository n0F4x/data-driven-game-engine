module;

#include <gsl-lite/gsl-lite.hpp>

export module plugins.window.WindowPlugin;

import utility.Size;

import core.window.Window;

namespace plugins {

namespace window {

// NOLINTNEXTLINE(*-member-init)
export struct WindowPlugin {
    util::Size2i       size;
    gsl_lite::czstring title;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()() const -> core::window::Window;
};

}   // namespace window

export using Window = window::WindowPlugin;

}   // namespace addons
