#pragma once

#include <gsl/gsl-lite.hpp>

#include "core/utility/Size.hpp"

class App;

namespace core::window {

class Window;

}   // namespace core::window

namespace plugins {

namespace window {

// NOLINTNEXTLINE(*-member-init)
struct Window {
    core::Size2i       size;
    gsl_lite::czstring title;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()() const -> core::window::Window;
};

}   // namespace window

using Window = window::Window;

}   // namespace plugins
