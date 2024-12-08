module;

#include <format>
#include <ranges>
#include <span>

#include <spdlog/spdlog.h>

#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>

module core.gltf.Model;

import core.utility.meta.functional;

import core.gltf.Image;
import core.gltf.ImageLoader;
import core.gltf.Mesh;
import core.gltf.Texture;

[[nodiscard]]
static auto load_asset(const std::filesystem::path& filepath)
    -> fastgltf::Expected<fastgltf::Asset>
{
    auto data{ fastgltf::GltfDataBuffer::FromPath(filepath) };
    if (data.error() != fastgltf::Error::None) {
        return data.error();
    }

    return fastgltf::Parser{ fastgltf::Extensions::KHR_texture_basisu }.loadGltf(
        data.get(),
        filepath.parent_path(),
        fastgltf::Options::LoadExternalBuffers | fastgltf::Options::GenerateMeshIndices
    );
}

[[nodiscard]]
static auto load_image(
    const std::filesystem::path& filepath,
    const fastgltf::Asset&       asset,
    const fastgltf::Image&       image
) -> std::optional<core::gltf::Image>
{
    return image.data.visit(
        fastgltf::visitor{
            [](std::monostate) -> std::optional<core::gltf::Image> {
                assert(false &&
                    "Got `std::monostate` while visiting fastgltf::DataSource, which is "
                    "an error in fastgltf."
                );
            },
            [](const auto&) -> std::optional<core::gltf::Image> {
                assert(
                    false && "Got an unexpected glTF image data source. Can't load image."
                );
            },
            [&](const fastgltf::sources::BufferView& buffer_view) {
                const auto& [buffer_index, byte_offset, _, _, _, _, _]{
                    asset.bufferViews[buffer_view.bufferViewIndex]
                };
                const auto& [_, data, _]{ asset.buffers[buffer_index] };

                return data.visit(
                    fastgltf::visitor{
                        [](const auto&) -> std::optional<core::gltf::Image> {
                            throw std::runtime_error(
                                "Got an unexpected glTF image data source. "
                                "Can't load image from buffer view."
                            );
                        },
                        [&](const fastgltf::sources::Array& array) {
                            return core::gltf::ImageLoader::load_from(
                                std::span{ array.bytes }.subspan(byte_offset),
                                buffer_view.mimeType
                            );
                        },
                        [&](const fastgltf::sources::Vector& vector) {
                            return core::gltf::ImageLoader::load_from(
                                std::span{ vector.bytes }.subspan(byte_offset),
                                buffer_view.mimeType
                            );
                        } }
                );
            },
            [&](const fastgltf::sources::URI& uri) {
                assert(
                    uri.fileByteOffset == 0 && "We don't support offsets with stbi"
                );   // TODO: Support offsets?
                assert(uri.uri.isLocalPath());

                return core::gltf::ImageLoader::load_from(
                    std::filesystem::absolute(filepath.parent_path() / uri.uri.fspath())
                );
            },
            [&](const fastgltf::sources::Array& array) {
                return core::gltf::ImageLoader::load_from(
                    std::span{ array.bytes }, array.mimeType
                );
            },
            [&](const fastgltf::sources::Vector& vector) {
                return core::gltf::ImageLoader::load_from(
                    std::span{ vector.bytes }, vector.mimeType
                );
            },
        }
    );
}

[[nodiscard]]
static auto convert_to_mag_filter(fastgltf::Optional<fastgltf::Filter> filter)
    -> core::gltf::Sampler::MagFilter
{
    using enum core::gltf::Sampler::MagFilter;

    if (!filter.has_value()) {
        return eLinear;
    }

    using enum fastgltf::Filter;
    switch (filter.value()) {
        case Nearest: return eNearest;
        case Linear:  return eLinear;
        default:      std::unreachable();
    }
}

[[nodiscard]]
static auto convert_to_min_filter(fastgltf::Optional<fastgltf::Filter> filter)
    -> core::gltf::Sampler::MinFilter
{
    using enum core::gltf::Sampler::MinFilter;

    if (!filter.has_value()) {
        return eLinear;
    }

    using enum fastgltf::Filter;
    switch (filter.value()) {
        case Nearest:              return eNearest;
        case Linear:               return eLinear;
        case NearestMipMapNearest: return eNearestMipmapNearest;
        case LinearMipMapNearest:  return eLinearMipmapNearest;
        case NearestMipMapLinear:  return eNearestMipmapLinear;
        case LinearMipMapLinear:   return eLinearMipmapLinear;
    }
    std::unreachable();
}

[[nodiscard]]
static auto convert(const fastgltf::Wrap wrap) noexcept -> core::gltf::Sampler::WrapMode
{
    using enum fastgltf::Wrap;
    using enum core::gltf::Sampler::WrapMode;
    switch (wrap) {
        case ClampToEdge:    return eClampToEdge;
        case MirroredRepeat: return eMirroredRepeat;
        case Repeat:         return eRepeat;
    }
    std::unreachable();
}

