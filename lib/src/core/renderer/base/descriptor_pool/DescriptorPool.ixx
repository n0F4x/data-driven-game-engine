module;

#include <cstdint>
#include <vector>

export module core.renderer.base.descriptor_pool.DescriptorPool;

import vulkan_hpp;

namespace core::renderer::base {

export class DescriptorPool {
public:
    class Builder;

    [[nodiscard]]
    static auto create() noexcept -> Builder;

    explicit DescriptorPool(vk::UniqueDescriptorPool&& descriptor_pool) noexcept;

    [[nodiscard]]
    auto get() const noexcept -> vk::DescriptorPool;

private:
    vk::UniqueDescriptorPool m_descriptor_pool;
};

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

template <typename Self>
auto core::renderer::base::DescriptorPool::Builder::set_flags(
    this Self&&                   self,
    vk::DescriptorPoolCreateFlags flags
) noexcept -> Self
{
    self.m_flags |= flags;
    return std::forward<Self>(self);
}

template <typename Self>
auto core::renderer::base::DescriptorPool::Builder::request_descriptor_sets(
    this Self&& self,
    uint32_t    count
) -> Self
{
    self.m_set_count += count;
    return std::forward<Self>(self);
}

template <typename Self>
auto core::renderer::base::DescriptorPool::Builder::request_descriptors(
    this Self&&                   self,
    const vk::DescriptorPoolSize& pool_size
) -> Self
{
    self.m_pool_sizes.push_back(pool_size);
    return std::forward<Self>(self);
}

template <typename Self>
auto core::renderer::base::DescriptorPool::Builder::request_descriptors(
    this Self&&               self,
    std::ranges::range auto&& pool_sizes
) -> Self
{
    self.m_pool_sizes.append_range(std::forward<decltype(pool_sizes)>(pool_sizes));
    return std::forward<Self>(self);
}
