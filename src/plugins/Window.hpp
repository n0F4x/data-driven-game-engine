#pragma once

#include <gsl/gsl-lite.hpp>

#include "core/utility/Size.hpp"

#include "app.hpp"

namespace plugins {

class Window {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(Store& store, const core::Size2i& size, gsl_lite::czstring title)
        const -> void;
};

}   // namespace plugins
