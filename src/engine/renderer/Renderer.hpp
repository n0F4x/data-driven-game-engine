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

class Renderer {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Instance    = vulkan::Instance;
    using Device      = vulkan::Device;
    using CommandPool = vulkan::CommandPool;
    using SwapChain   = vulkan::SwapChain;

private:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    //    template <renderer::SurfaceCreator CreateSurfaceCallback>
    //    explicit Renderer(
    //        vulkan::Device&& t_device,
    //        CreateSurfaceCallback&&    t_surface_creator
    //    );

public:
    Renderer(Renderer&&) noexcept = default;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(Renderer&&) noexcept -> Renderer& = default;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void begin_frame();
    void end_frame();

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        Instance&&                    t_instance,
        renderer::SurfaceCreator auto t_surface_creator
    ) noexcept -> std::optional<Renderer>;

private:
    [[nodiscard]] static auto create(
        Instance&&           t_instance,
        SwapChain::Surface&& t_surface
    ) noexcept -> std::optional<Renderer>;


    ///-------------///
    ///  Variables  ///
    ///-------------///
    Instance                 m_instance;
    Device                   m_device;
    CommandPool              m_command_pool;
    std::optional<SwapChain> m_swap_chain;

public:
    ///--------------------///
    ///  Static Variables  ///
    ///--------------------///
    constexpr static uint32_t s_MAX_FRAMES_IN_FLIGHT{ 2 };
    static const uint32_t MAX_THREADS;
};

}   // namespace engine

#include "Renderer.inl"
