#include "RenderModel.hpp"

#include "core/renderer/material_system/GraphicsPipelineBuilder.hpp"
#include "core/renderer/memory/Image.hpp"

using namespace core;
using namespace core::renderer;

struct ShaderVertex {
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec2 UV0;
    glm::vec2 UV1;
    glm::vec4 color;
};

struct ShaderTexture {
    uint32_t image_index;
    uint32_t sampler_index;
};

struct ShaderTextureInfo {
    uint32_t index;
    uint32_t texCoord;
};

struct ShaderPbrMetallicRoughness {
    glm::vec4         baseColorFactor;
    ShaderTextureInfo baseColorTexture;
    float             metallicFactor;
    float             roughnessFactor;
    ShaderTextureInfo metallicRoughnessTexture;
};

struct ShaderNormalTextureInfo {
    uint32_t index;
    uint32_t texCoord;
    float    scale;
};

struct ShaderOcclusionTextureInfo {
    uint32_t index;
    uint32_t texCoord;
    float    strength;
};

struct ShaderMaterial {
    ShaderPbrMetallicRoughness pbrMetallicRoughness;
    ShaderNormalTextureInfo    normalTexture;
    ShaderOcclusionTextureInfo occlusionTexture;
    ShaderTextureInfo          emissiveTexture;
    glm::vec2                  _padding0;
    glm::vec3                  emissiveFactor;
    float                      alphaCutoff;
    glm::vec4                  _padding1;
    glm::vec4                  _padding2;
};

struct PushConstants {
    uint32_t transform_index;
    uint32_t material_index;
};

[[nodiscard]]
static auto create_descriptor_set_layouts(
    const vk::Device                                  t_device,
    const RenderModel::DescriptorSetLayoutCreateInfo& t_info
) -> std::array<vk::UniqueDescriptorSetLayout, 3>
{
    const std::array bindings_0{
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
        // specular-glossiness materials
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 6,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
    };
    const vk::DescriptorSetLayoutCreateInfo create_info_0{
        .bindingCount = static_cast<uint32_t>(bindings_0.size()),
        .pBindings    = bindings_0.data(),
    };

    const std::array bindings_1{
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 0,
                                       .descriptorType  = vk::DescriptorType::eSampledImage,
                                       .descriptorCount = static_cast<uint32_t>(t_info.max_image_count),
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
    };
    const std::array flags_1{
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
                                       .descriptorCount = static_cast<uint32_t>(t_info.max_sampler_count),
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
    };
    const std::array flags_2{
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
        t_device.createDescriptorSetLayoutUnique(create_info_0),
        t_device.createDescriptorSetLayoutUnique(create_info_1),
        t_device.createDescriptorSetLayoutUnique(create_info_2),
    };
}

