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

struct FrameData {
    std::vector<vulkan::CommandPool>                   command_pools;
    std::vector<std::vector<CommandNode>>              command_buffers;
    vulkan::Fence                                      fence;
};

}   // namespace engine::renderer
