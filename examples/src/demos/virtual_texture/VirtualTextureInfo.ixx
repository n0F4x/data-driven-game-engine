module;

#include <glm/vec2.hpp>

export module demos.virtual_texture.VirtualTextureInfo;

namespace demo {

export struct VirtualTextureInfo {
    glm::uvec2 baseExtent;
    glm::uvec2 granularity;
    uint32_t   block_count;
    uint32_t   mip_tail_first_lod;
};

}   // namespace demo
