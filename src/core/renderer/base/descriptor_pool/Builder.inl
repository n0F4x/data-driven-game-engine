#pragma once

namespace core::renderer::base {

auto DescriptorPool::Builder::request_descriptors(std::ranges::range auto&& pool_sizes)
    -> DescriptorPool::Builder&
{
    m_pool_sizes.append_range(std::forward<decltype(pool_sizes)>(pool_sizes));
    return *this;
}

}   // namespace core::renderer
