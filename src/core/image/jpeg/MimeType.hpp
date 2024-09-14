#pragma once

#include <span>

namespace core::image::jpeg {

struct MimeType {
    [[nodiscard]]
    constexpr static auto magic() noexcept -> std::span<const std::byte, 3>;
};

}

#include "MimeType.inl"
