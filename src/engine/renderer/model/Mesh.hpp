#pragma once

#include <vector>

#include "engine/renderer/material_system/Material.hpp"

namespace engine::renderer {

struct Primitive {
    uint32_t first_index_index;
    uint32_t index_count;
    uint32_t vertex_count;
};

//struct Mesh {
//    std::vector<Primitive> primitives;
//    Material*              material;
//};

}   // namespace engine::renderer