[[nodiscard]]
static auto create_sampler(const fastgltf::Sampler& sampler) -> core::gltf::Sampler
{
    return core::gltf::Sampler{
        .mag_filter = ::convert_to_mag_filter(sampler.magFilter),
        .min_filter = ::convert_to_min_filter(sampler.minFilter),
        .wrap_s     = ::convert(sampler.wrapS),
        .wrap_t     = ::convert(sampler.wrapT),
    };
}

template <typename T, typename ReturnType>
[[nodiscard]]
static auto convert(const fastgltf::Optional<T>& optional) noexcept
    -> std::optional<ReturnType>
{
    if (!optional.has_value()) {
        return std::nullopt;
    }
    return static_cast<ReturnType>(optional.value());
}

static auto create_texture(const fastgltf::Texture& texture) -> core::gltf::Texture
{
    if (texture.basisuImageIndex.has_value()) {
        return core::gltf::Texture{
            .sampler_index = ::convert<size_t, uint32_t>(texture.samplerIndex),
            .image_index   = static_cast<uint32_t>(texture.basisuImageIndex.value()),
        };
    }

    if (texture.imageIndex.has_value()) {
        return core::gltf::Texture{
            .sampler_index = ::convert<size_t, uint32_t>(texture.samplerIndex),
            .image_index   = static_cast<uint32_t>(texture.imageIndex.value()),
        };
    }

    throw std::runtime_error{
        "glTF texture's `source` is not provided or an image extension is not supported."
    };
}

[[nodiscard]]
static auto convert(const fastgltf::PrimitiveType topology) noexcept
    -> core::gltf::Mesh::Primitive::Topology
{
    using enum core::gltf::Mesh::Primitive::Topology;
    switch (topology) {
        case fastgltf::PrimitiveType::Points:        return ePoints;
        case fastgltf::PrimitiveType::Lines:         return eLines;
        case fastgltf::PrimitiveType::LineLoop:      return eLineLoops;
        case fastgltf::PrimitiveType::LineStrip:     return eLineStrips;
        case fastgltf::PrimitiveType::Triangles:     return eTriangles;
        case fastgltf::PrimitiveType::TriangleStrip: return eTriangleStrips;
        case fastgltf::PrimitiveType::TriangleFan:   return eTriangleFans;
    }
    std::unreachable();
}

[[nodiscard]]
static auto convert(fastgltf::Optional<std::size_t> optional) noexcept
    -> std::optional<uint32_t>
{
    if (!optional.has_value()) {
        return std::nullopt;
    }
    return static_cast<uint32_t>(optional.value());
}

[[nodiscard]]
static auto make_accessor_loader(
    const fastgltf::Asset&                  asset,
    std::vector<core::gltf::Model::Vertex>& vertices,
    size_t                                  first_vertex_index
)
{
    return [&, first_vertex_index]<typename Projection, typename Transformation>(
               const fastgltf::Accessor& accessor,
               Projection                project,
               Transformation            transform
           ) -> void {
        using ElementType = std::remove_cvref_t<
            std::tuple_element_t<0, core::meta::arguments_of_t<Transformation>>>;
        using AttributeType = std::remove_cvref_t<
            std::invoke_result_t<Projection, const core::gltf::Model::Vertex&>>;

        fastgltf::iterateAccessorWithIndex<ElementType>(
            asset,
            accessor,
            [&, first_vertex_index](const ElementType& element, const size_t index) {
                std::invoke(project, vertices[first_vertex_index + index]) =
                    std::invoke_r<AttributeType>(transform, element);
            }
        );
    };
}

[[nodiscard]]
static auto make_identity_accessor_loader(
    const fastgltf::Asset&                  asset,
    std::vector<core::gltf::Model::Vertex>& vertices,
    size_t                                  first_vertex_index
)
{
    return [&, first_vertex_index]<typename Projection>(
               const fastgltf::Accessor& accessor, Projection project
           ) -> void {
        using AttributeType = std::remove_cvref_t<
            std::invoke_result_t<Projection, const core::gltf::Model::Vertex&>>;

        fastgltf::iterateAccessorWithIndex<AttributeType>(
            asset,
            accessor,
            [&, first_vertex_index](const AttributeType& element, const size_t index) {
                std::invoke(project, vertices[first_vertex_index + index]) = element;
            }
        );
    };
}

