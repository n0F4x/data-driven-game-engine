#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class CommandPool {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit CommandPool(
        vk::Device      t_device,
        vk::CommandPool t_command_pool
    ) noexcept;
    CommandPool(CommandPool&&) noexcept;
    ~CommandPool() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(CommandPool&&) noexcept -> CommandPool& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::CommandPool;
    [[nodiscard]] auto operator->() const noexcept -> const vk::CommandPool*;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        vk::Device                 t_device,
        vk::CommandPoolCreateFlags t_flags,
        uint32_t                   t_queue_family_index
    ) noexcept -> std::optional<CommandPool>;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::Device      m_device;
    vk::CommandPool m_command_pool;
};

}   // namespace engine::vulkan
