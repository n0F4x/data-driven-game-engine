module;

#include <array>

module core.renderer.model.ModelLayout;

[[nodiscard]]
static auto create_descriptor_set_layouts(
    const vk::Device                                                  device,
    const core::renderer::ModelLayout::DescriptorSetLayoutCreateInfo& info
) -> std::array<vk::UniqueDescriptorSetLayout, 3>
{
    constexpr static std::array bindings_0{
        // vertices
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 0,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eVertex  },
        // transforms
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 1,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eVertex  },
        // defaultSampler
        vk::DescriptorSetLayoutBinding{ .binding         = 2,
                                       .descriptorType  = vk::DescriptorType::eSampler,
                                       .descriptorCount = 1,
                                       .stageFlags = vk::ShaderStageFlagBits::eFragment     },
        // textures
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 3,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
        // defaultMaterial
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 4,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
        // materials
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 5,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
    };
    constexpr static vk::DescriptorSetLayoutCreateInfo create_info_0{
        .bindingCount = static_cast<uint32_t>(bindings_0.size()),
        .pBindings    = bindings_0.data(),
    };

    const std::array bindings_1{
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 0,
                                       .descriptorType  = vk::DescriptorType::eSampledImage,
                                       .descriptorCount = static_cast<uint32_t>(info.max_image_count),
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
    };
    constexpr static std::array flags_1{
        vk::DescriptorBindingFlags{
            vk::DescriptorBindingFlagBits::eVariableDescriptorCount },
    };
    vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT binding_flags_1{
        .bindingCount  = static_cast<uint32_t>(flags_1.size()),
        .pBindingFlags = flags_1.data(),
    };
    const vk::DescriptorSetLayoutCreateInfo create_info_1{
        .pNext        = &binding_flags_1,
        .bindingCount = static_cast<uint32_t>(bindings_1.size()),
        .pBindings    = bindings_1.data(),
    };

    const std::array bindings_2{
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 0,
                                       .descriptorType  = vk::DescriptorType::eSampler,
                                       .descriptorCount = static_cast<uint32_t>(info.max_sampler_count),
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
    };
    constexpr static std::array flags_2{
        vk::DescriptorBindingFlags{
            vk::DescriptorBindingFlagBits::eVariableDescriptorCount },
    };
    vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT binding_flags_2{
        .bindingCount  = static_cast<uint32_t>(flags_2.size()),
        .pBindingFlags = flags_2.data(),
    };
    const vk::DescriptorSetLayoutCreateInfo create_info_2{
        .pNext        = &binding_flags_2,
        .bindingCount = static_cast<uint32_t>(bindings_2.size()),
        .pBindings    = bindings_2.data(),
    };

    return std::array{
        device.createDescriptorSetLayoutUnique(create_info_0),
        device.createDescriptorSetLayoutUnique(create_info_1),
        device.createDescriptorSetLayoutUnique(create_info_2),
    };
}

namespace core::renderer {

auto ModelLayout::descriptor_set_count() noexcept -> uint32_t
{
    return 3;
}

auto ModelLayout::descriptor_pool_sizes(const DescriptorSetLayoutCreateInfo& info)
    -> std::vector<vk::DescriptorPoolSize>
{
    std::vector pool_sizes{
        // vertices
        vk::DescriptorPoolSize{ .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = 1u },
        // transforms
        vk::DescriptorPoolSize{ .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = 1u },
        // defaultSampler
        vk::DescriptorPoolSize{       .type            = vk::DescriptorType::eSampler,
                               .descriptorCount = 1u },
        // textures
        vk::DescriptorPoolSize{ .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = 1u },
        // defaultMaterial
        vk::DescriptorPoolSize{ .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = 1u },
        // materials
        vk::DescriptorPoolSize{ .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = 1u },
    };

    if (info.max_image_count > 0) {
        pool_sizes.push_back(
            vk::DescriptorPoolSize{
                .type            = vk::DescriptorType::eSampledImage,
                .descriptorCount = info.max_image_count,
            }
        );
    }
    if (info.max_sampler_count > 0) {
        pool_sizes.push_back(
            vk::DescriptorPoolSize{
                .type            = vk::DescriptorType::eSampler,
                .descriptorCount = info.max_sampler_count,
            }
        );
    }

    return pool_sizes;
}

auto ModelLayout::create_descriptor_set_layouts(
    const vk::Device                     device,
    const DescriptorSetLayoutCreateInfo& info
) -> std::array<vk::UniqueDescriptorSetLayout, 3>
{
    return ::create_descriptor_set_layouts(device, info);
}

auto ModelLayout::push_constant_range() noexcept -> vk::PushConstantRange
{
    return vk::PushConstantRange{
        .stageFlags = vk::ShaderStageFlagBits::eVertex
                    | vk::ShaderStageFlagBits::eFragment,
        .size = sizeof(PushConstants),
    };
}

}   // namespace core::renderer
