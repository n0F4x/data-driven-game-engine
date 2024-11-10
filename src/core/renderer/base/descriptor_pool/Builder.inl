#pragma once

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
