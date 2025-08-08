module;

export module modules.config.vulkan;

import vulkan_hpp;

namespace modules::config::vulkan {

export auto init() -> void;
export auto init(vk::Instance instance) noexcept -> void;
export auto init(vk::Device device) noexcept -> void;

export [[nodiscard]]
auto dispatcher() noexcept -> const vk::DispatchLoaderDynamic&;

}   // namespace modules::config::vulkan
