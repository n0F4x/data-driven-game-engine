#pragma once

#include "vulkan/vulkan.hpp"

namespace engine::renderer::vulkan {

class Device {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Device(vk::Device t_device) noexcept;
    Device(Device&&) noexcept;
    ~Device() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Device&&) noexcept -> Device& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Device;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Device*;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Device m_device;
};

}   // namespace engine::renderer::vulkan
