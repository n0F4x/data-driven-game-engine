#pragma once

#include "app.hpp"

namespace plugins {

class Cache {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Builder& t_builder) const noexcept -> void;
};

static_assert(PluginConcept<Cache>);

}   // namespace plugins
