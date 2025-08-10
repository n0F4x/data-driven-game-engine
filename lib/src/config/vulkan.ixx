module;

export module ddge.config.vulkan;

import vulkan_hpp;

namespace ddge::config::vulkan {

export auto init() -> void;
export auto init(vk::Instance instance) noexcept -> void;
export auto init(vk::Device device) noexcept -> void;

export [[nodiscard]]
auto dispatcher() noexcept -> const vk::DispatchLoaderDynamic&;

}   // namespace ddge::config::vulkan
