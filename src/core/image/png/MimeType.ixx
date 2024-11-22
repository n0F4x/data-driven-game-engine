module;

#include <span>
#include <array>

export module core.image.png.MimeType;

namespace core::image::png {

export struct MimeType {
    [[nodiscard]]
    constexpr static auto magic() noexcept -> std::span<const std::byte, 8>;
};

}

#include "MimeType.inl"
