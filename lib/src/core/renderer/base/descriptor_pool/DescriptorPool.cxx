module;

#include <utility>

module core.renderer.base.descriptor_pool.DescriptorPool;

namespace core::renderer::base {

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

}   // namespace core::renderer
