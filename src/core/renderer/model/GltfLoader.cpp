#include "GltfLoader.hpp"

#include <functional>
#include <type_traits>

namespace core::renderer {

auto GltfLoader::load(const fastgltf::Asset& t_asset) -> void
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

// auto GltfLoader::load_image(const fastgltf::Image& t_image) -> void
//{
//     auto getLevelCount = [](int width, int height) -> GLsizei {
//         return static_cast<GLsizei>(1 + floor(log2(width > height ? width : height)));
//     };
//
//     std::visit(
//         fastgltf::visitor{
//             [](auto&) {},
//             [&](fastgltf::sources::URI& filePath) {
//                 assert(filePath.fileByteOffset == 0);   // We don't support offsets
//                 with
//                     // stbi.
//                     assert(filePath.uri.isLocalPath());   // We're only capable of
//                 loading
//                     // local files.
//                     int width,
//                     height, nrChannels;
//
//                 const path{ filePath.uri.fspath() };   // Thanks C++.
//                 unsigned char* data = stbi_load(
//                     path.generic_string().c_str(), &width, &height, &nrChannels, 4
//                 );
//                 glTextureStorage2D(
//                     texture, getLevelCount(width, height), GL_RGBA8, width, height
//                 );
//                 glTextureSubImage2D(
//                     texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data
//                 );
//                 stbi_image_free(data);
//             },
//             [&](fastgltf::sources::Array& vector) {
//                 int            width, height, nrChannels;
//                 unsigned char* data = stbi_load_from_memory(
//                     vector.bytes.data(),
//                     static_cast<int>(vector.bytes.size()),
//                     &width,
//                     &height,
//                     &nrChannels,
//                     4
//                 );
//                 glTextureStorage2D(
//                     texture, getLevelCount(width, height), GL_RGBA8, width, height
//                 );
//                 glTextureSubImage2D(
//                     texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data
//                 );
//                 stbi_image_free(data);
//             },
//             [&](fastgltf::sources::BufferView& view) {
//                 auto& bufferView = viewer->asset.bufferViews[view.bufferViewIndex];
//                 auto& buffer     = viewer->asset.buffers[bufferView.bufferIndex];
//                 // Yes, we've already loaded every buffer into some GL buffer. However,
//                 // with GL it's simpler to just copy the buffer data again for the
//                 // texture. Besides, this is just an example.
//                 std::visit(
//                     fastgltf::visitor{ // We only care about VectorWithMime here,
//                                        because
//                                            // we specify LoadExternalBuffers, meaning
//                                            all
//                                            // buffers are already loaded into a vector.
//                                            [](auto& arg){},
//                                        [&](fastgltf::sources::Array& vector) {
//                                            int            width, height, nrChannels;
//                                            unsigned char* data = stbi_load_from_memory(
//                                                vector.bytes.data() +
//                                                bufferView.byteOffset,
//                                                static_cast<int>(bufferView.byteLength),
//                                                &width,
//                                                &height,
//                                                &nrChannels,
//                                                4
//                                            );
//                                            glTextureStorage2D(
//                                                texture,
//                                                getLevelCount(width, height),
//                                                GL_RGBA8,
//                                                width,
//                                                height
//                                            );
//                                            glTextureSubImage2D(
//                                                texture,
//                                                0,
//                                                0,
//                                                0,
//                                                width,
//                                                height,
//                                                GL_RGBA,
//                                                GL_UNSIGNED_BYTE,
//                                                data
//                                            );
//                                            stbi_image_free(data);
//                                        } },
//                     buffer.data
//                 );
//             },
//         },
//         t_image.data
//     );
// }

auto GltfLoader::load_node(
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

auto GltfLoader::load_mesh(
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

auto GltfLoader::load_primitive(
    StagingModel::Mesh&        t_mesh,
    const fastgltf::Asset&     t_asset,
    const fastgltf::Primitive& t_primitive
) noexcept -> void
{
    auto&      primitive{ t_mesh.primitives.emplace_back() };
    const auto first_vertex_index{ vertices.size() };

    if (!load_vertices(primitive, t_asset, t_primitive.attributes)) {
        return;
    }

    if (auto indices_accessor_index{ t_primitive.indicesAccessor }) {
        load_indices(
            primitive,
            static_cast<uint32_t>(first_vertex_index),
            t_asset,
            t_asset.accessors[*indices_accessor_index]
        );
    }
}

static auto make_accessor_loader(
    const fastgltf::Asset& t_asset,
    std::vector<Vertex>&   t_vertices,
    size_t                 t_first_vertex_index
)
{
    return
        [&,
         t_first_vertex_index]<typename Projection, typename Transformation = std::identity>(
            const fastgltf::Accessor& t_accessor,
            Projection                project,
            Transformation            transform = {}
        )
            ->void
    {
        using AttributeType =
            std::remove_cvref_t<std::invoke_result_t<Projection, const Vertex&>>;

        fastgltf::iterateAccessorWithIndex<AttributeType>(
            t_asset,
            t_accessor,
            [&, t_first_vertex_index](const AttributeType& attribute, const size_t index) {
                std::invoke(project, t_vertices[t_first_vertex_index + index]) =
                    std::invoke(transform, attribute);
            }
        );
    };
}

auto GltfLoader::load_vertices(
    Primitive&                                                                t_primitive,
    const fastgltf::Asset&                                                    t_asset,
    const fastgltf::pmr::SmallVector<fastgltf::Primitive::attribute_type, 4>& t_attributes
) noexcept -> bool
{
    const auto position_iter{ std::ranges::find(
        t_attributes, "POSITION", &fastgltf::Primitive::attribute_type::first
    ) };
    if (position_iter == t_attributes.end()) {
        return false;
    }

    auto load_accessor{ make_accessor_loader(t_asset, vertices, vertices.size()) };

    const auto& position_accessor{ t_asset.accessors[position_iter->second] };

    t_primitive.vertex_count = static_cast<uint32_t>(position_accessor.count);
    vertices.resize(position_accessor.count + vertices.size());

    load_accessor(position_accessor, &Vertex::position);

    for (const auto& [name, accessor_index] : t_attributes) {
        if (name == "NORMAL") {
            load_accessor(t_asset.accessors[accessor_index], &Vertex::normal);
        }
        else if (name == "TANGENT") {
            load_accessor(t_asset.accessors[accessor_index], &Vertex::tangent);
        }
        else if (name == "TEXCOORD_0") {
            load_accessor(t_asset.accessors[accessor_index], &Vertex::uv_0);
        }
        else if (name == "TEXCOORD_1") {
            load_accessor(t_asset.accessors[accessor_index], &Vertex::uv_1);
        }
        else if (name == "COLOR_0") {
            load_accessor(t_asset.accessors[accessor_index], &Vertex::color);
            load_accessor(
                t_asset.accessors[accessor_index],
                &Vertex::color,
                [](glm::vec3 vec3) { return glm::make_vec4(vec3); }
            );
        }
    }

    return true;
}

auto GltfLoader::load_indices(
    Primitive&                t_primitive,
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

}   // namespace core::renderer
