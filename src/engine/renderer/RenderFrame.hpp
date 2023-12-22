#pragma once

#include <functional>

#include <vulkan/vulkan.hpp>

#include "Device.hpp"

namespace engine::renderer {

class RenderFrame {
public:
    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        Device&  t_device,
        unsigned t_thread_count,
        unsigned t_frame_count = 2
    ) noexcept -> tl::optional<RenderFrame>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    RenderFrame(RenderFrame&&) noexcept = default;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto reset(vk::Device t_device) noexcept -> vk::Result;

    [[nodiscard]] auto request_command_buffer(
        vk::Device             t_device,
        vk::CommandBufferLevel t_level     = vk::CommandBufferLevel::ePrimary,
        unsigned               t_thread_id = 0
    ) -> vk::CommandBuffer;

private:
    ///******************///
    ///  Nested classes  ///
    ///******************///
    struct ThreadData {
        vk::UniqueCommandPool          command_pool;
        std::vector<vk::CommandBuffer> command_buffers;
        size_t                         requested_command_buffers{};
    };

    struct FrameData {
        std::vector<ThreadData>            thread_data;
        vk::UniqueFence                    fence;
        std::vector<std::function<void()>> pre_updates;
    };

    ///*************///
    ///  Variables  ///
    ///*************///
    std::vector<FrameData> m_frame_data;
    size_t                 m_frame_index;

    ///****************///
    /// Static methods ///
    ///****************///
    [[nodiscard]] static auto create_frame_data(
        Device&  t_device,
        unsigned t_thread_count,
        unsigned t_frame_count
    ) -> std::vector<FrameData>;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit RenderFrame(std::vector<FrameData>&& t_frame_data);

    ///***********///
    ///  Methods  ///
    ///***********///
    auto current_frame() noexcept -> FrameData&;
};

}   // namespace engine::renderer
