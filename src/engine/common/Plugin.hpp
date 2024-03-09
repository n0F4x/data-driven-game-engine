#pragma once

#include "engine/app/Plugin.hpp"

namespace engine::common {

class Plugin {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Builder& t_builder) noexcept -> void;
};

static_assert(PluginConcept<Plugin>);

}   // namespace engine::common
