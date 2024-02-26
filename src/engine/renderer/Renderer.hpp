#pragma once

#include <vulkan/vulkan.hpp>

#include "engine/renderer/base/Allocator.hpp"
#include "engine/renderer/base/Device.hpp"
#include "engine/renderer/base/Swapchain.hpp"

namespace engine::renderer {

class Renderer {
public:
    explicit Renderer(
        const Instance&    t_instance,
        vk::SurfaceKHR     t_surface,
        vk::PhysicalDevice t_physical_device
    );

private:
    Device    m_device;
    Allocator m_allocator;
    Swapchain m_swapchain;
};

}   // namespace engine::renderer
