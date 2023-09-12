#pragma once

#include "engine/app/Plugin.hpp"
#include "engine/plugins/window/Window.hpp"

namespace engine::plugins {

class Window {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Window(
        sf::VideoMode         t_video_mode,
        std::string_view      t_title,
        window::Window::Style t_style
    ) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto inject(App::Context& t_context) const noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    sf::VideoMode         m_video_mode;
    std::string_view      m_title;
    window::Window::Style m_style;
};

static_assert(PluginConcept<Window>);

}   // namespace engine::plugins
