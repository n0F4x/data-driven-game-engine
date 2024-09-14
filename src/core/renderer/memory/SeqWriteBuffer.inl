namespace core::renderer {

template <typename T>
template <size_t E>
auto SeqWriteBuffer<T>::set(std::span<const T, E> data, const size_t offset) const -> void
{
    assert(data.size_bytes() <= size_bytes() - offset * sizeof(T));
    vk::resultCheck(
        vk::Result{ vmaCopyMemoryToAllocation(
            allocator(), data.data(), allocation(), 0, data.size_bytes()
        ) },
        "vmaCopyMemoryToAllocation failed"
    );
}

template <typename T>
auto SeqWriteBuffer<T>::set(const T& data, const size_t offset) const -> void
{
    assert(sizeof(T) <= size_bytes() - offset * sizeof(T));
    vk::resultCheck(
        vk::Result{
            vmaCopyMemoryToAllocation(allocator(), &data, allocation(), 0, sizeof(T)) },
        "vmaCopyMemoryToAllocation failed"
    );
}

template <typename T>
auto SeqWriteBuffer<T>::size() const noexcept -> size_t
{
    return size_bytes() * sizeof(T);
}

}