namespace core::gltf {

auto Loader::load_from_file(const std::filesystem::path& filepath) -> std::optional<Model>
{
    fastgltf::Expected<fastgltf::Asset> asset{ ::load_asset(filepath) };
    if (asset.error() != fastgltf::Error::None) {
        SPDLOG_ERROR("Failed to load glTF: {}", fastgltf::to_underlying(asset.error()));
        return std::nullopt;
    }

    return load_model(filepath, asset.get(), asset->defaultScene.value_or(0));
}

auto Loader::load_from_file(
    const std::filesystem::path& filepath,
    const size_t                 scene_index
) -> std::optional<Model>
{
    fastgltf::Expected<fastgltf::Asset> asset{ ::load_asset(filepath) };
    if (asset.error() != fastgltf::Error::None) {
        SPDLOG_ERROR("Failed to load glTF: {}", fastgltf::to_underlying(asset.error()));
        return std::nullopt;
    }

    return load_model(filepath, asset.get(), scene_index);
}

auto Loader::load_model(
    const std::filesystem::path& filepath,
    const fastgltf::Asset&       asset,
    const size_t                 scene_index
) -> Model
{
    assert(scene_index < asset.scenes.size());

    Model model;

    load_nodes(model, asset, asset.scenes[scene_index].nodeIndices);
    load_images(model, asset, filepath);
    load_materials(model, asset);

    return model;
}

auto Loader::load_nodes(
    Model&                                              model,
    const fastgltf::Asset&                              asset,
    const fastgltf::pmr::MaybeSmallVector<std::size_t>& node_indices
) -> void
{
    model.m_root_node_indices.reserve(node_indices.size());
    model.m_nodes.reserve(asset.nodes.size());
    std::unordered_map<size_t, size_t> node_index_map;
    for (const auto node_index : node_indices) {
        model.m_root_node_indices.push_back(node_index);
        node_index_map.try_emplace(node_index, model.m_nodes.size());
        model.m_nodes = load_node(
            model,
            std::move(model.m_nodes),
            asset,
            asset.nodes[node_index],
            std::nullopt,
            node_index_map
        );
    }
    adjust_node_indices(model, node_index_map);
}

auto Loader::load_images(
    Model&                       model,
    const fastgltf::Asset&       asset,
    const std::filesystem::path& filepath
) -> void
{
    model.m_images.reserve(asset.images.size());
    for (const fastgltf::Image& image : asset.images) {
        if (std::optional<Image> loaded_image = ::load_image(filepath, asset, image);
            loaded_image.has_value())
        {
            model.m_images.push_back(std::move(loaded_image.value()));
        }
        else {
            throw std::runtime_error{ std::format(
                "Failed to load image {} from gltf image {}",
                image.name,
                filepath.generic_string()
            ) };
        }
    }

    model.m_samplers.reserve(asset.samplers.size());
    for (const fastgltf::Sampler& sampler : asset.samplers) {
        model.m_samplers.push_back(::create_sampler(sampler));
    }

    model.m_textures.reserve(asset.textures.size());
    for (const fastgltf::Texture& texture : asset.textures) {
        model.m_textures.push_back(::create_texture(texture));
    }
}

auto Loader::load_materials(Model& model, const fastgltf::Asset& asset) -> void
{
    model.m_materials.reserve(asset.materials.size());
    for (const fastgltf::Material& material : asset.materials) {
        model.m_materials.push_back(Material::create(material));
    }
}

auto Loader::load_node(
    Model&                              model,
    std::vector<Node>&&                 nodes,
    const fastgltf::Asset&              asset,
    const fastgltf::Node&               source_node,
    std::optional<size_t>               parent_index,
    std::unordered_map<size_t, size_t>& node_index_map
) -> std::vector<Node>
{
    size_t node_index{ nodes.size() };
    Node&  node{ nodes.emplace_back(
        parent_index,
        source_node.children,
        source_node.meshIndex.has_value()
             ? std::optional{ load_mesh(model, asset, asset.meshes[*source_node.meshIndex]) }
             : std::nullopt
    ) };

    fastgltf::math::fvec3 scale{ 1.f, 1.f, 1.f };
    fastgltf::math::fquat rotation{ 0.f, 0.f, 0.f, 1.f };
    fastgltf::math::fvec3 translation{};
    source_node.transform.visit(
        fastgltf::visitor{ [&](const fastgltf::TRS& transform) {
                              scale       = transform.scale;
                              rotation    = transform.rotation;
                              translation = transform.translation;
                          },
                           [&](const fastgltf::math::fmat4x4& matrix) {
                               fastgltf::math::decomposeTransformMatrix(
                                   matrix, scale, rotation, translation
                               );
                           } }
    );
    node.scale()       = glm::make_vec3(scale.data());
    node.rotation()    = glm::make_quat(rotation.data());
    node.translation() = glm::make_vec3(translation.data());

    for (const size_t child_index : source_node.children) {
        if (node_index_map.try_emplace(child_index, model.m_nodes.size()).second) {
            nodes = load_node(
                model,
                std::move(nodes),
                asset,
                asset.nodes[child_index],
                node_index,
                node_index_map
            );
        }
    }

    return nodes;
}

auto Loader::load_mesh(
    Model&                 model,
    const fastgltf::Asset& asset,
    const fastgltf::Mesh&  source_mesh
) -> size_t
{
    const size_t index = model.m_meshes.size();
    auto& [primitives]{ model.m_meshes.emplace_back() };

    primitives.reserve(source_mesh.primitives.size());
    for (const fastgltf::Primitive& source_primitive : source_mesh.primitives) {
        if (const std::optional<Mesh::Primitive> primitive{
                load_primitive(model, asset, source_primitive) };
            primitive.has_value())
        {
            primitives.push_back(primitive.value());
        }
    }

    return index;
}

auto Loader::load_primitive(
    Model&                     model,
    const fastgltf::Asset&     asset,
    const fastgltf::Primitive& source_primitive
) -> std::optional<Mesh::Primitive>
{
    Mesh::Primitive primitive{
        .mode           = ::convert(source_primitive.type),
        .material_index = ::convert(source_primitive.materialIndex),
    };

    const auto first_vertex_index{ model.m_vertices.size() };

    if (!load_vertices(model, primitive, asset, source_primitive.attributes)) {
        return std::nullopt;
    }

    if (auto indices_accessor_index{ source_primitive.indicesAccessor }) {
        load_indices(
            model,
            primitive,
            static_cast<uint32_t>(first_vertex_index),
            asset,
            asset.accessors[*indices_accessor_index]
        );
    }

    return primitive;
}

auto Loader::load_vertices(
    Model&                                                    model,
    Mesh::Primitive&                                          primitive,
    const fastgltf::Asset&                                    asset,
    const fastgltf::pmr::SmallVector<fastgltf::Attribute, 4>& attributes
) -> bool
{
    const auto* const position_iter{
        std::ranges::find(attributes, "POSITION", &fastgltf::Attribute::name)
    };
    if (position_iter == attributes.end()) {
        return false;
    }

    auto load_accessor{
        ::make_accessor_loader(asset, model.m_vertices, model.m_vertices.size())
    };
    auto load_identity_accessor{
        ::make_identity_accessor_loader(asset, model.m_vertices, model.m_vertices.size())
    };

    const fastgltf::Accessor& position_accessor{
        asset.accessors[position_iter->accessorIndex]
    };

    primitive.vertex_count = static_cast<uint32_t>(position_accessor.count);
    model.m_vertices.resize(model.m_vertices.size() + position_accessor.count);

    load_identity_accessor(position_accessor, &Model::Vertex::position);

    for (const auto& [name, accessor_index] : attributes) {
        if (name == "NORMAL") {
            load_identity_accessor(
                asset.accessors[accessor_index], &Model::Vertex::normal
            );
        }
        else if (name == "TANGENT") {
            load_identity_accessor(
                asset.accessors[accessor_index], &Model::Vertex::tangent
            );
        }
        else if (name == "TEXCOORD_0") {
            load_identity_accessor(asset.accessors[accessor_index], &Model::Vertex::uv_0);
        }
        else if (name == "TEXCOORD_1") {
            load_identity_accessor(asset.accessors[accessor_index], &Model::Vertex::uv_1);
        }
        else if (name == "COLOR_0") {
            if (asset.accessors[accessor_index].type == fastgltf::AccessorType::Vec4) {
                load_identity_accessor(
                    asset.accessors[accessor_index], &Model::Vertex::color
                );
            }
            else {
                load_accessor(
                    asset.accessors[accessor_index],
                    &Model::Vertex::color,
                    [](const glm::vec3& vec3) {
                        return glm::vec4{ vec3.x, vec3.y, vec3.z, 1 };
                    }
                );
            }
        }
    }

    return true;
}

auto Loader::load_indices(
    Model&                    model,
    Mesh::Primitive&          primitive,
    const uint32_t            first_vertex_index,
    const fastgltf::Asset&    asset,
    const fastgltf::Accessor& accessor
) -> void
{
    primitive.first_index_index = static_cast<uint32_t>(model.m_indices.size());
    primitive.index_count       = static_cast<uint32_t>(accessor.count);

    model.m_indices.reserve(model.m_indices.size() + accessor.count);

    fastgltf::iterateAccessor<uint32_t>(asset, accessor, [&](const uint32_t index) {
        model.m_indices.push_back(first_vertex_index + index);
    });
}

auto Loader::adjust_node_indices(
    Model&                                    model,
    const std::unordered_map<size_t, size_t>& node_index_map
) -> void
{
    for (size_t& root_node_index : model.m_root_node_indices) {
        root_node_index = node_index_map.at(root_node_index);
    }

    for (Node& node : model.m_nodes) {
        for (size_t& child_index : node.m_child_indices) {
            child_index = node_index_map.at(child_index);
        }
    }
}

}   // namespace core::gltf
