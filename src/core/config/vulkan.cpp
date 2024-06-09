#include "vulkan.hpp"

#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace core::config::vulkan {

auto init() VULKAN_HPP_NOEXCEPT -> void
{
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
#endif
}

auto init([[maybe_unused]] VULKAN_HPP_NAMESPACE::Instance t_instance
) VULKAN_HPP_NOEXCEPT -> void
{
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
    VULKAN_HPP_DEFAULT_DISPATCHER.init(t_instance);
#endif
}

auto init([[maybe_unused]] VULKAN_HPP_NAMESPACE::Device t_device
) VULKAN_HPP_NOEXCEPT -> void
{
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
    VULKAN_HPP_DEFAULT_DISPATCHER.init(t_device);
#endif
}

auto instance_proc_address() noexcept -> PFN_vkGetInstanceProcAddr
{
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
    return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
#else
    return &vkGetInstanceProcAddr;
#endif
}

auto device_proc_address() noexcept -> PFN_vkGetDeviceProcAddr
{
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
    return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;
#else
    return &vkGetDeviceProcAddr;
#endif
}

[[nodiscard]]
auto dispatcher() -> const vk::DispatchLoaderDynamic& {
    return VULKAN_HPP_DEFAULT_DISPATCHER;
}

}   // namespace core::config::vulkan
