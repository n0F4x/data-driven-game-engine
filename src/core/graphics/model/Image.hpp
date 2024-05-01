#pragma once

#include <variant>

#include "core/asset/image/KtxImage.hpp"
#include "core/asset/image/StbImage.hpp"

namespace core::graphics {

using Image = std::variant<asset::StbImage, asset::KtxImage>;

}   // namespace core::graphics
