#pragma once

#include <optional>

#include <vulkan/vulkan_raii.hpp>

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
    template <renderer::SurfaceCreator CreateSurfaceCallback>
    explicit Renderer(
        const vk::ApplicationInfo& t_app_info,
        CreateSurfaceCallback&&    t_surface_creator
    );

    template <renderer::SurfaceCreator CreateSurfaceCallback>
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
    Device                   m_device;
    std::optional<SwapChain> m_swap_chain;
    vk::raii::CommandPool    m_command_pool;
    vk::raii::CommandBuffers m_command_buffers;

    ///--------------------///
    ///  Static Variables  ///
    ///--------------------///
    constexpr static uint32_t s_MAX_FRAMES_IN_FLIGHT = 2;
};

}   // namespace engine

#include "Renderer.inl"