[[nodiscard]]
static auto create_staging_buffer(
    const Allocator&                          t_allocator,
    const void*                               t_data,
    const uint32_t                            t_size,
    const std::optional<const vk::DeviceSize> t_min_alignment = std::nullopt
) -> MappedBuffer
{
    if (t_data == nullptr || t_size == 0) {
        return MappedBuffer{};
    }

    const vk::BufferCreateInfo staging_buffer_create_info{
        .size  = t_size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    return t_min_alignment
        .transform([&](const vk::DeviceSize min_alignment) {
            return t_allocator.allocate_mapped_buffer_with_alignment(
                staging_buffer_create_info, min_alignment, t_data
            );
        })
        .value_or(t_allocator.allocate_mapped_buffer(staging_buffer_create_info, t_data));
}

template <typename T>
[[nodiscard]]
static auto create_staging_buffer(
    const Allocator&                          t_allocator,
    const std::span<T>                        t_data,
    const std::optional<const vk::DeviceSize> t_min_alignment = std::nullopt
) -> MappedBuffer
{
    return create_staging_buffer(
        t_allocator,
        t_data.data(),
        static_cast<uint32_t>(t_data.size_bytes()),
        t_min_alignment
    );
}

[[nodiscard]]
static auto create_gpu_only_buffer(
    const Allocator&                          t_allocator,
    const vk::BufferUsageFlags                t_usage_flags,
    const uint32_t                            t_size,
    const std::optional<const vk::DeviceSize> t_min_alignment = std::nullopt
) -> Buffer
{
    if (t_size == 0) {
        return Buffer{};
    }

    const vk::BufferCreateInfo buffer_create_info = {
        .size = t_size, .usage = t_usage_flags | vk::BufferUsageFlagBits::eTransferDst
    };

    return t_min_alignment
        .transform([&](const vk::DeviceSize min_alignment) {
            return t_allocator.allocate_buffer_with_alignment(
                buffer_create_info, min_alignment
            );
        })
        .value_or(t_allocator.allocate_buffer(buffer_create_info));
}

template <typename UniformBlock>
[[nodiscard]]
static auto create_buffer(const Allocator& t_allocator) -> MappedBuffer
{
    constexpr vk::BufferCreateInfo buffer_create_info = {
        .size = sizeof(UniformBlock), .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    return t_allocator.allocate_mapped_buffer(buffer_create_info);
}

[[nodiscard]]
static auto convert_material(const gltf::Material& t_material) noexcept -> ShaderMaterial
{
    return ShaderMaterial{
        .pbrMetallicRoughness =
            ShaderPbrMetallicRoughness{
                                       .baseColorFactor = t_material.pbr_metallic_roughness.base_color_factor,
                                       .baseColorTexture =
                    ShaderTextureInfo{
                        .index = t_material.pbr_metallic_roughness.base_color_texture_info
                                     .transform([](const gltf::TextureInfo& texture_info
                                                ) { return texture_info.texture_index; })
                                     .value_or(std::numeric_limits<uint32_t>::max()),
                        .texCoord =
                            t_material.pbr_metallic_roughness.base_color_texture_info
                                .transform([](const gltf::TextureInfo& texture_info) {
                                    return texture_info.tex_coord_index;
                                })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                    }, .metallicFactor  = t_material.pbr_metallic_roughness.metallic_factor,
                                       .roughnessFactor = t_material.pbr_metallic_roughness.roughness_factor,
                                       .metallicRoughnessTexture =
                    ShaderTextureInfo{
                        .index = t_material.pbr_metallic_roughness
                                     .metallic_roughness_texture_info
                                     .transform([](const gltf::TextureInfo& texture_info
                                                ) { return texture_info.texture_index; })
                                     .value_or(std::numeric_limits<uint32_t>::max()),
                        .texCoord =
                            t_material.pbr_metallic_roughness
                                .metallic_roughness_texture_info
                                .transform([](const gltf::TextureInfo& texture_info) {
                                    return texture_info.tex_coord_index;
                                })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                    }, },
        .normalTexture =
            ShaderNormalTextureInfo{
                                       .index =
                    t_material.normal_texture_info
                        .transform([](const gltf::Material::NormalTextureInfo& texture_info
                                   ) { return texture_info.texture_index; })
                        .value_or(std::numeric_limits<uint32_t>::max()),
                                       .texCoord =
                    t_material.normal_texture_info
                        .transform([](const gltf::Material::NormalTextureInfo& texture_info
                                   ) { return texture_info.tex_coord_index; })
                        .value_or(std::numeric_limits<uint32_t>::max()),
                                       .scale =
                    t_material.normal_texture_info
                        .transform([](const gltf::Material::NormalTextureInfo& texture_info
                                   ) { return texture_info.scale; })
                        .value_or(1.f),
                                       },
        .occlusionTexture =
            ShaderOcclusionTextureInfo{
                                       .index = t_material.occlusion_texture_info
                             .transform(
                                 [](const gltf::Material::OcclusionTextureInfo& texture_info
                                 ) { return texture_info.texture_index; }
                             )
                             .value_or(std::numeric_limits<uint32_t>::max()),
                                       .texCoord = t_material.occlusion_texture_info
                                .transform([](const gltf::Material::OcclusionTextureInfo&
                                                  texture_info
                                           ) { return texture_info.tex_coord_index; })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                                       .strength = t_material.occlusion_texture_info
                                .transform([](const gltf::Material::OcclusionTextureInfo&
                                                  texture_info
                                           ) { return texture_info.strength; })
                                .value_or(1.f),
                                       },
        .emissiveTexture =
            ShaderTextureInfo{
                                       .index = t_material.emissive_texture_info
                             .transform([](const gltf::TextureInfo& texture_info) {
                                 return texture_info.texture_index;
                             })
                             .value_or(std::numeric_limits<uint32_t>::max()),
                                       .texCoord = t_material.emissive_texture_info
                                .transform([](const gltf::TextureInfo& texture_info) {
                                    return texture_info.tex_coord_index;
                                })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                                       },
        .emissiveFactor = t_material.emissive_factor,
        .alphaCutoff    = t_material.alpha_mode == core::gltf::Material::AlphaMode::eMask
                            ? t_material.alpha_cutoff
                            : -1.f,
    };
}

[[nodiscard]]
static auto create_base_descriptor_set(
    const vk::Device              t_device,
    const vk::DescriptorSetLayout t_descriptor_set_layout,
    const vk::DescriptorPool      t_descriptor_pool,
    const vk::Buffer              t_vertex_uniform,
    const vk::Buffer              t_transform_uniform,
    const vk::Sampler             t_default_sampler,
    const vk::Buffer              t_texture_uniform,
    const vk::Buffer              t_default_material_uniform,
    const vk::Buffer              t_material_uniform
) -> vk::UniqueDescriptorSet
{
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .descriptorPool     = t_descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &t_descriptor_set_layout,
    };
    auto descriptor_sets{
        t_device.allocateDescriptorSetsUnique(descriptor_set_allocate_info)
    };

    const vk::DescriptorBufferInfo vertex_buffer_info{
        .buffer = t_vertex_uniform,
        .range  = sizeof(vk::DeviceAddress),
    };
    const vk::DescriptorBufferInfo transform_buffer_info{
        .buffer = t_transform_uniform,
        .range  = sizeof(vk::DeviceAddress),
    };
    const vk::DescriptorImageInfo default_sampler_image_info{
        .sampler = t_default_sampler,
    };
    const vk::DescriptorBufferInfo texture_buffer_info{
        .buffer = t_texture_uniform,
        .range  = sizeof(vk::DeviceAddress),
    };
    const vk::DescriptorBufferInfo default_material_buffer_info{
        .buffer = t_default_material_uniform,
        .range  = sizeof(ShaderMaterial),
    };
    const vk::DescriptorBufferInfo material_buffer_info{
        .buffer = t_material_uniform,
        .range  = sizeof(vk::DeviceAddress),
    };

    std::array write_descriptor_sets{
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 0,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &vertex_buffer_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 1,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &transform_buffer_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 2,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eSampler,
                               .pImageInfo      = &default_sampler_image_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 3,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &texture_buffer_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 4,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &default_material_buffer_info,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 5,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &material_buffer_info,
                               },
    };

    t_device.updateDescriptorSets(
        static_cast<uint32_t>(write_descriptor_sets.size()),
        write_descriptor_sets.data(),
        0,
        nullptr
    );

    return std::move(descriptor_sets.front());
}

