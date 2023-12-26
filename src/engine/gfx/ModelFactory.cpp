#include "ModelFactory.hpp"

#include <ranges>
#include <tuple>
#include <vector>

#include <spdlog/spdlog.h>

#include <glm/gtc/type_ptr.hpp>

namespace engine::gfx {

namespace {

struct BufferData {
    const float* buffer{};
    size_t       vertex_count{};
    int          byte_stride{};
};

[[nodiscard]] auto get_buffer_data(
    const tinygltf::Model&     t_model,
    const tinygltf::Primitive& t_primitive,
    const char*                t_attribute,
    uint32_t                   t_type
) noexcept -> BufferData
{
    BufferData result{};

    if (const auto iter{ t_primitive.attributes.find(t_attribute) };
        iter != t_primitive.attributes.end())
    {
        const auto& accessor    = t_model.accessors[iter->second];
        const auto& buffer_view = t_model.bufferViews[accessor.bufferView];
        result.buffer           = reinterpret_cast<const float*>(
            &t_model.buffers[buffer_view.buffer]
                 .data[accessor.byteOffset + buffer_view.byteOffset]
        );
        result.vertex_count = t_model.accessors[iter->second].count;
        result.byte_stride =
            accessor.ByteStride(buffer_view)
                ? static_cast<int>(
                    (accessor.ByteStride(buffer_view) / sizeof(float))
                )
                : tinygltf::GetNumComponentsInType(t_type);
    }

    return result;
}

[[nodiscard]] auto get_vertices(
    const tinygltf::Model&     t_model,
    const tinygltf::Primitive& t_primitive
) noexcept -> std::vector<Model::Vertex>
{
    const auto position_data{
        get_buffer_data(t_model, t_primitive, "POSITION", TINYGLTF_TYPE_VEC3)
    };
    const auto color_data{
        get_buffer_data(t_model, t_primitive, "COLOR_0", TINYGLTF_TYPE_VEC3)
    };
    const auto normal_data{
        get_buffer_data(t_model, t_primitive, "NORMAL", TINYGLTF_TYPE_VEC3)
    };

    return std::ranges::iota_view{ size_t{ 0 }, position_data.vertex_count }
         | std::views::transform([&](auto index) {
               return Model::Vertex{
                   .position = glm::vec4(
                       position_data.buffer ? glm::make_vec3(
                           &position_data
                                .buffer[index * position_data.byte_stride]
                       )
                                            : glm::vec3(0.f),
                       1.0f
                   ),
                   .color = glm::vec4(
                       color_data.buffer ? glm::make_vec3(
                           &color_data.buffer[index * color_data.byte_stride]
                       )
                                         : glm::vec3(1.f),
                       1.0f
                   ),
                   .normal = glm::normalize(glm::vec3(
                       normal_data.buffer ? glm::make_vec3(
                           &normal_data.buffer[index * normal_data.byte_stride]
                       )
                                          : glm::vec3(0.f)
                   )),
               };
           })
         | std::ranges::to<std::vector>();
}

template <typename IndexType>
[[nodiscard]] auto get_indices(
    const tinygltf::Model&    t_model,
    const tinygltf::Accessor& t_accessor,
    uint32_t                  t_first_vertex_index
) noexcept -> std::vector<uint32_t>
{
    const auto& buffer_view = t_model.bufferViews[t_accessor.bufferView];
    const auto& buffer      = t_model.buffers[buffer_view.buffer];
    const auto* buf         = reinterpret_cast<const IndexType*>(
        &buffer.data[t_accessor.byteOffset + buffer_view.byteOffset]
    );

    return std::ranges::iota_view{ size_t{ 0 }, t_accessor.count }
         | std::views::transform([buf, t_first_vertex_index](auto index) {
               return buf[index] + t_first_vertex_index;
           })
         | std::ranges::to<std::vector>();
}

[[nodiscard]] auto get_indices_opt(
    const tinygltf::Model&     t_model,
    const tinygltf::Primitive& t_primitive,
    uint32_t                   t_first_vertex_index
) noexcept -> tl::optional<std::vector<uint32_t>>
{
    const auto& accessor = t_model.accessors[t_primitive.indices];
    switch (accessor.componentType) {
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
            return get_indices<uint32_t>(
                t_model, accessor, t_first_vertex_index
            );
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
            return get_indices<uint16_t>(
                t_model, accessor, t_first_vertex_index
            );
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
            return get_indices<uint8_t>(
                t_model, accessor, t_first_vertex_index
            );
        }
        default:
            SPDLOG_WARN(
                "GLTF index component type {} is not supported!",
                accessor.componentType
            );
            return tl::nullopt;
    }
}

struct MeshInfo {
    std::vector<Model::Vertex>    vertices;
    std::vector<uint32_t>         indices;
    std::vector<Model::Primitive> primitives;
};

[[nodiscard]] auto load_mesh(
    const tinygltf::Node&  t_node,
    const tinygltf::Model& t_model,
    uint32_t               t_first_vertex_index,
    uint32_t               t_first_index_index
) noexcept -> tl::optional<MeshInfo>
{
    std::vector<Model::Vertex>    vertices;
    std::vector<uint32_t>         indices;
    std::vector<Model::Primitive> primitives;

    for (const auto& primitive : t_model.meshes[t_node.mesh].primitives) {
        uint32_t first_vertex_index{ static_cast<uint32_t>(vertices.size())
                                     + t_first_vertex_index };
        uint32_t first_index_index{ static_cast<uint32_t>(indices.size())
                                    + t_first_index_index };

        auto indices_opt{
            get_indices_opt(t_model, primitive, first_vertex_index)
        };
        if (!indices_opt) {
            return tl::nullopt;
        }
        indices.append_range(*indices_opt);

        auto     temp_vertices{ get_vertices(t_model, primitive) };
        uint32_t vertex_count = static_cast<uint32_t>(temp_vertices.size());
        vertices.append_range(std::move(temp_vertices));

        primitives.emplace_back(
            first_index_index,
            static_cast<uint32_t>(indices_opt->size()),
            vertex_count
        );
    }

    return MeshInfo{ std::move(vertices),
                     std::move(indices),
                     std::move(primitives) };
}

struct NodeInfo {
    std::vector<Model::Vertex> vertices;
    std::vector<uint32_t>      indices;
    Model::Node                node;
};

auto load_node(
    Model::Node*           t_parent,
    const tinygltf::Node&  t_node,
    const tinygltf::Model& t_model,
    uint32_t               t_first_vertex_index,
    uint32_t               t_first_index_index
) noexcept -> NodeInfo
{
    NodeInfo result;
    auto&    node{ result.node };
    auto&    vertices{ result.vertices };
    auto&    indices{ result.indices };

    node.parent = t_parent;

    if (t_node.matrix.size() == 16) {
        node.matrix = glm::make_mat4x4(t_node.matrix.data());
    }

    if (t_node.mesh > -1) {
        load_mesh(
            t_node,
            t_model,
            t_first_vertex_index + static_cast<uint32_t>(vertices.size()),
            t_first_index_index + static_cast<uint32_t>(indices.size())
        )
            .transform([&](MeshInfo&& t_mesh_info) {
                vertices.append_range(std::move(t_mesh_info.vertices));
                indices.append_range(std::move(t_mesh_info.indices));
                node.mesh =
                    Model::Mesh{ .primitives    = t_mesh_info.primitives,
                                 .uniform_block = { .matrix = node.matrix } };
            });
    }

    for (auto index : t_node.children) {
        auto child{ load_node(
            &node,
            t_model.nodes[index],
            t_model,
            t_first_vertex_index + static_cast<uint32_t>(vertices.size()),
            t_first_index_index + static_cast<uint32_t>(indices.size())
        ) };

        node.children.push_back(std::move(child.node));
        vertices.append_range(std::move(child.vertices));
        indices.append_range(std::move(child.indices));
    }

    return result;
}

}   // namespace

auto ModelFactory::create_model(const tinygltf::Model& t_source) noexcept
    -> Model
{
    std::vector<Model::Vertex> vertices;
    std::vector<uint32_t>      indices;
    std::vector<Model::Node>   nodes;

    const tinygltf::Scene& scene =
        t_source.scenes[t_source.defaultScene > -1 ? t_source.defaultScene : 0];
    for (int i : scene.nodes) {
        auto node{ load_node(
            nullptr,
            t_source.nodes[i],
            t_source,
            static_cast<uint32_t>(vertices.size()),
            static_cast<uint32_t>(indices.size())
        ) };
        vertices.append_range(std::move(node.vertices));
        indices.append_range(std::move(node.indices));
        nodes.push_back(std::move(node.node));
    }

    return Model{ std::move(vertices), std::move(indices), std::move(nodes) };
}

}   // namespace engine::gfx
