#pragma once

#include <optional>

namespace core::gltf {

struct Mesh {
    struct Primitive {
        enum class Topology {
            ePoints,
            eLineStrips,
            eLineLoops,
            eLines,
            eTriangles,
            eTriangleStrips,
            eTriangleFans
        };

        Topology                mode;
        std::optional<uint32_t> material_index;
        uint32_t                first_index_index;
        uint32_t                index_count;
        uint32_t                vertex_count;
    };

    std::vector<Primitive> primitives;
};

}   // namespace core::gltf