[[nodiscard]]
static auto create_image(
    const Allocator&    t_allocator,
    const gltf::Image&  t_image,
    vk::ImageTiling     t_tiling,
    vk::ImageUsageFlags t_usage
) -> Image
{
    const vk::ImageCreateInfo image_create_info{
        .imageType     = vk::ImageType::e2D,
        .format        = t_image->format(),
        .extent        = vk::Extent3D{ .width  = t_image->width(),
                                      .height = t_image->height(),
                                      .depth  = t_image->depth() },
        .mipLevels     = t_image->mip_levels(),
        .arrayLayers   = 1,
        .samples       = vk::SampleCountFlagBits::e1,
        .tiling        = t_tiling,
        .usage         = t_usage,
        .sharingMode   = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage    = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.f,
    };

    return t_allocator.allocate_image(image_create_info, allocation_create_info);
}

[[nodiscard]]
static auto create_image_view(
    const vk::Device t_device,
    const vk::Image  t_image,
    const vk::Format t_format
) -> vk::UniqueImageView
{
    const vk::ImageViewCreateInfo view_create_info{
        .image    = t_image,
        .viewType = vk::ImageViewType::e2D,
        .format   = t_format,
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                      .levelCount     = 1,
                                      .layerCount     = 1 },
    };

    return t_device.createImageViewUnique(view_create_info);
}

[[nodiscard]]
static auto create_image_descriptor_set(
    const vk::Device                        t_device,
    const vk::DescriptorSetLayout           t_descriptor_set_layout,
    const vk::DescriptorPool                t_descriptor_pool,
    const std::vector<vk::UniqueImageView>& t_image_views
) -> vk::UniqueDescriptorSet
{
    const uint32_t descriptor_count{ static_cast<uint32_t>(t_image_views.size()) };
    const vk::DescriptorSetVariableDescriptorCountAllocateInfo variable_info{
        .descriptorSetCount = 1,
        .pDescriptorCounts  = &descriptor_count,
    };
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .pNext              = &variable_info,
        .descriptorPool     = t_descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &t_descriptor_set_layout,
    };
    auto descriptor_sets{
        t_device.allocateDescriptorSetsUnique(descriptor_set_allocate_info)
    };

    const std::vector image_infos{
        t_image_views | std::views::transform([](const vk::UniqueImageView& image_view) {
            return vk::DescriptorImageInfo{
                .imageView   = image_view.get(),
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
            };
        })
        | std::ranges::to<std::vector>()
    };

    std::vector write_descriptor_sets{ std::views::iota(0u, image_infos.size())
                                       | std::views::transform([&](const uint32_t index) {
                                             return vk::WriteDescriptorSet{
                                                 .dstSet = descriptor_sets.front().get(),
                                                 .dstBinding      = 0,
                                                 .dstArrayElement = index,
                                                 .descriptorCount = 1,
                                                 .descriptorType =
                                                     vk::DescriptorType::eSampledImage,
                                                 .pImageInfo = &image_infos.at(index),
                                             };
                                         })
                                       | std::ranges::to<std::vector>() };

    t_device.updateDescriptorSets(
        static_cast<uint32_t>(write_descriptor_sets.size()),
        write_descriptor_sets.data(),
        0,
        nullptr
    );

    return std::move(descriptor_sets.front());
}

[[nodiscard]]
static auto to_mag_filter(gltf::Sampler::MagFilter t_mag_filter) noexcept -> vk::Filter
{
    using enum gltf::Sampler::MagFilter;
    switch (t_mag_filter) {
        case eNearest: return vk::Filter::eNearest;
        case eLinear: return vk::Filter::eLinear;
    }
}

[[nodiscard]]
static auto to_min_filter(gltf::Sampler::MinFilter t_min_filter) noexcept -> vk::Filter
{
    using enum gltf::Sampler::MinFilter;
    switch (t_min_filter) {
        case eNearest: return vk::Filter::eNearest;
        case eLinear: return vk::Filter::eLinear;
        case eNearestMipmapNearest: return vk::Filter::eNearest;
        case eLinearMipmapNearest: return vk::Filter::eLinear;
        case eNearestMipmapLinear: return vk::Filter::eNearest;
        case eLinearMipmapLinear: return vk::Filter::eLinear;
    }
}

