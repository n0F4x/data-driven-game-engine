#include "Loader.hpp"

#include <array>
#include <ranges>

#include <spdlog/spdlog.h>

#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>

#include "core/utility/functional.hpp"

#include "ImageLoader.hpp"
#include "Model.hpp"

[[nodiscard]]
static auto load_asset(const std::filesystem::path& t_filepath
) -> fastgltf::Expected<fastgltf::Asset>
{
    fastgltf::Parser parser;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(t_filepath);

    return parser.loadGltf(
        &data,
        t_filepath.parent_path(),
        fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers
            | fastgltf::Options::GenerateMeshIndices
    );
}

[[nodiscard]]
static auto load_image(
    const std::filesystem::path& t_filepath,
    const fastgltf::Asset&       t_asset,
    const fastgltf::Image&       t_image
) -> std::optional<core::gltf::Image>
{
    return std::visit(
        fastgltf::visitor{
            [](std::monostate) -> std::optional<core::gltf::Image> {
                constexpr static std::string_view error_message{
                    "Got `std::monostate` while visiting fastgltf::DataSource, which is "
                    "an error in fastgltf."
                };
                SPDLOG_ERROR(error_message);
                throw std::runtime_error{ error_message.data() };
            },
            [](const auto&) -> std::optional<core::gltf::Image> {
                throw std::runtime_error(
                    "Got an unexpected glTF image data source. Can't load image."
                );
            },
            [&](const fastgltf::sources::BufferView& buffer_view) {
                const auto& view   = t_asset.bufferViews[buffer_view.bufferViewIndex];
                const auto& buffer = t_asset.buffers[view.bufferIndex];

                return std::visit(
                    fastgltf::visitor{
                        [](const auto&) -> std::optional<core::gltf::Image> {
                            throw std::runtime_error(
                                "Got an unexpected glTF image data source. "
                                "Can't load image from buffer view."
                            );
                        },
                        [&](const fastgltf::sources::Array& array) {
                            return core::gltf::ImageLoader::load_from_memory(
                                std::span(array.bytes.data(), array.bytes.size())
                                    .subspan(view.byteOffset),
                                buffer_view.mimeType
                            );
                        },
                        [&](const fastgltf::sources::Vector& vector) {
                            return core::gltf::ImageLoader::load_from_memory(
                                std::span(vector.bytes.data(), vector.bytes.size())
                                    .subspan(view.byteOffset),
                                buffer_view.mimeType
                            );
                        } },
                    buffer.data
                );
            },
            [&](const fastgltf::sources::URI& filepath) {
                assert(
                    filepath.fileByteOffset == 0 && "We don't support offsets with stbi"
                );   // TODO: Support offsets?
                assert(filepath.uri.isLocalPath());

                return core::gltf::ImageLoader::load_from_file(std::filesystem::absolute(
                    t_filepath.parent_path() / filepath.uri.fspath()
                ));
            },
            [&](const fastgltf::sources::Array& array) {
                return core::gltf::ImageLoader::load_from_memory(
                    std::span{ array.bytes }, array.mimeType
                );
            },
            [&](const fastgltf::sources::Vector& vector) {
                return core::gltf::ImageLoader::load_from_memory(
                    std::span{ vector.bytes }, vector.mimeType
                );
            },
        },
        t_image.data
    );
}

[[nodiscard]]
static auto convert_to_mag_filter(fastgltf::Optional<fastgltf::Filter> t_filter
) -> std::optional<core::gltf::Sampler::MagFilter>
{
    if (!t_filter.has_value()) {
        return std::nullopt;
    }

    using enum fastgltf::Filter;
    using enum core::gltf::Sampler::MagFilter;
    switch (t_filter.value()) {
        case Nearest: return eNearest;
        case Linear: return eLinear;
        default: std::unreachable();
    }
}

[[nodiscard]]
static auto convert_to_min_filter(fastgltf::Optional<fastgltf::Filter> t_filter
) -> std::optional<core::gltf::Sampler::MinFilter>
{
    if (!t_filter.has_value()) {
        return std::nullopt;
    }

    using enum fastgltf::Filter;
    using enum core::gltf::Sampler::MinFilter;
    switch (t_filter.value()) {
        case Nearest: return eNearest;
        case Linear: return eLinear;
        case NearestMipMapNearest: return eNearestMipmapNearest;
        case LinearMipMapNearest: return eLinearMipmapNearest;
        case NearestMipMapLinear: return eNearestMipmapLinear;
        case LinearMipMapLinear: return eLinearMipmapLinear;
    }
    std::unreachable();
}

