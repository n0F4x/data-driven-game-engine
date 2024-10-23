#include "Image.hpp"

#include <spdlog/spdlog.h>

#include <vulkan/vulkan.hpp>

static auto transcode(ktxTexture2* texture) -> void
{
    if (ktxTexture2_NeedsTranscoding(texture)) {
        constexpr ktx_transcode_fmt_e target_format{ KTX_TTF_RGBA32 };

        const auto error{
            ktxTexture2_TranscodeBasis(texture, target_format, KTX_TF_HIGH_QUALITY)
        };
        if (error != KTX_SUCCESS) {
            throw std::runtime_error(fmt::format(
                "Failed to transcode basisu from KTX2 texture: {}",
                std::to_underlying(error)
            ));
        }
    }
}

namespace core::image::ktx2 {

auto Image::load_from(const std::filesystem::path& filepath) -> Image
{
    ktxTexture2* texture{};

    if (const ktxResult result{ ktxTexture2_CreateFromNamedFile(
            filepath.generic_string().c_str(),
            KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
            &texture
        ) };
        result != KTX_SUCCESS)
    {
        throw std::runtime_error(std::format(
            "ktxTexture2_CreateFromNamedFile failed loading file {}: '{}'",
            filepath.generic_string(),
            ktxErrorString(result)
        ));
    }

    ::transcode(texture);

    return Image{ gsl::make_not_null(texture) };
}

auto Image::load_from(const std::span<const std::byte> data) -> Image
{
    ktxTexture2* texture{};

    if (const ktxResult result{ ktxTexture2_CreateFromMemory(
            reinterpret_cast<const ktx_uint8_t*>(data.data()),
            data.size(),
            KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
            &texture
        ) };
        result != KTX_SUCCESS)
    {
        throw std::runtime_error{ std::format(
            "ktxTexture2_CreateFromMemory failed with '{}'", ktxErrorString(result)
        ) };
    }

    ::transcode(texture);

    return Image{ gsl::make_not_null(texture) };
}

auto Image::data() const noexcept -> std::span<const std::byte>
{
    return std::as_bytes(std::span{ m_impl->pData, m_impl->dataSize });
}

auto Image::width() const noexcept -> uint32_t
{
    return m_impl->baseWidth;
}

auto Image::height() const noexcept -> uint32_t
{
    return m_impl->baseHeight;
}

auto Image::depth() const noexcept -> uint32_t
{
    return m_impl->baseDepth;
}

auto Image::mip_level_count() const noexcept -> uint32_t
{
    return std::max(m_impl->numLevels, 1u);
}

auto Image::needs_mip_generation() const noexcept -> bool
{
    return m_impl->numLevels == 0;
}

auto Image::format() const noexcept -> vk::Format
{
    return static_cast<vk::Format>(m_impl->vkFormat);
}

auto Image::offset_of(
    const uint32_t mip_level,
    const uint32_t layer,
    const uint32_t face_slice
) const noexcept -> uint64_t
{
    assert(mip_level < mip_level_count());
    assert(layer < m_impl->numLayers);
    assert(face_slice < m_impl->numFaces);

    ktx_size_t offset{};
    ktxTexture_GetImageOffset(
        ktxTexture(m_impl.get()), mip_level, layer, face_slice, &offset
    );
    return offset;
}

auto Image::Deleter::operator()(ktxTexture2* texture) const noexcept -> void
{
    ktxTexture_Destroy(ktxTexture(texture));
}

Image::Image(gsl_lite::not_null<ktxTexture2*> texture) noexcept : m_impl{ texture } {}

}   // namespace core::image::ktx2
