#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "engine/utility/Result.hpp"
#include "engine/utility/vulkan/CommandPool.hpp"
#include "engine/utility/vulkan/Fence.hpp"
#include "engine/utility/vulkan/Surface.hpp"
#include "engine/utility/vulkan/SwapChain.hpp"

#include "RenderDevice.hpp"

namespace engine {

namespace renderer {

template <typename Func>
concept SurfaceCreator = std::is_nothrow_invocable_r_v<
    std::optional<vk::SurfaceKHR>,
    Func,
    vk::Instance,
    vk::Optional<const vk::AllocationCallbacks>>;

}   // namespace renderer

class Renderer {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct CreateInfo {
        std::string_view engine_name;
        uint32_t         engine_version{};
        std::string_view app_name;
        uint32_t         app_version{};
    };

    ///---------------------///
    ///  Static  Variables  ///
    ///---------------------///
    constexpr static uint32_t s_MAX_FRAMES_IN_FLIGHT = 2;

private:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Renderer(
        renderer::RenderDevice&& t_render_device,
        vulkan::Surface&&        t_surface,
        std::array<std::vector<vulkan::CommandPool>, s_MAX_FRAMES_IN_FLIGHT>&&
                                     t_command_pools,
        std::vector<vulkan::Fence>&& t_fences
    ) noexcept;

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto command_pools_per_frame() const noexcept -> size_t;

    auto set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept -> void;

    auto begin_frame() noexcept -> Result;
    auto end_frame() noexcept -> void;

    auto wait_idle() noexcept -> void;

private:
    auto recreate_swap_chain(vk::Extent2D t_framebuffer_size) noexcept -> void;

public:
    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        CreateInfo                    t_context,
        renderer::SurfaceCreator auto t_create_surface,
        vk::Extent2D                  t_framebuffer_size
    ) noexcept -> std::optional<Renderer>;

private:
    [[nodiscard]] static auto create(
        vulkan::Instance&& t_instance,
        vulkan::Surface&&  t_surface,
        vk::Extent2D       t_framebuffer_size
    ) noexcept -> std::optional<Renderer>;

    ///-------------///
    ///  Variables  ///
    ///-------------///
    bool                             m_in_frame{ false };
    renderer::RenderDevice           m_render_device;
    vulkan::Surface                  m_surface;
    std::optional<vulkan::SwapChain> m_swap_chain;

    uint32_t m_frame_index{};
    std::array<std::vector<vulkan::CommandPool>, s_MAX_FRAMES_IN_FLIGHT>
                               m_command_pools;
    std::vector<vulkan::Fence> m_fences;
};

}   // namespace engine

#include "Renderer.inl"
