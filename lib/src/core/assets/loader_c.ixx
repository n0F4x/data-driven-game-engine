module;

#include <concepts>
#include <filesystem>

export module core.assets.loader_c;

import core.assets.asset_c;

import utility.meta.concepts.decayed;

namespace core::assets {

export template <typename T>
concept loader_c = util::meta::decayed_c<T>   //
                && requires { requires asset_c<typename T::Asset>; }
                && requires(T loader, std::filesystem::path path) {
                       {
                           loader.load(path)
                       } -> std::same_as<typename T::Asset>;
                   };

}   // namespace core::assets
