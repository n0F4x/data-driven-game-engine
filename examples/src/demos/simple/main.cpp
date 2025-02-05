#include <print>

#include <SFML/Graphics.hpp>

import core.app;
import addons;
import extensions;
import plugins;

import ecs;

using Position = float;
using Velocity = double;
using Health   = int;

struct Renderable {
    int hi{};
};

struct Collider {
    int hi{};
};

auto game_loop(
    const ecs::Res<sf::RenderWindow> window,
    const ecs::Query<
        const Position,
        ecs::Without<Health, const Velocity>,
        ecs::Optional<Renderable, const Collider>> entities
) -> void
{
    for (auto [position, opt_renderable, opt_collider] : entities)
    {
        static_assert(std::same_as<
                      decltype(opt_renderable),
                      std::optional<std::reference_wrapper<Renderable>>>);
        static_assert(std::same_as<
                      decltype(opt_collider),
                      std::optional<std::reference_wrapper<const Collider>>>);

        std::println("{}", position);
    }

    window->create(sf::VideoMode::getDesktopMode(), "Simple Window");

    while (window->isOpen()) {
        while (const std::optional event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window->close();
            }
        }

        window->clear();
        window->display();
    }
}

auto main() -> int
{
    core::app::create()
        .extend_with<extensions::ResourceManager>()
        .use_resource(sf::RenderWindow{})
        .extend_with<extensions::AddonManager>()
        .use_addon<ecs::RegistryAddon>()
        .use_addon<ecs::SchedulerAddon>(ecs::Scheduler{}.schedule(::game_loop))
        .extend_with<extensions::Runnable>()
        .run(ecs::schedule_runner);
}
