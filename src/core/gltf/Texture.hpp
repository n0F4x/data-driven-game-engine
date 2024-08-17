#pragma once

#include <optional>

#include <fastgltf/types.hpp>

namespace core::gltf {

struct Sampler {
    enum class MagFilter {
        eNearest,
        eLinear
    };
    enum class MinFilter {
        eNearest,
        eLinear,
        eNearestMipmapNearest,
        eLinearMipmapNearest,
        eNearestMipmapLinear,
        eLinearMipmapLinear,
    };

    enum class WrapMode {
        eClampToEdge,
        eMirroredRepeat,
        eRepeat
    };

    std::optional<MagFilter> mag_filter;
    std::optional<MinFilter> min_filter;
    WrapMode                 wrap_s{ WrapMode::eRepeat };
    WrapMode                 wrap_t{ WrapMode::eRepeat };
};

struct Texture {
    std::optional<uint32_t> sampler_index;
    uint32_t                image_index;
};

struct TextureInfo {
    uint32_t texture_index;
    uint32_t tex_coord_index{};

    [[nodiscard]]
    static auto create(const fastgltf::TextureInfo& source) -> TextureInfo;
};

}   // namespace core::gltf
