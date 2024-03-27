#pragma once

#include <span>
#include <string>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include "core/utility/vma/Allocator.hpp"
#include "core/utility/vma/Buffer.hpp"

namespace core::renderer {

class Instance;
class Device;

class Allocator {
public:
    [[nodiscard]] static auto recommended_instance_extensions() noexcept
        -> std::span<const std::string>;
    [[nodiscard]] static auto recommended_device_extensions() noexcept
        -> std::span<const std::string>;
    [[nodiscard]] static auto recommended_device_extension_structs(
        std::span<const std::string> t_enabled_device_extensions
    ) noexcept
        -> vk::StructureChain<
            vk::DeviceCreateInfo,
            vk::PhysicalDeviceCoherentMemoryFeaturesAMD,
            vk::PhysicalDeviceBufferDeviceAddressFeatures,
            vk::PhysicalDeviceMemoryPriorityFeaturesEXT,
            vk::PhysicalDeviceMaintenance4Features>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Allocator(const Instance& t_instance, const Device& t_device);

    ///-------------///
    ///  Operators  ///
    ///-------------///
    [[nodiscard]] auto operator*() const noexcept -> VmaAllocator;
    [[nodiscard]] auto operator->() const noexcept -> const VmaAllocator*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto create_buffer(
        const vk::BufferCreateInfo&    t_buffer_create_info,
        const VmaAllocationCreateInfo& t_allocation_create_info,
        const void*                    t_data = nullptr
    ) const noexcept -> tl::optional<std::pair<vma::Buffer, VmaAllocationInfo>>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vma::Allocator m_allocator;
};

}   // namespace core::renderer
