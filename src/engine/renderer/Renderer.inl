#include <ranges>

namespace engine::renderer {

auto Renderer::create(
    vk::Instance                    t_instance,
    vk::SurfaceKHR                  t_surface,
    vk::Extent2D                    t_framebuffer_size,
    const CreateDeviceConcept auto& t_create_device,
    Config&                         t_config
) noexcept -> std::optional<Renderer>
{
    if (!t_instance || !t_surface) {
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
    auto view{ physical_devices
               | std::views::filter([&](vk::PhysicalDevice t_physical_device) {
                     return renderer::Device::adequate(
                                t_physical_device, t_surface
                            )
                         && (!t_config.filter_physical_device
                             || t_config.filter_physical_device(
                                 t_physical_device, t_surface
                             ));
                 }) };
    std::vector<vk::PhysicalDevice> adequate_devices{ view.begin(),
                                                      view.end() };
    if (adequate_devices.empty()) {
        return std::nullopt;
    }

    auto physical_device{ adequate_devices.front() };
    if (t_config.rank_physical_device) {
        auto highest_rank{
            t_config.rank_physical_device(physical_device, t_surface)
        };
        for (auto adequate_device : adequate_devices) {
            if (auto rank{
                    t_config.rank_physical_device(adequate_device, t_surface) };
                rank > highest_rank)
            {
                physical_device = adequate_device;
                highest_rank    = rank;
            }
        }
    }

    auto device{ t_create_device(t_instance, t_surface, physical_device) };
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
                     t_surface,
                     t_framebuffer_size,
                     std::move(adequate_devices),
                     std::move(*device),
                     std::move(*render_frame) };
}

}   // namespace engine::renderer
