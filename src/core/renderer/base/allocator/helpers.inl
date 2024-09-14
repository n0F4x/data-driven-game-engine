namespace core::renderer::details {

template <VmaAllocationCreateFlags Flags>
auto create_mapped_buffer(
    const VmaAllocator                  allocator,
    const vk::BufferCreateInfo&         buffer_create_info,
    const void*                         data
) -> std::tuple<VmaAllocation, vk::Buffer, VmaAllocationInfo>
{
    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | Flags,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    VmaAllocation     allocation{};
    vk::Buffer        buffer;
    VmaAllocationInfo allocation_info{};
    std::tie(allocation, buffer, allocation_info) = details::create_buffer(
        allocator, buffer_create_info, allocation_create_info
    );

    if (data != nullptr) {
        details::copy(
            allocation_info.pMappedData, data, buffer_create_info.size, allocator, allocation
        );
    }

    return std::make_tuple(allocation, buffer, allocation_info);
}

}
