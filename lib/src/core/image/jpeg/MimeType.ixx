module;

#include <array>
#include <span>

export module core.image.jpeg.MimeType;

namespace core::image::jpeg {

export struct MimeType {
    [[nodiscard]]
    constexpr static auto magic() noexcept -> std::span<const std::byte, 3>;
};

}   // namespace core::image::jpeg

constexpr auto core::image::jpeg::MimeType::magic() noexcept
    -> std::span<const std::byte, 3>
{
    constexpr static std::array s_magic{ std::byte{ 0xFF },
                                         std::byte{ 0xD8 },
                                         std::byte{ 0xFF } };

    return s_magic;
}
