#include "KtxImage.hpp"

#include <spdlog/spdlog.h>

static auto transcode(ktxTexture2* t_texture) -> void
{
    if (ktxTexture2_NeedsTranscoding(t_texture)) {
        const ktx_transcode_fmt_e target_format = KTX_TTF_RGBA32;

        const auto error{
            ktxTexture2_TranscodeBasis(t_texture, target_format, KTX_TF_HIGH_QUALITY)
        };
        if (error != KTX_SUCCESS) {
            throw std::runtime_error(fmt::format(
                "Failed to transcode basisu from KTX2 texture: {}",
                std::to_underlying(error)
            ));
        }
    }
}

namespace core::image {

auto KtxImage::load_from_file(const std::filesystem::path& t_filepath
) -> std::optional<KtxImage>
{
    ktxTexture2* texture{};

    if (const ktxResult result{ ktxTexture2_CreateFromNamedFile(
            t_filepath.generic_string().c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &texture
        ) };
        result != KTX_SUCCESS && result != KTX_UNKNOWN_FILE_FORMAT)
    {
        SPDLOG_ERROR(
            "ktxTexture2_CreateFromNamedFile failed loading file {} with '{}'",
            t_filepath.generic_string(),
            ktxErrorString(result)
        );

        return std::nullopt;
    }

    transcode(texture);

    return KtxImage{ texture };
}

auto KtxImage::load_from_memory(const std::span<const std::uint8_t> t_data
) -> std::optional<KtxImage>
{
    ktxTexture2* texture{};

    if (ktxTexture2_CreateFromMemory(
            t_data.data(), t_data.size(), KTX_TEXTURE_CREATE_NO_FLAGS, nullptr
        )
        != KTX_SUCCESS)
    {
        return std::nullopt;
    }

    if (const ktxResult result{ ktxTexture2_CreateFromMemory(
            t_data.data(), t_data.size(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture
        ) };
        result != KTX_SUCCESS)
    {
        SPDLOG_ERROR(
            "ktxTexture2_CreateFromMemory failed with '{}'", ktxErrorString(result)
        );

        return std::nullopt;
    }

    transcode(texture);

    return KtxImage{ texture };
}

auto KtxImage::operator->() const noexcept -> ktxTexture2*
{
    return m_ktxTexture.operator->().operator->();
}

auto KtxImage::operator*() noexcept -> ktxTexture2&
{
    return m_ktxTexture.operator*();
}

auto KtxImage::operator*() const noexcept -> const ktxTexture2&
{
    return m_ktxTexture.operator*();
}

auto KtxImage::get() const noexcept -> ktxTexture2*
{
    return m_ktxTexture.get();
}

auto KtxImage::data() const noexcept -> void*
{
    return m_ktxTexture->pData;
}

auto KtxImage::size() const noexcept -> size_t
{
    return m_ktxTexture->dataSize;
}

auto KtxImage::width() const noexcept -> uint32_t
{
    return m_ktxTexture->baseWidth;
}

auto KtxImage::height() const noexcept -> uint32_t
{
    return m_ktxTexture->baseHeight;
}

auto KtxImage::depth() const noexcept -> uint32_t
{
    return m_ktxTexture->baseDepth;
}

auto KtxImage::mip_levels() const noexcept -> uint32_t
{
    return m_ktxTexture->numLevels;
}

auto KtxImage::format() const noexcept -> vk::Format
{
    return static_cast<vk::Format>(m_ktxTexture->vkFormat);
}

auto KtxImage::offset(uint32_t t_mip_level, uint32_t t_layer, uint32_t t_face_slice)
    const noexcept -> uint64_t
{
    assert(t_mip_level < mip_levels());
    assert(t_layer < m_ktxTexture->numLayers);
    assert(t_face_slice < m_ktxTexture->numFaces);

    uint64_t offset{};
    ktxTexture_GetImageOffset(
        ktxTexture(m_ktxTexture.get()), t_mip_level, t_layer, t_face_slice, &offset
    );
    return offset;
}

auto KtxImage::Deleter::operator()(ktxTexture2* t_ktxTexture) const noexcept -> void
{
    ktxTexture_Destroy(ktxTexture(t_ktxTexture));
}

KtxImage::KtxImage(ktxTexture2* t_ktxTexture) noexcept : m_ktxTexture{ t_ktxTexture } {}

}   // namespace core::image
