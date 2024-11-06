#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include <core/renderer/resources/Image.hpp>

namespace examples::base::init {

[[nodiscard]]
auto create_command_pool(vk::Device device, uint32_t queue_family_index)
    -> vk::UniqueCommandPool;

[[nodiscard]]
auto create_command_buffers(vk::Device device, vk::CommandPool command_pool, uint32_t count)
    -> std::vector<vk::CommandBuffer>;

[[nodiscard]]
auto create_semaphores(vk::Device device, uint32_t count)
    -> std::vector<vk::UniqueSemaphore>;

[[nodiscard]]
auto create_fences(vk::Device device, uint32_t count) -> std::vector<vk::UniqueFence>;

}   // namespace init
