namespace core::renderer {

auto DescriptorPool::Builder::request_descriptors(std::ranges::range auto&& t_pool_sizes
) -> DescriptorPool::Builder&
{
    m_pool_sizes.append_range(std::forward<decltype(t_pool_sizes)>(t_pool_sizes));
    return *this;
}

}   // namespace core::renderer
