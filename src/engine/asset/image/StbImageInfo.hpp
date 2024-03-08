#pragma once

#include <glm/glm.hpp>

namespace engine::asset {

enum class StbChannelTypes {
    eGrey,
    eGreyAlpha,
    eRGB,
    eRGBA
};

struct StbImageInfo {
    glm::vec3 extent;
    StbChannelTypes channel_type;
};

}
