#include "RenderModel.hpp"

#include <ranges>
#include <source_location>

#include <spdlog/spdlog.h>

#include "core/renderer/base/memory/Image.hpp"
#include "core/renderer/material_system/GraphicsPipelineBuilder.hpp"
#include "core/renderer/model/ModelLayout.hpp"

using namespace core;
using namespace core::gltf;

namespace {

struct ShaderVertex {
    glm::vec3 position;
    glm::vec3 normal;
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

}   // namespace

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

template <typename T>
[[nodiscard]]
static auto create_staging_buffer(
    const renderer::base::Allocator& allocator,
    const std::span<T>               data
) -> renderer::base::SeqWriteBuffer<std::remove_const_t<T>>
{
    if (data.empty()) {
        return renderer::base::SeqWriteBuffer<std::remove_const_t<T>>{};
    }

    const vk::BufferCreateInfo staging_buffer_create_info{
        .size  = data.size_bytes(),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    return allocator.allocate_seq_write_buffer<std::remove_const_t<T>>(
        staging_buffer_create_info, data.data()
    );
}

[[nodiscard]]
static auto create_gpu_only_buffer(
    const renderer::base::Allocator& allocator,
    const vk::BufferUsageFlags       usage_flags,
    const uint32_t                   size
) -> renderer::base::Buffer
{
    if (size == 0) {
        return renderer::base::Buffer{};
    }

    const vk::BufferCreateInfo buffer_create_info = {
        .size = size, .usage = usage_flags | vk::BufferUsageFlagBits::eTransferDst
    };

    return allocator.allocate_buffer(buffer_create_info);
}

template <typename UniformBlock>
[[nodiscard]]
static auto create_buffer(const renderer::base::Allocator& allocator)
    -> renderer::base::RandomAccessBuffer<UniformBlock>
{
    constexpr static vk::BufferCreateInfo buffer_create_info = {
        .size = sizeof(UniformBlock), .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    return allocator.allocate_random_access_buffer<UniformBlock>(buffer_create_info);
}

[[nodiscard]]
static auto convert_material(const Material& material) noexcept -> ShaderMaterial
{
    return ShaderMaterial{
        .pbrMetallicRoughness =
            ShaderPbrMetallicRoughness{
                                       .baseColorFactor = material.pbr_metallic_roughness.base_color_factor,
                                       .baseColorTexture =
                    ShaderTextureInfo{
                        .index = material.pbr_metallic_roughness.base_color_texture_info
                                     .transform([](const TextureInfo& texture_info) {
                                         return texture_info.texture_index;
                                     })
                                     .value_or(std::numeric_limits<uint32_t>::max()),
                        .texCoord =
                            material.pbr_metallic_roughness.base_color_texture_info
                                .transform([](const TextureInfo& texture_info) {
                                    return texture_info.tex_coord_index;
                                })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                    }, .metallicFactor  = material.pbr_metallic_roughness.metallic_factor,
                                       .roughnessFactor = material.pbr_metallic_roughness.roughness_factor,
                                       .metallicRoughnessTexture =
                    ShaderTextureInfo{
                        .index = material.pbr_metallic_roughness
                                     .metallic_roughness_texture_info
                                     .transform([](const TextureInfo& texture_info) {
                                         return texture_info.texture_index;
                                     })
                                     .value_or(std::numeric_limits<uint32_t>::max()),
                        .texCoord = material.pbr_metallic_roughness
                                        .metallic_roughness_texture_info
                                        .transform([](const TextureInfo& texture_info) {
                                            return texture_info.tex_coord_index;
                                        })
                                        .value_or(std::numeric_limits<uint32_t>::max()),
                    }, },
        .normalTexture =
            ShaderNormalTextureInfo{
                                       .index = material.normal_texture_info
                             .transform([](const Material::NormalTextureInfo& texture_info
                                        ) { return texture_info.texture_index; })
                             .value_or(std::numeric_limits<uint32_t>::max()),
                                       .texCoord =
                    material.normal_texture_info
                        .transform([](const Material::NormalTextureInfo& texture_info) {
                            return texture_info.tex_coord_index;
                        })
                        .value_or(std::numeric_limits<uint32_t>::max()),
                                       .scale = material.normal_texture_info
                             .transform([](const Material::NormalTextureInfo& texture_info
                                        ) { return texture_info.scale; })
                             .value_or(1.f),
                                       },
        .occlusionTexture =
            ShaderOcclusionTextureInfo{
                                       .index =
                    material.occlusion_texture_info
                        .transform([](const Material::OcclusionTextureInfo& texture_info
                                   ) { return texture_info.texture_index; })
                        .value_or(std::numeric_limits<uint32_t>::max()),
                                       .texCoord =
                    material.occlusion_texture_info
                        .transform([](const Material::OcclusionTextureInfo& texture_info
                                   ) { return texture_info.tex_coord_index; })
                        .value_or(std::numeric_limits<uint32_t>::max()),
                                       .strength =
                    material.occlusion_texture_info
                        .transform([](const Material::OcclusionTextureInfo& texture_info
                                   ) { return texture_info.strength; })
                        .value_or(1.f),
                                       },
        .emissiveTexture =
            ShaderTextureInfo{
                                       .index = material.emissive_texture_info
                             .transform([](const TextureInfo& texture_info) {
                                 return texture_info.texture_index;
                             })
                             .value_or(std::numeric_limits<uint32_t>::max()),
                                       .texCoord = material.emissive_texture_info
                                .transform([](const TextureInfo& texture_info) {
                                    return texture_info.tex_coord_index;
                                })
                                .value_or(std::numeric_limits<uint32_t>::max()),
                                       },
        .emissiveFactor = material.emissive_factor,
        .alphaCutoff    = material.alpha_mode == Material::AlphaMode::eMask
                            ? material.alpha_cutoff
                            : -1.f,
    };
}

[[nodiscard]]
static auto create_base_descriptor_set(
    const vk::Device              device,
    const vk::DescriptorSetLayout descriptor_set_layout,
    const vk::DescriptorPool      descriptor_pool,
    const renderer::base::Buffer& vertex_uniform,
    const renderer::base::Buffer& transform_uniform,
    const vk::Sampler             default_sampler,
    const renderer::base::Buffer& texture_uniform,
    const renderer::base::Buffer& default_material_uniform,
    const renderer::base::Buffer& material_uniform
) -> vk::UniqueDescriptorSet
{
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &descriptor_set_layout,
    };
    auto descriptor_sets{ device.allocateDescriptorSetsUnique(descriptor_set_allocate_info
    ) };

    const vk::DescriptorBufferInfo vertex_buffer_info{
        .buffer = vertex_uniform.get(),
        .range  = vertex_uniform.size_bytes(),
    };
    const vk::DescriptorBufferInfo transform_buffer_info{
        .buffer = transform_uniform.get(),
        .range  = transform_uniform.size_bytes(),
    };
    const vk::DescriptorImageInfo default_sampler_image_info{
        .sampler = default_sampler,
    };
    const vk::DescriptorBufferInfo texture_buffer_info{
        .buffer = texture_uniform.get(),
        .range  = texture_uniform.size_bytes(),
    };
    const vk::DescriptorBufferInfo default_material_buffer_info{
        .buffer = default_material_uniform.get(),
        .range  = default_material_uniform.size_bytes(),
    };
    const vk::DescriptorBufferInfo material_buffer_info{
        .buffer = material_uniform.get(),
        .range  = material_uniform.size_bytes(),
    };

    const std::array write_descriptor_sets{
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

    device.updateDescriptorSets(
        write_descriptor_sets.size(), write_descriptor_sets.data(), 0, nullptr
    );

    return std::move(descriptor_sets.front());
}

[[nodiscard]]
static auto create_image(
    const renderer::base::Allocator& allocator,
    const Image&                     image,
    const vk::ImageTiling            tiling,
    const vk::ImageUsageFlags        usage
) -> renderer::base::Image
{
    const vk::ImageCreateInfo image_create_info{
        .imageType     = vk::ImageType::e2D,
        .format        = image->format(),
        .extent        = vk::Extent3D{ .width  = image->width(),
                                      .height = image->height(),
                                      .depth  = image->depth() },
        .mipLevels     = image->mip_levels(),
        .arrayLayers   = 1,
        .samples       = vk::SampleCountFlagBits::e1,
        .tiling        = tiling,
        .usage         = usage,
        .sharingMode   = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage    = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.f,
    };

    return allocator.allocate_image(image_create_info, allocation_create_info);
}

[[nodiscard]]
static auto create_image_view(
    const vk::Device device,
    const vk::Image  image,
    const vk::Format format
) -> vk::UniqueImageView
{
    const vk::ImageViewCreateInfo view_create_info{
        .image    = image,
        .viewType = vk::ImageViewType::e2D,
        .format   = format,
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask = vk::ImageAspectFlagBits::eColor,
                                      .levelCount = 1,
                                      .layerCount = 1 },
    };

    return device.createImageViewUnique(view_create_info);
}

[[nodiscard]]
static auto create_image_descriptor_set(
    const vk::Device                        device,
    const vk::DescriptorSetLayout           descriptor_set_layout,
    const vk::DescriptorPool                descriptor_pool,
    const std::vector<vk::UniqueImageView>& image_views
) -> vk::UniqueDescriptorSet
{
    const uint32_t descriptor_count{ static_cast<uint32_t>(image_views.size()) };
    const vk::DescriptorSetVariableDescriptorCountAllocateInfo variable_info{
        .descriptorSetCount = 1,
        .pDescriptorCounts  = &descriptor_count,
    };
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .pNext              = &variable_info,
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &descriptor_set_layout,
    };
    auto descriptor_sets{ device.allocateDescriptorSetsUnique(descriptor_set_allocate_info
    ) };

    const std::vector image_infos{
        image_views | std::views::transform([](const vk::UniqueImageView& image_view) {
            return vk::DescriptorImageInfo{
                .imageView   = image_view.get(),
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
            };
        })
        | std::ranges::to<std::vector>()
    };

    const std::vector write_descriptor_sets{
        std::views::iota(0u, image_infos.size())
        | std::views::transform([&](const uint32_t index) {
              return vk::WriteDescriptorSet{
                  .dstSet          = descriptor_sets.front().get(),
                  .dstBinding      = 0,
                  .dstArrayElement = index,
                  .descriptorCount = 1,
                  .descriptorType  = vk::DescriptorType::eSampledImage,
                  .pImageInfo      = &image_infos.at(index),
              };
          })
        | std::ranges::to<std::vector>()
    };

    device.updateDescriptorSets(
        static_cast<uint32_t>(write_descriptor_sets.size()),
        write_descriptor_sets.data(),
        0,
        nullptr
    );

    return std::move(descriptor_sets.front());
}

[[nodiscard]]
static auto to_mag_filter(const Sampler::MagFilter mag_filter) noexcept -> vk::Filter
{
    using enum Sampler::MagFilter;
    switch (mag_filter) {
        case eNearest: return vk::Filter::eNearest;
        case eLinear:  return vk::Filter::eLinear;
    }
    std::unreachable();
}

[[nodiscard]]
static auto to_min_filter(const Sampler::MinFilter min_filter) noexcept -> vk::Filter
{
    using enum Sampler::MinFilter;
    switch (min_filter) {
        case eNearest:              return vk::Filter::eNearest;
        case eLinear:               return vk::Filter::eLinear;
        case eNearestMipmapNearest: return vk::Filter::eNearest;
        case eLinearMipmapNearest:  return vk::Filter::eLinear;
        case eNearestMipmapLinear:  return vk::Filter::eNearest;
        case eLinearMipmapLinear:   return vk::Filter::eLinear;
    }
    std::unreachable();
}

[[nodiscard]]
static auto to_mipmap_mode(const Sampler::MinFilter min_filter) noexcept
    -> vk::SamplerMipmapMode
{
    using enum Sampler::MinFilter;
    switch (min_filter) {
        case eNearest:              [[fallthrough]];
        case eLinear:               return vk::SamplerMipmapMode::eLinear;
        case eNearestMipmapNearest: [[fallthrough]];
        case eLinearMipmapNearest:  return vk::SamplerMipmapMode::eNearest;
        case eNearestMipmapLinear:  [[fallthrough]];
        case eLinearMipmapLinear:   return vk::SamplerMipmapMode::eLinear;
    }
    std::unreachable();
}

[[nodiscard]]
static auto to_address_mode(const Sampler::WrapMode wrap_mode) noexcept
    -> vk::SamplerAddressMode
{
    using enum Sampler::WrapMode;
    switch (wrap_mode) {
        case eClampToEdge:    return vk::SamplerAddressMode::eClampToEdge;
        case eMirroredRepeat: return vk::SamplerAddressMode::eMirroredRepeat;
        case eRepeat:         return vk::SamplerAddressMode::eRepeat;
    }
    std::unreachable();
}

[[nodiscard]]
static auto create_sampler(const vk::Device device, const Sampler& sampler_info)
    -> vk::UniqueSampler
{
    const vk::SamplerCreateInfo sampler_create_info{
        .magFilter    = ::to_mag_filter(sampler_info.mag_filter),
        .minFilter    = ::to_min_filter(sampler_info.min_filter),
        .mipmapMode   = ::to_mipmap_mode(sampler_info.min_filter),
        .addressModeU = ::to_address_mode(sampler_info.wrap_s),
        .addressModeV = ::to_address_mode(sampler_info.wrap_t),
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .maxLod       = vk::LodClampNone,
    };

    return device.createSamplerUnique(sampler_create_info);
}

[[nodiscard]]
static auto create_sampler_descriptor_set(
    const vk::Device                      device,
    const vk::DescriptorSetLayout         descriptor_set_layout,
    const vk::DescriptorPool              descriptor_pool,
    const std::vector<vk::UniqueSampler>& samplers
) -> vk::UniqueDescriptorSet
{
    const uint32_t descriptor_count{ static_cast<uint32_t>(samplers.size()) };
    const vk::DescriptorSetVariableDescriptorCountAllocateInfo variable_info{
        .descriptorSetCount = 1,
        .pDescriptorCounts  = &descriptor_count,
    };
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .pNext              = &variable_info,
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &descriptor_set_layout,
    };
    auto descriptor_sets{ device.allocateDescriptorSetsUnique(descriptor_set_allocate_info
    ) };

