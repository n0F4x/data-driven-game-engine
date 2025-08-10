module;

#include <VkBootstrap.h>

export module ddge.modules.renderer.base.device.Device;

import vulkan_hpp;

namespace ddge::renderer::base {

export class Device {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Device(const vkb::Device& device);
    explicit Device(vkb::Device&& device) noexcept;
    Device(const Device&) = delete;
    Device(Device&&) noexcept;
    ~Device();

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(const Device&)                = delete;
    auto operator=(Device&&) noexcept -> Device& = default;
    [[nodiscard]]
    auto operator*() const noexcept -> vk::Device;
    [[nodiscard]]
    auto operator->() const noexcept -> const vk::Device*;
    [[nodiscard]]
    explicit operator vkb::Device() const;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto get() const noexcept -> vk::Device;
    [[nodiscard]]
    auto physical_device() const noexcept -> vk::PhysicalDevice;
    [[nodiscard]]
    auto info() const -> vkb::Device;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vkb::Device m_device;
};

}   // namespace ddge::renderer::base
