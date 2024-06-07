#pragma once

#include <memory>

#include "core/image/Image.hpp"

namespace core::gltf {

using Image = std::unique_ptr<image::Image>;

}   // namespace core::gltf
