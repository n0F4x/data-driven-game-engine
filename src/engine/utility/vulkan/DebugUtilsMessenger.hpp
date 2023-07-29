#pragma once

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class DebugUtilsMessenger {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit DebugUtilsMessenger(
        vk::Instance               t_instance,
        vk::DebugUtilsMessengerEXT t_messenger
    ) noexcept;
    DebugUtilsMessenger(DebugUtilsMessenger&&) noexcept;
    ~DebugUtilsMessenger() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(DebugUtilsMessenger&&) noexcept
        -> DebugUtilsMessenger& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::DebugUtilsMessengerEXT;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::Instance               m_instance;
    vk::DebugUtilsMessengerEXT m_messenger;
};

}   // namespace engine::vulkan
