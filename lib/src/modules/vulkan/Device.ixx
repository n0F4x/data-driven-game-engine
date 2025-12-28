module;

#include <vector>

export module ddge.modules.vulkan.Device;

import vulkan_hpp;

import ddge.modules.vulkan.QueueGroup;
import ddge.utility.containers.StringLiteral;

namespace ddge::vulkan {

export struct Device {
    vk::raii::PhysicalDevice         physical_device;
    vk::raii::Device                 logical_device;
    QueueGroup                       queues;
    std::vector<util::StringLiteral> enabled_extension_names;
};

}   // namespace ddge::vulkan
