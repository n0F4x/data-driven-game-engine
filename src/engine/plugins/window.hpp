#pragma once

#include "engine/common/app/Plugin.hpp"
#include "engine/plugins/window/Window.hpp"

namespace engine::window {

class WindowPlugin {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit WindowPlugin(
        sf::VideoMode t_video_mode,
        std::string   t_title,
        Window::Style t_style
    ) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto set_context(App::Context& t_context) const noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    sf::VideoMode m_video_mode;
    std::string   m_title;
    Window::Style m_style;
};

static_assert(PluginConcept<WindowPlugin>);

}   // namespace engine::window
