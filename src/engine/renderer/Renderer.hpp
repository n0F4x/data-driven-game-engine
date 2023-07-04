#pragma once

#include <optional>

#include <vulkan/vulkan_raii.hpp>

#include <SFML/Window/Vulkan.hpp>

#include "engine/core/concepts.hpp"
#include "engine/core/vulkan.hpp"

#include "Device.hpp"
#include "SwapChain.hpp"

namespace engine {

class Renderer {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Device    = renderer::Device;
    using SwapChain = renderer::SwapChain;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    template <utils::Invocable_R<
        vk::raii::SurfaceKHR,
        const vk::raii::Instance&,
        vk::Optional<const vk::AllocationCallbacks>> CreateSurfaceCallback>
    explicit Renderer(
        const vk::ApplicationInfo& t_app_info,
        CreateSurfaceCallback&&    t_surface_creator
    );

    template <utils::Invocable_R<
        vk::raii::SurfaceKHR,
        const vk::raii::Instance&,
        vk::Optional<const vk::AllocationCallbacks>> CreateSurfaceCallback>
    explicit Renderer(
        const vk::ApplicationInfo& t_app_info,
        CreateSurfaceCallback&&    t_surface_creator,
        const vk::Extent2D&        t_frame_buffer_size
    );

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void begin_frame();
    void end_frame();

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::raii::Instance       m_instance;
    vk::raii::SurfaceKHR     m_surface;
    Device                   m_device;
    std::optional<SwapChain> m_swap_chain;
    vk::raii::CommandPool    m_command_pool;
    vk::raii::CommandBuffers m_command_buffers;

    ///--------------------///
    ///  Static Variables  ///
    ///--------------------///
    constexpr static uint32_t s_MAX_FRAMES_IN_FLIGHT = 2;
};

///////////////////////////////////
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////
template <utils::Invocable_R<
    vk::raii::SurfaceKHR,
    const vk::raii::Instance&,
    vk::Optional<const vk::AllocationCallbacks>> CreateSurfaceCallback>
Renderer::Renderer(
    const vk::ApplicationInfo& t_app_info,
    CreateSurfaceCallback&&    t_surface_creator
)
    : m_instance{ utils::create_instance(
              t_app_info,
              utils::create_validation_layers(),
              std::span{
                        sf::Vulkan::getGraphicsRequiredInstanceExtensions() }
          ) },
      m_surface(t_surface_creator(m_instance, nullptr)),
      m_device{
          m_instance,
          m_surface
},
      m_command_pool{
          m_device.device(),
          vk::CommandPoolCreateInfo{
              .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
              .queueFamilyIndex = m_device.graphics_queue_family() }
      },
      m_command_buffers{ m_device.device(),
                         vk::CommandBufferAllocateInfo{
                             .commandPool = *m_command_pool,
                             .level       = vk::CommandBufferLevel::ePrimary,
                             .commandBufferCount = s_MAX_FRAMES_IN_FLIGHT } }
{}

template <utils::Invocable_R<
    vk::raii::SurfaceKHR,
    const vk::raii::Instance&,
    vk::Optional<const vk::AllocationCallbacks>> CreateSurfaceCallback>
Renderer::Renderer(
    const vk::ApplicationInfo& t_app_info,
    CreateSurfaceCallback&&    t_surface_creator,
    const vk::Extent2D&        t_frame_buffer_size
)
    : Renderer{ t_app_info, t_surface_creator }
{
    m_swap_chain = SwapChain::create(m_surface, t_frame_buffer_size, m_device);
}

}   // namespace engine
