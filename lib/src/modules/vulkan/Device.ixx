export module ddge.modules.vulkan.Device;

import vulkan_hpp;

import ddge.modules.vulkan.QueueGroup;

namespace ddge::vulkan {

export struct Device {
    vk::raii::PhysicalDevice physical_device;
    vk::raii::Device         logical_device;
    QueueGroup               queues;
};

}   // namespace ddge::vulkan
