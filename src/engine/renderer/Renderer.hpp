#pragma once

#include <optional>
#include <string_view>
#include <type_traits>

#include <vulkan/vulkan.hpp>

#include "engine/utility/Result.hpp"
#include "engine/utility/vulkan/CommandPool.hpp"
#include "engine/utility/vulkan/FrameBuffer.hpp"
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

private:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Renderer(
        renderer::RenderDevice&& t_render_device,
        vulkan::Surface&&        t_surface
    ) noexcept;

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept
        -> Result;

    void begin_frame() noexcept;
    void end_frame() noexcept;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        CreateInfo                    t_context,
        renderer::SurfaceCreator auto t_create_surface,
        vk::Extent2D                  t_framebuffer_size
    ) noexcept -> std::optional<Renderer>;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    renderer::RenderDevice           m_render_device;
    vulkan::Surface                  m_surface;
    std::optional<vulkan::SwapChain> m_swap_chain;
    bool                             m_in_frame{ false };
};

}   // namespace engine

#include "Renderer.inl"
