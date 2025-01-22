#include <SFML/Graphics.hpp>

import core.app;
import addons.store;
import addons.runnable;

constexpr auto run{ [](core::app::mixed_with_c<addons::store::Mixin> auto&& app) -> void {
    sf::RenderWindow& window = app.resources.template at<sf::RenderWindow>();

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        window.display();
    }
} };

auto main() -> int
{
    core::app::create()
        .customize<addons::store::Customization>()
        .use([] {
            sf::RenderWindow window{ sf::VideoMode{ { 1'920u, 1'080u } },
                                     "CMake SFML Project" };
            window.setFramerateLimit(144);
            return window;
        }())
        .customize<addons::runnable::Customization>()
        .run(run);
}
