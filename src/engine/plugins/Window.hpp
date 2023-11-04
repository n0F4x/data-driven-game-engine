#pragma once

#include "engine/app/Plugin.hpp"
#include "engine/plugins/window/Window.hpp"

namespace engine::plugins {

class Window {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(
        App::Store&           t_store,
        const sf::VideoMode&  t_video_mode,
        std::string_view      t_title,
        window::Window::Style t_style
    ) noexcept -> void;
};

static_assert(PluginConcept<
              Window,
              const sf::VideoMode&,
              std::string_view,
              window::Window::Style>);

}   // namespace engine::plugins
