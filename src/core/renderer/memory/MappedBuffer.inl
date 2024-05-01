namespace core::renderer {

template <typename T>
auto MappedBuffer::set(const T& t_data) const -> void
{
    vk::resultCheck(
        vk::Result{
            vmaCopyMemoryToAllocation(allocator(), &t_data, allocation(), 0, sizeof(T)) },
        "vmaCopyMemoryToAllocation failed"
    );
}

}   // namespace core::renderer
