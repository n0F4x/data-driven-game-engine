module;

#include <algorithm>
#include <print>
#include <random>

#include <SFML/Graphics/RectangleShape.hpp>

export module snake.game.spawn_apple;

import core.assets.Handle;
import core.ecs;
import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.assets.Cached;
import extensions.scheduler.accessors.ecs.Registry;

import snake.assets.TextureLoader;
import snake.game.Apple;
import snake.game.Cell;
import snake.game.Position;
import snake.game.Snake;

using namespace core::ecs::query_parameter_tags;

using namespace extensions::scheduler::accessors::ecs;

using CachedTextureLoader =
    extensions::scheduler::accessors::assets::Cached<assets::TextureLoader>;

namespace game {

export inline constexpr auto spawn_apple =   //
    [](const CachedTextureLoader texture_loader,
       const Registry            registry) -> void      //
{
    static std::random_device random_device;
    static std::mt19937       random_engine{ random_device() };

    std::vector<core::ecs::ID> empty_cell_ids;
    core::ecs::query(
        registry.get(),
        [&empty_cell_ids](const core::ecs::ID id, With<Cell>, Without<Snake>, Without<Apple>) {
            empty_cell_ids.push_back(id);
        }
    );

    std::uniform_int_distribution distribution{ 0uz, empty_cell_ids.size() - 1uz };

    const core::ecs::ID new_apple_id{ empty_cell_ids.at(distribution(random_engine)) };

    const core::assets::Handle texture_handle{ texture_loader->load("PixelApple.png") };

    Cell& cell{ registry->get_single<Cell>(new_apple_id) };
    cell.shape.setTexture(texture_handle.get());

    registry->insert(new_apple_id, Apple{});
};

}   // namespace game
