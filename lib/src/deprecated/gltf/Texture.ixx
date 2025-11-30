module;

#include <cstdint>
#include <optional>

#include <fastgltf/types.hpp>

export module ddge.deprecated.gltf.Texture;

namespace ddge::gltf {

export struct Sampler {
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

    MagFilter mag_filter{ MagFilter::eLinear };
    MinFilter min_filter{ MinFilter::eLinear };
    WrapMode  wrap_s{ WrapMode::eRepeat };
    WrapMode  wrap_t{ WrapMode::eRepeat };
};

// NOLINTNEXTLINE(*-member-init)
export struct Texture {
    std::optional<uint32_t> sampler_index;
    uint32_t                image_index;
};

// NOLINTNEXTLINE(*-member-init)
export struct TextureInfo {
    uint32_t texture_index;
    uint32_t tex_coord_index{};

    [[nodiscard]]
    static auto create(const fastgltf::TextureInfo& source) -> TextureInfo;
};

}   // namespace ddge::gltf