[[nodiscard]]
static auto convert(const fastgltf::Wrap t_wrap) noexcept -> core::gltf::Sampler::WrapMode
{
    using enum fastgltf::Wrap;
    using enum core::gltf::Sampler::WrapMode;
    switch (t_wrap) {
        case ClampToEdge: return eClampToEdge;
        case MirroredRepeat: return eMirroredRepeat;
        case Repeat: return eRepeat;
    }
    std::unreachable();
}

[[nodiscard]]
static auto create_sampler(const fastgltf::Sampler& t_sampler) -> core::gltf::Sampler
{
    return core::gltf::Sampler{
        .mag_filter = convert_to_mag_filter(t_sampler.magFilter),
        .min_filter = convert_to_min_filter(t_sampler.minFilter),
        .wrap_s     = convert(t_sampler.wrapS),
        .wrap_t     = convert(t_sampler.wrapT),
    };
}

template <typename T, typename ReturnType>
[[nodiscard]]
static auto convert(const fastgltf::Optional<T>& t_optional
) noexcept -> std::optional<ReturnType>
{
    if (!t_optional.has_value()) {
        return std::nullopt;
    }
    return static_cast<ReturnType>(t_optional.value());
}

static auto create_texture(const fastgltf::Texture& t_texture) -> core::gltf::Texture
{
    assert(t_texture.imageIndex.has_value() && "glTF Image extensions are not handled");
    return core::gltf::Texture{
        .sampler_index = convert<size_t, uint32_t>(t_texture.samplerIndex),
        .image_index   = static_cast<uint32_t>(t_texture.imageIndex.value()),
    };
}

