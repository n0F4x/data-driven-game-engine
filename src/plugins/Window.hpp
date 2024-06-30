#pragma once

#include <string>

#include "app.hpp"

namespace plugins {

class Window {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(
        App::Builder&      t_builder,
        uint16_t           t_width,
        uint16_t           t_height,
        const std::string& t_title
    ) const -> void;

    auto operator()(
        App::Builder&       t_builder,
        uint16_t            t_width,
        uint16_t            t_height,
        const std::string&  t_title,
        std::invocable auto t_configure
    ) const -> void;
};

static_assert(PluginConcept<Window, uint16_t, uint16_t, const std::string&>);

}   // namespace plugins

#include "Window.inl"