    const std::vector image_infos{
        samplers | std::views::transform([](const vk::UniqueSampler& sampler) {
            return vk::DescriptorImageInfo{
                .sampler = sampler.get(),
            };
        })
        | std::ranges::to<std::vector>()
    };

    const std::vector write_descriptor_sets{
        std::views::iota(0u, image_infos.size())
        | std::views::transform([&](const uint32_t index) {
              return vk::WriteDescriptorSet{
                  .dstSet          = descriptor_sets.front().get(),
                  .dstBinding      = 0,
                  .dstArrayElement = index,
                  .descriptorCount = 1,
                  .descriptorType  = vk::DescriptorType::eSampler,
                  .pImageInfo      = &image_infos.at(index),
              };
          })
        | std::ranges::to<std::vector>()
    };

    device.updateDescriptorSets(
        static_cast<uint32_t>(write_descriptor_sets.size()),
        write_descriptor_sets.data(),
        0,
        nullptr
    );

    return std::move(descriptor_sets.front());
}

[[nodiscard]]
static auto convert(const Mesh::Primitive::Topology topology) -> vk::PrimitiveTopology
{
    using enum Mesh::Primitive::Topology;
    switch (topology) {
        case ePoints:     return vk::PrimitiveTopology::ePointList;
        case eLineStrips: return vk::PrimitiveTopology::eLineStrip;
        case eLineLoops:
            throw std::runtime_error{ std::format(
                "Unsupported primitive topology: {}", std::to_underlying(eLineLoops)
            ) };
        case eLines:          return vk::PrimitiveTopology::eLineList;
        case eTriangles:      return vk::PrimitiveTopology::eTriangleList;
        case eTriangleStrips: return vk::PrimitiveTopology::eTriangleStrip;
        case eTriangleFans:   return vk::PrimitiveTopology::eTriangleFan;
    }
    std::unreachable();
}

