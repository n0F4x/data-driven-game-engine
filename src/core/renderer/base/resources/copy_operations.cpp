#include "copy_operations.hpp"

auto core::renderer::base::copy(
    const void*       source,
    const CopyRegion& destination,
    vk::DeviceSize    size
) -> void
{
    const std::span<std::byte> mapped_destination{
        destination.allocation.get().map().subspan(destination.offset)
    };

    if (size == vk::WholeSize) {
        size = mapped_destination.size();
    }

    assert(size <= mapped_destination.size());

    std::memcpy(mapped_destination.data(), source, size);

    destination.allocation.get().unmap();

    if (!(destination.allocation.get().memory_properties()
          & vk::MemoryPropertyFlagBits::eHostCoherent))
    {
        destination.allocation.get().flush(0, size);
    }
}
