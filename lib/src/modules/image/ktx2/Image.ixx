module;

#include <filesystem>
#include <span>

#include <gsl-lite/gsl-lite.hpp>

#include <ktx.h>

export module modules.image.ktx2.Image;

import vulkan_hpp;

import modules.image.Image;

namespace modules::image::ktx2 {

export class Image : public image::Image {
public:
    [[nodiscard]]
    static auto load_from(const std::filesystem::path& filepath) -> Image;

    [[nodiscard]]
    static auto load_from(std::span<const std::byte> data) -> Image;

    Image()           = delete;
    Image(Image&&)    = default;
    ~Image() override = default;

    auto operator=(Image&&) -> Image& = default;

    [[nodiscard]]
    auto data() const noexcept -> std::span<const std::byte> final;

    [[nodiscard]]
    auto width() const noexcept -> uint32_t final;
    [[nodiscard]]
    auto height() const noexcept -> uint32_t final;
    [[nodiscard]]
    auto depth() const noexcept -> uint32_t final;

    [[nodiscard]]
    auto mip_level_count() const noexcept -> uint32_t final;
    [[nodiscard]]
    auto needs_mip_generation() const noexcept -> bool final;

    [[nodiscard]]
    auto format() const noexcept -> vk::Format final;

    [[nodiscard]]
    auto offset_of(uint32_t mip_level, uint32_t layer, uint32_t face_slice) const noexcept
        -> uint64_t final;

private:
    struct Deleter {
        auto operator()(ktxTexture2* texture) const noexcept -> void;
    };

    gsl_lite::not_null<std::unique_ptr<ktxTexture2, Deleter>> m_impl;

    explicit Image(gsl_lite::not_null<ktxTexture2*> texture) noexcept;
};

}   // namespace modules::image::ktx2
