#pragma once

namespace demo {

struct VirtualTextureInfo {
    glm::uvec2 baseExtent;
    glm::uvec2 granularity;
    uint32_t   block_count;
};

}   // namespace demo
