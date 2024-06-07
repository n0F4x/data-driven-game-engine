#pragma once

#include <memory>

#include "core/asset/image/Image.hpp"

namespace core::gltf {

using Image = std::unique_ptr<asset::Image>;

}   // namespace core::gltf
