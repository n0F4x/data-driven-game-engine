module;

#include <gsl/gsl-lite.hpp>

export module addons.window.WindowPlugin;

import utility.Size;

import core.window.Window;

namespace addons {

namespace window {

// NOLINTNEXTLINE(*-member-init)
export struct WindowPlugin {
    utils::Size2i       size;
    gsl_lite::czstring title;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()() const -> core::window::Window;
};

}   // namespace window

export using Window = window::WindowPlugin;

}   // namespace addons
