module;

#include <filesystem>

#include <SFML/Graphics.hpp>

export module snake.assets.TextureLoader;

import snake.assets.root_directory;

namespace assets {

export struct TextureLoader {
    static auto operator()(const std::filesystem::path& filename) -> sf::Texture;
};

}   // namespace assets

module :private;

auto assets::TextureLoader::operator()(const std::filesystem::path& filename)
    -> sf::Texture
{
    return sf::Texture{ root_directory() / filename };
}
