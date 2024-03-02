#include "ModelLoader.hpp"

namespace engine::renderer {

auto ModelLoader::load(const fastgltf::Asset& t_asset) -> void
{
//    for (auto& image : t_asset.images) {
//        loadImage(&viewer, image);
//    }

    const auto [node_indices, _]{ t_asset.scenes[t_asset.defaultScene.value_or(0)] };
    nodes.resize(node_indices.size());
    for (const auto node_index : node_indices) {
        load_node(nullptr, t_asset, t_asset.nodes[node_index]);
    }
}

auto ModelLoader::load_image(const fastgltf::Image&) -> void {}

auto ModelLoader::load_node(
    StagingModel::Node*    t_parent,
    const fastgltf::Asset& t_asset,
    const fastgltf::Node&  t_node
) noexcept -> void
{
    StagingModel::Node& node{ [&, t_parent]() -> StagingModel::Node& {
        if (t_parent == nullptr) {
            return nodes.emplace_back();
        }
        return t_parent->children.emplace_back();
    }() };

    node.matrix = std::visit(
        fastgltf::visitor{
            [](const fastgltf::Node::TransformMatrix& matrix) {
                return glm::make_mat4x4(matrix.data());
            },
            [](const fastgltf::TRS& transform) {
                return glm::translate(
                           glm::mat4(1.f), glm::make_vec3(transform.translation.data())
                       )
                     * glm::toMat4(glm::make_quat(transform.rotation.data()))
                     * glm::scale(glm::mat4(1.0f), glm::make_vec3(transform.scale.data()));
            } },
        t_node.transform
    );

    if (t_node.meshIndex) {
        node.mesh = load_mesh(t_asset, t_asset.meshes[*t_node.meshIndex]);
    }

    node.children.resize(t_node.children.size());
    for (const auto child_index : t_node.children) {
        load_node(&node, t_asset, t_asset.nodes[child_index]);
    }
}

auto ModelLoader::load_mesh(
    const fastgltf::Asset& t_asset,
    const fastgltf::Mesh&  t_mesh
) noexcept -> StagingModel::Mesh
{
    StagingModel::Mesh mesh;
    mesh.primitives.resize(t_mesh.primitives.size());

    for (const auto& primitive : t_mesh.primitives) {
        load_primitive(mesh, t_asset, primitive);
    }

    return mesh;
}

auto ModelLoader::load_primitive(
    StagingModel::Mesh&        t_mesh,
    const fastgltf::Asset&     t_asset,
    const fastgltf::Primitive& t_primitive
) noexcept -> void
{
    auto&      primitive{ t_mesh.primitives.emplace_back() };
    const auto first_vertex_index{ vertices.size() };

    load_vertices(primitive, t_asset, t_primitive.attributes);

    if (auto indices_accessor_index{ t_primitive.indicesAccessor }) {
        load_indices(
            primitive,
            static_cast<uint32_t>(first_vertex_index),
            t_asset,
            t_asset.accessors[*indices_accessor_index]
        );
    }
}

auto ModelLoader::load_vertices(
    StagingModel::Primitive&                                                  t_primitive,
    const fastgltf::Asset&                                                    t_asset,
    const fastgltf::pmr::SmallVector<fastgltf::Primitive::attribute_type, 4>& t_attributes
) noexcept -> void
{
    size_t first_vertex_index{ vertices.size() };

    // load vertex positions
    {
        const auto& position_accessor{
            t_asset.accessors[std::ranges::find(
                                  t_attributes,
                                  "POSITION",
                                  &fastgltf::Primitive::attribute_type::first
            )
                                  ->second]
        };
        t_primitive.vertex_count = static_cast<uint32_t>(position_accessor.count);

        vertices.resize(position_accessor.count + vertices.size());

        fastgltf::iterateAccessorWithIndex<glm::vec3>(
            t_asset,
            position_accessor,
            [&, first_vertex_index](const glm::vec3& position, const size_t index) {
                vertices[first_vertex_index + index].position = position;
            }
        );
    }

    // load vertex normals
    {
        if (const auto normal_accessor{ std::ranges::find(
                t_attributes, "NORMAL", &fastgltf::Primitive::attribute_type::first
            ) };
            normal_accessor != t_attributes.end())
        {
            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                t_asset,
                t_asset.accessors[normal_accessor->second],
                [&, first_vertex_index](const glm::vec3& normal, const size_t index) {
                    vertices[first_vertex_index + index].normal = normal;
                }
            );
        }
    }

    // load UVs
    {
        if (const auto uv_accessor{ std::ranges::find(
                t_attributes, "TEXCOORD_0", &fastgltf::Primitive::attribute_type::first
            ) };
            uv_accessor != t_attributes.end())
        {
            fastgltf::iterateAccessorWithIndex<glm::vec2>(
                t_asset,
                t_asset.accessors[uv_accessor->second],
                [&, first_vertex_index](const glm::vec2& uv, const size_t index) {
                    vertices[first_vertex_index + index].uv = uv;
                }
            );
        }
    }

    // load vertex colors
    {
        if (const auto color_accessor{ std::ranges::find(
                t_attributes, "COLOR_0", &fastgltf::Primitive::attribute_type::first
            ) };
            color_accessor != t_attributes.end())
        {
            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                t_asset,
                t_asset.accessors[color_accessor->second],
                [&, first_vertex_index](const glm::vec3& color, const size_t index) {
                    vertices[first_vertex_index + index].color = glm::vec4{ color, 1 };
                }
            );
            fastgltf::iterateAccessorWithIndex<glm::vec4>(
                t_asset,
                t_asset.accessors[color_accessor->second],
                [&, first_vertex_index](const glm::vec4& color, const size_t index) {
                    vertices[first_vertex_index + index].color = color;
                }
            );
        }
    }
}

auto ModelLoader::load_indices(
    StagingModel::Primitive&  t_primitive,
    const uint32_t            t_first_vertex_index,
    const fastgltf::Asset&    t_asset,
    const fastgltf::Accessor& t_accessor
) noexcept -> void
{
    t_primitive.first_index_index = static_cast<uint32_t>(indices.size());
    t_primitive.index_count       = static_cast<uint32_t>(t_accessor.count);

    indices.reserve(indices.size() + t_accessor.count);

    fastgltf::iterateAccessor<uint32_t>(t_asset, t_accessor, [&](const uint32_t index) {
        indices.push_back(t_first_vertex_index + index);
    });
}

}
