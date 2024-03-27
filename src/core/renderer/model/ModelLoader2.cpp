#include "ModelLoader2.hpp"

#include <array>
#include <ranges>

#include <spdlog/spdlog.h>

#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>

#include "GltfLoader2.hpp"

using namespace core::renderer;
using namespace core::renderer::detail;

[[nodiscard]] static auto load_asset(const std::filesystem::path& t_filepath) noexcept
    -> fastgltf::Expected<fastgltf::Asset>
{
    fastgltf::Parser parser;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(t_filepath);

    return parser.loadGltf(
        &data,
        t_filepath.parent_path(),
        fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers
            | fastgltf::Options::GenerateMeshIndices
            | fastgltf::Options::DecomposeNodeMatrices
    );
}

[[nodiscard]] static auto
    load_model(const fastgltf::Asset& t_asset, const fastgltf::Scene& t_scene) -> Model;

static auto load_node(
    GltfLoader2&           t_loader,
    Model::Node&           t_node,
    const fastgltf::Asset& t_asset,
    const fastgltf::Node&  t_source_node,
    Model::Node*           t_parent
) noexcept -> void;

[[nodiscard]] static auto load_mesh(
    GltfLoader2&           t_loader,
    const fastgltf::Asset& t_asset,
    const fastgltf::Mesh&  t_source_mesh
) noexcept -> Model::Mesh;

[[nodiscard]] static auto load_primitive(
    GltfLoader2&               t_loader,
    const fastgltf::Asset&     t_asset,
    const fastgltf::Primitive& t_source_primitive
) noexcept -> tl::optional<Model::Primitive>;

[[nodiscard]] static auto load_vertices(
    GltfLoader2&                                                              t_loader,
    Model::Primitive&                                                         t_primitive,
    const fastgltf::Asset&                                                    t_asset,
    const fastgltf::pmr::SmallVector<fastgltf::Primitive::attribute_type, 4>& t_attributes
) noexcept -> bool;

static auto load_indices(
    GltfLoader2&              t_loader,
    Model::Primitive&         t_primitive,
    uint32_t                  t_first_vertex_index,
    const fastgltf::Asset&    t_asset,
    const fastgltf::Accessor& t_accessor
) noexcept -> void;

static auto adjust_node_indices(GltfLoader2& t_loader) -> void;

namespace core::renderer {

ModelLoader::ModelLoader(Cache& t_cache) noexcept : m_cache{ t_cache } {}

auto ModelLoader::load_from_file(
    const std::filesystem::path& t_filepath,
    const tl::optional<size_t>   t_scene_id
) noexcept -> tl::optional<Handle<Model>>
{
    if (const auto cached{
            m_cache.and_then([&](const Cache& cache) -> tl::optional<Handle<Model>> {
                return cache.find<Model>(Model::hash(t_filepath, t_scene_id));
            }) };
        cached.has_value())
    {
        return cached.value();
    }

    auto asset{ load_asset(t_filepath) };
    if (asset.error() != fastgltf::Error::None) {
        SPDLOG_ERROR("Failed to load glTF: {}", fastgltf::to_underlying(asset.error()));
        return tl::nullopt;
    }

    size_t scene_id{ t_scene_id.value_or(asset->defaultScene.value_or(0)) };
    if (asset->scenes.size() <= scene_id) {
        return tl::nullopt;
    }
    const auto& scene{ asset->scenes[scene_id] };

    return m_cache
        .transform([&](Cache& cache) {
            return cache.insert<Model>(
                Model::hash(t_filepath, scene_id),
                make_handle<Model>(load_model(asset.get(), scene))
            );
        })
        .value_or(make_handle<Model>(load_model(asset.get(), scene)));
}

}   // namespace core::renderer

auto load_model(const fastgltf::Asset& t_asset, const fastgltf::Scene& t_scene) -> Model
{
    GltfLoader2 loader;

    const auto [node_indices, _]{ t_scene };
    loader.root_nodes.reserve(node_indices.size());
    for (const auto node_index : node_indices) {
        loader.root_nodes.push_back(node_index);
        loader.node_indices.try_emplace(node_index, loader.nodes.size());
        load_node(
            loader, loader.nodes.emplace_back(), t_asset, t_asset.nodes[node_index], nullptr
        );
    }

    adjust_node_indices(loader);

    return Model{ .vertices   = std::move(loader.vertices),
                  .indices    = std::move(loader.indices),
                  .images     = {},
                  .samplers   = {},
                  .textures   = {},
                  .materials  = {},
                  .nodes      = std::move(loader.nodes),
                  .root_nodes = std::move(loader.root_nodes) };
}