[[nodiscard]]
static auto to_mipmap_mode(gltf::Sampler::MinFilter t_min_filter
) noexcept -> vk::SamplerMipmapMode
{
    using enum gltf::Sampler::MinFilter;
    switch (t_min_filter) {
        case eNearest: [[fallthrough]];
        case eLinear: return vk::SamplerMipmapMode::eLinear;
        case eNearestMipmapNearest: [[fallthrough]];
        case eLinearMipmapNearest: return vk::SamplerMipmapMode::eNearest;
        case eNearestMipmapLinear: [[fallthrough]];
        case eLinearMipmapLinear: return vk::SamplerMipmapMode::eLinear;
    }
}

[[nodiscard]]
static auto to_address_mode(gltf::Sampler::WrapMode t_wrap_mode
) noexcept -> vk::SamplerAddressMode
{
    using enum gltf::Sampler::WrapMode;
    switch (t_wrap_mode) {
        case eClampToEdge: return vk::SamplerAddressMode::eClampToEdge;
        case eMirroredRepeat: return vk::SamplerAddressMode::eMirroredRepeat;
        case eRepeat: return vk::SamplerAddressMode::eRepeat;
    }
}

[[nodiscard]]
static auto create_sampler(const vk::Device t_device, const gltf::Sampler& t_sampler_info)
    -> vk::UniqueSampler
{
    const vk::SamplerCreateInfo sampler_create_info{
        .magFilter = t_sampler_info.mag_filter.transform(to_mag_filter)
                         .value_or(vk::Filter::eLinear),
        .minFilter = t_sampler_info.min_filter.transform(to_min_filter)
                         .value_or(vk::Filter::eLinear),
        .mipmapMode = t_sampler_info.min_filter.transform(to_mipmap_mode)
                          .value_or(vk::SamplerMipmapMode::eLinear),
        .addressModeU = to_address_mode(t_sampler_info.wrap_s),
        .addressModeV = to_address_mode(t_sampler_info.wrap_t),
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .maxLod       = vk::LodClampNone,
    };

    return t_device.createSamplerUnique(sampler_create_info);
}

[[nodiscard]]
static auto create_sampler_descriptor_set(
    const vk::Device                      t_device,
    const vk::DescriptorSetLayout         t_descriptor_set_layout,
    const vk::DescriptorPool              t_descriptor_pool,
    const std::vector<vk::UniqueSampler>& t_samplers
) -> vk::UniqueDescriptorSet
{
    const uint32_t descriptor_count{ static_cast<uint32_t>(t_samplers.size()) };
    const vk::DescriptorSetVariableDescriptorCountAllocateInfo variable_info{
        .descriptorSetCount = 1,
        .pDescriptorCounts  = &descriptor_count,
    };
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .pNext              = &variable_info,
        .descriptorPool     = t_descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &t_descriptor_set_layout,
    };
    auto descriptor_sets{
        t_device.allocateDescriptorSetsUnique(descriptor_set_allocate_info)
    };

    const std::vector image_infos{
        t_samplers | std::views::transform([](const vk::UniqueSampler& sampler) {
            return vk::DescriptorImageInfo{
                .sampler = sampler.get(),
            };
        })
        | std::ranges::to<std::vector>()
    };

    std::vector write_descriptor_sets{ std::views::iota(0u, image_infos.size())
                                       | std::views::transform([&](const uint32_t index) {
                                             return vk::WriteDescriptorSet{
                                                 .dstSet = descriptor_sets.front().get(),
                                                 .dstBinding      = 0,
                                                 .dstArrayElement = index,
                                                 .descriptorCount = 1,
                                                 .descriptorType =
                                                     vk::DescriptorType::eSampler,
                                                 .pImageInfo = &image_infos.at(index),
                                             };
                                         })
                                       | std::ranges::to<std::vector>() };

    t_device.updateDescriptorSets(
        static_cast<uint32_t>(write_descriptor_sets.size()),
        write_descriptor_sets.data(),
        0,
        nullptr
    );

    return std::move(descriptor_sets.front());
}

[[nodiscard]]
static auto convert(gltf::Mesh::Primitive::Topology t_topology) -> vk::PrimitiveTopology
{
    using enum gltf::Mesh::Primitive::Topology;
    switch (t_topology) {
        case ePoints: return vk::PrimitiveTopology::ePointList;
        case eLineStrips: return vk::PrimitiveTopology::eLineStrip;
        case eLineLoops:
            throw std::runtime_error{ std::format(
                "Unsupported primitive topology: {}", std::to_underlying(eLineLoops)
            ) };
        case eLines: return vk::PrimitiveTopology::eLineList;
        case eTriangles: return vk::PrimitiveTopology::eTriangleList;
        case eTriangleStrips: return vk::PrimitiveTopology::eTriangleStrip;
        case eTriangleFans: return vk::PrimitiveTopology::eTriangleFan;
    }
}

[[nodiscard]]
static auto create_pipeline(
    const vk::Device                       t_device,
    const RenderModel::PipelineCreateInfo& t_create_info,
    const gltf::Mesh::Primitive            t_primitive,
    const gltf::Material                   t_material,
    cache::Cache&                          t_cache
) -> cache::Handle<vk::UniquePipeline>
{
    GraphicsPipelineBuilder builder{ t_create_info.effect };

    builder.set_layout(t_create_info.layout);
    builder.set_render_pass(t_create_info.render_pass);
    builder.set_primitive_topology(convert(t_primitive.mode));
    builder.set_cull_mode(
        t_material.double_sided ? vk::CullModeFlagBits::eNone : vk::CullModeFlagBits::eBack
    );
    if (t_material.alpha_mode == core::gltf::Material::AlphaMode::eBlend) {
        builder.enable_blending();
    }

    auto hash{ hash_value(builder) };

    return t_cache.find<vk::UniquePipeline>(hash).value_or(
        t_cache.emplace<vk::UniquePipeline>(hash, builder.build(t_device))
    );
}

