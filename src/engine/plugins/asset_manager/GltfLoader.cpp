#include "GltfLoader.hpp"

#include <spdlog/spdlog.h>

namespace engine::asset_manager {

auto GltfLoader::load_model(
    GltfFormat         t_format,
    const std::string& t_filepath
) noexcept -> tl::optional<tinygltf::Model>
{
    tinygltf::Model model;

    std::string warning;
    std::string error;
    bool        success{};

    switch (t_format) {
        case GltfFormat::eAscii:
            success = tinygltf::TinyGLTF{}.LoadASCIIFromFile(
                &model, &error, &warning, t_filepath
            );
            break;
        case GltfFormat::eBinary:
            success = tinygltf::TinyGLTF{}.LoadBinaryFromFile(
                &model, &error, &warning, t_filepath
            );
            break;
    }

    if (!success) {
        if (!warning.empty()) {
            SPDLOG_WARN(warning);
        }
        if (!error.empty()) {
            SPDLOG_ERROR(error);
        }
        return tl::nullopt;
    }

    return model;
}

}   // namespace engine::asset_manager
