#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <SFML/Window/Vulkan.hpp>

#include "Device.hpp"
#include "engine/core/concepts.hpp"
#include "engine/core/vulkan.hpp"
#include "SwapChain.hpp"

namespace engine {

class Renderer {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Device    = renderer::Device;
    using SwapChain = renderer::SwapChain;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    template <utils::Invocable_R<vk::raii::SurfaceKHR,
                                 const vk::raii::Instance&,
                                 vk::Optional<const vk::AllocationCallbacks>>
                  CreateSurfaceCallback>
    explicit Renderer(const vk::ApplicationInfo& t_app_info,
                      CreateSurfaceCallback&&    t_surface_creator);

private:
    template <typename CreateSurfaceCallback>
    explicit Renderer(vk::raii::Instance&&    t_instance,
                      CreateSurfaceCallback&& t_surface_creator);
    explicit Renderer(vk::raii::SurfaceKHR&& t_surface,
                      vk::raii::Instance&&   t_instance);

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    void begin_frame();
    void end_frame();

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Device    m_device;
    SwapChain m_swap_chain;
};

///////////////////////////////////
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////
template <utils::Invocable_R<vk::raii::SurfaceKHR,
                             const vk::raii::Instance&,
                             vk::Optional<const vk::AllocationCallbacks>>
              CreateSurfaceCallback>
Renderer::Renderer(const vk::ApplicationInfo& t_app_info,
                   CreateSurfaceCallback&&    t_surface_creator)
    : Renderer{ utils::create_instance(
                    t_app_info,
                    utils::create_validation_layers(),
                    std::span{
                        sf::Vulkan::getGraphicsRequiredInstanceExtensions() }),
                t_surface_creator }
{
}

template <typename CreateSurfaceCallback>
Renderer::Renderer(vk::raii::Instance&&    t_instance,
                   CreateSurfaceCallback&& t_surface_creator)
    : Renderer{ t_surface_creator(t_instance, nullptr), std::move(t_instance) }
{
}

}   // namespace engine
