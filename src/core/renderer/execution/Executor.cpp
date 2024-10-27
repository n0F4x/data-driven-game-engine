#include "Executor.hpp"

#include "core/renderer/base/device/Device.hpp"

[[nodiscard]]
static auto create_command_pool(const vk::Device device, const uint32_t queue_family_index)
    -> vk::UniqueCommandPool
{
    const vk::CommandPoolCreateInfo command_pool_create_info{
        .flags            = vk::CommandPoolCreateFlagBits::eTransient,
        .queueFamilyIndex = queue_family_index
    };

    return device.createCommandPoolUnique(command_pool_create_info);
}

core::renderer::Executor::Executor(const base::Device& device)
    : m_device{ device.get() },
      m_graphics_queue{ device.info().get_queue(vkb::QueueType::graphics).value() },
      m_command_pool{ ::create_command_pool(
          m_device,
          device.info().get_queue_index(vkb::QueueType::graphics).value()
      ) }
{}

auto core::renderer::Executor::start_recording(
    const vk::Device      device,
    const vk::CommandPool command_pool
) -> vk::CommandBuffer
{
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };

    const vk::CommandBuffer command_buffer{
        device.allocateCommandBuffers(command_buffer_allocate_info).front()
    };

    constexpr static vk::CommandBufferBeginInfo begin_info{
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };
    command_buffer.begin(begin_info);

    return command_buffer;
}

auto core::renderer::Executor::finish_recording(const vk::CommandBuffer command_buffer
) const -> void
{
    command_buffer.end();

    const vk::SubmitInfo submit_info{
        .commandBufferCount = 1,
        .pCommandBuffers    = &command_buffer,
    };
    const vk::UniqueFence fence{ m_device.createFenceUnique({}) };
    m_graphics_queue.submit(submit_info, fence.get());
    std::ignore = m_device.waitForFences(fence.get(), vk::True, 100'000'000'000);

    m_device.resetCommandPool(m_command_pool.get());
}
