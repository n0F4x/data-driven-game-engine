#pragma once

#include "app/Plugin.hpp"

namespace plugins {

class Cache {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(app::App::Builder& t_builder) const noexcept -> void;
};

static_assert(app::PluginConcept<Cache>);

}   // namespace plugins
