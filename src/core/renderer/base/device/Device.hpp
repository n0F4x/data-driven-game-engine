#pragma once

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

namespace core::renderer {

class Device {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Device(const vkb::Device& t_device) noexcept;
    explicit Device(vkb::Device&& t_device) noexcept;
    Device(Device&&) noexcept = default;
    ~Device();

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto                   operator=(Device&&) noexcept -> Device& = default;
    [[nodiscard]] auto     operator*() const noexcept -> vk::Device;
    [[nodiscard]] auto     operator->() const noexcept -> const vk::Device*;
    [[nodiscard]] explicit operator vkb::Device() const noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto get() const noexcept -> vk::Device;
    [[nodiscard]] auto physical_device() const noexcept -> vk::PhysicalDevice;
    [[nodiscard]] auto info() const noexcept -> vkb::Device;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vkb::Device m_device;
};

}   // namespace core::renderer
