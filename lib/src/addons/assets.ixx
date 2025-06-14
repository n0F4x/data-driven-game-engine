export module addons.Assets;

import core.assets;

namespace addons {

export struct AssetsTag {};

export template <core::assets::loader_c... Loaders_T>
struct Assets : AssetsTag {
    core::assets::AssetServer<Loaders_T...> asset_server;
};

}   // namespace addons
