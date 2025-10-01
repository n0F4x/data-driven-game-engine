module;

#include <random>

#include <SFML/Graphics.hpp>

module snake.game.tasks.spawn_apple;

import ddge.modules.assets.Handle;
import ddge.modules.ecs;
import ddge.modules.time.FixedTimer;

import ddge.modules.execution.accessors.assets.Cached;
import ddge.modules.execution.accessors.ecs.Registry;
import ddge.modules.execution.providers.AssetProvider;
import ddge.modules.execution.providers.ECSProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.assets.TextureLoader;
import snake.game.Apple;
import snake.game.Cell;
import snake.game.Position;
import snake.game.Snake;

using namespace ddge::ecs::query_filter_tags;

using namespace ddge::exec::accessors::ecs;

using CachedTextureLoader = ddge::exec::accessors::assets::Cached<assets::TextureLoader>;

auto game::spawn_apple() -> ddge::exec::v2::TaskBuilder<void>
{
    return ddge::exec::v2::as_task(
        +[](const CachedTextureLoader texture_loader, const Registry registry) -> void {
            static std::random_device random_device;
            static std::mt19937       random_engine{ random_device() };

            std::vector<ddge::ecs::ID> empty_cell_ids;
            ddge::ecs::query(
                registry.get(),
                [&empty_cell_ids](const ddge::ecs::ID id, With<Cell>, Without<Snake>, Without<Apple>) {
                    empty_cell_ids.push_back(id);
                }
            );

            std::uniform_int_distribution distribution{ 0uz, empty_cell_ids.size() - 1uz };

            const ddge::ecs::ID new_apple_id{
                empty_cell_ids.at(distribution(random_engine))
            };

            const ddge::assets::Handle texture_handle{
                texture_loader->load("PixelApple.png")
            };

            Cell& cell{ registry->get_single<Cell>(new_apple_id) };
            cell.shape.setTexture(texture_handle.get());

            registry->insert(new_apple_id, Apple{});
        }
    );
}
