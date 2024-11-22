module;

#include <span>
#include <array>

export module core.image.jpeg.MimeType;

namespace core::image::jpeg {

export struct MimeType {
    [[nodiscard]]
    constexpr static auto magic() noexcept -> std::span<const std::byte, 3>;
};

}

#include "MimeType.inl"
