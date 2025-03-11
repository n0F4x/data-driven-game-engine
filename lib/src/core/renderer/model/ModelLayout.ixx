module;

#include <cstdint>
#include <vector>

#include <VkBootstrap.h>

export module core.renderer.model.ModelLayout;

import vulkan_hpp;

namespace core::renderer {

export class ModelLayout {
public:
    class Requirements;

    struct DescriptorSetLayoutCreateInfo {
        uint32_t max_image_count;
        uint32_t max_sampler_count;
    };

    struct PushConstants {
        uint32_t transform_index;
        uint32_t material_index;
    };

    [[nodiscard]]
    static auto descriptor_set_count() noexcept -> uint32_t;
    [[nodiscard]]
    static auto descriptor_pool_sizes(const DescriptorSetLayoutCreateInfo& info)
        -> std::vector<vk::DescriptorPoolSize>;

    [[nodiscard]]
    static auto create_descriptor_set_layouts(
        vk::Device                           device,
        const DescriptorSetLayoutCreateInfo& info
    ) -> std::array<vk::UniqueDescriptorSetLayout, 3>;
    [[nodiscard]]
    static auto push_constant_range() noexcept -> vk::PushConstantRange;
};

class ModelLayout::Requirements {
public:
    [[nodiscard]]
    static auto required_instance_settings_are_available(const vkb::SystemInfo& system_info
    ) -> bool;

    static auto enable_instance_settings(
        const vkb::SystemInfo& system_info,
        vkb::InstanceBuilder&  instance_builder
    ) -> void;

    static auto require_device_settings(
        vkb::PhysicalDeviceSelector& physical_device_selector
    ) -> void;

    static auto enable_optional_device_settings(vkb::PhysicalDevice& physical_device)
        -> void;
};

}   // namespace core::renderer
