#include "ModelLoader.hpp"

#include <ranges>

#include <spdlog/spdlog.h>

#include <fastgltf/core.hpp>

#include "GltfLoader.hpp"

[[nodiscard]]
static auto load_asset(const std::filesystem::path& t_filepath
) noexcept -> fastgltf::Expected<fastgltf::Asset>
{
    fastgltf::Parser parser;

    constexpr auto gltf_options{ fastgltf::Options::LoadGLBBuffers
                                 | fastgltf::Options::LoadExternalBuffers
                                 | fastgltf::Options::GenerateMeshIndices
                                 | fastgltf::Options::DecomposeNodeMatrices };

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(t_filepath);

    return parser.loadGltf(&data, t_filepath.parent_path(), gltf_options);
}

namespace core::renderer {

auto ModelLoader::load_from_file(
    const std::filesystem::path& t_filepath,
    const Allocator&             t_allocator
) -> tl::optional<StagingModel>
{
    auto asset{ load_asset(t_filepath) };
    if (asset.error() != fastgltf::Error::None) {
        SPDLOG_ERROR("Failed to load glTF: {}", fastgltf::to_underlying(asset.error()));
        return tl::nullopt;
    }

    GltfLoader model;
    model.load(asset.get());

    return StagingModel{ StagingMeshBuffer::create<graphics::Model::Vertex>(
                             t_allocator, model.vertices, model.indices
                         ),
                         std::move(model.nodes) };
}

}   // namespace core::renderer
