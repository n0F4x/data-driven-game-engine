module;

#include <utility>

export module snake.assets.inject_loaders;

import ddge.prelude;

import snake.assets.TextureLoader;

namespace assets {

export inline constexpr auto inject_loaders =
    []<ddge::app::decays_to_builder_c Builder_T>(Builder_T&& builder) {
        static_assert(ddge::app::has_plugins_c<Builder_T, ddge::plugins::Assets>);

        return std::forward<Builder_T>(builder).insert_loader(TextureLoader{});
    };

}   // namespace assets
