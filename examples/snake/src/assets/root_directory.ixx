module;

#include <filesystem>
#include <source_location>

export module snake.assets.root_directory;

namespace assets {

export [[nodiscard]]
auto root_directory() -> const std::filesystem::path&;

}   // namespace assets

module :private;

auto assets::root_directory() -> const std::filesystem::path&
{
    static const std::filesystem::path result{
        std::filesystem::path{ std::source_location::current().file_name() }
            .parent_path()
            .parent_path()
            .parent_path()
        / "assets"
    };

    return result;
}
