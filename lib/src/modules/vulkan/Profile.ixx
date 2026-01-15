module;

#include <cstdint>

export module ddge.modules.vulkan.Profile;

import ddge.modules.vulkan.PhysicalDeviceCapabilities;

namespace ddge::vulkan {

export struct Profile {
    uint32_t                   api_version;
    PhysicalDeviceCapabilities physical_device_capabilities;
};

}   // namespace ddge::vulkan
