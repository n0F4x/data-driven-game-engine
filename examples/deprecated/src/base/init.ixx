module;

#include <vector>

#include <vulkan/vulkan.hpp>

export module examples.base.init;

import ddge.modules.renderer.resources.Image;

namespace examples::base::init {

export [[nodiscard]]
auto create_command_pool(vk::Device device, uint32_t queue_family_index)
    -> vk::UniqueCommandPool;

export [[nodiscard]]
auto create_command_buffers(vk::Device device, vk::CommandPool command_pool, uint32_t count)
    -> std::vector<vk::CommandBuffer>;

export [[nodiscard]]
auto create_semaphores(vk::Device device, uint32_t count)
    -> std::vector<vk::UniqueSemaphore>;

export [[nodiscard]]
auto create_fences(vk::Device device, uint32_t count) -> std::vector<vk::UniqueFence>;

}   // namespace examples::base::init
