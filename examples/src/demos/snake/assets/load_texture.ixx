module;

#include <filesystem>
#include <source_location>

#include <SFML/Graphics.hpp>

export module snake.assets.load_texture;

import snake.assets.root_directory;

namespace assets {

export inline constexpr auto load_texture =                    //
    [](const std::filesystem::path& filename) -> sf::Texture   //
{                                                              //
    return sf::Texture{ root_directory() / "textures" / filename };
};

}   // namespace assets