namespace core::gltf {

auto Loader::load_from_file(const std::filesystem::path& t_filepath
) -> std::optional<Model>
{
    auto asset{ load_asset(t_filepath) };
    if (asset.error() != fastgltf::Error::None) {
        SPDLOG_ERROR("Failed to load glTF: {}", fastgltf::to_underlying(asset.error()));
        return std::nullopt;
    }

    return load_model(t_filepath, asset.get(), asset->defaultScene.value_or(0));
}

auto Loader::load_from_file(
    const std::filesystem::path& t_filepath,
    const size_t                 t_scene_id
) -> std::optional<Model>
{
    auto asset{ load_asset(t_filepath) };
    if (asset.error() != fastgltf::Error::None) {
        SPDLOG_ERROR("Failed to load glTF: {}", fastgltf::to_underlying(asset.error()));
        return std::nullopt;
    }

    return load_model(t_filepath, asset.get(), t_scene_id);
}

auto Loader::load_model(
    const std::filesystem::path& t_filepath,
    const fastgltf::Asset&       t_asset,
    size_t                       t_scene_id
) -> Model
{
    // TODO: make this an assertion
    if (t_asset.scenes.size() <= t_scene_id) {
        SPDLOG_ERROR(
            "The provided glTF model does not contain the requested scene: {}", t_scene_id
        );
    }

    Model model;

    load_nodes(model, t_asset, t_asset.scenes[t_scene_id].nodeIndices);
    load_images(model, t_asset, t_filepath);
    load_materials(model, t_asset);

    return model;
}

auto Loader::load_nodes(
    Model&                                              t_model,
    const fastgltf::Asset&                              t_asset,
    const fastgltf::pmr::MaybeSmallVector<std::size_t>& t_node_indices
) -> void
{
    t_model.m_root_node_indices.reserve(t_node_indices.size());
    std::unordered_map<size_t, size_t> node_index_map;
    for (const auto node_index : t_node_indices) {
        t_model.m_root_node_indices.push_back(node_index);
        node_index_map.try_emplace(node_index, t_model.m_nodes.size());
        load_node(
            t_model,
            t_model.m_nodes.emplace_back(),
            t_asset,
            t_asset.nodes[node_index],
            nullptr,
            node_index_map
        );
    }
    adjust_node_indices(t_model, node_index_map);
}

auto Loader::load_images(
    Model&                       t_model,
    const fastgltf::Asset&       t_asset,
    const std::filesystem::path& t_filepath
) -> void
{
    t_model.m_images.reserve(t_asset.images.size());
    for (const fastgltf::Image& image : t_asset.images) {
        if (std::optional<Image> loaded_image = load_image(t_filepath, t_asset, image);
            loaded_image.has_value())
        {
            t_model.m_images.push_back(std::move(loaded_image.value()));
        }
        else {
            throw std::runtime_error{ std::format(
                "Failed to load image {} from gltf image {}",
                image.name,
                t_filepath.generic_string()
            ) };
        }
    }

    t_model.m_samplers.reserve(t_asset.samplers.size());
    for (const fastgltf::Sampler& sampler : t_asset.samplers) {
        t_model.m_samplers.push_back(create_sampler(sampler));
    }

    t_model.m_textures.reserve(t_asset.textures.size());
    for (const fastgltf::Texture& texture : t_asset.textures) {
        t_model.m_textures.push_back(create_texture(texture));
    }
}

auto Loader::load_materials(Model& t_model, const fastgltf::Asset& t_asset) -> void
{
    t_model.m_materials.reserve(t_asset.materials.size());
    for (const fastgltf::Material& material : t_asset.materials) {
        t_model.m_materials.push_back(Material::create(material));

        if (auto specular_glossiness_material{ SpecularGlossiness::create_material(
                material, t_model.m_materials.size()
            ) })
        {
            t_model.m_extensions.specular_glossiness.materials.push_back(
                specular_glossiness_material.value()
            );
        }
    }
}

auto Loader::load_node(
    Model&                              t_model,
    Node&                               t_node,
    const fastgltf::Asset&              t_asset,
    const fastgltf::Node&               t_source_node,
    Node*                               t_parent,
    std::unordered_map<size_t, size_t>& t_node_index_map
) -> void
{
    if (t_parent != nullptr) {
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
        t_node.mesh_index =
            load_mesh(t_model, t_asset, t_asset.meshes[*t_source_node.meshIndex]);
    }

    t_node.child_indices.reserve(t_source_node.children.size());
    for (const auto child_index : t_source_node.children) {
        t_node.child_indices.push_back(child_index);
        if (t_node_index_map.try_emplace(child_index, t_model.m_nodes.size()).second) {
            load_node(
                t_model,
                t_model.m_nodes.emplace_back(),
                t_asset,
                t_asset.nodes[child_index],
                &t_node,
                t_node_index_map
            );
        }
    }
}

auto Loader::load_mesh(
    Model&                 t_model,
    const fastgltf::Asset& t_asset,
    const fastgltf::Mesh&  t_source_mesh
) -> size_t
{
    const size_t index = t_model.m_meshes.size();
    auto& [primitives]{ t_model.m_meshes.emplace_back() };

    primitives.reserve(t_source_mesh.primitives.size());
    for (const auto& source_primitive : t_source_mesh.primitives) {
        if (auto primitive{ load_primitive(t_model, t_asset, source_primitive) };
            primitive.has_value())
        {
            primitives.push_back(primitive.value());
        }
    }

    return index;
}

[[nodiscard]]
static auto convert(const fastgltf::PrimitiveType t_topology
) noexcept -> Mesh::Primitive::Topology
{
    using enum Mesh::Primitive::Topology;
    switch (t_topology) {
        case fastgltf::PrimitiveType::Points: return ePoints;
        case fastgltf::PrimitiveType::Lines: return eLines;
        case fastgltf::PrimitiveType::LineLoop: return eLineLoops;
        case fastgltf::PrimitiveType::LineStrip: return eLineStrips;
        case fastgltf::PrimitiveType::Triangles: return eTriangles;
        case fastgltf::PrimitiveType::TriangleStrip: return eTriangleStrips;
        case fastgltf::PrimitiveType::TriangleFan: return eTriangleFans;
    }
    std::unreachable();
}

[[nodiscard]]
static auto convert(fastgltf::Optional<std::size_t> optional
) noexcept -> std::optional<uint32_t>
{
    if (!optional.has_value()) {
        return std::nullopt;
    }
    return static_cast<uint32_t>(optional.value());
}

auto Loader::load_primitive(
    Model&                     t_model,
    const fastgltf::Asset&     t_asset,
    const fastgltf::Primitive& t_source_primitive
) -> std::optional<Mesh::Primitive>
{
    Mesh::Primitive primitive{
        .mode           = convert(t_source_primitive.type),
        .material_index = convert(t_source_primitive.materialIndex),
    };

    const auto first_vertex_index{ t_model.m_vertices.size() };

    if (!load_vertices(t_model, primitive, t_asset, t_source_primitive.attributes)) {
        return std::nullopt;
    }

    if (auto indices_accessor_index{ t_source_primitive.indicesAccessor }) {
        load_indices(
            t_model,
            primitive,
            static_cast<uint32_t>(first_vertex_index),
            t_asset,
            t_asset.accessors[*indices_accessor_index]
        );
    }

    return primitive;
}

[[nodiscard]]
static auto make_accessor_loader(
    const fastgltf::Asset&      t_asset,
    std::vector<Model::Vertex>& t_vertices,
    size_t                      t_first_vertex_index
)
{
    return [&, t_first_vertex_index]<typename Projection, typename Transformation>(
               const fastgltf::Accessor& t_accessor,
               Projection                project,
               Transformation            transform
           ) -> void {
        using ElementType = std::remove_cvref_t<
            std::tuple_element_t<0, decltype(core::utils::arguments(transform))>>;
        using AttributeType =
            std::remove_cvref_t<std::invoke_result_t<Projection, const Model::Vertex&>>;

        fastgltf::iterateAccessorWithIndex<ElementType>(
            t_asset,
            t_accessor,
            [&, t_first_vertex_index](const ElementType& element, const size_t index) {
                std::invoke(project, t_vertices[t_first_vertex_index + index]) =
                    std::invoke_r<AttributeType>(transform, element);
            }
        );
    };
}

[[nodiscard]]
static auto make_identity_accessor_loader(
    const fastgltf::Asset&      t_asset,
    std::vector<Model::Vertex>& t_vertices,
    size_t                      t_first_vertex_index
)
{
    return [&, t_first_vertex_index]<typename Projection>(
               const fastgltf::Accessor& t_accessor, Projection project
           ) -> void {
        using AttributeType =
            std::remove_cvref_t<std::invoke_result_t<Projection, const Model::Vertex&>>;

        fastgltf::iterateAccessorWithIndex<AttributeType>(
            t_asset,
            t_accessor,
            [&, t_first_vertex_index](const AttributeType& element, const size_t index) {
                std::invoke(project, t_vertices[t_first_vertex_index + index]) = element;
            }
        );
    };
}

auto Loader::load_vertices(
    Model&                                                                    t_model,
    Mesh::Primitive&                                                          t_primitive,
    const fastgltf::Asset&                                                    t_asset,
    const fastgltf::pmr::SmallVector<fastgltf::Primitive::attribute_type, 4>& t_attributes
) -> bool
{
    const auto* const position_iter{ std::ranges::find(
        t_attributes, "POSITION", &fastgltf::Primitive::attribute_type::first
    ) };
    if (position_iter == t_attributes.end()) {
        return false;
    }

    auto load_accessor{
        make_accessor_loader(t_asset, t_model.m_vertices, t_model.m_vertices.size())
    };
    auto load_identity_accessor{ make_identity_accessor_loader(
        t_asset, t_model.m_vertices, t_model.m_vertices.size()
    ) };

    const auto& position_accessor{ t_asset.accessors[position_iter->second] };

    t_primitive.vertex_count = static_cast<uint32_t>(position_accessor.count);
    t_model.m_vertices.resize(t_model.m_vertices.size() + position_accessor.count);

    load_accessor(position_accessor, &Model::Vertex::position, [](const glm::vec3& vec3) {
        return glm::make_vec4(vec3);
    });

    for (const auto& [name, accessor_index] : t_attributes) {
        if (name == "NORMAL") {
            load_accessor(
                t_asset.accessors[accessor_index],
                &Model::Vertex::normal,
                [](const glm::vec3& vec3) { return glm::make_vec4(vec3); }
            );
        }
        else if (name == "TANGENT") {
            load_identity_accessor(
                t_asset.accessors[accessor_index], &Model::Vertex::tangent
            );
        }
        else if (name == "TEXCOORD_0") {
            load_identity_accessor(
                t_asset.accessors[accessor_index], &Model::Vertex::uv_0
            );
        }
        else if (name == "TEXCOORD_1") {
            load_identity_accessor(
                t_asset.accessors[accessor_index], &Model::Vertex::uv_1
            );
        }
        else if (name == "COLOR_0") {
            load_identity_accessor(
                t_asset.accessors[accessor_index], &Model::Vertex::color
            );
            load_accessor(
                t_asset.accessors[accessor_index],
                &Model::Vertex::color,
                [](const glm::vec3& vec3) { return glm::make_vec4(vec3); }
            );
        }
    }

    return true;
}

auto Loader::load_indices(
    Model&                    t_model,
    Mesh::Primitive&          t_primitive,
    const uint32_t            t_first_vertex_index,
    const fastgltf::Asset&    t_asset,
    const fastgltf::Accessor& t_accessor
) -> void
{
    t_primitive.first_index_index = static_cast<uint32_t>(t_model.m_indices.size());
    t_primitive.index_count       = static_cast<uint32_t>(t_accessor.count);

    t_model.m_indices.reserve(t_model.m_indices.size() + t_accessor.count);

    fastgltf::iterateAccessor<uint32_t>(t_asset, t_accessor, [&](const uint32_t index) {
        t_model.m_indices.push_back(t_first_vertex_index + index);
    });
}

auto Loader::adjust_node_indices(
    Model&                                    t_model,
    const std::unordered_map<size_t, size_t>& t_node_index_map
) -> void
{
    for (size_t& root_node_index : t_model.m_root_node_indices) {
        root_node_index = t_node_index_map.at(root_node_index);
    }

    for (Node& node : t_model.m_nodes) {
        for (size_t& child_index : node.child_indices) {
            child_index = t_node_index_map.at(child_index);
        }
    }
}

}   // namespace core::gltf
