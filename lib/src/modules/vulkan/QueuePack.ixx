module;

#include <cstdint>

export module ddge.modules.vulkan.QueuePack;

import vulkan_hpp;

import ddge.modules.vulkan.QueueFamilyIndex;

namespace ddge::vulkan {

export struct QueuePack {
    QueueFamilyIndex family_index;
    uint32_t         queue_index;
    vk::raii::Queue  queue;
};

}   // namespace ddge::vulkan
