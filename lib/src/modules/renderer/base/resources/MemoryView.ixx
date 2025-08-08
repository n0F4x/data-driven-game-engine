module;

export module modules.renderer.base.resources.MemoryView;

import vulkan_hpp;

namespace modules::renderer::base {

export struct MemoryView {
    vk::DeviceMemory memory;
    vk::DeviceSize   offset{};
    vk::DeviceSize   size{};
};

}   // namespace modules::renderer::base
