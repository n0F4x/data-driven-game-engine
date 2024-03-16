#pragma once

#include <vulkan/vulkan.hpp>

#include "core/renderer/base/Allocator.hpp"
#include "core/renderer/base/Device.hpp"
#include "core/renderer/base/Swapchain.hpp"

namespace core::renderer {

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

}   // namespace core::renderer
