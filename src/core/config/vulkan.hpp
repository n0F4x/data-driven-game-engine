#pragma once

#include <vulkan/vulkan.hpp>

namespace core::config::vulkan {

auto init() VULKAN_HPP_NOEXCEPT -> void;
auto init(VULKAN_HPP_NAMESPACE::Instance t_instance) VULKAN_HPP_NOEXCEPT -> void;
auto init(VULKAN_HPP_NAMESPACE::Device t_device) VULKAN_HPP_NOEXCEPT -> void;

[[nodiscard]]
auto instance_proc_address() noexcept -> PFN_vkGetInstanceProcAddr;
[[nodiscard]]
auto device_proc_address() noexcept -> PFN_vkGetDeviceProcAddr;

[[nodiscard]]
auto dispatcher() -> const vk::DispatchLoaderDynamic&;

}   // namespace core::config::vulkan