static auto transition_image_layout(
    vk::CommandBuffer t_command_buffer,
    vk::Image         t_image,
    vk::ImageLayout   t_old_layout,
    vk::ImageLayout   t_new_layout
) -> void
{
    vk::ImageMemoryBarrier barrier{
        .oldLayout           = t_old_layout,
        .newLayout           = t_new_layout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = t_image,
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                      .baseMipLevel   = 0,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1 },
    };
    vk::PipelineStageFlags source_stage;
    vk::PipelineStageFlags destination_stage;

    if (t_old_layout == vk::ImageLayout::eUndefined
        && t_new_layout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eNone;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (t_old_layout == vk::ImageLayout::eTransferDstOptimal
             && t_new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        source_stage      = vk::PipelineStageFlagBits::eTransfer;
        destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    t_command_buffer.pipelineBarrier(
        source_stage,
        destination_stage,
        vk::DependencyFlags{},
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );
}

static auto copy_buffer_to_image(
    const vk::CommandBuffer t_command_buffer,
    const vk::Buffer        t_buffer,
    const vk::Image         t_image,
    const vk::DeviceSize    t_buffer_offset,
    const vk::Extent2D      t_extent
) -> void
{
    // TODO account for mip-maps
    // TODO account for image offset
    const vk::BufferImageCopy region{
        .bufferOffset = t_buffer_offset,
        .imageSubresource =
            vk::ImageSubresourceLayers{ .aspectMask = vk::ImageAspectFlagBits::eColor,
                                       .layerCount = 1 },
        .imageExtent = vk::Extent3D{ t_extent.width, t_extent.height, 1 },
    };

    t_command_buffer.copyBufferToImage(
        t_buffer, t_image, vk::ImageLayout::eTransferDstOptimal, 1, &region
    );
}

namespace core::renderer {

auto RenderModel::descriptor_set_count() noexcept -> uint32_t
{
    return 3;
}

auto RenderModel::descriptor_pool_sizes(const DescriptorSetLayoutCreateInfo& t_info
) -> std::vector<vk::DescriptorPoolSize>
{
    std::vector<vk::DescriptorPoolSize> pool_sizes{
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

    if (t_info.max_image_count > 0) {
        pool_sizes.push_back(vk::DescriptorPoolSize{
            .type            = vk::DescriptorType::eSampledImage,
            .descriptorCount = t_info.max_image_count,
        });
    }
    if (t_info.max_sampler_count > 0) {
        pool_sizes.push_back(vk::DescriptorPoolSize{
            .type            = vk::DescriptorType::eSampler,
            .descriptorCount = t_info.max_sampler_count,
        });
    }

    return pool_sizes;
}

auto RenderModel::create_loader(
    const vk::Device                                  t_device,
    const Allocator&                                  t_allocator,
    const std::span<const vk::DescriptorSetLayout, 3> t_descriptor_set_layouts,
    const PipelineCreateInfo&                         t_pipeline_create_info,
    const vk::DescriptorPool                          t_descriptor_pool,
    cache::Handle<gltf::Model>                        t_model,
    cache::Cache&                                     t_cache
) -> std::packaged_task<RenderModel(vk::CommandBuffer)>
{
    // TODO: handle model buffers with no elements

    MappedBuffer index_staging_buffer{
        create_staging_buffer(t_allocator, std::span{ t_model->indices() })
    };
    Buffer index_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eIndexBuffer,
        static_cast<uint32_t>(std::span{ t_model->indices() }.size_bytes())
    ) };

    std::vector<ShaderVertex> vertices{
        t_model->vertices()
        | std::views::transform([](const gltf::Model::Vertex& vertex) {
              return ShaderVertex{
                  .position = vertex.position,
                  .normal   = vertex.normal,
                  .UV0      = vertex.uv_0,
                  .UV1      = vertex.uv_1,
                  .color    = vertex.color,
              };
          })
        | std::ranges::to<std::vector>()
    };
    MappedBuffer vertex_staging_buffer{
        create_staging_buffer(t_allocator, std::span{ vertices }, sizeof(ShaderVertex))
    };
    Buffer       vertex_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ vertices }.size_bytes()),
        sizeof(ShaderVertex)
    ) };
    MappedBuffer vertex_uniform{ create_buffer<vk::DeviceAddress>(t_allocator) };

    std::vector nodes_with_mesh{
        t_model->nodes() | std::views::filter([](const gltf::Node& node) {
            return node.mesh_index.has_value();
        })
        | std::views::transform([](const gltf::Node& node) { return std::cref(node); })
        | std::ranges::to<std::vector>()
    };
    std::vector<glm::mat4> transforms(nodes_with_mesh.size());
    std::ranges::for_each(nodes_with_mesh, [&transforms](const gltf::Node& node) {
        transforms.at(node.mesh_index.value()) = node.matrix();
    });
    MappedBuffer transform_staging_buffer{
        create_staging_buffer(t_allocator, std::span{ transforms })
    };
    Buffer       transform_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ transforms }.size_bytes())
    ) };
    MappedBuffer transform_uniform{ create_buffer<vk::DeviceAddress>(t_allocator) };

    vk::UniqueSampler default_sampler{
        create_sampler(t_device, gltf::Model::default_sampler())
    };

    std::vector<ShaderTexture> textures{
        t_model->textures() | std::views::transform([](const gltf::Texture& texture) {
            return ShaderTexture{
                .image_index = texture.image_index,
                .sampler_index =
                    texture.sampler_index.value_or(std::numeric_limits<uint32_t>::max()),
            };
        })
        | std::ranges::to<std::vector>()
    };
    MappedBuffer texture_staging_buffer{
        create_staging_buffer(t_allocator, std::span{ textures })
    };
    Buffer       texture_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ textures }.size_bytes())
    ) };
    MappedBuffer texture_uniform{ create_buffer<vk::DeviceAddress>(t_allocator) };

    const ShaderMaterial default_material{ convert_material(gltf::Model::default_material(
    )) };
    MappedBuffer         default_material_uniform{ t_allocator.allocate_mapped_buffer(
        vk::BufferCreateInfo{
                    .size  = static_cast<uint32_t>(sizeof(ShaderMaterial)),
                    .usage = vk::BufferUsageFlagBits::eUniformBuffer,
        },
        &default_material
    ) };

    std::vector<ShaderMaterial> materials{ t_model->materials()
                                           | std::views::transform(convert_material)
                                           | std::ranges::to<std::vector>() };
    MappedBuffer                material_staging_buffer{
        create_staging_buffer(t_allocator, std::span{ materials }, sizeof(ShaderMaterial))
    };
    Buffer       material_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ materials }.size_bytes()),
        sizeof(ShaderMaterial)
    ) };
    MappedBuffer material_uniform{ create_buffer<vk::DeviceAddress>(t_allocator) };

    vk::UniqueDescriptorSet base_descriptor_set{ create_base_descriptor_set(
        t_device,
        t_descriptor_set_layouts[0],
        t_descriptor_pool,
        vertex_uniform.get(),
        transform_uniform.get(),
        default_sampler.get(),
        texture_uniform.get(),
        default_material_uniform.get(),
        material_uniform.get()
    ) };

    std::vector<vk::Extent2D> image_extents{
        t_model->images() | std::views::transform([&](const gltf::Image& image) {
            return vk::Extent2D{ image->width(), image->height() };
        })
        | std::ranges::to<std::vector>()
    };
    // TODO: account for mipmaps
    std::vector<uint64_t> image_offsets{
        t_model->images() | std::views::transform([&](const gltf::Image& image) {
            return image->offset(0, 0, 0);
        })
        | std::ranges::to<std::vector>()
    };
    std::vector<MappedBuffer> image_staging_buffers{
        t_model->images() | std::views::transform([&](const gltf::Image& image) {
            return create_staging_buffer(
                t_allocator, image->data(), static_cast<uint32_t>(image->size())
            );
        })
        | std::ranges::to<std::vector>()
    };
    std::vector<Image> images{ t_model->images()
                               | std::views::transform([&](const gltf::Image& image) {
                                     return create_image(
                                         t_allocator,
                                         image,
                                         vk::ImageTiling::eOptimal,
                                         vk::ImageUsageFlagBits::eTransferDst
                                             | vk::ImageUsageFlagBits::eSampled
                                     );
                                 })
                               | std::ranges::to<std::vector>() };
    std::vector<vk::UniqueImageView> image_views{
        std::views::zip(
            images | std::views::transform(&Image::get),
            t_model->images() | std::views::transform([](const gltf::Image& image) {
                return image->format();
            })
        )
        | std::views::transform([t_device](auto image_and_format) {
              return std::apply(
                  std::bind_front(create_image_view, t_device), image_and_format
              );
          })
        | std::ranges::to<std::vector>()
    };
    vk::UniqueDescriptorSet image_descriptor_set{ create_image_descriptor_set(
        t_device, t_descriptor_set_layouts[1], t_descriptor_pool, image_views
    ) };

    std::vector<vk::UniqueSampler> samplers{
        t_model->samplers()
        | std::views::transform(std::bind_front(create_sampler, t_device))
        | std::ranges::to<std::vector>()
    };
    vk::UniqueDescriptorSet sampler_descriptor_set{ create_sampler_descriptor_set(
        t_device, t_descriptor_set_layouts[2], t_descriptor_pool, samplers
    ) };

    std::vector<Mesh> meshes{
        t_model->meshes() | std::views::transform([&](const gltf::Mesh& mesh) {
            return Mesh{
                .primitives =
                    mesh.primitives
                    | std::views::transform([&](const gltf::Mesh::Primitive& primitive) {
                          return Mesh::Primitive{
                              .pipeline = create_pipeline(
                                  t_device,
                                  t_pipeline_create_info,
                                  primitive,
                                  primitive.material_index
                                      .transform([&t_model](const size_t material_index) {
                                          return t_model->materials().at(material_index);
                                      })
                                      .value_or(gltf::Model::default_material()),
                                  t_cache
                              ),
                              .material_index    = primitive.material_index,
                              .first_index_index = primitive.first_index_index,
                              .index_count       = primitive.index_count,
                              .vertex_count      = primitive.vertex_count,
                          };
                      })
                    | std::ranges::to<std::vector>()
            };
        })
        | std::ranges::to<std::vector>()
    };

    return std::packaged_task<RenderModel(vk::CommandBuffer)>{
        [device = t_device,
         index_buffer_size =
             static_cast<uint32_t>(std::span{ t_model->indices() }.size_bytes()),
         index_staging_buffer = auto{ std::move(index_staging_buffer) },
         index_buffer         = auto{ std::move(index_buffer) },
         vertex_buffer_size   = static_cast<uint32_t>(std::span{ vertices }.size_bytes()),
         vertex_staging_buffer = auto{ std::move(vertex_staging_buffer) },
         vertex_buffer         = auto{ std::move(vertex_buffer) },
         vertex_uniform        = auto{ std::move(vertex_uniform) },
         transform_buffer_size =
             static_cast<uint32_t>(std::span{ transforms }.size_bytes()),
         transform_staging_buffer = auto{ std::move(transform_staging_buffer) },
         transform_buffer         = auto{ std::move(transform_buffer) },
         transform_uniform        = auto{ std::move(transform_uniform) },
         default_sampler          = auto{ std::move(default_sampler) },
         texture_buffer_size = static_cast<uint32_t>(std::span{ textures }.size_bytes()),
         texture_staging_buffer   = auto{ std::move(texture_staging_buffer) },
         texture_buffer           = auto{ std::move(texture_buffer) },
         texture_uniform          = auto{ std::move(texture_uniform) },
         default_material_uniform = auto{ std::move(default_material_uniform) },
         material_buffer_size = static_cast<uint32_t>(std::span{ materials }.size_bytes()),
         material_staging_buffer = auto{ std::move(material_staging_buffer) },
         material_buffer         = auto{ std::move(material_buffer) },
         material_uniform        = auto{ std::move(material_uniform) },
         base_descriptor_set     = auto{ std::move(base_descriptor_set) },
         image_offsets           = auto{ std::move(image_offsets) },
         image_extents           = auto{ std::move(image_extents) },
         image_staging_buffers   = auto{ std::move(image_staging_buffers) },
         images                  = auto{ std::move(images) },
         image_views             = auto{ std::move(image_views) },
         image_descriptor_set    = auto{ std::move(image_descriptor_set) },
         samplers                = auto{ std::move(samplers) },
         sampler_descriptor_set  = auto{ std::move(sampler_descriptor_set) },
         meshes                  = auto{ std::move(meshes
         ) }](const vk::CommandBuffer t_transfer_command_buffer) mutable -> RenderModel {
            t_transfer_command_buffer.copyBuffer(
                index_staging_buffer.get(),
                index_buffer.get(),
                std::array{ vk::BufferCopy{ .size = index_buffer_size } }
            );

            t_transfer_command_buffer.copyBuffer(
                vertex_staging_buffer.get(),
                vertex_buffer.get(),
                std::array{ vk::BufferCopy{ .size = vertex_buffer_size } }
            );

            t_transfer_command_buffer.copyBuffer(
                transform_staging_buffer.get(),
                transform_buffer.get(),
                std::array{ vk::BufferCopy{ .size = transform_buffer_size } }
            );

            if (texture_buffer_size > 0) {
                t_transfer_command_buffer.copyBuffer(
                    texture_staging_buffer.get(),
                    texture_buffer.get(),
                    std::array{ vk::BufferCopy{ .size = texture_buffer_size } }
                );
            }

            if (material_buffer_size > 0) {
                t_transfer_command_buffer.copyBuffer(
                    material_staging_buffer.get(),
                    material_buffer.get(),
                    std::array{ vk::BufferCopy{ .size = material_buffer_size } }
                );
            }

            for (auto&& [buffer, texture_image, offset, extent] : std::views::zip(
                     image_staging_buffers, images, image_offsets, image_extents
                 ))
            {
                transition_image_layout(
                    t_transfer_command_buffer,
                    texture_image.get(),
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eTransferDstOptimal
                );
                copy_buffer_to_image(
                    t_transfer_command_buffer,
                    buffer.get(),
                    texture_image.get(),
                    offset,
                    extent
                );
                transition_image_layout(
                    t_transfer_command_buffer,
                    texture_image.get(),
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal
                );
            }

            return RenderModel{ device,
                                std::move(index_buffer),
                                std::move(vertex_buffer),
                                std::move(vertex_uniform),
                                std::move(transform_buffer),
                                std::move(transform_uniform),
                                std::move(default_sampler),
                                std::move(texture_buffer),
                                std::move(texture_uniform),
                                std::move(default_material_uniform),
                                std::move(material_buffer),
                                std::move(material_uniform),
                                std::move(base_descriptor_set),
                                std::move(images),
                                std::move(image_views),
                                std::move(image_descriptor_set),
                                std::move(samplers),
                                std::move(sampler_descriptor_set),
                                std::move(meshes) };
        }
    };
}

