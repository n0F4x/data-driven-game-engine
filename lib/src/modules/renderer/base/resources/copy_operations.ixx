module;

#include <functional>

export module modules.renderer.base.resources.copy_operations;

import vulkan_hpp;

import modules.renderer.base.resources.Allocation;

namespace modules::renderer::base {

// NOLINTNEXTLINE(*-member-init)
export struct CopyRegion {
    std::reference_wrapper<const Allocation> allocation;
    vk::DeviceSize                           offset{};
};

export auto copy(const void* source, const CopyRegion& destination, vk::DeviceSize size)
    -> void;

export auto copy(const CopyRegion& source, void* destination, vk::DeviceSize size)
    -> void;

}   // namespace modules::renderer::base
