module;

#include <stdexcept>

export module ddge.modules.vulkan.result.VulkanError;

import vulkan_hpp;

namespace ddge::vulkan {

struct VulkanErrorPrecondition {
    explicit VulkanErrorPrecondition(vk::Result runtime_error_code);
};

export class VulkanError : public VulkanErrorPrecondition, public std::runtime_error {
public:
    explicit VulkanError(vk::Result runtime_error_code);
};

}   // namespace ddge::vulkan