auto RenderModel::create_descriptor_set_layouts(
    const vk::Device                     t_device,
    const DescriptorSetLayoutCreateInfo& t_info
) -> std::array<vk::UniqueDescriptorSetLayout, 3>
{
    return ::create_descriptor_set_layouts(t_device, t_info);
}

auto RenderModel::push_constant_range() noexcept -> vk::PushConstantRange
{
    return vk::PushConstantRange{
        .stageFlags = vk::ShaderStageFlagBits::eVertex
                    | vk::ShaderStageFlagBits::eFragment,
        .size = sizeof(PushConstants),
    };
}

auto RenderModel::draw(
    vk::CommandBuffer  t_graphics_command_buffer,
    vk::PipelineLayout t_pipeline_layout
) const noexcept -> void
{
    t_graphics_command_buffer.bindIndexBuffer(
        m_index_buffer.get(), 0, vk::IndexType::eUint32
    );

    t_graphics_command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        t_pipeline_layout,
        1,
        std::array{ m_base_descriptor_set.get(),
                    m_image_descriptor_set.get(),
                    m_sampler_descriptor_set.get() },
        nullptr
    );

    for (const auto& [mesh, mesh_index] :
         std::views::zip(m_meshes, std::views::iota(0u, m_meshes.size())))
    {
        for (const auto& primitive : mesh.primitives) {
            t_graphics_command_buffer.bindPipeline(
                vk::PipelineBindPoint::eGraphics, primitive.pipeline.get()->get()
            );

            PushConstants push_constants{
                .transform_index = mesh_index,
                .material_index  = primitive.material_index.value_or(
                    std::numeric_limits<uint32_t>::max()
                ),
            };
            t_graphics_command_buffer.pushConstants(
                t_pipeline_layout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(PushConstants),
                &push_constants
            );

            t_graphics_command_buffer.drawIndexed(
                primitive.index_count, 1, primitive.first_index_index, 0, 0
            );
        }
    }
}

