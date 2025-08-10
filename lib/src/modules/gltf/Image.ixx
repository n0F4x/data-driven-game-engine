module;

#include <memory>

#include <gsl-lite/gsl-lite.hpp>

export module ddge.modules.gltf.Image;

import ddge.modules.image.Image;

namespace ddge::gltf {

export using Image = gsl_lite::not_null_ic<std::unique_ptr<image::Image>>;

}   // namespace ddge::gltf
