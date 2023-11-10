#pragma once

#include <SFML/Window/VideoMode.hpp>

#include "engine/common/Plugin.hpp"

#include "Style.hpp"

namespace engine::window {

class Plugin {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(
        Store&               t_store,
        const sf::VideoMode& t_video_mode,
        std::string_view     t_title,
        Style                t_style
    ) noexcept -> void;
};

static_assert(PluginConcept<
              Plugin,
              const sf::VideoMode&,
              std::string_view,
              Style>);

}   // namespace engine::window
