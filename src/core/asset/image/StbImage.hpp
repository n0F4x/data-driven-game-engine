#pragma once

#include <filesystem>
#include <span>

#include <tl/optional.hpp>

#include <stb_image.h>

namespace core::asset {

class StbImage {
public:
    [[nodiscard]] static auto load_from_file(const std::filesystem::path& t_filepath)
        -> tl::optional<StbImage>;

    [[nodiscard]] static auto load_from_memory(const std::span<const std::uint8_t> t_data)
        -> tl::optional<StbImage>;

private:
    //    StbImageInfo m_info;
    //    stbi_uc      m_data;
};

}   // namespace core::asset
