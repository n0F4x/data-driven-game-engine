module;

#include <cstdint>

module ddge.deprecated.renderer.base.descriptor_pool.DescriptorPool;

auto ddge::renderer::base::DescriptorPool::Builder::build(const vk::Device device) noexcept
    -> DescriptorPool
{
    const vk::DescriptorPoolCreateInfo descriptor_pool_create_info{
        .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets       = m_set_count,
        .poolSizeCount = static_cast<uint32_t>(m_pool_sizes.size()),
        .pPoolSizes    = m_pool_sizes.data()
    };

    return DescriptorPool{
        device.createDescriptorPoolUnique(descriptor_pool_create_info)
    };
}
