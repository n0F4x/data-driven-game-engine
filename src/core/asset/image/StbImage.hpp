#pragma once

#include <filesystem>
#include <optional>
#include <span>

#include <stb_image.h>

namespace core::asset {

class StbImage {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& t_filepath
    ) -> std::optional<StbImage>;

    [[nodiscard]]
    static auto load_from_memory(std::span<const std::uint8_t> t_data
    ) -> std::optional<StbImage>;

    [[nodiscard]]
    auto data() const noexcept -> stbi_uc*;
    [[nodiscard]]
    auto width() const noexcept -> int;
    [[nodiscard]]
    auto height() const noexcept -> int;

private:
    std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> m_data;
    int                                                  m_width;
    int                                                  m_height;

    explicit StbImage(stbi_uc* t_data, int t_width, int t_height) noexcept;
};

}   // namespace core::asset
