#pragma once

#include <vulkan/vulkan.hpp>

namespace core::config::vulkan {

auto init() -> void;
auto init(vk::Instance instance) noexcept -> void;
auto init(vk::Device device) noexcept -> void;

[[nodiscard]]
auto dispatcher() -> const vk::DispatchLoaderDynamic&;

}   // namespace core::config::vulkan
