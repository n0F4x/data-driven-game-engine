#pragma once

#include <vulkan/vulkan.hpp>

namespace core::renderer {

class ModelLayout {
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

}   // namespace core::renderer
