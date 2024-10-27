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

    if (!(destination.allocation.get().memory_properties()
          & vk::MemoryPropertyFlagBits::eHostCoherent))
    {
        destination.allocation.get().flush(0, size);
    }

    destination.allocation.get().unmap();
}

auto core::renderer::base::copy(
    const CopyRegion& source,
    void*             destination,
    vk::DeviceSize    size
) -> void
{
    const std::span<const std::byte> mapped_source{
        source.allocation.get().map().subspan(source.offset)
    };

    if (size == vk::WholeSize) {
        size = mapped_source.size();
    }

    assert(size <= mapped_source.size());

    if (!(source.allocation.get().memory_properties()
          & vk::MemoryPropertyFlagBits::eHostCoherent))
    {
        source.allocation.get().invalidate(0, size);
    }

    std::memcpy(destination, mapped_source.data(), size);

    source.allocation.get().unmap();
}
