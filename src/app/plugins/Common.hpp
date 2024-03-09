#pragma once

#include "app/core/Plugin.hpp"

namespace app::plugins {

class Common {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Builder& t_builder) noexcept -> void;
};

static_assert(PluginConcept<Common>);

}   // namespace engine::common
