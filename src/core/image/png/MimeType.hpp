#pragma once

#include <span>

namespace core::image::png {

struct MimeType {
    [[nodiscard]]
    constexpr static auto magic() noexcept -> std::span<const std::byte, 8>;
};

}

#include "MimeType.inl"
