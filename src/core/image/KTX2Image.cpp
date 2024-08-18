#include "KTX2Image.hpp"

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

namespace core::image {

auto KTX2Image::load_from_file(const std::filesystem::path& filepath)
    -> std::optional<KTX2Image>
{
    ktxTexture2* texture{};

    if (const ktxResult result{ ktxTexture2_CreateFromNamedFile(
            filepath.generic_string().c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &texture
        ) };
        result != KTX_SUCCESS && result != KTX_UNKNOWN_FILE_FORMAT)
    {
        SPDLOG_ERROR(
            "ktxTexture2_CreateFromNamedFile failed loading file {} with '{}'",
            filepath.generic_string(),
            ktxErrorString(result)
        );

        return std::nullopt;
    }

    transcode(texture);

    return KTX2Image{ gsl::make_not_null(texture) };
}

auto KTX2Image::load_from_memory(const std::span<const std::byte> data)
    -> std::optional<KTX2Image>
{
    ktxTexture2* texture{};

    if (ktxTexture2_CreateFromMemory(
            reinterpret_cast<const ktx_uint8_t*>(data.data()),
            data.size(),
            KTX_TEXTURE_CREATE_NO_FLAGS,
            nullptr
        )
        != KTX_SUCCESS)
    {
        return std::nullopt;
    }

    if (const ktxResult result{ ktxTexture2_CreateFromMemory(
            reinterpret_cast<const ktx_uint8_t*>(data.data()),
            data.size(),
            KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
            &texture
        ) };
        result != KTX_SUCCESS)
    {
        SPDLOG_ERROR(
            "ktxTexture2_CreateFromMemory failed with '{}'", ktxErrorString(result)
        );

        return std::nullopt;
    }

    transcode(texture);

    return KTX2Image{ gsl::make_not_null(texture) };
}

auto KTX2Image::operator->() const noexcept -> gsl_lite::not_null<ktxTexture2*>
{
    return gsl::make_not_null(m_impl.operator->().operator->());
}

auto KTX2Image::operator*() noexcept -> ktxTexture2&
{
    return m_impl.operator*();
}

auto KTX2Image::operator*() const noexcept -> const ktxTexture2&
{
    return m_impl.operator*();
}

auto KTX2Image::get() const noexcept -> gsl_lite::not_null<ktxTexture2*>
{
    return gsl::make_not_null(m_impl.get());
}

auto KTX2Image::data() const noexcept -> std::span<const std::byte>
{
    return std::as_bytes(std::span{ m_impl->pData, m_impl->dataSize });
}

auto KTX2Image::width() const noexcept -> uint32_t
{
    return m_impl->baseWidth;
}

auto KTX2Image::height() const noexcept -> uint32_t
{
    return m_impl->baseHeight;
}

auto KTX2Image::depth() const noexcept -> uint32_t
{
    return m_impl->baseDepth;
}

auto KTX2Image::mip_levels() const noexcept -> uint32_t
{
    return m_impl->numLevels;
}

auto KTX2Image::format() const noexcept -> vk::Format
{
    return static_cast<vk::Format>(m_impl->vkFormat);
}

auto KTX2Image::offset(
    const uint32_t mip_level,
    const uint32_t layer,
    const uint32_t face_slice
) const noexcept -> uint64_t
{
    assert(mip_level < mip_levels());
    assert(layer < m_impl->numLayers);
    assert(face_slice < m_impl->numFaces);

    uint64_t offset{};
    ktxTexture_GetImageOffset(
        ktxTexture(m_impl.get()), mip_level, layer, face_slice, &offset
    );
    return offset;
}

auto KTX2Image::Deleter::operator()(ktxTexture2* texture) const noexcept -> void
{
    ktxTexture_Destroy(ktxTexture(texture));
}

KTX2Image::KTX2Image(gsl_lite::not_null<ktxTexture2*> texture) noexcept
    : m_impl{ texture }
{}

}   // namespace core::image
