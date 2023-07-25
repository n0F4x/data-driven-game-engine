#pragma once

#include <expected>
#include <vector>
#include <span>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class Device {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend Builder;

private:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Device(vk::Device t_device) noexcept;

public:
    Device(Device&&) noexcept;
    ~Device() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Device&&) noexcept -> Device& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Device;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Device*;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::Device m_device;
};

class Device::Builder {
    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend Device;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Builder() noexcept = default;

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() noexcept -> std::expected<Device, vk::Result>;

    auto set_physical_device(vk::PhysicalDevice t_physical_device) noexcept
        -> Builder&;
    auto add_queue_create_info(vk::DeviceQueueCreateInfo t_queue_create_info
    ) noexcept -> Builder&;
    auto add_queue_create_infos(
        std::span<const vk::DeviceQueueCreateInfo> t_queue_create_info
    ) noexcept -> Builder&;
    auto add_enabled_layer(const char* t_enabled_layer) noexcept -> Builder&;
    auto add_enabled_layers(std::span<const char* const> t_enabled_layers
    ) noexcept -> Builder&;
    auto add_enabled_extension(const char* t_enabled_extension) noexcept
        -> Builder&;
    auto add_enabled_extensions(std::span<const char* const> t_enabled_extensions
    ) noexcept -> Builder&;
    auto set_enabled_features(vk::PhysicalDeviceFeatures t_enabled_features
    ) noexcept -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::PhysicalDevice                     m_physical_device{ nullptr };
    std::vector<vk::DeviceQueueCreateInfo> m_queue_create_infos;
    std::vector<const char*>               m_enabled_layers;
    std::vector<const char*>               m_enabled_extensions;
    vk::PhysicalDeviceFeatures             m_enabled_features{};
};

}   // namespace engine::vulkan
