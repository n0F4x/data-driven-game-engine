#pragma once

#include <algorithm>
#include <atomic>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <thread>

#include <vulkan/vulkan.hpp>

#include "engine/utility/vulkan/raii_wrappers.hpp"

#include "Device.hpp"
#include "RenderFrame.hpp"
#include "Swapchain.hpp"

namespace engine::renderer {

template <typename Func>
concept CreateSurfaceConcept = std::
    is_nothrow_invocable_r_v<std::optional<vk::SurfaceKHR>, Func, vk::Instance>;

template <typename Func>
concept CreateDeviceConcept = std::is_nothrow_invocable_r_v<
    std::optional<renderer::Device>,
    Func,
    vk::Instance,
    vk::SurfaceKHR,
    vk::PhysicalDevice>;

class Renderer {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct Config {
        std::function<std::optional<vk::DebugUtilsMessengerEXT>(vk::Instance)>
            create_debug_messenger;
        std::function<bool(vk::PhysicalDevice, vk::SurfaceKHR)>
            filter_physical_device;
        std::function<size_t(vk::PhysicalDevice, vk::SurfaceKHR)>
                 rank_physical_device{ Renderer::rank_physical_device };
        unsigned thread_count{
            std::max(std::jthread::hardware_concurrency(), 1u)
        };
        unsigned frame_count{ 2 };
    };

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        vk::Instance                     t_instance,
        const CreateSurfaceConcept auto& t_create_surface,
        vk::Extent2D                     t_framebuffer_size,
        const CreateDeviceConcept auto&  t_create_device,
        Config&                          t_config
    ) noexcept -> std::optional<Renderer>;

    [[nodiscard]] static auto create_default(
        const CreateSurfaceConcept auto& t_create_surface,
        vk::Extent2D                     t_framebuffer_size
    ) noexcept -> std::optional<Renderer>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept -> void;

private:
    ///********************///
    ///  Static Variables  ///
    ///********************///
    [[nodiscard]] static auto rank_physical_device(
        vk::PhysicalDevice,
        vk::SurfaceKHR
    ) noexcept -> size_t;

    ///*************///
    ///  Variables  ///
    ///*************///
    utils::vulkan::Instance                    m_instance;
    std::unique_ptr<std::atomic<vk::Extent2D>> m_framebuffer_size;
    std::vector<vk::PhysicalDevice>            m_adequate_devices;
    Device                                     m_device;
    Swapchain                                  m_swapchain;
    RenderFrame                                m_render_frame;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    Renderer(
        vk::Instance                      t_instance,
        vk::DebugUtilsMessengerEXT        t_debug_messenger,
        vk::SurfaceKHR                    t_surface,
        vk::Extent2D                      t_framebuffer_size,
        std::vector<vk::PhysicalDevice>&& t_adequate_devices,
        Device&&                          t_device,
        RenderFrame&&                     t_render_frame
    );
};

}   // namespace engine::renderer

#include "Renderer.inl"
