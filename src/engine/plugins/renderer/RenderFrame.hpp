#pragma once

#include <expected>
#include <functional>
#include <limits>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include "engine/utility/vulkan/raii_wrappers.hpp"

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
    ) noexcept -> std::optional<RenderFrame>;

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
    ) noexcept -> std::expected<vk::CommandBuffer, vk::Result>;

private:
    ///******************///
    ///  Nested classes  ///
    ///******************///
    struct ThreadData {
        utils::vulkan::CommandPool     command_pool;
        std::vector<vk::CommandBuffer> command_buffers;
        size_t                         requested_command_buffers{};
    };

    struct FrameData {
        std::vector<ThreadData>            thread_data;
        utils::vulkan::Fence               fence;
        std::vector<std::function<void()>> pre_updates;
    };

    ///*************///
    ///  Variables  ///
    ///*************///
    std::vector<FrameData> m_frame_data;
    size_t                 m_frame_index;

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
