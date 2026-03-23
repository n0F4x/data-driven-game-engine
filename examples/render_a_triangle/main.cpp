#include <cstdint>
#include <filesystem>
#include <print>
#include <source_location>

import vulkan_hpp;

import ddge.modules.app;
import ddge.modules.renderer;
import ddge.modules.resources;
import ddge.modules.vulkan;
import ddge.modules.wsi;
import ddge.util.containers.StringLiteral;

using namespace ddge;

constexpr static util::StringLiteral application_name{ "render_a_triangle" };
constexpr static uint32_t            application_version{ 0 };

[[nodiscard]]
constexpr auto meta_info() -> app::extensions::MetaInfo
{
    return app::extensions::MetaInfo{
        application_name,
        application_version,
    };
}

using App = app::App<app::MetaInfoAddon, resources::Addon>;

[[nodiscard]]
auto inject_window(const wsi::Context& wsi_context, const renderer::Context& render_context)
    -> wsi::VulkanWindow;

struct Demo {
    vk::raii::PipelineLayout pipeline_layout;
    vulkan::ShaderModule     shader_module;
    vk::raii::Pipeline       pipeline;

    explicit Demo(const vulkan::Device& device, const wsi::VulkanWindow& window);
};

[[nodiscard]]
auto inject_demo(const renderer::Context& render_context, const wsi::VulkanWindow& window)
    -> Demo;

auto run(App app) -> void;

auto main() -> int
try {
    app::create()
        .plug_in(app::extensions::MetaInfoPlugin{ meta_info() })
        .plug_in(resources::Plugin{})
        .plug_in(renderer::Plugin{})
        .plug_in(app::extensions::RunnablePlugin{})
        .add_render_context(&renderer::ContextInjection::request_default_debug_messenger)
        .inject_resource(inject_window)
        .inject_resource(inject_demo)
        .run(run);

} catch (const app::BuildFailedError& error) {
    std::println("{}", error.what());
}

auto inject_window(
    const wsi::Context&      wsi_context,
    const renderer::Context& render_context
) -> wsi::VulkanWindow
{
    constexpr wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    constexpr wsi::Window::CreateInfo window_info{
        .title    = "Hello Window!",
        .settings = screen_settings,
    };

    return wsi::VulkanWindow{
        wsi::Window{ auto{ wsi_context }, window_info },
        render_context.instance,
        render_context.device,
        1
    };
}

Demo::Demo(const vulkan::Device& device, const wsi::VulkanWindow& window)
    : pipeline_layout{
          vulkan::check_result(device.logical_device.createPipelineLayout({}
          ))
},
      shader_module{
          *vulkan::ShaderModule::load_from_file(
              std::filesystem::path{ std::source_location::current().file_name() }
                  .parent_path()
              / "shaders" / "triangle.spv"
          )   //
      },
      pipeline{
          vulkan::GraphicsPipelineBuilder{ shader_module,
                                           shader_module,
                                           pipeline_layout,
                                           window.surface_format().format }
              .build(device)   //
      }
{}

auto inject_demo(const renderer::Context& render_context, const wsi::VulkanWindow& window)
    -> Demo
{
    return Demo{ render_context.device, window };
}

auto run(App app) -> void
{
    const wsi::Context& wsi_context{ app.resource_manager.at<wsi::Context>() };
    wsi::VulkanWindow&  window{ app.resource_manager.at<wsi::VulkanWindow>() };

    while (!window.should_close()) {
        wsi::poll_events(wsi_context);

        if (window.key_pressed(wsi::Key::eEscape)) {
            window.request_close();
        }
    }
}
