#pragma once

#include <ranges>
#include <vector>

#include <spdlog/spdlog.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/types.hpp>

#include "engine/renderer/model/Mesh.hpp"
#include "engine/renderer/model/Vertex.hpp"

#include "StagingModel.hpp"

namespace engine::renderer {

struct GltfLoader {
    std::vector<Vertex>             vertices;
    std::vector<uint32_t>           indices;
    std::vector<StagingModel::Node> nodes;
    std::vector<StagingModel>       models;

    auto load(const fastgltf::Asset& t_asset) -> void;

private:
    auto load_image(const fastgltf::Image&) -> void;

    auto load_node(
        StagingModel::Node*    t_parent,
        const fastgltf::Asset& t_asset,
        const fastgltf::Node&  t_node
    ) noexcept -> void;

    [[nodiscard]] auto
        load_mesh(const fastgltf::Asset& t_asset, const fastgltf::Mesh& t_mesh) noexcept
        -> StagingModel::Mesh;

    auto load_primitive(
        StagingModel::Mesh&        t_mesh,
        const fastgltf::Asset&     t_asset,
        const fastgltf::Primitive& t_primitive
    ) noexcept -> void;

    [[nodiscard]] auto load_vertices(
        Primitive&             t_primitive,
        const fastgltf::Asset& t_asset,
        const fastgltf::pmr::SmallVector<fastgltf::Primitive::attribute_type, 4>& t_attributes
    ) noexcept -> bool;

    auto load_indices(
        Primitive&                t_primitive,
        uint32_t                  t_first_vertex_index,
        const fastgltf::Asset&    t_asset,
        const fastgltf::Accessor& t_accessor
    ) noexcept -> void;
};

}   // namespace engine::renderer
