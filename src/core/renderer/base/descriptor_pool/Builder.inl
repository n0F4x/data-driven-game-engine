namespace core::renderer {

auto DescriptorPool::Builder::request_descriptors(std::ranges::range auto&& t_pool_sizes
) -> DescriptorPool::Builder&
{
    // TODO: use append_range
    m_pool_sizes.insert(m_pool_sizes.end(), t_pool_sizes.cbegin(), t_pool_sizes.cend());
    return *this;
}

}   // namespace core::renderer
