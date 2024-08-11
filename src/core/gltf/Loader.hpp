#pragma once

#include <filesystem>
#include <unordered_map>

#include "Model.hpp"

namespace core::gltf {

class Loader {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& filepath
    ) -> std::optional<Model>;

    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& filepath, size_t scene_id)
        -> std::optional<Model>;

private:
    [[nodiscard]]
    static auto load_model(
        const std::filesystem::path& filepath,
        const fastgltf::Asset&       asset,
        size_t                       scene_id
    ) -> Model;

    static auto load_nodes(
        Model&                                              model,
        const fastgltf::Asset&                              asset,
        const fastgltf::pmr::MaybeSmallVector<std::size_t>& node_indices
    ) -> void;

    static auto load_images(
        Model&                       model,
        const fastgltf::Asset&       asset,
        const std::filesystem::path& filepath
    ) -> void;

    static auto load_materials(Model& model, const fastgltf::Asset& asset) -> void;

    static auto load_node(
        Model&                              model,
        std::vector<Node>&&                 nodes,
        const fastgltf::Asset&              asset,
        const fastgltf::Node&               source_node,
        std::optional<size_t>               parent_index,
        std::unordered_map<size_t, size_t>& node_index_map
    ) -> std::vector<Node>;

    [[nodiscard]]
    static auto load_mesh(
        Model&                 model,
        const fastgltf::Asset& asset,
        const fastgltf::Mesh&  source_mesh
    ) -> size_t;

    [[nodiscard]]
    static auto load_primitive(
        Model&                     model,
        const fastgltf::Asset&     asset,
        const fastgltf::Primitive& source_primitive
    ) -> std::optional<Mesh::Primitive>;

    [[nodiscard]]
    static auto load_vertices(
        Model&                 model,
        Mesh::Primitive&       primitive,
        const fastgltf::Asset& asset,
        const fastgltf::pmr::SmallVector<fastgltf::Primitive::attribute_type, 4>& attributes
    ) -> bool;

    static auto load_indices(
        Model&                    model,
        Mesh::Primitive&          primitive,
        const uint32_t            first_vertex_index,
        const fastgltf::Asset&    asset,
        const fastgltf::Accessor& accessor
    ) -> void;

    static auto adjust_node_indices(
        Model&                                    model,
        const std::unordered_map<size_t, size_t>& node_index_map
    ) -> void;
};

}   // namespace core::gltf
