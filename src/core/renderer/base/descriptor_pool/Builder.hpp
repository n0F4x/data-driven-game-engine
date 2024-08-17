#pragma once

#include "DescriptorPool.hpp"

namespace core::renderer {

class DescriptorPool::Builder {
public:
    auto set_flags(vk::DescriptorPoolCreateFlags flags) noexcept -> Builder&;
    auto request_descriptor_sets(uint32_t count) -> Builder&;
    auto request_descriptors(const vk::DescriptorPoolSize& pool_size) -> Builder&;
    auto request_descriptors(std::ranges::range auto&& pool_sizes) -> Builder&;

    [[nodiscard]]
    auto build(vk::Device device) noexcept -> DescriptorPool;

private:
    vk::DescriptorPoolCreateFlags       m_flags;
    uint32_t                            m_set_count;
    std::vector<vk::DescriptorPoolSize> m_pool_sizes;
};

}   // namespace core::renderer

#include "Builder.inl"
