#pragma once

#include "app/Plugin.hpp"

namespace plugins {

class Common {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(app::App::Builder& t_builder) const noexcept -> void;
};

static_assert(app::PluginConcept<Common>);

}   // namespace plugins
