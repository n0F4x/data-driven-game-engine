module;

#include <string>
#include <utility>

#include <SFML/Window.hpp>

export module snake.window.setup;

import app.decays_to_builder_c;
import app.has_plugins_c;

import modules.time.FixedTimer;

import snake.window.DisplayTimer;
import snake.window.Settings;
import snake.window.Window;

import plugins.events;
import plugins.resources;

namespace window {

export inline constexpr auto setup =
    []<app::decays_to_builder_c Builder_T>(Builder_T&& builder) -> Builder_T   //
{
    constexpr static Settings settings{
        .width  = 1'280,
        .height = 720,
        .title{ "Snake" },
    };

    static_assert(app::has_plugins_c<Builder_T, plugins::Resources>);
    static_assert(app::has_plugins_c<Builder_T, plugins::Events>);

    return std::forward<Builder_T>(builder)
        .insert_resource(settings)
        .insert_resource(
            Window{ sf::VideoMode{ sf::Vector2u{ settings.width, settings.height } },
                    std::string{ settings.title } }
        )
        .insert_resource(DisplayTimer{})
        .template register_event<sf::Event>();
};

}   // namespace window
