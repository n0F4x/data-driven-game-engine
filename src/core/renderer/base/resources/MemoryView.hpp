#pragma once

#include <vulkan/vulkan.hpp>

namespace core::renderer::base {

struct MemoryView {
    vk::DeviceMemory memory;
    vk::DeviceSize   offset{};
    vk::DeviceSize   size{};
};

}   // namespace core::renderer::base
