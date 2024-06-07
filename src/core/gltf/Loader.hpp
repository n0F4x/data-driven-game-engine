#pragma once

#include <filesystem>

#include "Model.hpp"

namespace core::gltf {

class Loader {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& t_filepath
    ) -> std::optional<Model>;

    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& t_filepath, size_t t_scene_id)
        -> std::optional<Model>;

private:
    [[nodiscard]]
    static auto load_model(
        const std::filesystem::path& t_filepath,
        const fastgltf::Asset&       t_asset,
        size_t                       t_scene_id
    ) -> Model;

    static auto load_node(
        Model&                              t_model,
        Node&                               t_node,
        const fastgltf::Asset&              t_asset,
        const fastgltf::Node&               t_source_node,
        Node*                               t_parent,
        std::unordered_map<size_t, size_t>& t_node_index_map
    ) -> void;

    [[nodiscard]]
    static auto load_mesh(
        Model&                 t_model,
        const fastgltf::Asset& t_asset,
        const fastgltf::Mesh&  t_source_mesh
    ) -> size_t;

    [[nodiscard]]
    static auto load_primitive(
        Model&                     t_model,
        const fastgltf::Asset&     t_asset,
        const fastgltf::Primitive& t_source_primitive
    ) -> std::optional<Mesh::Primitive>;

    [[nodiscard]]
    static auto load_vertices(
        Model&                 t_model,
        Mesh::Primitive&       t_primitive,
        const fastgltf::Asset& t_asset,
        const fastgltf::pmr::SmallVector<fastgltf::Primitive::attribute_type, 4>& t_attributes
    ) -> bool;

    static auto load_indices(
        Model&                    t_model,
        Mesh::Primitive&          t_primitive,
        const uint32_t            t_first_vertex_index,
        const fastgltf::Asset&    t_asset,
        const fastgltf::Accessor& t_accessor
    ) -> void;

    static auto adjust_node_indices(
        Model&                                    t_model,
        const std::unordered_map<size_t, size_t>& t_node_index_map
    ) -> void;
};

}   // namespace core::gltf
