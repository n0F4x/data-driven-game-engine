module;

export module core.renderer.base.resources.MemoryView;

import vulkan_hpp;

namespace core::renderer::base {

export struct MemoryView {
    vk::DeviceMemory memory;
    vk::DeviceSize   offset{};
    vk::DeviceSize   size{};
};

}   // namespace core::renderer::base
