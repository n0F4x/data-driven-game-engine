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
    static const vulkan::Profile profile{ [] {
        vulkan::Profile          result;

        result.physical_device_capabilities.upgrade_version(vk::ApiVersion14);

        return result;
    }() };

    return profile;
}

}   // namespace ddge::renderer
