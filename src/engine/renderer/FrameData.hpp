#pragma once

#include <limits>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include "engine/utility/vulkan/CommandPool.hpp"
#include "engine/utility/vulkan/Fence.hpp"

namespace engine::renderer {

struct CommandNode {
    vk::CommandBuffer command_buffer;
    size_t            work_load{};
};

using CommandHandle = size_t;

struct CommandNodeInfo {
    size_t worker_id;
    size_t index;
};

struct FrameData {
    std::vector<vulkan::CommandPool>                   command_pools;
    std::vector<std::vector<CommandNode>>              command_buffers;
    std::unordered_map<CommandHandle, CommandNodeInfo> command_map;
    CommandHandle                                      m_next_command_handle{};
    vulkan::Fence                                      fence;
};

}   // namespace engine::renderer
