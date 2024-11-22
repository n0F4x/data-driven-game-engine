module;

#include <vulkan/vulkan.hpp>

export module core.renderer.base.resources.MemoryView;

namespace core::renderer::base {

export struct MemoryView {
    vk::DeviceMemory memory;
    vk::DeviceSize   offset{};
    vk::DeviceSize   size{};
};

}   // namespace core::renderer::base
