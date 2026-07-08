module;

#include <vulkan/vulkan_hpp_macros.hpp>

module ddge.deprecated.config.vulkan;

import vulkan_hpp;

namespace ddge::config::vulkan {

auto init() -> void
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
}

auto init(const vk::Instance instance) noexcept -> void
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
}

auto init(const vk::Device device) noexcept -> void
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
}

auto dispatcher() noexcept -> const vk::DispatchLoaderDynamic&
{
    return VULKAN_HPP_DEFAULT_DISPATCHER;
}

}   // namespace ddge::config::vulkan
