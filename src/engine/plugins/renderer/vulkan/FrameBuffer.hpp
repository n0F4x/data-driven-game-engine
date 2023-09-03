#pragma once

#include <vulkan/vulkan.hpp>

namespace engine::renderer::vulkan {

class FrameBuffer {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit FrameBuffer(
        vk::Device      t_device,
        vk::Framebuffer t_frame_buffer
    ) noexcept;
    FrameBuffer(FrameBuffer&&) noexcept;
    ~FrameBuffer() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(FrameBuffer&&) noexcept -> FrameBuffer& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Framebuffer;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Device      m_device;
    vk::Framebuffer m_frame_buffer;
};

}   // namespace engine::renderer::vulkan
