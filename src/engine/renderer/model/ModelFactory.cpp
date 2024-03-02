#include "ModelFactory.hpp"

#include <ranges>

#include <spdlog/spdlog.h>

#include <fastgltf/core.hpp>

#include "ModelLoader.hpp"

[[nodiscard]] static auto load_asset(const std::filesystem::path& t_filepath) noexcept
    -> fastgltf::Expected<fastgltf::Asset>
{
    fastgltf::Parser parser;

    constexpr auto gltf_options{ fastgltf::Options::DontRequireValidAssetMember
                                 | fastgltf::Options::LoadGLBBuffers
                                 | fastgltf::Options::LoadExternalBuffers
                                 | fastgltf::Options::LoadExternalImages };

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(t_filepath);

    return parser.loadGltf(&data, t_filepath.parent_path(), gltf_options);
}

namespace engine::renderer {

auto ModelFactory::load_gltf(
    const std::filesystem::path& t_filepath,
    const renderer::Allocator&   t_allocator
) noexcept -> tl::optional<StagingModel>
{
    auto asset{ load_asset(t_filepath) };
    if (asset.error() != fastgltf::Error::None) {
        SPDLOG_ERROR("Failed to load glTF: {}", fastgltf::to_underlying(asset.error()));
        return tl::nullopt;
    }

    ModelLoader model;
    model.load(asset.get());

    return StagingMeshBuffer::create<StagingModel::Vertex>(
               t_allocator, model.vertices, model.indices
    )
        .transform([&](StagingMeshBuffer&& t_staging_mesh_buffer) {
            return StagingModel{ std::move(t_staging_mesh_buffer),
                                 std::move(model.nodes) };
        });
}

}   // namespace engine::renderer
