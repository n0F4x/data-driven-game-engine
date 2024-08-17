namespace core::renderer {

template <typename T>
auto MappedBuffer::set(const T& data) const -> void
{
    vk::resultCheck(
        vk::Result{
            vmaCopyMemoryToAllocation(allocator(), &data, allocation(), 0, sizeof(T)) },
        "vmaCopyMemoryToAllocation failed"
    );
}

}   // namespace core::renderer