auto load_node(
    GltfLoader2&           t_loader,
    Model::Node&           t_node,
    const fastgltf::Asset& t_asset,
    const fastgltf::Node&  t_source_node,
    Model::Node*           t_parent
) noexcept -> void
{
    if (t_parent) {
        t_node.parent = t_parent;
    }

    std::array<float, 3> scale{ 1.f, 1.f, 1.f };
    std::array<float, 4> rotation{ 0.f, 0.f, 0.f, 1.f };
    std::array<float, 3> translation{};
    std::visit(
        fastgltf::visitor{
            [&](const fastgltf::TRS& transform) {
                scale       = transform.scale;
                rotation    = transform.rotation;
                translation = transform.translation;
            },
            [&](const fastgltf::Node::TransformMatrix& matrix) {
                fastgltf::decomposeTransformMatrix(matrix, scale, rotation, translation);
            } },
        t_source_node.transform
    );
    t_node.scale       = glm::make_vec3(scale.data());
    t_node.rotation    = glm::make_quat(rotation.data());
    t_node.translation = glm::make_vec3(translation.data());

    if (t_source_node.meshIndex) {
        t_node.mesh =
            load_mesh(t_loader, t_asset, t_asset.meshes[*t_source_node.meshIndex]);
    }

    t_node.children.reserve(t_source_node.children.size());
    for (const auto child_index : t_source_node.children) {
        t_node.children.push_back(child_index);
        if (t_loader.node_indices.try_emplace(child_index, t_loader.nodes.size()).second)
        {
            load_node(
                t_loader,
                t_loader.nodes.emplace_back(),
                t_asset,
                t_asset.nodes[child_index],
                &t_node
            );
        }
    }
}

auto load_mesh(
    GltfLoader2&           t_loader,
    const fastgltf::Asset& t_asset,
    const fastgltf::Mesh&  t_source_mesh
) noexcept -> Model::Mesh
{
    Model::Mesh mesh;

    mesh.primitives.reserve(t_source_mesh.primitives.size());
    for (const auto& source_primitive : t_source_mesh.primitives) {
        load_primitive(t_loader, t_asset, source_primitive)
            .transform([&](Model::Primitive&& primitive) {
                mesh.primitives.push_back(primitive);
            });
    }

    return mesh;
}

auto load_primitive(
    GltfLoader2&               t_loader,
    const fastgltf::Asset&     t_asset,
    const fastgltf::Primitive& t_source_primitive
) noexcept -> tl::optional<Model::Primitive>
{
    Model::Primitive primitive;

    const auto first_vertex_index{ t_loader.vertices.size() };

    if (!load_vertices(t_loader, primitive, t_asset, t_source_primitive.attributes)) {
        return tl::nullopt;
    }

    if (auto indices_accessor_index{ t_source_primitive.indicesAccessor }) {
        load_indices(
            t_loader,
            primitive,
            static_cast<uint32_t>(first_vertex_index),
            t_asset,
            t_asset.accessors[*indices_accessor_index]
        );
    }

    return primitive;
}

[[nodiscard]] static auto make_accessor_loader(
    const fastgltf::Asset&      t_asset,
    std::vector<Model::Vertex>& t_vertices,
    size_t                      t_first_vertex_index
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
            std::remove_cvref_t<std::invoke_result_t<Projection, const Model::Vertex&>>;

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

auto load_vertices(
    GltfLoader2&                                                              t_loader,
    Model::Primitive&                                                         t_primitive,
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

    auto load_accessor{
        make_accessor_loader(t_asset, t_loader.vertices, t_loader.vertices.size())
    };

    const auto& position_accessor{ t_asset.accessors[position_iter->second] };

    t_primitive.vertex_count = static_cast<uint32_t>(position_accessor.count);
    t_loader.vertices.resize(t_loader.vertices.size() + position_accessor.count);

    load_accessor(position_accessor, &Model::Vertex::position);

    for (const auto& [name, accessor_index] : t_attributes) {
        if (name == "NORMAL") {
            load_accessor(t_asset.accessors[accessor_index], &Model::Vertex::normal);
        }
        else if (name == "TANGENT") {
            load_accessor(t_asset.accessors[accessor_index], &Model::Vertex::tangent);
        }
        else if (name == "TEXCOORD_0") {
            load_accessor(t_asset.accessors[accessor_index], &Model::Vertex::uv_0);
        }
        else if (name == "TEXCOORD_1") {
            load_accessor(t_asset.accessors[accessor_index], &Model::Vertex::uv_1);
        }
        else if (name == "COLOR_0") {
            load_accessor(t_asset.accessors[accessor_index], &Model::Vertex::color);
            load_accessor(
                t_asset.accessors[accessor_index],
                &Model::Vertex::color,
                [](const glm::vec3& vec3) { return glm::make_vec4(vec3); }
            );
        }
    }

    return true;
}

auto load_indices(
    GltfLoader2&              t_loader,
    Model::Primitive&         t_primitive,
    const uint32_t            t_first_vertex_index,
    const fastgltf::Asset&    t_asset,
    const fastgltf::Accessor& t_accessor
) noexcept -> void
{
    t_primitive.first_index_index = static_cast<uint32_t>(t_loader.indices.size());
    t_primitive.index_count       = static_cast<uint32_t>(t_accessor.count);

    t_loader.indices.reserve(t_loader.indices.size() + t_accessor.count);

    fastgltf::iterateAccessor<uint32_t>(t_asset, t_accessor, [&](const uint32_t index) {
        t_loader.indices.push_back(t_first_vertex_index + index);
    });
}

auto adjust_node_indices(GltfLoader2& t_loader) -> void
{
    for (size_t& root_node_index : t_loader.root_nodes) {
        root_node_index = t_loader.node_indices.at(root_node_index);
    }

    for (Model::Node& node : t_loader.nodes) {
        for (size_t& child_index : node.children) {
            child_index = t_loader.node_indices.at(child_index);
        }
    }
}
