module;

#include <filesystem>
#include <source_location>

#include <SFML/Graphics.hpp>

export module snake.assets.load_texture;

namespace assets {

export inline constexpr auto load_texture =                    //
    [](const std::filesystem::path& filename) -> sf::Texture   //
{                                                              //
    static const std::filesystem::path root_directory{
        std::filesystem::path{ std::source_location::current().file_name() }
            .parent_path()
            .parent_path()
            .parent_path()
            .parent_path()
        / "assets" / "textures"
    };

    return sf::Texture{ root_directory / filename };
};

}   // namespace assets
