#pragma once

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

namespace core::renderer::helpers {

struct QueueInfos {
    uint32_t                               graphics_family;
    uint32_t                               graphics_index;
    uint32_t                               compute_family;
    uint32_t                               compute_index;
    uint32_t                               transfer_family;
    uint32_t                               transfer_index;
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
};

[[nodiscard]] auto
    find_queue_families(vk::PhysicalDevice queue_family_index, vk::SurfaceKHR t_surface)
        -> tl::optional<QueueInfos>;

}   // namespace core::renderer::helpers
