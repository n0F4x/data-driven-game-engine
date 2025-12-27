module;

#include <cassert>
#include <expected>
#include <span>

#include <GLFW/glfw3.h>

#include "utility/contract_macros.hpp"

export module ddge.modules.wsi.vulkan_instance_extensions;

import ddge.modules.wsi.Context;
import ddge.utility.containers.StringLiteral;
import ddge.utility.contracts;

namespace ddge::wsi {

export struct VulkanSurfaceCreationNotSupportedError {};

export [[nodiscard]]
auto vulkan_instance_extensions(const Context&) -> std::
    expected<std::span<const util::StringLiteral>, VulkanSurfaceCreationNotSupportedError>
{
    uint32_t     count{};
    const char** extension_names{ glfwGetRequiredInstanceExtensions(&count) };
    if (extension_names == nullptr) {
        const int error_code = glfwGetError(nullptr);
        PRECOND(error_code != GLFW_NOT_INITIALIZED);
        PRECOND(error_code != GLFW_API_UNAVAILABLE && "Vulkan support is not available");
        assert(error_code == GLFW_NO_ERROR && "Other error codes are unspecified");

        return std::unexpected{ VulkanSurfaceCreationNotSupportedError{} };
    }

    return std::span{
        reinterpret_cast<const util::StringLiteral*>(extension_names),
        count,
    };
}

}   // namespace ddge::wsi
