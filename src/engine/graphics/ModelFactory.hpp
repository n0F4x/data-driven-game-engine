#pragma once

#include <tiny_gltf.h>

#include "Model.hpp"

namespace engine::gfx {

class ModelFactory {
public:
    [[nodiscard]] static auto create_model(const tinygltf::Model& t_source
    ) noexcept -> Model;
};

}   // namespace engine::gfx
