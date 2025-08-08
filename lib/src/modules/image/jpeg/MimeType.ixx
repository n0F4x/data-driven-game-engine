module;

#include <array>
#include <span>

export module modules.image.jpeg.MimeType;

namespace modules::image::jpeg {

export struct MimeType {
    [[nodiscard]]
    constexpr static auto magic() noexcept -> std::span<const std::byte, 3>;
};

}   // namespace modules::image::jpeg

constexpr auto modules::image::jpeg::MimeType::magic() noexcept
    -> std::span<const std::byte, 3>
{
    constexpr static std::array s_magic{ std::byte{ 0xFF },
                                         std::byte{ 0xD8 },
                                         std::byte{ 0xFF } };

    return s_magic;
}
