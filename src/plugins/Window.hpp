#pragma once

#include <gsl/gsl-lite.hpp>

#include "core/utility/Size.hpp"

#include "app.hpp"

namespace plugins {

class Window {
public:
    core::Size2i       size;
    gsl_lite::czstring title;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App& app) const -> void;

};

}   // namespace plugins
