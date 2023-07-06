#pragma once

#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include <SFML/Window/Vulkan.hpp>

#include "engine/core/concepts.hpp"
#include "engine/core/vulkan.hpp"

namespace engine::renderer {

template <typename SurfaceCreatorFunction>
concept SurfaceCreator = utils::Invocable_R<
    SurfaceCreatorFunction,
    vk::raii::SurfaceKHR,
    const vk::raii::Instance&,
    vk::Optional<const vk::AllocationCallbacks>>;

class Device {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    template <SurfaceCreator CreateSurfaceCallback>
    explicit Device(
        const vk::ApplicationInfo& t_app_info,
        CreateSurfaceCallback&&    t_surface_creator
    );

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto surface() const noexcept -> const vk::raii::SurfaceKHR&;
    [[nodiscard]] auto physical_device() const noexcept
        -> const vk::raii::PhysicalDevice&;
    [[nodiscard]] auto device() const noexcept -> const vk::raii::Device&;

    [[nodiscard]] auto graphics_queue_family() const noexcept -> uint32_t;
    [[nodiscard]] auto present_queue_family() const noexcept -> uint32_t;

    [[nodiscard]] auto graphics_queue() const noexcept
        -> const vk::raii::Queue&;
    [[nodiscard]] auto present_queue() const noexcept -> const vk::raii::Queue&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::raii::Instance       m_instance;
    vk::raii::SurfaceKHR     m_surface;
    vk::raii::PhysicalDevice m_physical_device;
    vk::raii::Device         m_device;
    uint32_t                 m_graphics_queue_family;
    uint32_t                 m_present_queue_family;
    vk::raii::Queue          m_graphics_queue;
    vk::raii::Queue          m_present_queue;
};

}   // namespace engine::renderer

#include "Device.inl"
