#pragma once

#include <concepts>
#include <expected>
#include <span>
#include <string_view>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace engine::renderer::vulkan {

class Instance {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Instance(
        vk::Instance               t_instance,
        vk::DebugUtilsMessengerEXT t_debug_messenger = nullptr
    ) noexcept;
    Instance(Instance&&) noexcept;
    ~Instance() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Instance&&) noexcept -> Instance& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Instance;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Instance*;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Instance               m_instance;
    vk::DebugUtilsMessengerEXT m_debug_messenger;
};

}   // namespace engine::renderer::vulkan
