#pragma once

#include <filesystem>
#include <optional>
#include <span>

#include <ktx.h>

#include "Image.hpp"

namespace core::image {

class KtxImage final : public Image {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& t_filepath
    ) -> std::optional<KtxImage>;

    [[nodiscard]]
    static auto load_from_memory(std::span<const std::uint8_t> t_data
    ) -> std::optional<KtxImage>;

    [[nodiscard]]
    auto operator->() const noexcept -> ktxTexture2*;
    [[nodiscard]]
    auto operator*() noexcept -> ktxTexture2&;
    [[nodiscard]]
    auto operator*() const noexcept -> const ktxTexture2&;

    [[nodiscard]]
    auto get() const noexcept -> ktxTexture2*;

    [[nodiscard]]
    auto data() const noexcept -> void* override;
    [[nodiscard]]
    auto size() const noexcept -> size_t override;

    [[nodiscard]]
    auto width() const noexcept -> uint32_t override;
    [[nodiscard]]
    auto height() const noexcept -> uint32_t override;
    [[nodiscard]]
    auto depth() const noexcept -> uint32_t override;

    [[nodiscard]]
    auto mip_levels() const noexcept -> uint32_t override;

    [[nodiscard]]
    auto format() const noexcept -> vk::Format override;

private:
    struct Deleter {
        auto operator()(ktxTexture2* t_ktxTexture) const noexcept -> void;
    };

    std::unique_ptr<ktxTexture2, Deleter> m_ktxTexture;

    explicit KtxImage(ktxTexture2* t_ktxTexture) noexcept;
};

}   // namespace core::image
