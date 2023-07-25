#pragma once

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class FrameBuffer {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit FrameBuffer(
        vk::Instance   t_instance,
        vk::Framebuffer t_frame_buffer
    ) noexcept;
    FrameBuffer(FrameBuffer&&) noexcept;
    ~FrameBuffer() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(FrameBuffer&&) noexcept -> FrameBuffer& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Framebuffer;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Framebuffer*;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::Instance   m_instance;
    vk::Framebuffer m_frame_buffer;
};

}   // namespace engine::vulkan

