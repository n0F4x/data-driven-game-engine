namespace engine {

///////////////////////////////////
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

template <renderer::SurfaceCreator CreateSurfaceCallback>
Renderer::Renderer(
    const vk::ApplicationInfo& t_app_info,
    CreateSurfaceCallback&&    t_surface_creator
)
    : m_device{
          t_app_info,
          t_surface_creator
},
      m_command_pool{
          m_device.device(),
          vk::CommandPoolCreateInfo{
              .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
              .queueFamilyIndex = m_device.graphics_queue_family()
          }
      },
      m_command_buffers{
          m_device.device(),
          vk::CommandBufferAllocateInfo{
              .commandPool = *m_command_pool,
              .level       = vk::CommandBufferLevel::ePrimary,
              .commandBufferCount = s_MAX_FRAMES_IN_FLIGHT
          }
      }
{}

template <renderer::SurfaceCreator CreateSurfaceCallback>
Renderer::Renderer(
    const vk::ApplicationInfo& t_app_info,
    CreateSurfaceCallback&&    t_surface_creator,
    const vk::Extent2D&        t_frame_buffer_size
)
    : Renderer{ t_app_info, t_surface_creator }
{
    m_swap_chain = SwapChain::create(m_device, t_frame_buffer_size);
}

}   // namespace engine
