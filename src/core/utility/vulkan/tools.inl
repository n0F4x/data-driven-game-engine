#include <ranges>
#include <set>

namespace core::vulkan {

auto filter(
    const std::ranges::range auto& t_available,
    const std::ranges::range auto& t_required,
    const std::ranges::range auto& t_optional
) noexcept -> std::vector<std::string>
{
    std::vector<std::string> filtered{ std::from_range, t_required };

    filtered.append_range(t_optional | std::views::filter([&](const auto& optional) {
                              return std::ranges::find(t_available, optional)
                                  != std::cend(t_available);
                          }));

    return filtered;
}

auto supports_extensions(
    const vk::PhysicalDevice       t_physical_device,
    const std::ranges::range auto& t_extension_names
) -> bool
{
    assert(t_physical_device);
    if (!t_physical_device) {
        return false;
    }

    const auto extension_properties{ t_physical_device.enumerateDeviceExtensionProperties(
    ) };

    std::set<std::string_view> required_extensions{ std::from_range, t_extension_names };

    for (const auto& extension : extension_properties) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

}   // namespace core::vulkan
