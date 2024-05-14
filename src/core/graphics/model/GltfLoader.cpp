#include "GltfLoader.hpp"

#include <array>
#include <ranges>

#include <spdlog/spdlog.h>

#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>

#include "core/utility/functional.hpp"

#include "ImageLoader.hpp"

using namespace core::graphics;

namespace internal {

struct GltfModel {
    std::vector<size_t>                root_nodes;
    std::unordered_map<size_t, size_t> node_indices;
    std::vector<Model::Node>           nodes;

    std::vector<Model::Vertex> vertices;
    std::vector<uint32_t>      indices;

    std::vector<Model::Mesh> meshes;

    std::vector<Model::Image>    images;
    std::vector<Model::Sampler>  samplers;
    std::vector<Model::Texture>  textures;
    std::vector<Model::Material> materials;
};

}   // namespace internal

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
            | fastgltf::Options::DecomposeNodeMatrices
    );
}

static auto load_node(
    internal::GltfModel&   t_loader,
    Model::Node&           t_node,
    const fastgltf::Asset& t_asset,
    const fastgltf::Node&  t_source_node,
    Model::Node*           t_parent
) -> void;

[[nodiscard]]
static auto load_mesh(
    internal::GltfModel&   t_loader,
    const fastgltf::Asset& t_asset,
    const fastgltf::Mesh&  t_source_mesh
) -> size_t;

[[nodiscard]]
static auto load_primitive(
    internal::GltfModel&       t_loader,
    const fastgltf::Asset&     t_asset,
    const fastgltf::Primitive& t_source_primitive
) -> std::optional<Model::Mesh::Primitive>;

[[nodiscard]]
static auto load_vertices(
    internal::GltfModel&                                                      t_loader,
    Model::Mesh::Primitive&                                                   t_primitive,
    const fastgltf::Asset&                                                    t_asset,
    const fastgltf::pmr::SmallVector<fastgltf::Primitive::attribute_type, 4>& t_attributes
) -> bool;

static auto load_indices(
    internal::GltfModel&      t_loader,
    Model::Mesh::Primitive&   t_primitive,
    uint32_t                  t_first_vertex_index,
    const fastgltf::Asset&    t_asset,
    const fastgltf::Accessor& t_accessor
) -> void;

static auto adjust_node_indices(internal::GltfModel& t_loader) -> void;

[[nodiscard]]
static auto load_image(
    const std::filesystem::path& t_filepath,
    const fastgltf::Asset&       t_asset,
    const fastgltf::Image&       t_image
) -> std::optional<Model::Image>;

[[nodiscard]]
static auto create_sampler(const fastgltf::Sampler& sampler) -> Model::Sampler;

[[nodiscard]]
static auto create_texture(const fastgltf::Texture& texture) -> Model::Texture;

[[nodiscard]]
static auto create_material(const fastgltf::Material& material) -> Model::Material;

namespace core::graphics {

auto GltfLoader::load_from_file(const std::filesystem::path& t_filepath
) -> std::optional<Model>
{
    auto asset{ load_asset(t_filepath) };
    if (asset.error() != fastgltf::Error::None) {
        SPDLOG_ERROR("Failed to load glTF: {}", fastgltf::to_underlying(asset.error()));
        return std::nullopt;
    }

    return load_model(t_filepath, asset.get(), asset->defaultScene.value_or(0));
}

auto GltfLoader::load_from_file(
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

}   // namespace core::graphics

auto GltfLoader::load_model(
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

    const auto&         scene{ t_asset.scenes[t_scene_id] };
    internal::GltfModel loader;

    const auto [node_indices, _]{ scene };
    loader.root_nodes.reserve(node_indices.size());
    for (const auto node_index : node_indices) {
        loader.root_nodes.push_back(node_index);
        loader.node_indices.try_emplace(node_index, loader.nodes.size());
        load_node(
            loader, loader.nodes.emplace_back(), t_asset, t_asset.nodes[node_index], nullptr
        );
    }
    adjust_node_indices(loader);

    loader.images.reserve(t_asset.images.size());
    for (const fastgltf::Image& image : t_asset.images) {
        std::optional<Model::Image> loaded_image = load_image(t_filepath, t_asset, image);
        if (loaded_image.has_value()) {
            loader.images.push_back(std::move(loaded_image.value()));
        }
        else {
            throw std::runtime_error{ std::format(
                "Failed to load image {} from gltf asset {}",
                image.name,
                t_filepath.generic_string()
            ) };
        }
    }

    loader.samplers.reserve(t_asset.samplers.size());
    for (const fastgltf::Sampler& sampler : t_asset.samplers) {
        loader.samplers.push_back(create_sampler(sampler));
    }

    loader.textures.reserve(t_asset.textures.size());
    for (const fastgltf::Texture& texture : t_asset.textures) {
        loader.textures.push_back(create_texture(texture));
    }

    loader.materials.reserve(t_asset.materials.size());
    for (const fastgltf::Material& material : t_asset.materials) {
        loader.materials.push_back(create_material(material));
    }

    Model result;
    result.m_vertices          = std::move(loader.vertices);
    result.m_indices           = std::move(loader.indices);
    result.m_images            = std::move(loader.images);
    result.m_samplers          = std::move(loader.samplers);
    result.m_textures          = std::move(loader.textures);
    result.m_materials         = std::move(loader.materials);
    result.m_meshes            = std::move(loader.meshes);
    result.m_nodes             = std::move(loader.nodes);
    result.m_root_node_indices = std::move(loader.root_nodes);
    return result;
}

auto load_node(
    internal::GltfModel&   t_loader,
    Model::Node&           t_node,
    const fastgltf::Asset& t_asset,
    const fastgltf::Node&  t_source_node,
    Model::Node*           t_parent
) -> void
{
    if (t_parent != nullptr) {
        t_node.parent = t_parent;
    }

    std::array<float, 3> scale{ 1.f, 1.f, 1.f };
    std::array<float, 4> rotation{
        0.f, 0.f, 0.f, 1.f
    };   // NOLINT(*-uppercase-literal-suffix)
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
            load_mesh(t_loader, t_asset, t_asset.meshes[*t_source_node.meshIndex]);
    }

