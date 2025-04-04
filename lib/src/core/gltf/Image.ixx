module;

#include <memory>

#include <gsl/gsl-lite.hpp>

export module core.gltf.Image;

import core.image.Image;

namespace core::gltf {

export using Image = gsl_lite::not_null_ic<std::unique_ptr<image::Image>>;

}   // namespace core::gltf
