#include "App.hpp"

#include <SFML/Window/Vulkan.hpp>

#include "engine/utility/vulkan/helpers.hpp"

namespace {

auto create_surface_creator(engine::App::Window& t_window) noexcept
{
    return [&t_window](
               vk::Instance                                t_instance,
               vk::Optional<const vk::AllocationCallbacks> t_allocator
           ) noexcept -> std::optional<vk::SurfaceKHR> {
        return t_window.createVulkanSurface(
            t_instance, &t_allocator->operator const VkAllocationCallbacks&()
        );
    };
}

}   // namespace

namespace engine {

//////////////////////////////
///----------------------- ///
///  App   IMPLEMENTATION  ///
///------------------------///
//////////////////////////////
App::App(
    RenderDevice&&    t_render_device,
    vulkan::Surface&& t_surface,
    Window&&          t_window
) noexcept
    : m_render_device{ std::move(t_render_device) },
      m_surface{ std::move(t_surface) },
      m_window{ std::move(t_window) }
{}

auto App::create() noexcept -> App::Builder
{
    return Builder{};
}

///////////////////////////////////////
///---------------------------------///
///  App::Builder   IMPLEMENTATION  ///
///---------------------------------///
///////////////////////////////////////
auto App::Builder::build() && noexcept -> std::optional<App>
{
    if (!m_window.has_value()) {
        m_window = Window::create().build();
    }

    if (!m_window.has_value()) {
        return std::nullopt;
    }

    auto instance{
        vulkan::Instance::create()
            .set_api_version(VK_API_VERSION_1_0)
            .add_layers(vulkan::validation_layers())
            .add_extensions(sf::Vulkan::getGraphicsRequiredInstanceExtensions())
            .build()
    };
    if (!instance.has_value()) {
        return std::nullopt;
    }

    auto raw_surface{ create_surface_creator(*m_window)(**instance, nullptr) };
    if (!raw_surface.has_value()) {
        return std::nullopt;
    }
    vulkan::Surface surface{ **instance, *raw_surface };

    auto renderer{ RenderDevice::create(std::move(*instance), *raw_surface) };
    if (!renderer.has_value()) {
        return std::nullopt;
    }

    return App{ std::move(*renderer),
                std::move(surface),
                std::move(*m_window) };
}

auto App::Builder::set_window(std::optional<Window> t_window) && noexcept
    -> Builder
{
    m_window = std::move(t_window);
    return std::move(*this);
}

}   // namespace engine
