module;

#include <string>
#include <utility>

#include <SFML/Window.hpp>

export module snake.window.setup;

import ddge.modules.app.decays_to_builder_c;
import ddge.modules.app.has_plugins_c;
import ddge.modules.events.Plugin;
import ddge.modules.resources.Plugin;
import ddge.modules.time.FixedTimer;

import snake.window.DisplayTimer;
import snake.window.Settings;
import snake.window.Window;

namespace window {

export inline constexpr auto setup =
    []<ddge::app::decays_to_builder_c Builder_T>(Builder_T&& builder) -> Builder_T   //
{
    constexpr static Settings settings{
        .width  = 1'280,
        .height = 720,
        .title{ "Snake" },
    };

    static_assert(ddge::app::has_plugins_c<Builder_T, ddge::resources::Plugin>);
    static_assert(ddge::app::has_plugins_c<Builder_T, ddge::events::Plugin>);

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
