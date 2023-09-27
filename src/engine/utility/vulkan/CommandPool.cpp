#include "CommandPool.hpp"

namespace engine::utils::vulkan {

//////////////////////////////////////
///--------------------------------///
///  CommandPool   IMPLEMENTATION  ///
///--------------------------------///
//////////////////////////////////////

CommandPool::CommandPool(
    vk::Device      t_device,
    vk::CommandPool t_command_pool
) noexcept
    : m_device{ t_device },
      m_command_pool{ t_command_pool }
{}

CommandPool::CommandPool(CommandPool&& t_other) noexcept
    : m_device{ t_other.m_device },
      m_command_pool{ t_other.m_command_pool }
{
    t_other.m_command_pool = nullptr;
}

CommandPool::~CommandPool() noexcept
{
    if (m_command_pool) {
        m_device.destroy(m_command_pool);
    }
}

auto CommandPool::operator*() const noexcept -> vk::CommandPool
{
    return m_command_pool;
}

auto CommandPool::create(
    vk::Device                 t_device,
    vk::CommandPoolCreateFlags t_flags,
    uint32_t                   t_queue_family_index
) noexcept -> std::optional<CommandPool>
{
    auto [result, command_pool]{
        t_device.createCommandPool(vk::CommandPoolCreateInfo{
            .flags = t_flags, .queueFamilyIndex = t_queue_family_index })
    };

    if (result != vk::Result::eSuccess) {
        return std::nullopt;
    }

    return CommandPool{ t_device, command_pool };
}

}   // namespace engine::utils::vulkan
