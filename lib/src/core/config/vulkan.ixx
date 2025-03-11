module;

export module core.config.vulkan;

import vulkan_hpp;

namespace core::config::vulkan {

export auto init() -> void;
export auto init(vk::Instance instance) noexcept -> void;
export auto init(vk::Device device) noexcept -> void;

export [[nodiscard]]
auto dispatcher() noexcept -> const vk::DispatchLoaderDynamic&;

}   // namespace core::config::vulkan
