#include "Builder.hpp"

namespace core::renderer {

auto DescriptorPool::Builder::set_flags(vk::DescriptorPoolCreateFlags flags
) noexcept -> Builder&
{
    m_flags |= flags;
    return *this;
}

auto DescriptorPool::Builder::request_descriptor_sets(uint32_t count) -> Builder&
{
    m_set_count += count;
    return *this;
}

auto DescriptorPool::Builder::request_descriptors(const vk::DescriptorPoolSize& pool_size
) -> Builder&
{
    m_pool_sizes.push_back(pool_size);
    return *this;
}

auto DescriptorPool::Builder::build(const vk::Device device) noexcept -> DescriptorPool
{
    const vk::DescriptorPoolCreateInfo descriptor_pool_create_info{
        .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets       = m_set_count,
        .poolSizeCount = static_cast<uint32_t>(m_pool_sizes.size()),
        .pPoolSizes    = m_pool_sizes.data()
    };

    return DescriptorPool{ device.createDescriptorPoolUnique(descriptor_pool_create_info
    ) };
}

}   // namespace core::renderer