[[nodiscard]]
static auto create_program(const vk::Device device, cache::Cache& cache)
    -> renderer::Program
{
    static const std::filesystem::path shader_path{
        std::filesystem::path{ std::source_location::current().file_name() }.parent_path()
        / "shaders"
    };
    static const std::filesystem::path vertex_shader_path{ shader_path
                                                           / "model.vert.spv" };
    static const std::filesystem::path fragment_shader_path{ shader_path
                                                             / "pbr.frag.spv" };

    return renderer::Program{
        renderer::Shader{ cache.lazy_emplace<const renderer::ShaderModule>(
            renderer::ShaderModule::hash(vertex_shader_path),
            [device]() { return renderer::ShaderModule::load(device, vertex_shader_path); }
        ) },
        renderer::Shader{ cache.lazy_emplace<const renderer::ShaderModule>(
            renderer::ShaderModule::hash(fragment_shader_path),
            [device]() {
                return renderer::ShaderModule::load(device, fragment_shader_path);
            }
        ) }
    };
}

[[nodiscard]]
static auto create_pipeline(
    const vk::Device                       device,
    const RenderModel::PipelineCreateInfo& create_info,
    const Mesh::Primitive                  primitive,
    const Material                         material,
    cache::Cache&                          cache
) -> cache::Handle<vk::UniquePipeline>
{
    renderer::GraphicsPipelineBuilder builder{ ::create_program(device, cache) };

    builder.set_layout(create_info.layout);
    builder.set_render_pass(create_info.render_pass);
    builder.set_primitive_topology(::convert(primitive.mode));
    builder.set_cull_mode(
        material.double_sided ? vk::CullModeFlagBits::eNone : vk::CullModeFlagBits::eBack
    );
    if (material.alpha_mode == Material::AlphaMode::eBlend) {
        builder.enable_blending();
    }

    const size_t hash{ hash_value(builder) };

    return cache.lazy_emplace<vk::UniquePipeline>(
        hash, std::bind_front(&renderer::GraphicsPipelineBuilder::build, builder, device)
    );
}

