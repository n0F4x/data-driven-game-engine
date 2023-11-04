#pragma once

#include "engine/app/Plugin.hpp"

namespace engine::plugins {

class AssetManager {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Store& t_store) noexcept -> void;
};

static_assert(PluginConcept<AssetManager>);

}   // namespace engine::plugins
