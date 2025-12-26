module;

#include <concepts>
#include <cstdint>
#include <optional>
#include <ranges>
#include <vector>

export module ddge.modules.vulkan.queue_properties;

import vulkan_hpp;

import ddge.modules.vulkan.QueueFamilyIndex;

namespace ddge::vulkan {

template <typename T>
concept queue_family_matcher_c = requires {
    std::convertible_to<
        std::invoke_result_t<
            const T&,
            const vk::raii::PhysicalDevice&,
            QueueFamilyIndex,
            const vk::QueueFamilyProperties2&>,
        bool>;
};

export [[nodiscard]]
auto has_dedicated_compute_queue_family(const vk::raii::PhysicalDevice& physical_device)
    -> bool;

export [[nodiscard]]
auto has_dedicated_transfer_queue_family(const vk::raii::PhysicalDevice& physical_device)
    -> bool;

export [[nodiscard]]
auto has_dedicated_sparse_binding_queue_family(
    const vk::raii::PhysicalDevice& physical_device
) -> bool;

export template <queue_family_matcher_c Match_T>
[[nodiscard]]
auto has_matching_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device,
    const Match_T&                  match
) -> bool;

export [[nodiscard]]
auto first_graphics_queue_family_index(const vk::raii::PhysicalDevice& physical_device)
    -> std::optional<QueueFamilyIndex>;

export [[nodiscard]]
auto first_dedicated_compute_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device
) -> std::optional<QueueFamilyIndex>;

export [[nodiscard]]
auto first_dedicated_transfer_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device
) -> std::optional<QueueFamilyIndex>;

export [[nodiscard]]
auto first_dedicated_sparse_binding_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device
) -> std::optional<QueueFamilyIndex>;

export template <queue_family_matcher_c Match_T>
[[nodiscard]]
auto first_matching_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device,
    const Match_T&                  match
) -> std::optional<QueueFamilyIndex>;

}   // namespace ddge::vulkan

namespace ddge::vulkan {

auto has_dedicated_compute_queue_family(const vk::raii::PhysicalDevice& physical_device)
    -> bool
{
    return first_dedicated_compute_queue_family_index(physical_device).has_value();
}

auto has_dedicated_transfer_queue_family(const vk::raii::PhysicalDevice& physical_device)
    -> bool
{
    return first_dedicated_transfer_queue_family_index(physical_device).has_value();
}

auto has_dedicated_sparse_binding_queue_family(
    const vk::raii::PhysicalDevice& physical_device
) -> bool
{
    return first_dedicated_sparse_binding_queue_family_index(physical_device).has_value();
}

auto first_graphics_queue_family_index(const vk::raii::PhysicalDevice& physical_device)
    -> std::optional<QueueFamilyIndex>
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
            return QueueFamilyIndex{ index };
        }
    }

    return std::nullopt;
}

auto first_dedicated_compute_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device
) -> std::optional<QueueFamilyIndex>
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
            return QueueFamilyIndex{ index };
        }
    }

    return std::nullopt;
}

auto first_dedicated_transfer_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device
) -> std::optional<QueueFamilyIndex>
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
            return QueueFamilyIndex{ index };
        }
    }

    return std::nullopt;
}

auto first_dedicated_sparse_binding_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device
) -> std::optional<QueueFamilyIndex>
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
            return QueueFamilyIndex{ index };
        }
    }

    return std::nullopt;
}

template <queue_family_matcher_c Match_T>
auto has_matching_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device,
    const Match_T&                  match
) -> bool
{
    return first_matching_queue_family_index(physical_device, match).has_value();
}

template <queue_family_matcher_c Match_T>
auto first_matching_queue_family_index(
    const vk::raii::PhysicalDevice& physical_device,
    const Match_T&                  match
) -> std::optional<QueueFamilyIndex>
{
    // TODO: use std::views:enumerate
    for (const std::pair<uint32_t, const vk::QueueFamilyProperties2&>
             queue_family_index_and_properties : std::views::zip(
                 std::views::iota(uint32_t{}), physical_device.getQueueFamilyProperties2()
             ))
    {
        const QueueFamilyIndex queue_family_index{
            queue_family_index_and_properties.first
        };
        const vk::QueueFamilyProperties2& queue_family_properties =
            queue_family_index_and_properties.second;

        if (match(physical_device, queue_family_index, queue_family_properties)) {
            return QueueFamilyIndex{ queue_family_index };
        }
    }

    return std::nullopt;
}

}   // namespace ddge::vulkan
