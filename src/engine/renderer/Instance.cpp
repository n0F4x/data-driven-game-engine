#include "Instance.hpp"

#include <ranges>
#include <utility>

#include <spdlog/spdlog.h>

#include "helpers.hpp"

namespace engine::renderer {

auto Instance::create(const CreateInfo& t_create_info) noexcept
    -> tl::optional<Instance>
try {
    auto enabled_layer_names{ t_create_info.layers
                              | std::views::transform(&std::string::c_str)
                              | std::ranges::to<std::vector>() };
    auto enabled_extension_names{ t_create_info.extensions
                                  | std::views::transform(&std::string::c_str)
                                  | std::ranges::to<std::vector>() };

    const vk::InstanceCreateInfo create_info{
        .pNext             = t_create_info.next,
        .flags             = t_create_info.flags,
        .pApplicationInfo  = &t_create_info.application_info,
        .enabledLayerCount = static_cast<uint32_t>(enabled_layer_names.size()),
        .ppEnabledLayerNames = enabled_layer_names.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(enabled_extension_names.size()),
        .ppEnabledExtensionNames = enabled_extension_names.data()
    };

    auto instance{ vk::createInstanceUnique(create_info) };

    vk::UniqueDebugUtilsMessengerEXT debug_messenger{};
    if (t_create_info.create_debug_messenger
        && std::ranges::find(
               t_create_info.extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME
           ) != std::cend(t_create_info.extensions))
    {
        debug_messenger = t_create_info.create_debug_messenger(instance.get());
    }

    return Instance{ t_create_info.application_info,
                     t_create_info.layers,
                     t_create_info.extensions,
                     std::move(instance),
                     std::move(debug_messenger) };
} catch (const vk::Error& t_error) {
    SPDLOG_ERROR(t_error.what());
    return tl::nullopt;
}

auto Instance::create_default() noexcept -> tl::optional<Instance>
{
    return create(CreateInfo{
        .application_info = helpers::application_info(),
        .layers           = helpers::layers() | std::ranges::to<std::vector>(),
        .extensions =
            helpers::instance_extensions() | std::ranges::to<std::vector>(),
        .create_debug_messenger =
#ifdef ENGINE_VULKAN_DEBUG
            helpers::create_debug_messenger
#else
            nullptr
#endif
    });
}

auto Instance::operator*() const noexcept -> vk::Instance
{
    return *m_instance;
}

auto Instance::operator->() const noexcept -> const vk::Instance*
{
    return m_instance.operator->();
}

auto Instance::application_info() const noexcept -> const vk::ApplicationInfo&
{
    return m_application_info;
}

auto Instance::enabled_layers() const noexcept -> std::span<const std::string>
{
    return m_layers;
}

auto Instance::enabled_extensions() const noexcept
    -> std::span<const std::string>
{
    return m_extensions;
}

Instance::Instance(
    const vk::ApplicationInfo&         t_application_info,
    std::span<const std::string>       t_layers,
    std::span<const std::string>       t_extensions,
    vk::UniqueInstance&&               t_instance,
    vk::UniqueDebugUtilsMessengerEXT&& t_debug_utils_messenger
) noexcept
    : m_application_info{ t_application_info },
      m_layers{ t_layers.begin(), t_layers.end() },
      m_extensions{ t_extensions.begin(), t_extensions.end() },
      m_instance{ std::move(t_instance) },
      m_debug_utils_messenger{ std::move(t_debug_utils_messenger) }
{
    try {
        const auto instance_version{ vk::enumerateInstanceVersion() };

        std::string instance_info{};

        instance_info += "\nEnabled instance layers:";
        for (const auto& layer : m_layers) {
            instance_info += '\n';
            instance_info += '\t';
            instance_info += layer;
        }

        instance_info += "\nEnabled instance extensions:";
        for (const auto& extension : m_extensions) {
            instance_info += '\n';
            instance_info += '\t';
            instance_info += extension;
        }

        SPDLOG_INFO(
            "Found Vulkan Instance version: {}.{}.{}",
            VK_VERSION_MAJOR(instance_version),
            VK_VERSION_MINOR(instance_version),
            VK_VERSION_PATCH(instance_version)
        );

        SPDLOG_DEBUG(instance_info);
    } catch (const vk::Error& t_error) {
        SPDLOG_ERROR(t_error.what());
    }
}

}   // namespace engine::renderer
