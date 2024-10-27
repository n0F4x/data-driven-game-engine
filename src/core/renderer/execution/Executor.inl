#pragma once

template <std::invocable<vk::CommandBuffer> Recorder>
auto core::renderer::Executor::execute_single_command(Recorder&& recorder) const
    -> std::invoke_result_t<Recorder, vk::CommandBuffer>
{
    const vk::CommandBuffer command_buffer{
        start_recording(m_device, m_command_pool.get())
    };

    if constexpr (std::is_void_v<std::invoke_result_t<Recorder, vk::CommandBuffer>>) {
        std::invoke(std::forward<Recorder>(recorder), command_buffer);
        finish_recording(command_buffer);
        return;
    }
    else {
        decltype(auto
        ) result{ std::invoke(std::forward<Recorder>(recorder), command_buffer) };
        finish_recording(command_buffer);
        return result;
    }
}
