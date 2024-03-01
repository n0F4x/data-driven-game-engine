#include "SceneManager.hpp"

#include <ktxvulkan.h>

namespace engine::renderer {

// auto SceneManager::load_texture(std::filesystem::path t_filepath) -> Texture&
//{
//     if (const auto iter{ m_textures.find(t_filepath) }; iter != m_textures.end()) {
//         return iter->second;
//     }
//
//     ktxTexture*    texture;
//     KTX_error_code result;
//     ktx_size_t     offset;
//     ktx_uint8_t*   image;
//     ktx_uint32_t   level, layer, faceSlice;
//
//     result = ktxTexture_CreateFromNamedFile(
//         "mytex3d.ktx", KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture
//     );
// }

}   // namespace engine::renderer
