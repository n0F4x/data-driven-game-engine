#pragma once

#include "ImageInfo.hpp"

namespace engine::renderer {

struct ModelInfo {
    entt::id_type              hash;
    std::vector<entt::id_type> cached_images;
    std::vector<ImageInfo>     images;
};

}   // namespace engine::renderer
