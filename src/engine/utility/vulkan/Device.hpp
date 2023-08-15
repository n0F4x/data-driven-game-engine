#pragma once

#include <expected>
#include <span>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class Device {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Device(Device&&) noexcept;
    ~Device() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Device&&) noexcept -> Device& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Device;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Device*;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Builder;

    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Device m_device;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Device(vk::Device t_device) noexcept;
};

class Device::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build(const void* t_next) noexcept
        -> std::expected<Device, vk::Result>;

    auto set_physical_device(vk::PhysicalDevice t_physical_device) noexcept
        -> Builder&;
    auto add_queue_create_infos(
        std::span<const vk::DeviceQueueCreateInfo> t_queue_create_info
    ) noexcept -> Builder&;
    auto add_enabled_layers(std::span<const char* const> t_enabled_layers
    ) noexcept -> Builder&;
    auto add_enabled_extensions(
        std::span<const char* const> t_enabled_extensions
    ) noexcept -> Builder&;
    auto set_enabled_features(vk::PhysicalDeviceFeatures t_enabled_features
    ) noexcept -> Builder&;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Device;

    ///*************///
    ///  Variables  ///
    ///*************///
    vk::PhysicalDevice                     m_physical_device{ nullptr };
    std::vector<vk::DeviceQueueCreateInfo> m_queue_create_infos;
    std::vector<const char*>               m_enabled_layers;
    std::vector<const char*>               m_enabled_extensions;
    vk::PhysicalDeviceFeatures             m_enabled_features{};

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    Builder() noexcept = default;
};

}   // namespace engine::vulkan
