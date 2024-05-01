#include "DescriptorPool.hpp"

#include "Builder.hpp"

namespace core::renderer {

auto DescriptorPool::create() noexcept -> Builder
{
    return Builder{};
}

DescriptorPool::DescriptorPool(vk::UniqueDescriptorPool&& t_descriptor_pool) noexcept
    : m_descriptor_pool{ std::move(t_descriptor_pool) }
{}

auto DescriptorPool::get() const noexcept -> vk::DescriptorPool
{
    return m_descriptor_pool.get();
}

}   // namespace core::renderer
