#include "Texture.hpp"

namespace core::gltf {

auto TextureInfo::create(const std::optional<fastgltf::TextureInfo>& t_optional
) -> std::optional<TextureInfo>
{
    if (!t_optional.has_value()) {
        return std::nullopt;
    }
    return core::gltf::TextureInfo{
        .texture_index   = static_cast<uint32_t>(t_optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(t_optional->texCoordIndex),
    };
}

}   // namespace core::gltf
