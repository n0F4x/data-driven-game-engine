module;

#include <ranges>

#include <vulkan/vulkan.hpp>

module examples.base.init;

import modules.renderer.base.device.Device;

auto examples::base::init::create_command_pool(const vk::Device device, const uint32_t queue_family_index)
    -> vk::UniqueCommandPool
{
    const vk::CommandPoolCreateInfo command_pool_create_info{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = queue_family_index
    };

    return device.createCommandPoolUnique(command_pool_create_info);
}

auto examples::base::init::create_command_buffers(
    const vk::Device      device,
    const vk::CommandPool command_pool,
    const uint32_t        count
) -> std::vector<vk::CommandBuffer>
{
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = count
    };

    return device.allocateCommandBuffers(command_buffer_allocate_info);
}

auto examples::base::init::create_semaphores(const vk::Device device, const uint32_t count)
    -> std::vector<vk::UniqueSemaphore>
{
    return std::views::repeat(device, count)
         | std::views::transform([](const vk::Device t_device) {
               constexpr static vk::SemaphoreCreateInfo create_info{};
               return t_device.createSemaphoreUnique(create_info);
           })
         | std::ranges::to<std::vector>();
}

auto examples::base::init::create_fences(const vk::Device device, const uint32_t count)
    -> std::vector<vk::UniqueFence>
{
    return std::views::repeat(device, count)
         | std::views::transform([](const vk::Device t_device) {
               constexpr static vk::FenceCreateInfo create_info{
                   .flags = vk::FenceCreateFlagBits::eSignaled
               };
               return t_device.createFenceUnique(create_info);
           })
         | std::ranges::to<std::vector>();
}