    t_node.child_indices.reserve(t_source_node.children.size());
    for (const auto child_index : t_source_node.children) {
        t_node.child_indices.push_back(child_index);
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
    internal::GltfModel&   t_loader,
    const fastgltf::Asset& t_asset,
    const fastgltf::Mesh&  t_source_mesh
) -> size_t
{
    const size_t index = t_loader.meshes.size();
    Model::Mesh& mesh{ t_loader.meshes.emplace_back() };

    mesh.primitives.reserve(t_source_mesh.primitives.size());
    for (const auto& source_primitive : t_source_mesh.primitives) {
        auto primitive{ load_primitive(t_loader, t_asset, source_primitive) };
        if (primitive.has_value()) {
            mesh.primitives.push_back(primitive.value());
        }
    }

    return index;
}

[[nodiscard]]
static auto convert(fastgltf::PrimitiveType t_topology
) noexcept -> Model::Mesh::Primitive::Topology
{
    using enum Model::Mesh::Primitive::Topology;
    switch (t_topology) {
        case fastgltf::PrimitiveType::Points: return ePoints;
        case fastgltf::PrimitiveType::Lines: return eLines;
        case fastgltf::PrimitiveType::LineLoop: return eLineLoops;
        case fastgltf::PrimitiveType::LineStrip: return eLineStrips;
        case fastgltf::PrimitiveType::Triangles: return eTriangles;
        case fastgltf::PrimitiveType::TriangleStrip: return eTriangleStrips;
        case fastgltf::PrimitiveType::TriangleFan: return eTriangleFans;
    }
}

[[nodiscard]]
static auto convert(fastgltf::Optional<std::size_t> optional
) noexcept -> std::optional<uint32_t>
{
    if (!optional.has_value()) {
        return std::nullopt;
    }
    return optional.value();
}

auto load_primitive(
    internal::GltfModel&       t_loader,
    const fastgltf::Asset&     t_asset,
    const fastgltf::Primitive& t_source_primitive
) -> std::optional<Model::Mesh::Primitive>
{
    Model::Mesh::Primitive primitive{
        .mode           = convert(t_source_primitive.type),
        .material_index = convert(t_source_primitive.materialIndex),
    };

    const auto first_vertex_index{ t_loader.vertices.size() };

    if (!load_vertices(t_loader, primitive, t_asset, t_source_primitive.attributes)) {
        return std::nullopt;
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

auto load_vertices(
    internal::GltfModel&                                                      t_loader,
    Model::Mesh::Primitive&                                                   t_primitive,
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
        make_accessor_loader(t_asset, t_loader.vertices, t_loader.vertices.size())
    };
    auto load_identity_accessor{ make_identity_accessor_loader(
        t_asset, t_loader.vertices, t_loader.vertices.size()
    ) };

    const auto& position_accessor{ t_asset.accessors[position_iter->second] };

    t_primitive.vertex_count = static_cast<uint32_t>(position_accessor.count);
    t_loader.vertices.resize(t_loader.vertices.size() + position_accessor.count);

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

auto load_indices(
    internal::GltfModel&      t_loader,
    Model::Mesh::Primitive&         t_primitive,
    const uint32_t            t_first_vertex_index,
    const fastgltf::Asset&    t_asset,
    const fastgltf::Accessor& t_accessor
) -> void
{
    t_primitive.first_index_index = static_cast<uint32_t>(t_loader.indices.size());
    t_primitive.index_count       = static_cast<uint32_t>(t_accessor.count);

    t_loader.indices.reserve(t_loader.indices.size() + t_accessor.count);

    fastgltf::iterateAccessor<uint32_t>(t_asset, t_accessor, [&](const uint32_t index) {
        t_loader.indices.push_back(t_first_vertex_index + index);
    });
}

auto adjust_node_indices(internal::GltfModel& t_loader) -> void
{
    for (size_t& root_node_index : t_loader.root_nodes) {
        root_node_index = t_loader.node_indices.at(root_node_index);
    }

    for (Model::Node& node : t_loader.nodes) {
        for (size_t& child_index : node.child_indices) {
            child_index = t_loader.node_indices.at(child_index);
        }
    }
}

auto load_image(
    const std::filesystem::path& t_filepath,
    const fastgltf::Asset&       t_asset,
    const fastgltf::Image&       t_image
) -> std::optional<Model::Image>
{
    return std::visit(
        fastgltf::visitor{
            [](const auto&) -> std::optional<Model::Image> {
                throw std::runtime_error(
                    "Got an unexpected glTF image data source. Can't load image."
                );
            },
            [&](const fastgltf::sources::URI& filepath) {
                assert(
                    filepath.fileByteOffset == 0 && "We don't support offsets with stbi"
                );   // TODO: Support offsets?
                assert(filepath.uri.isLocalPath());

                return ImageLoader::load_from_file(std::filesystem::absolute(
                    t_filepath.parent_path() / filepath.uri.fspath()
                ));
            },
            [&](const fastgltf::sources::Array& array) {
                return ImageLoader::load_from_memory(
                    std::span{ array.bytes }, array.mimeType
                );
            },
            [&](const fastgltf::sources::Vector& vector) {
                return ImageLoader::load_from_memory(
                    std::span{ vector.bytes }, vector.mimeType
                );
            },
            [&](const fastgltf::sources::BufferView& buffer_view) {
                const auto& view   = t_asset.bufferViews[buffer_view.bufferViewIndex];
                const auto& buffer = t_asset.buffers[view.bufferIndex];

                return std::visit(
                    fastgltf::visitor{
                        [](const auto&) -> std::optional<Model::Image> {
                            throw std::runtime_error(
                                "Got an unexpected glTF image data source. "
                                "Can't load image from buffer view."
                            );
                        },
                        [&](const fastgltf::sources::Array& array) {
                            return ImageLoader::load_from_memory(
                                std::span(array.bytes.data(), array.bytes.size())
                                    .subspan(view.byteOffset),
                                buffer_view.mimeType
                            );
                        },
                        [&](const fastgltf::sources::Vector& vector) {
                            return ImageLoader::load_from_memory(
                                std::span(vector.bytes.data(), vector.bytes.size())
                                    .subspan(view.byteOffset),
                                buffer_view.mimeType
                            );
                        } },
                    buffer.data
                );
            },
        },
        t_image.data
    );
}

[[nodiscard]]
auto convert_to_mag_filter(fastgltf::Optional<fastgltf::Filter> t_filter
) -> std::optional<Model::Sampler::MagFilter>
{
    if (!t_filter.has_value()) {
        return std::nullopt;
    }

    using enum fastgltf::Filter;
    using enum Model::Sampler::MagFilter;
    switch (t_filter.value()) {
        case Nearest: return eNearest;
        case Linear: return eLinear;
        default: std::unreachable();
    }
}

[[nodiscard]]
auto convert_to_min_filter(fastgltf::Optional<fastgltf::Filter> t_filter
) -> std::optional<Model::Sampler::MinFilter>
{
    if (!t_filter.has_value()) {
        return std::nullopt;
    }

    using enum fastgltf::Filter;
    using enum Model::Sampler::MinFilter;
    switch (t_filter.value()) {
        case Nearest: return eNearest;
        case Linear: return eLinear;
        case NearestMipMapNearest: return eNearestMipmapNearest;
        case LinearMipMapNearest: return eLinearMipmapNearest;
        case NearestMipMapLinear: return eNearestMipmapLinear;
        case LinearMipMapLinear: return eLinearMipmapLinear;
    }
}

[[nodiscard]]
auto convert(fastgltf::Wrap t_wrap) noexcept -> Model::Sampler::WrapMode
{
    using enum fastgltf::Wrap;
    using enum Model::Sampler::WrapMode;
    switch (t_wrap) {
        case ClampToEdge: return eClampToEdge;
        case MirroredRepeat: return eMirroredRepeat;
        case Repeat: return eRepeat;
    }
}

[[nodiscard]]
auto create_sampler(const fastgltf::Sampler& t_sampler) -> Model::Sampler
{
    return Model::Sampler{
        .mag_filter = convert_to_mag_filter(t_sampler.magFilter),
        .min_filter = convert_to_min_filter(t_sampler.minFilter),
        .wrap_s     = convert(t_sampler.wrapS),
        .wrap_t     = convert(t_sampler.wrapT),
    };
}

template <typename T>
[[nodiscard]]
auto convert(const fastgltf::Optional<T>& t_optional) noexcept -> std::optional<T>
{
    if (!t_optional.has_value()) {
        return std::nullopt;
    }
    return t_optional.value();
}

auto create_texture(const fastgltf::Texture& t_texture) -> Model::Texture
{
    assert(t_texture.imageIndex.has_value() && "glTF Image extensions are not handled");
    return Model::Texture{
        .sampler_index = convert<size_t>(t_texture.samplerIndex),
        .image_index   = static_cast<uint32_t>(t_texture.imageIndex.value()),
    };
}

[[nodiscard]]
static auto convert(const fastgltf::Optional<fastgltf::TextureInfo>& t_optional
) -> std::optional<Model::TextureInfo>
{
    if (!t_optional.has_value()) {
        return std::nullopt;
    }
    return Model::TextureInfo{
        .texture_index   = static_cast<uint32_t>(t_optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(t_optional->texCoordIndex),
    };
}

[[nodiscard]]
static auto convert(const fastgltf::Optional<fastgltf::NormalTextureInfo>& t_optional
) -> std::optional<Model::Material::NormalTextureInfo>
{
    if (!t_optional.has_value()) {
        return std::nullopt;
    }
    return Model::Material::NormalTextureInfo{
        .texture_index   = static_cast<uint32_t>(t_optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(t_optional->texCoordIndex),
        .scale           = t_optional->scale,
    };
}

[[nodiscard]]
static auto convert(const fastgltf::Optional<fastgltf::OcclusionTextureInfo>& t_optional
) -> std::optional<Model::Material::OcclusionTextureInfo>
{
    if (!t_optional.has_value()) {
        return std::nullopt;
    }
    return Model::Material::OcclusionTextureInfo{
        .texture_index   = static_cast<uint32_t>(t_optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(t_optional->texCoordIndex),
        .strength        = t_optional->strength,
    };
}

[[nodiscard]]
static auto convert(fastgltf::AlphaMode t_alpha_mode
) noexcept -> Model::Material::AlphaMode
{
    using enum Model::Material::AlphaMode;
    switch (t_alpha_mode) {
        case fastgltf::AlphaMode::Opaque: return eOpaque;
        case fastgltf::AlphaMode::Mask: return eMask;
        case fastgltf::AlphaMode::Blend: return eBlend;
    }
}

auto create_material(const fastgltf::Material& t_material) -> Model::Material
{
    using PbrMetallicRoughness = Model::Material::PbrMetallicRoughness;
    return Model::Material{
        .pbr_metallic_roughness =
            PbrMetallicRoughness{
                                 .base_color_factor =
                    glm::make_vec4(t_material.pbrData.baseColorFactor.data()),
                                 .base_color_texture_info = convert(t_material.pbrData.baseColorTexture),
                                 .metallic_factor         = t_material.pbrData.metallicFactor,
                                 .roughness_factor        = t_material.pbrData.roughnessFactor,
                                 .metallic_roughness_texture_info =
                    convert(t_material.pbrData.metallicRoughnessTexture),
                                 },
        .normal_texture_info    = convert(t_material.normalTexture),
        .occlusion_texture_info = convert(t_material.occlusionTexture),
        .emissive_texture_info  = convert(t_material.emissiveTexture),
        .emissive_factor        = glm::make_vec3(t_material.emissiveFactor.data()),
        .alpha_mode             = convert(t_material.alphaMode),
        .alpha_cutoff           = t_material.alphaCutoff,
        .double_sided           = t_material.doubleSided
    };
}
