module;

#include <utility>

module modules.renderer.base.descriptor_pool.DescriptorPool;

namespace modules::renderer::base {

auto DescriptorPool::create() noexcept -> Builder
{
    return Builder{};
}

DescriptorPool::DescriptorPool(vk::UniqueDescriptorPool&& descriptor_pool) noexcept
    : m_descriptor_pool{ std::move(descriptor_pool) }
{}

auto DescriptorPool::get() const noexcept -> vk::DescriptorPool
{
    return m_descriptor_pool.get();
}

}   // namespace modules::renderer::base
