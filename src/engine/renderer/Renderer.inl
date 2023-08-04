#include <thread>

#include "engine/utility/vulkan/helpers.hpp"

namespace engine {

//////////////////////////////////!
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

auto Renderer::create(
    CreateInfo                    t_context,
    renderer::SurfaceCreator auto t_create_surface,
    vk::Extent2D                  t_framebuffer_size
) noexcept -> std::optional<Renderer>
{
    auto instance{ vulkan::Instance::create()
                       .set_engine_name(t_context.engine_name)
                       .set_engine_version(t_context.engine_version)
                       .set_application_name(t_context.app_name)
                       .set_application_version(t_context.app_version)
                       .set_api_version(VK_API_VERSION_1_0)
                       .add_layers(vulkan::validation_layers())
                       .add_extensions(vulkan::instance_extensions())
                       .build() };
    if (!instance.has_value()) {
        return std::nullopt;
    }

    std::optional<vulkan::DebugUtilsMessenger> debug_messenger{
        vulkan::create_debug_messenger(**instance)
            .transform([instance =
                            **instance](vk::DebugUtilsMessengerEXT messenger) {
                return vulkan::DebugUtilsMessenger{ instance, messenger };
            })
    };

    std::optional<vulkan::Surface> surface{
        t_create_surface(**instance, nullptr)
            .transform([instance = **instance](vk::SurfaceKHR surface) {
                return vulkan::Surface{ instance, surface };
            })
    };
    if (!surface.has_value()) {
        return std::nullopt;
    }

    auto render_device{ renderer::RenderDevice::create(
        std::move(*instance), std::move(debug_messenger), **surface
    ) };
    if (!render_device.has_value()) {
        return std::nullopt;
    }

    std::vector<vulkan::CommandPool> command_pools;
    for (size_t i{}; i < std::thread::hardware_concurrency(); i++) {
        if (auto command_pool{ vulkan::CommandPool::create(
                **render_device,
                vk::CommandPoolCreateFlagBits::eTransient,
                render_device->graphics_queue_family_index()
            ) })
        {
            command_pools.push_back(std::move(*command_pool));
        }
    }
    if (command_pools.empty()) {
        return std::nullopt;
    }

    Renderer renderer{ std::move(*render_device),
                       std::move(*surface),
                       std::move(command_pools) };
    renderer.set_framebuffer_size(t_framebuffer_size);

    return renderer;
}

}   // namespace engine
