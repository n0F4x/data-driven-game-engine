module;

#include <cstdint>
#include <optional>
#include <ranges>
#include <vector>

export module ddge.modules.vulkan.queue_properties;

import vulkan_hpp;

namespace ddge::vulkan {

export [[nodiscard]]
auto has_dedicated_compute_queue_family(const vk::PhysicalDevice& physical_device)
    -> bool;

export [[nodiscard]]
auto has_dedicated_transfer_queue_family(const vk::PhysicalDevice& physical_device)
    -> bool;

export [[nodiscard]]
auto has_dedicated_sparse_binding_queue_family(const vk::PhysicalDevice& physical_device)
    -> bool;

export [[nodiscard]]
auto graphics_queue_family_index(const vk::PhysicalDevice& physical_device)
    -> std::optional<uint32_t>;

export [[nodiscard]]
auto dedicated_compute_queue_family_index(const vk::PhysicalDevice& physical_device)
    -> std::optional<uint32_t>;

export [[nodiscard]]
auto dedicated_transfer_queue_family_index(const vk::PhysicalDevice& physical_device)
    -> std::optional<uint32_t>;

export [[nodiscard]]
auto dedicated_sparse_binding_queue_family_index(const vk::PhysicalDevice& physical_device)
    -> std::optional<uint32_t>;

}   // namespace ddge::vulkan

namespace ddge::vulkan {

auto has_dedicated_compute_queue_family(const vk::PhysicalDevice& physical_device) -> bool
{
    return dedicated_compute_queue_family_index(physical_device).has_value();
}

auto has_dedicated_transfer_queue_family(const vk::PhysicalDevice& physical_device)
    -> bool
{
    return dedicated_transfer_queue_family_index(physical_device).has_value();
}

auto has_dedicated_sparse_binding_queue_family(const vk::PhysicalDevice& physical_device)
    -> bool
{
    return dedicated_sparse_binding_queue_family_index(physical_device).has_value();
}

auto graphics_queue_family_index(const vk::PhysicalDevice& physical_device)
    -> std::optional<uint32_t>
{
    std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    for (const auto&& [index, queue_family] : std::views::zip(
             std::views::iota(uint32_t{}),
             (queue_family_properties
              | std::views::transform(&vk::QueueFamilyProperties2::queueFamilyProperties))
         ))
    {
        if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics) {
            return index;
        }
    }

    return std::nullopt;
}

auto dedicated_compute_queue_family_index(const vk::PhysicalDevice& physical_device)
    -> std::optional<uint32_t>
{
    std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    for (const auto&& [index, queue_family] : std::views::zip(
             std::views::iota(uint32_t{}),
             (queue_family_properties
              | std::views::transform(&vk::QueueFamilyProperties2::queueFamilyProperties))
         ))
    {
        if (!(queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
            && queue_family.queueFlags & vk::QueueFlagBits::eCompute)
        {
            return index;
        }
    }

    return std::nullopt;
}

auto dedicated_transfer_queue_family_index(const vk::PhysicalDevice& physical_device)
    -> std::optional<uint32_t>
{
    std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    for (const auto&& [index, queue_family] : std::views::zip(
             std::views::iota(uint32_t{}),
             (queue_family_properties
              | std::views::transform(&vk::QueueFamilyProperties2::queueFamilyProperties))
         ))
    {
        if (!(queue_family.queueFlags & vk::QueueFlagBits::eCompute)
            && !(queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
            && queue_family.queueFlags & vk::QueueFlagBits::eTransfer)
        {
            return index;
        }
    }

    return std::nullopt;
}

auto dedicated_sparse_binding_queue_family_index(const vk::PhysicalDevice& physical_device)
    -> std::optional<uint32_t>
{
    std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    for (const auto&& [index, queue_family] : std::views::zip(
             std::views::iota(uint32_t{}),
             (queue_family_properties
              | std::views::transform(&vk::QueueFamilyProperties2::queueFamilyProperties))
         ))
    {
        if (!(queue_family.queueFlags & vk::QueueFlagBits::eGraphics
              || queue_family.queueFlags & vk::QueueFlagBits::eCompute
              || queue_family.queueFlags & vk::QueueFlagBits::eTransfer)
            && (queue_family.queueFlags & vk::QueueFlagBits::eSparseBinding))
        {
            return index;
        }
    }

    return std::nullopt;
}

}   // namespace ddge::vulkan
