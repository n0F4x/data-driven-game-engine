#include "Builder.hpp"

namespace core::renderer {

auto DescriptorPool::Builder::set_flags(vk::DescriptorPoolCreateFlags t_flags
) noexcept -> Builder&
{
    m_flags |= t_flags;
    return *this;
}

auto DescriptorPool::Builder::request_descriptor_sets(uint32_t t_count) -> Builder&
{
    m_set_count += t_count;
    return *this;
}

auto DescriptorPool::Builder::request_descriptors(const vk::DescriptorPoolSize& t_pool_size
) -> Builder&
{
    m_pool_sizes.push_back(t_pool_size);
    return *this;
}

auto DescriptorPool::Builder::build(const vk::Device t_device) noexcept -> DescriptorPool
{
    const vk::DescriptorPoolCreateInfo descriptor_pool_create_info{
        .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets       = m_set_count,
        .poolSizeCount = static_cast<uint32_t>(m_pool_sizes.size()),
        .pPoolSizes    = m_pool_sizes.data()
    };

    return DescriptorPool{ t_device.createDescriptorPoolUnique(descriptor_pool_create_info
    ) };
}

}   // namespace core::renderer
