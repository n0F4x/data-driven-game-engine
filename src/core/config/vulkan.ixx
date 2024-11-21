module;

#include <vulkan/vulkan.hpp>

export module core.config.vulkan;

namespace core::config::vulkan {

export auto init() -> void;
export auto init(vk::Instance instance) noexcept -> void;
export auto init(vk::Device device) noexcept -> void;

export [[nodiscard]]
auto dispatcher() -> const vk::DispatchLoaderDynamic&;

}   // namespace core::config::vulkan
