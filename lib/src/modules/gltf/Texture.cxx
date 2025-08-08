module;

#include <cstdint>

#include <fastgltf/types.hpp>

module modules.gltf.Texture;

namespace modules::gltf {

auto TextureInfo::create(const fastgltf::TextureInfo& source) -> TextureInfo
{
    return TextureInfo{
        .texture_index   = static_cast<uint32_t>(source.textureIndex),
        .tex_coord_index = static_cast<uint32_t>(source.texCoordIndex),
    };
}

}   // namespace modules::gltf
