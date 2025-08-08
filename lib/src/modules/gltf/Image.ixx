module;

#include <memory>

#include <gsl-lite/gsl-lite.hpp>

export module modules.gltf.Image;

import modules.image.Image;

namespace modules::gltf {

export using Image = gsl_lite::not_null_ic<std::unique_ptr<image::Image>>;

}   // namespace modules::gltf
