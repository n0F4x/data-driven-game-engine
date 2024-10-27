#pragma once

#include <vulkan/vulkan.hpp>

namespace core::renderer {

namespace base {

class Device;

}   // namespace base

class Executor {
public:
    explicit Executor(const base::Device& device);

    template <std::invocable<vk::CommandBuffer> Recorder>
    auto execute_single_command(Recorder&& recorder) const
        -> std::invoke_result_t<Recorder, vk::CommandBuffer>;

private:
    vk::Device            m_device;
    vk::Queue             m_graphics_queue;
    vk::UniqueCommandPool m_command_pool;

    [[nodiscard]]
    static auto start_recording(vk::Device device, vk::CommandPool command_pool)
        -> vk::CommandBuffer;

    auto finish_recording(vk::CommandBuffer command_buffer) const -> void;
};

}   // namespace core::renderer

#include "Executor.inl"
