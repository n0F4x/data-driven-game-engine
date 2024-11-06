#pragma once

#include <gsl/gsl-lite.hpp>

#include "core/utility/Size.hpp"

class App;

namespace plugins {

namespace window {

// NOLINTNEXTLINE(*-member-init)
struct Window {
    core::Size2i       size;
    gsl_lite::czstring title;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App& app) const -> void;
};

}   // namespace window

using Window = window::Window;

}   // namespace plugins
