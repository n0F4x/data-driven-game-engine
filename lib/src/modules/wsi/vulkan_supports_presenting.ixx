module;

#include <cassert>

#include <GLFW/glfw3.h>

#include "utility/contract_macros.hpp"

export module ddge.modules.wsi.vulkan_supports_presenting;

import vulkan_hpp;

import ddge.modules.vulkan.QueueFamilyIndex;
import ddge.modules.wsi.Context;
import ddge.modules.wsi.Error;
import ddge.utility.contracts;

namespace ddge::wsi {

export [[nodiscard]]
auto vulkan_supports_presenting(
    const Context&,
    const vk::raii::Instance&       instance,
    const vk::raii::PhysicalDevice& physical_device,
    const vulkan::QueueFamilyIndex  queue_family_index
) -> bool
{
    const int result{ glfwGetPhysicalDevicePresentationSupport(
        *instance, *physical_device, queue_family_index.underlying()
    ) };
    if (result != GLFW_TRUE) {
        const char* error_description{};
        const int   error_code = glfwGetError(&error_description);
        PRECOND(error_code != GLFW_NOT_INITIALIZED);
        PRECOND(error_code != GLFW_API_UNAVAILABLE && "Vulkan support is not available");
        if (error_code == GLFW_PLATFORM_ERROR) {
            throw Error{ error_description };
        }
        assert(error_code == GLFW_NO_ERROR && "Other error codes are unspecified");
    }

    return result == GLFW_TRUE;
}

}   // namespace ddge::wsi
