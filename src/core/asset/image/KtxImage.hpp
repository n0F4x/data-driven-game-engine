#pragma once

#include <filesystem>
#include <span>

#include <tl/optional.hpp>

#include <ktx.h>

namespace core::asset {

class KtxImage {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& t_filepath
    ) -> tl::optional<KtxImage>;

    [[nodiscard]]
    static auto load_from_memory(std::span<const std::uint8_t> t_data
    ) -> tl::optional<KtxImage>;

    [[nodiscard]]
    auto operator->() const noexcept -> ktxTexture*;
    [[nodiscard]]
    auto operator*() noexcept -> ktxTexture&;
    [[nodiscard]]
    auto operator*() const noexcept -> const ktxTexture&;

    [[nodiscard]]
    auto get() const noexcept -> ktxTexture*;

private:
    struct Deleter {
        auto operator()(ktxTexture* t_ktxTexture) const noexcept -> void;
    };

    std::unique_ptr<ktxTexture, Deleter> m_ktxTexture;

    explicit KtxImage(ktxTexture* t_ktxTexture) noexcept;
};

}   // namespace core::asset
