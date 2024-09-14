#pragma once

constexpr auto core::image::jpeg::MimeType::magic() noexcept
    -> std::span<const std::byte, 3>
{
    constexpr static std::array s_magic{
        std::byte{ 0xFF }, std::byte{ 0xD8  }, std::byte{ 0xFF }
    };

    return s_magic;
}
