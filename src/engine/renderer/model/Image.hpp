#pragma once

#include <variant>

#include "engine/asset/image/KtxImage.hpp"
#include "engine/asset/image/StbImage.hpp"

namespace engine::renderer {

using Image = std::variant<asset::StbImage, asset::KtxImage>;

}   // namespace engine::renderer
