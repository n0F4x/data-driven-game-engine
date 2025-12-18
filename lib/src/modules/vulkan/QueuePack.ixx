module;

#include <cstdint>

export module ddge.modules.vulkan.QueuePack;

import vulkan_hpp;

namespace ddge::vulkan {

export struct QueuePack {
    uint32_t        family_index;
    uint32_t        queue_index;
    vk::raii::Queue queue;
};

}   // namespace ddge::vulkan
