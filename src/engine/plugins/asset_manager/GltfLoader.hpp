#pragma once

#include <string>

#include <tl/optional.hpp>

#include <tiny_gltf.h>

namespace engine::asset_manager {

enum class GltfFormat {
    eAscii,
    eBinary
};

class GltfLoader {
public:
    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto load_model(
        GltfFormat         t_format,
        const std::string& t_filepath
    ) noexcept -> tl::optional<tinygltf::Model>;
};

}   // namespace engine::asset_manager
