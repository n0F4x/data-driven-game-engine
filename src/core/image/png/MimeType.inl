#pragma once

constexpr auto core::image::png::MimeType::magic() noexcept
    -> std::span<const std::byte, 8>
{
    constexpr static std::array s_magic{ std::byte{ 0x89 }, std::byte{ 0x50 },
                                         std::byte{ 0x4E }, std::byte{ 0x47 },
                                         std::byte{ 0x0D }, std::byte{ 0x0A },
                                         std::byte{ 0x1A }, std::byte{ 0x0A } };

    return s_magic;
}