module;

export module config.vulkan;

import vulkan_hpp;

namespace config::vulkan {

export auto init() -> void;
export auto init(vk::Instance instance) noexcept -> void;
export auto init(vk::Device device) noexcept -> void;

export [[nodiscard]]
auto dispatcher() noexcept -> const vk::DispatchLoaderDynamic&;

}   // namespace config::vulkan
