module;

export module ddge.deprecated.renderer.base.resources.MemoryView;

import vulkan_hpp;

namespace ddge::renderer::base {

export struct MemoryView {
    vk::DeviceMemory memory;
    vk::DeviceSize   offset{};
    vk::DeviceSize   size{};
};

}   // namespace ddge::renderer::base
