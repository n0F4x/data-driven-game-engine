#include "vulkan.hpp"

#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace core::config::vulkan {

auto init() -> void
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
}

auto init(vk::Instance instance) noexcept -> void
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
}

auto init(vk::Device device) noexcept -> void
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
}

auto dispatcher() -> const vk::DispatchLoaderDynamic&
{
    return VULKAN_HPP_DEFAULT_DISPATCHER;
}

}   // namespace core::config::vulkan
