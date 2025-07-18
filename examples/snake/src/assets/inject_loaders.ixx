module;

#include <utility>

export module snake.assets.inject_loaders;

import app;
import plugins.assets;

import snake.assets.TextureLoader;

namespace assets {

export inline constexpr auto inject_loaders =
    []<app::decays_to_builder_c Builder_T>(Builder_T&& builder) {
        static_assert(app::has_plugins_c<Builder_T, plugins::AssetsTag>);

        return std::forward<Builder_T>(builder).insert_loader(TextureLoader{});
    };

}   // namespace assets
