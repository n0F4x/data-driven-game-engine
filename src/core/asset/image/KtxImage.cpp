#include "KtxImage.hpp"

#include <spdlog/spdlog.h>

namespace core::asset {

auto KtxImage::load_from_file(const std::filesystem::path& t_filepath
) -> std::optional<KtxImage>
{
    ktxTexture* ktxTexture;

    if (const ktxResult result{ ktxTexture_CreateFromNamedFile(
            t_filepath.generic_string().c_str(),
            KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
            &ktxTexture
        ) };
        result != KTX_SUCCESS && result != KTX_UNKNOWN_FILE_FORMAT)
    {
        SPDLOG_ERROR(
            "ktxTexture_CreateFromNamedFile failed loading file {} with '{}'",
            t_filepath.generic_string(),
            ktxErrorString(result)
        );

        return std::nullopt;
    }

    return KtxImage{ ktxTexture };
}

auto KtxImage::load_from_memory(const std::span<const std::uint8_t> t_data
) -> std::optional<KtxImage>
{
    ktxTexture* ktxTexture;

    if (const ktxResult result{ ktxTexture_CreateFromMemory(
            t_data.data(), t_data.size(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture
        ) };
        result != KTX_SUCCESS && result != KTX_UNKNOWN_FILE_FORMAT)
    {
        SPDLOG_ERROR(
            "ktxTexture_CreateFromMemory failed with '{}'", ktxErrorString(result)
        );

        return std::nullopt;
    }

    return KtxImage{ ktxTexture };
}

auto KtxImage::operator->() const noexcept -> ktxTexture*
{
    return m_ktxTexture.operator->();
}

auto KtxImage::operator*() noexcept -> ktxTexture&
{
    return m_ktxTexture.operator*();
}

auto KtxImage::operator*() const noexcept -> const ktxTexture&
{
    return m_ktxTexture.operator*();
}

auto KtxImage::get() const noexcept -> ktxTexture*
{
    return m_ktxTexture.get();
}

auto KtxImage::Deleter::operator()(ktxTexture* t_ktxTexture) const noexcept -> void
{
    ktxTexture_Destroy(t_ktxTexture);
}

KtxImage::KtxImage(ktxTexture* t_ktxTexture) noexcept : m_ktxTexture{ t_ktxTexture } {}

}   // namespace core::asset
