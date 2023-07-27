#pragma once

#include <optional>
#include <thread>
#include <type_traits>

#include <vulkan/vulkan.hpp>

#include "engine/utility/vulkan/CommandPool.hpp"
#include "engine/utility/vulkan/Device.hpp"
#include "engine/utility/vulkan/FrameBuffer.hpp"
#include "engine/utility/vulkan/Instance.hpp"
#include "engine/utility/vulkan/SwapChain.hpp"

namespace engine {

namespace renderer {

template <typename Func>
concept SurfaceCreator = std::is_nothrow_invocable_r_v<
    std::optional<vk::SurfaceKHR>,
    Func,
    vk::Instance,
    vk::Optional<const vk::AllocationCallbacks>>;

}   // namespace renderer

class RenderDevice {
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit RenderDevice(
        vulkan::Instance&& t_instance,
        vk::PhysicalDevice t_physical_device,
        uint32_t           t_graphics_family_index,
        uint32_t           t_present_family_index,
        vulkan::Device&&   t_device
    ) noexcept;

public:
    RenderDevice(RenderDevice&&) noexcept = default;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(RenderDevice&&) noexcept -> RenderDevice& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Device;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Device*;

    ///-----------///
    ///  Methods  ///
    ///-----------///

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        vulkan::Instance&& t_instance,
        vk::SurfaceKHR     t_surface
    ) noexcept -> std::optional<RenderDevice>;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vulkan::Instance   m_instance;
    vk::PhysicalDevice m_physical_device;
    uint32_t           m_graphics_family_index;
    uint32_t           m_present_family_index;
    vulkan::Device     m_device;
};

}   // namespace engine
