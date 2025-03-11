module;

#include <span>
#include <array>

export module core.image.ktx2.MimeType;

namespace core::image::ktx2 {

export struct MimeType {
    [[nodiscard]]
    constexpr static auto magic() noexcept -> std::span<const std::byte, 12>;
};

}

#include "MimeType.inl"
