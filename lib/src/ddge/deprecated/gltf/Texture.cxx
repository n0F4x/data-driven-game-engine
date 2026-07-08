module;

#include <cstdint>

#include <fastgltf/types.hpp>

module ddge.deprecated.gltf.Texture;

namespace ddge::gltf {

auto TextureInfo::create(const fastgltf::TextureInfo& source) -> TextureInfo
{
    return TextureInfo{
        .texture_index   = static_cast<uint32_t>(source.textureIndex),
        .tex_coord_index = static_cast<uint32_t>(source.texCoordIndex),
    };
}

}   // namespace ddge::gltf
