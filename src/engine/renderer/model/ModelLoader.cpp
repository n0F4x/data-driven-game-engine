#include "ModelLoader.hpp"

#include <ranges>

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include <fastgltf/core.hpp>

#include "GltfLoader.hpp"

[[nodiscard]] static auto load_asset(const std::filesystem::path& t_filepath) noexcept
    -> fastgltf::Expected<fastgltf::Asset>
{
    fastgltf::Parser parser;

    constexpr auto gltf_options{ fastgltf::Options::DontRequireValidAssetMember
                                 | fastgltf::Options::LoadGLBBuffers
                                 | fastgltf::Options::LoadExternalBuffers
                                 | fastgltf::Options::GenerateMeshIndices };

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(t_filepath);

    return parser.loadGltf(&data, t_filepath.parent_path(), gltf_options);
}

namespace engine::renderer {

auto ModelLoader::hash(const std::filesystem::path& t_filepath) -> entt::id_type
{
    std::string absolute_path{ std::filesystem::absolute(t_filepath).generic_string() };
    return entt::hashed_string{ absolute_path.c_str(), absolute_path.size() };
}

ModelLoader::ModelLoader(ResourceManager& t_resource_manager) noexcept
    : m_resource_manager{ t_resource_manager }
{}

auto ModelLoader::load_from_file(
    const std::filesystem::path& t_filepath,
    const renderer::Allocator&   t_allocator
) noexcept -> tl::optional<StagingModel>
{
    auto asset{ load_asset(t_filepath) };
    if (asset.error() != fastgltf::Error::None) {
        SPDLOG_ERROR("Failed to load glTF: {}", fastgltf::to_underlying(asset.error()));
        return tl::nullopt;
    }

    GltfLoader model;
    model.load(asset.get());

    return StagingMeshBuffer::create<Vertex>(t_allocator, model.vertices, model.indices)
        .transform([&](StagingMeshBuffer&& t_staging_mesh_buffer) {
            return StagingModel{ std::move(t_staging_mesh_buffer),
                                 std::move(model.nodes) };
        });
}

}   // namespace engine::renderer
