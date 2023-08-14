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
    vk::Extent2D                  t_framebuffer_size,
    unsigned                      t_hardware_concurrency
) noexcept -> std::optional<Renderer>
{
    auto instance{ vulkan::Instance::create()
                       .set_engine_name(t_context.engine_name)
                       .set_engine_version(t_context.engine_version)
                       .set_application_name(t_context.app_name)
                       .set_application_version(t_context.app_version)
                       .set_api_version(renderer::RenderDevice::s_API_VERSION)
                       .add_layers(vulkan::validation_layers())
                       .add_extensions(vulkan::instance_extensions())
                       .build() };
    if (!instance.has_value()) {
        return std::nullopt;
    }

    std::optional<vulkan::Surface> surface{
        t_create_surface(**instance, nullptr)
            .transform([instance = **instance](vk::SurfaceKHR surface) {
                return vulkan::Surface{ instance, surface };
            })
    };
    if (!surface.has_value()) {
        return std::nullopt;
    }

    return create(
        std::move(*instance),
        std::move(*surface),
        t_framebuffer_size,
        t_hardware_concurrency
    );
}

}   // namespace engine
