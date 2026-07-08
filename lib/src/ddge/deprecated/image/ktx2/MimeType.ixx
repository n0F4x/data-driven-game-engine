module;

#include <array>
#include <span>

export module ddge.deprecated.image.ktx2.MimeType;

namespace ddge::image::ktx2 {

export struct MimeType {
    [[nodiscard]]
    constexpr static auto magic() noexcept -> std::span<const std::byte, 12>;
};

}   // namespace ddge::image::ktx2

constexpr auto ddge::image::ktx2::MimeType::magic() noexcept
    -> std::span<const std::byte, 12>
{
    constexpr static std::array s_magic{
        std::byte{ 0xAB }, std::byte{ 0x4B }, std::byte{ 0x54 }, std::byte{ 0x58 },
        std::byte{ 0x20 }, std::byte{ 0x32 }, std::byte{ 0x30 }, std::byte{ 0xBB },
        std::byte{ 0x0D }, std::byte{ 0x0A }, std::byte{ 0x1A }, std::byte{ 0x0A }
    };

    return s_magic;
}
