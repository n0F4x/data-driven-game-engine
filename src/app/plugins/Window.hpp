#pragma once

#include <concepts>
#include <string>

#include "app/core/Plugin.hpp"

namespace app::plugins {

class Window {
public:
    ///------------------///
    ///  Static methods  ///
    ///------------------///
    static auto default_configure() -> void;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(
        App::Builder&      t_builder,
        uint16_t           t_width,
        uint16_t           t_height,
        const std::string& t_title
    ) -> void;

    auto operator()(
        App::Builder&       t_builder,
        uint16_t            t_width,
        uint16_t            t_height,
        const std::string&  t_title,
        std::invocable auto t_configure
    ) -> void;
};

static_assert(PluginConcept<Window, uint16_t, uint16_t, const std::string&>);

}   // namespace engine::window

#include "Window.inl"
