#include "Renderer.hpp"

#include "plugins/renderer/DefaultSurfaceProvider.hpp"

using namespace renderer;

namespace plugins {

auto Renderer::operator()(app::App::Builder& t_builder) const -> void
{
    const auto window{ t_builder.store().find<core::window::Window>() };
    if (!window.has_value()) {
        SPDLOG_WARN(
            "Default window could not be found in store. "
            "Consider using another surface provider than the default."
        );
        return;
    }

    operator()(
        t_builder,
        renderer::Options{
            VK_API_VERSION_1_0, renderer::DefaultSurfaceProvider{ window.value() }, {} }
    );
}

}   // namespace plugins
