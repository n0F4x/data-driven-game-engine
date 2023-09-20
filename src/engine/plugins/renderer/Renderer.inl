#include <ranges>

#include "engine/plugins/renderer/vulkan/helpers.hpp"

namespace engine::renderer {

auto Renderer::create(
    vk::Instance                     t_instance,
    const CreateSurfaceConcept auto& t_create_surface,
    vk::Extent2D                     t_framebuffer_size,
    const CreateDeviceConcept auto&  t_create_device,
    Config&                          t_config
) noexcept -> std::optional<Renderer>
{
    if (!t_instance) {
        return std::nullopt;
    }

    auto surface{ t_create_surface(t_instance) };
    if (!surface.has_value()) {
        return std::nullopt;
    }

    std::optional<vk::DebugUtilsMessengerEXT> debug_messenger;
    if (t_config.create_debug_messenger) {
        debug_messenger = t_config.create_debug_messenger(t_instance);
    }

    auto [result, physical_devices]{ t_instance.enumeratePhysicalDevices() };
    if (result != vk::Result::eSuccess) {
        return std::nullopt;
    }
    auto view{
        physical_devices
        | std::views::filter([&](vk::PhysicalDevice t_physical_device) {
              return renderer::Device::adequate(t_physical_device, *surface)
                  && (!t_config.filter_physical_device
                      || t_config.filter_physical_device(
                          t_physical_device, *surface
                      ));
          })
    };
    std::vector<vk::PhysicalDevice> adequate_devices{ view.begin(),
                                                      view.end() };
    if (adequate_devices.empty()) {
        return std::nullopt;
    }

    auto physical_device{ adequate_devices.front() };
    if (t_config.rank_physical_device) {
        auto highest_rank{
            t_config.rank_physical_device(physical_device, *surface)
        };
        for (auto adequate_device : adequate_devices) {
            if (auto rank{
                    t_config.rank_physical_device(adequate_device, *surface) };
                rank > highest_rank)
            {
                physical_device = adequate_device;
                highest_rank    = rank;
            }
        }
    }

    auto device{ t_create_device(t_instance, *surface, physical_device) };
    if (!device.has_value()) {
        return std::nullopt;
    }

    auto render_frame{ RenderFrame::create(
        *device, t_config.thread_count, t_config.frame_count
    ) };
    if (!render_frame.has_value()) {
        return std::nullopt;
    }

    return Renderer{ t_instance,
                     debug_messenger ? *debug_messenger : nullptr,
                     *surface,
                     t_framebuffer_size,
                     std::move(adequate_devices),
                     std::move(*device),
                     std::move(*render_frame) };
}

inline auto create_default_instance() noexcept
    -> std::expected<vk::Instance, vk::Result>
{
    vk::ApplicationInfo app_info{ .apiVersion = VK_API_VERSION_1_3 };
    auto                layers{ renderer::vulkan::validation_layers() };
    auto                extensions{ renderer::vulkan::instance_extensions() };

    vk::InstanceCreateInfo create_info{
        .pApplicationInfo        = &app_info,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    auto [result, instance]{ vk::createInstance(create_info) };
    if (result == vk::Result::eSuccess) {
        return instance;
    }
    return std::unexpected{ result };
}

auto Renderer::create_default(
    const CreateSurfaceConcept auto& t_create_surface,
    vk::Extent2D                     t_framebuffer_size
) noexcept -> std::optional<Renderer>
{
    auto instance{ create_default_instance() };
    if (!instance.has_value()) {
        return std::nullopt;
    }

    Renderer::Config config{
        .create_debug_messenger = vulkan::create_debug_messenger,
        .filter_physical_device =
            [](vk::PhysicalDevice t_physical_device, vk::SurfaceKHR) {
                return vulkan::supports_extensions(
                    t_physical_device, Device::default_extensions()
                );
            }
    };
    auto create_device{ [](vk::Instance       t_instance,
                           vk::SurfaceKHR     t_surface,
                           vk::PhysicalDevice t_physical_device) noexcept {
        return Device::create_default(t_instance, t_surface, t_physical_device);
    } };

    return Renderer::create(
        *instance, t_create_surface, t_framebuffer_size, create_device, config
    );
}

}   // namespace engine::renderer
