export module ddge.modules.renderer.vulkan_profile;

import vulkan_hpp;

import ddge.modules.vulkan.PhysicalDeviceCapabilities;
import ddge.modules.vulkan.Profile;

namespace ddge::renderer {

export [[nodiscard]]
auto vulkan_profile() -> const vulkan::Profile&;

}   // namespace ddge::renderer

namespace ddge::renderer {

auto vulkan_profile() -> const vulkan::Profile&
{
    static const vulkan::Profile profile{ [] static {
        vulkan::Profile          result{
                     .api_version = vk::ApiVersion14,
        };

        result.physical_device_capabilities.upgrade_version(vk::ApiVersion14);

        result.physical_device_capabilities.insert_features(
            vk::PhysicalDeviceVulkan11Features{
                .shaderDrawParameters = vk::True,
            }
        );
        result.physical_device_capabilities.insert_features(
            vk::PhysicalDeviceVulkan13Features{
                .dynamicRendering = vk::True,
            }
        );
        result.physical_device_capabilities.insert_features(
            vk::PhysicalDeviceVulkan14Features{
                .maintenance5 = vk::True,
            }
        );

        return result;
    }() };

    return profile;
}

}   // namespace ddge::renderer