static auto transition_image_layout(
    const vk::CommandBuffer command_buffer,
    const vk::Image         image,
    const vk::ImageLayout   old_layout,
    const vk::ImageLayout   new_layout
) -> void
{
    vk::ImageMemoryBarrier barrier{
        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image,
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                      .baseMipLevel   = 0,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1 },
    };
    vk::PipelineStageFlags source_stage;
    vk::PipelineStageFlags destination_stage;

    if (old_layout == vk::ImageLayout::eUndefined
        && new_layout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eNone;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        source_stage      = vk::PipelineStageFlagBits::eTransfer;
        destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    command_buffer.pipelineBarrier(
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
    const vk::CommandBuffer command_buffer,
    const vk::Buffer        buffer,
    const vk::Image         image,
    const vk::DeviceSize    buffer_offset,
    const vk::Extent3D&     extent
) -> void
{
    // TODO account for mip-maps
    // TODO account for base layer
    const vk::BufferImageCopy region{
        .bufferOffset = buffer_offset,
        .imageSubresource =
            vk::ImageSubresourceLayers{ .aspectMask = vk::ImageAspectFlagBits::eColor,
                                       .layerCount = 1 },
        .imageExtent = extent,
    };

    command_buffer.copyBufferToImage(
        buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region
    );
}

namespace core::gltf {

auto RenderModel::create_loader(
    const vk::Device                                  device,
    const renderer::base::Allocator&                  allocator,
    const std::span<const vk::DescriptorSetLayout, 3> descriptor_set_layouts,
    const PipelineCreateInfo&                         pipeline_create_info,
    const vk::DescriptorPool                          descriptor_pool,
    const Model&                                      model,
    cache::Cache&                                     cache
) -> std::packaged_task<RenderModel(vk::CommandBuffer)>
{
    // TODO: handle model buffers with no elements

    renderer::base::SeqWriteBuffer<uint32_t> index_staging_buffer{
        ::create_staging_buffer(allocator, std::span{ model.indices() })
    };
    renderer::base::Buffer index_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eIndexBuffer,
        static_cast<uint32_t>(std::span{ model.indices() }.size_bytes())
    ) };

    std::vector<ShaderVertex> vertices{
        model.vertices() | std::views::transform([](const Model::Vertex& vertex) {
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
    renderer::base::SeqWriteBuffer<ShaderVertex> vertex_staging_buffer{
        ::create_staging_buffer(allocator, std::span{ vertices })
    };
    renderer::base::Buffer vertex_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ vertices }.size_bytes())
    ) };
    renderer::base::RandomAccessBuffer<vk::DeviceAddress> vertex_uniform{
        ::create_buffer<vk::DeviceAddress>(allocator)
    };

    std::vector nodes_with_mesh{ model.nodes() | std::views::filter([](const Node& node) {
                                     return node.mesh_index().has_value();
                                 })
                                 | std::views::transform([](const Node& node) {
                                       return std::cref(node);
                                   })
                                 | std::ranges::to<std::vector>() };
    std::vector<glm::mat4> transforms(nodes_with_mesh.size());
    std::ranges::for_each(
        nodes_with_mesh,
        [&transforms, model = std::cref(model)](const Node& node) {
            transforms.at(node.mesh_index().value()) = node.matrix(model);
        }
    );
    renderer::base::SeqWriteBuffer<glm::mat4> transform_staging_buffer{
        ::create_staging_buffer(allocator, std::span{ transforms })
    };
    renderer::base::Buffer transform_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ transforms }.size_bytes())
    ) };
    renderer::base::RandomAccessBuffer<vk::DeviceAddress> transform_uniform{
        ::create_buffer<vk::DeviceAddress>(allocator)
    };

    vk::UniqueSampler default_sampler{
        ::create_sampler(device, Model::default_sampler())
    };

    std::vector<ShaderTexture> textures{
        model.textures() | std::views::transform([](const Texture& texture) {
            return ShaderTexture{
                .image_index = texture.image_index,
                .sampler_index =
                    texture.sampler_index.value_or(std::numeric_limits<uint32_t>::max()),
            };
        })
        | std::ranges::to<std::vector>()
    };
    renderer::base::SeqWriteBuffer<ShaderTexture> texture_staging_buffer{
        ::create_staging_buffer(allocator, std::span{ textures })
    };
    renderer::base::Buffer texture_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ textures }.size_bytes())
    ) };
    renderer::base::RandomAccessBuffer<vk::DeviceAddress> texture_uniform{
        ::create_buffer<vk::DeviceAddress>(allocator)
    };

    const ShaderMaterial default_material{ ::convert_material(Model::default_material()) };
    renderer::base::RandomAccessBuffer<ShaderMaterial> default_material_uniform{
        allocator.allocate_random_access_buffer<ShaderMaterial>(
            vk::BufferCreateInfo{
                .size  = static_cast<uint32_t>(sizeof(ShaderMaterial)),
                .usage = vk::BufferUsageFlagBits::eUniformBuffer,
            },
            &default_material
        )
    };

    std::vector<ShaderMaterial>                    materials{ model.materials()
                                           | std::views::transform(::convert_material)
                                           | std::ranges::to<std::vector>() };
    renderer::base::SeqWriteBuffer<ShaderMaterial> material_staging_buffer{
        ::create_staging_buffer(allocator, std::span{ materials })
    };
    renderer::base::Buffer material_buffer{ ::create_gpu_only_buffer(
        allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ materials }.size_bytes())
    ) };
    renderer::base::RandomAccessBuffer<vk::DeviceAddress> material_uniform{
        ::create_buffer<vk::DeviceAddress>(allocator)
    };

    vk::UniqueDescriptorSet base_descriptor_set{ ::create_base_descriptor_set(
        device,
        descriptor_set_layouts[0],
        descriptor_pool,
        vertex_uniform,
        transform_uniform,
        default_sampler.get(),
        texture_uniform,
        default_material_uniform,
        material_uniform
    ) };

    std::vector<vk::Extent3D> image_extents{
        model.images() | std::views::transform([&](const Image& image) {
            return vk::Extent3D{ image->width(), image->height(), image->depth() };
        })
        | std::ranges::to<std::vector>()
    };
    std::vector<uint64_t> image_offsets{ model.images()
                                         | std::views::transform([&](const Image& image) {
                                               return image->offset(0, 0, 0);
                                           })
                                         | std::ranges::to<std::vector>() };
    std::vector<renderer::base::SeqWriteBuffer<>> image_staging_buffers{
        model.images() | std::views::transform([&](const Image& image) {
            return ::create_staging_buffer(allocator, image->data());
        })
        | std::ranges::to<std::vector>()
    };
    std::vector<renderer::base::Image> images{
        model.images() | std::views::transform([&](const Image& image) {
            return ::create_image(
                allocator,
                image,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
            );
        })
        | std::ranges::to<std::vector>()
    };
    std::vector<vk::UniqueImageView> image_views{
        std::views::zip(
            images | std::views::transform(&renderer::base::Image::get),
            model.images(
            ) | std::views::transform([](const Image& image) { return image->format(); })
        )
        | std::views::transform([device](auto image_and_format) {
              return std::apply(
                  std::bind_front(::create_image_view, device), image_and_format
              );
          })
        | std::ranges::to<std::vector>()
    };
    vk::UniqueDescriptorSet image_descriptor_set{ ::create_image_descriptor_set(
        device, descriptor_set_layouts[1], descriptor_pool, image_views
    ) };

    std::vector<vk::UniqueSampler> samplers{
        model.samplers()
        | std::views::transform(std::bind_front(::create_sampler, device))
        | std::ranges::to<std::vector>()
    };
    vk::UniqueDescriptorSet sampler_descriptor_set{ ::create_sampler_descriptor_set(
        device, descriptor_set_layouts[2], descriptor_pool, samplers
    ) };

    std::vector<Mesh> meshes{
        model.meshes() | std::views::transform([&](const gltf::Mesh& mesh) {
            return Mesh{
                .primitives =
                    mesh.primitives
                    | std::views::transform([&](const gltf::Mesh::Primitive& primitive) {
                          return Mesh::Primitive{
                              .pipeline = ::create_pipeline(
                                  device,
                                  pipeline_create_info,
                                  primitive,
                                  primitive.material_index
                                      .transform([&model](const size_t material_index) {
                                          return model.materials().at(material_index);
                                      })
                                      .value_or(Model::default_material()),
                                  cache
                              ),
                              .material_index    = primitive.material_index,
                              .first_index_index = primitive.first_index_index,
                              .index_count       = primitive.index_count,
                          };
                      })
                    | std::ranges::to<std::vector>()
            };
        })
        | std::ranges::to<std::vector>()
    };

    return std::packaged_task{
        [device = device,
         index_buffer_size =
             static_cast<uint32_t>(std::span{ model.indices() }.size_bytes()),
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
                ::transition_image_layout(
                    t_transfer_command_buffer,
                    texture_image.get(),
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eTransferDstOptimal
                );
                ::copy_buffer_to_image(
                    t_transfer_command_buffer,
                    buffer.get(),
                    texture_image.get(),
                    offset,
                    extent
                );
                ::transition_image_layout(
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

auto RenderModel::draw(
    const vk::CommandBuffer  graphics_command_buffer,
    const vk::PipelineLayout pipeline_layout
) const noexcept -> void
{
    graphics_command_buffer.bindIndexBuffer(
        m_index_buffer.get(), 0, vk::IndexType::eUint32
    );

    graphics_command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        pipeline_layout,
        1,
        std::array{ m_base_descriptor_set.get(),
                    m_image_descriptor_set.get(),
                    m_sampler_descriptor_set.get() },
        nullptr
    );

    for (const auto& [mesh, mesh_index] :
         std::views::zip(m_meshes, std::views::iota(0u, m_meshes.size())))
    {
        for (const auto& [pipeline, material_index, first_index_index, index_count] :
             mesh.primitives)
        {
            graphics_command_buffer.bindPipeline(
                vk::PipelineBindPoint::eGraphics, pipeline.get()->get()
            );

            renderer::ModelLayout::PushConstants push_constants{
                .transform_index = mesh_index,
                .material_index =
                    material_index.value_or(std::numeric_limits<uint32_t>::max()),
            };
            graphics_command_buffer.pushConstants(
                pipeline_layout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(renderer::ModelLayout::PushConstants),
                &push_constants
            );

            graphics_command_buffer.drawIndexed(index_count, 1, first_index_index, 0, 0);
        }
    }
}

RenderModel::RenderModel(
    const vk::Device                                        device,
    renderer::base::Buffer&&                                index_buffer,
    renderer::base::Buffer&&                                vertex_buffer,
    renderer::base::RandomAccessBuffer<vk::DeviceAddress>&& vertex_uniform,
    renderer::base::Buffer&&                                transform_buffer,
    renderer::base::RandomAccessBuffer<vk::DeviceAddress>&& transform_uniform,
    vk::UniqueSampler&&                                     default_sampler,
    renderer::base::Buffer&&                                texture_buffer,
    renderer::base::RandomAccessBuffer<vk::DeviceAddress>&& texture_uniform,
    renderer::base::RandomAccessBuffer<ShaderMaterial>&&    default_material_uniform,
    renderer::base::Buffer&&                                material_buffer,
    renderer::base::RandomAccessBuffer<vk::DeviceAddress>&& material_uniform,
    vk::UniqueDescriptorSet&&                               base_descriptor_set,
    std::vector<renderer::base::Image>&&                    images,
    std::vector<vk::UniqueImageView>&&                      image_views,
    vk::UniqueDescriptorSet&&                               image_descriptor_set,
    std::vector<vk::UniqueSampler>&&                        samplers,
    vk::UniqueDescriptorSet&&                               sampler_descriptor_set,
    std::vector<Mesh>&&                                     meshes
)
    : m_index_buffer{ std::move(index_buffer) },
      m_vertex_buffer{ std::move(vertex_buffer) },
      m_vertex_uniform{ std::move(vertex_uniform) },
      m_transform_buffer{ std::move(transform_buffer) },
      m_transform_uniform{ std::move(transform_uniform) },
      m_default_sampler{ std::move(default_sampler) },
      m_texture_buffer{ std::move(texture_buffer) },
      m_texture_uniform{ std::move(texture_uniform) },
      m_default_material_uniform{ std::move(default_material_uniform) },
      m_material_buffer{ std::move(material_buffer) },
      m_material_uniform{ std::move(material_uniform) },
      m_base_descriptor_set{ std::move(base_descriptor_set) },
      m_images{ std::move(images) },
      m_image_views{ std::move(image_views) },
      m_image_descriptor_set{ std::move(image_descriptor_set) },
      m_samplers{ std::move(samplers) },
      m_sampler_descriptor_set{ std::move(sampler_descriptor_set) },
      m_meshes{ std::move(meshes) }
{
    m_vertex_buffer_address = device.getBufferAddress(vk::BufferDeviceAddressInfo{
        .buffer = m_vertex_buffer.get(),
    });
    m_vertex_uniform.set(m_vertex_buffer_address);

    m_transform_buffer_address = device.getBufferAddress(vk::BufferDeviceAddressInfo{
        .buffer = m_transform_buffer.get(),
    });
    m_transform_uniform.set(m_transform_buffer_address);

    if (m_texture_buffer.get()) {
        m_texture_buffer_address = device.getBufferAddress(vk::BufferDeviceAddressInfo{
            .buffer = m_texture_buffer.get(),
        });
    }
    else {
        m_texture_buffer_address = vk::DeviceAddress{};
    }
    m_texture_uniform.set(m_texture_buffer_address);

    if (m_material_buffer.get()) {
        m_material_buffer_address = device.getBufferAddress(vk::BufferDeviceAddressInfo{
            .buffer = m_material_buffer.get(),
        });
    }
    else {
        m_material_buffer_address = vk::DeviceAddress{};
    }
    m_material_uniform.set(m_material_buffer_address);
}

}   // namespace core::gltf
