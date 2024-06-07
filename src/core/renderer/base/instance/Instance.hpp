#pragma once

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

namespace core::renderer {

class Instance {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Instance(const vkb::Instance& t_instance) noexcept;
    Instance(const Instance&)     = delete;
    Instance(Instance&&) noexcept = default;
    ~Instance();

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(const Instance&) -> Instance&     = delete;
    auto operator=(Instance&&) noexcept -> Instance& = default;
    [[nodiscard]]
    auto operator*() const noexcept -> vk::Instance;
    [[nodiscard]]
    auto operator->() const noexcept -> const vk::Instance*;
    [[nodiscard]]
    explicit operator vkb::Instance() const noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto get() const noexcept -> vk::Instance;
    [[nodiscard]]
    auto debug_messenger() const noexcept -> vk::DebugUtilsMessengerEXT;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vkb::Instance m_instance;
};

}   // namespace core::renderer
