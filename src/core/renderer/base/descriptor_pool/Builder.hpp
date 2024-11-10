#pragma once

#include <cstdint>
#include <ranges>

#include "DescriptorPool.hpp"

namespace core::renderer::base {

class DescriptorPool::Builder {
public:
    template <typename Self>
    auto set_flags(this Self&& self, vk::DescriptorPoolCreateFlags flags) noexcept
        -> Self;
    template <typename Self>
    auto request_descriptor_sets(this Self&& self, uint32_t count) -> Self;
    template <typename Self>
    auto request_descriptors(this Self&& self, const vk::DescriptorPoolSize& pool_size)
        -> Self;
    template <typename Self>
    auto request_descriptors(this Self&& self, std::ranges::range auto&& pool_sizes)
        -> Self;

    [[nodiscard]]
    auto build(vk::Device device) noexcept -> DescriptorPool;

private:
    vk::DescriptorPoolCreateFlags       m_flags;
    uint32_t                            m_set_count{};
    std::vector<vk::DescriptorPoolSize> m_pool_sizes;
};

}   // namespace core::renderer::base

#include "Builder.inl"
