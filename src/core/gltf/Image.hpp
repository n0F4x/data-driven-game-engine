#pragma once

#include <memory>

#include <gsl-lite/gsl-lite.hpp>

#include "core/image/Image.hpp"

namespace core::gltf {

using Image = gsl_lite::not_null_ic<std::unique_ptr<image::Image>>;

}   // namespace core::gltf
