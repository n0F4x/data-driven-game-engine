module;

#include <functional>
#include <optional>
#include <span>
#include <vector>

export module ddge.deprecated.renderer.base.swapchain.SwapchainHolder;

import vulkan_hpp;

import ddge.deprecated.renderer.base.swapchain.Swapchain;

import ddge.deprecated.renderer.base.device.Device;

namespace ddge::renderer::base {

export class SwapchainHolder {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using FramebufferSizeGetter   = std::function<vk::Extent2D()>;
    using SwapchainRecreatedEvent = std::function<void(const Swapchain&)>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit SwapchainHolder(
        vk::SurfaceKHR          surface,
        const Device&           device,
        FramebufferSizeGetter&& get_framebuffer_size = {}
    );

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto surface() const noexcept -> vk::SurfaceKHR;
    [[nodiscard]]
    auto get() const noexcept -> const std::optional<Swapchain>&;

    auto set_framebuffer_size(vk::Extent2D framebuffer_size) -> void;

    [[nodiscard]]
    auto acquire_next_image(vk::Semaphore semaphore = {}, vk::Fence fence = {})
        -> std::optional<uint32_t>;

    auto present(std::span<const vk::Semaphore> wait_semaphores = {}) -> void;
    auto present(vk::Semaphore wait_semaphore) -> void;

    auto on_swapchain_recreated(SwapchainRecreatedEvent&& swapchain_recreated_event)
        -> uint32_t;

    auto remove_swapchain_recreated_event(uint32_t id) noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::SurfaceKHR                       m_surface;
    std::reference_wrapper<const Device> m_device;
    FramebufferSizeGetter                m_get_framebuffer_size;
    std::optional<Swapchain>             m_swapchain;
    uint32_t                             m_image_index{};
    std::vector<std::pair<uint32_t, SwapchainRecreatedEvent>> m_swapchain_recreated_events;
    uint32_t m_swapchain_recreated_events_counter{};

    ///***********///
    ///  Methods  ///
    ///***********///
    auto recreate_swapchain(vk::Extent2D framebuffer_size) -> void;
    auto recreate_swapchain() -> void;
};

}   // namespace ddge::renderer::base
