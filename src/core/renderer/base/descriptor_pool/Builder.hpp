#pragma once

#include "DescriptorPool.hpp"

namespace core::renderer {

class DescriptorPool::Builder {
public:
    auto set_flags(vk::DescriptorPoolCreateFlags t_flags) noexcept -> Builder&;
    auto request_descriptors(const vk::DescriptorPoolSize& t_pool_size) -> Builder&;
    auto request_descriptors(std::ranges::range auto&& t_pool_sizes) -> Builder&;

    [[nodiscard]]
    auto build(vk::Device t_device, uint32_t t_maximum_sets) noexcept -> DescriptorPool;

private:
    vk::DescriptorPoolCreateFlags       m_flags;
    std::vector<vk::DescriptorPoolSize> m_pool_sizes;
};

}   // namespace core::renderer

#include "Builder.inl"