RenderModel::RenderModel(
    vk::Device                         t_device,
    Buffer&&                           t_index_buffer,
    Buffer&&                           t_vertex_buffer,
    MappedBuffer&&                     t_vertex_uniform,
    Buffer&&                           t_transform_buffer,
    MappedBuffer&&                     t_transform_uniform,
    vk::UniqueSampler&&                t_default_sampler,
    Buffer&&                           t_texture_buffer,
    MappedBuffer&&                     t_texture_uniform,
    MappedBuffer&&                     t_default_material_uniform,
    Buffer&&                           t_material_buffer,
    MappedBuffer&&                     t_material_uniform,
    vk::UniqueDescriptorSet&&          t_base_descriptor_set,
    std::vector<Image>&&               t_images,
    std::vector<vk::UniqueImageView>&& t_image_views,
    vk::UniqueDescriptorSet&&          t_image_descriptor_set,
    std::vector<vk::UniqueSampler>&&   t_samplers,
    vk::UniqueDescriptorSet&&          t_sampler_descriptor_set,
    std::vector<Mesh>&&                t_meshes
)
    : m_index_buffer{ std::move(t_index_buffer) },
      m_vertex_buffer{ std::move(t_vertex_buffer) },
      m_vertex_uniform{ std::move(t_vertex_uniform) },
      m_transform_buffer{ std::move(t_transform_buffer) },
      m_transform_uniform{ std::move(t_transform_uniform) },
      m_default_sampler{ std::move(t_default_sampler) },
      m_texture_buffer{ std::move(t_texture_buffer) },
      m_texture_uniform{ std::move(t_texture_uniform) },
      m_default_material_uniform{ std::move(t_default_material_uniform) },
      m_material_buffer{ std::move(t_material_buffer) },
      m_material_uniform{ std::move(t_material_uniform) },
      m_base_descriptor_set{ std::move(t_base_descriptor_set) },
      m_images{ std::move(t_images) },
      m_image_views{ std::move(t_image_views) },
      m_image_descriptor_set{ std::move(t_image_descriptor_set) },
      m_samplers{ std::move(t_samplers) },
      m_sampler_descriptor_set{ std::move(t_sampler_descriptor_set) },
      m_meshes{ std::move(t_meshes) }
{
    m_vertex_buffer_address = t_device.getBufferAddress(vk::BufferDeviceAddressInfo{
        .buffer = m_vertex_buffer.get(),
    });
    m_vertex_uniform.set(m_vertex_buffer_address);

    m_transform_buffer_address = t_device.getBufferAddress(vk::BufferDeviceAddressInfo{
        .buffer = m_transform_buffer.get(),
    });
    m_transform_uniform.set(m_transform_buffer_address);

    if (m_texture_buffer.get()) {
        m_texture_buffer_address = t_device.getBufferAddress(vk::BufferDeviceAddressInfo{
            .buffer = m_texture_buffer.get(),
        });
    }
    else {
        m_texture_buffer_address = vk::DeviceAddress{};
    }
    m_texture_uniform.set(m_texture_buffer_address);

    if (m_material_buffer.get()) {
        m_material_buffer_address = t_device.getBufferAddress(vk::BufferDeviceAddressInfo{
            .buffer = m_material_buffer.get(),
        });
    }
    else {
        m_material_buffer_address = vk::DeviceAddress{};
    }
    m_material_uniform.set(m_material_buffer_address);
}

}   // namespace core::renderer
