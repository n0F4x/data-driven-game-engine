#pragma once

#include <span>

namespace core::image::ktx2 {

struct MimeType {
    [[nodiscard]]
    constexpr static auto magic() noexcept -> std::span<const std::byte, 12>;
};

}

#include "MimeType.inl"
