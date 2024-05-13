#include "RenderModel.hpp"

#include "core/renderer/material_system/GraphicsPipelineBuilder.hpp"
#include "core/renderer/memory/Image.hpp"

using namespace core;
using namespace core::renderer;

struct PushConstants {
    uint32_t transform_index;
};

[[nodiscard]]
static auto create_descriptor_set_layouts(
    const vk::Device                                  t_device,
    const RenderModel::DescriptorSetLayoutCreateInfo& t_info
) -> std::array<vk::UniqueDescriptorSetLayout, 3>
{
    const std::array bindings_0{
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 0,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eVertex },
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 1,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eVertex }
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
    const uint32_t             size{ static_cast<uint32_t>(t_size) };
    const vk::BufferCreateInfo staging_buffer_create_info = {
        .size  = size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    return t_min_alignment
        .transform([&](const vk::DeviceSize min_alignment) {
            return t_allocator.create_mapped_buffer_with_alignment(
                staging_buffer_create_info, min_alignment, t_data
            );
        })
        .value_or(t_allocator.create_mapped_buffer(staging_buffer_create_info, t_data));
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
    const vk::BufferCreateInfo buffer_create_info = {
        .size = t_size, .usage = t_usage_flags | vk::BufferUsageFlagBits::eTransferDst
    };

    return t_min_alignment
        .transform([&](const vk::DeviceSize min_alignment) {
            return t_allocator.create_buffer_with_alignment(
                buffer_create_info, min_alignment
            );
        })
        .value_or(t_allocator.create_buffer(buffer_create_info));
}

template <typename UniformBlock>
[[nodiscard]]
static auto create_buffer(const Allocator& t_allocator) -> MappedBuffer
{
    constexpr vk::BufferCreateInfo buffer_create_info = {
        .size = sizeof(UniformBlock), .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    return t_allocator.create_mapped_buffer(buffer_create_info);
}

template <typename UniformBlock0, typename UniformBlock1>
[[nodiscard]]
static auto create_base_descriptor_set(
    const vk::Device              t_device,
    const vk::DescriptorSetLayout t_descriptor_set_layout,
    const vk::DescriptorPool      t_descriptor_pool,
    const vk::Buffer              t_buffer_0,
    const vk::Buffer              t_buffer_1
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

    const vk::DescriptorBufferInfo buffer_info_1{
        .buffer = t_buffer_0,
        .range  = sizeof(UniformBlock0),
    };
    const vk::DescriptorBufferInfo buffer_info_2{
        .buffer = t_buffer_1,
        .range  = sizeof(UniformBlock1),
    };

    std::array write_descriptor_sets{
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 0,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &buffer_info_1,
                               },
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 1,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &buffer_info_2,
                               }
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
    uint32_t            t_width,
    uint32_t            t_height,
    vk::Format          t_format,
    vk::ImageTiling     t_tiling,
    vk::ImageUsageFlags t_usage
) -> Image
{
    const vk::ImageCreateInfo image_create_info{
        .imageType     = vk::ImageType::e2D,
        .format        = t_format,
        .extent        = vk::Extent3D{ .width = t_width, .height = t_height, .depth = 1 },
        .mipLevels     = 1,
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

    return t_allocator.create_image(image_create_info, allocation_create_info);
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
                                      .baseMipLevel   = 0,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
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
static auto to_mag_filter(graphics::Model::Sampler::MagFilter t_mag_filter
) noexcept -> vk::Filter
{
    using enum graphics::Model::Sampler::MagFilter;
    switch (t_mag_filter) {
        case eNearest: return vk::Filter::eNearest;
        case eLinear: return vk::Filter::eLinear;
    }
}

[[nodiscard]]
static auto to_min_filter(graphics::Model::Sampler::MinFilter t_min_filter
) noexcept -> vk::Filter
{
    using enum graphics::Model::Sampler::MinFilter;
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
static auto to_mipmap_mode(graphics::Model::Sampler::MinFilter t_min_filter
) noexcept -> vk::SamplerMipmapMode
{
    using enum graphics::Model::Sampler::MinFilter;
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
static auto to_address_mode(graphics::Model::Sampler::WrapMode t_wrap_mode
) noexcept -> vk::SamplerAddressMode
{
    using enum graphics::Model::Sampler::WrapMode;
    switch (t_wrap_mode) {
        case eClampToEdge: return vk::SamplerAddressMode::eClampToEdge;
        case eMirroredRepeat: return vk::SamplerAddressMode::eMirroredRepeat;
        case eRepeat: return vk::SamplerAddressMode::eRepeat;
    }
}

[[nodiscard]]
static auto create_sampler(
    const vk::Device                t_device,
    const graphics::Model::Sampler& t_sampler_info
) -> vk::UniqueSampler
{
    const vk::SamplerCreateInfo sampler_create_info{
        .magFilter = t_sampler_info.mag_filter.transform(to_mag_filter)
                         .value_or(vk::Filter::eNearest),
        .minFilter = t_sampler_info.min_filter.transform(to_min_filter)
                         .value_or(vk::Filter::eNearest),
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
static auto create_pipeline(
    const vk::Device                       t_device,
    const RenderModel::PipelineCreateInfo& t_create_info
) -> vk::UniquePipeline
{
    GraphicsPipelineBuilder builder{ t_create_info.effect };

    builder.set_layout(t_create_info.layout);
    builder.set_render_pass(t_create_info.render_pass);

    return builder.build(t_device);
}

void transition_image_layout(
    vk::CommandBuffer t_command_buffer,
    vk::Image         t_image,
    vk::ImageLayout   t_old_layout,
    vk::ImageLayout   t_new_layout
)
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

void copy_buffer_to_image(
    vk::CommandBuffer t_command_buffer,
    vk::Buffer        t_buffer,
    vk::Image         t_image,
    uint32_t          t_width,
    uint32_t          t_height
)
{
    const vk::BufferImageCopy region{
        .imageSubresource =
            vk::ImageSubresourceLayers{ .aspectMask = vk::ImageAspectFlagBits::eColor,
                                       .layerCount = 1 },
        .imageExtent = vk::Extent3D{ t_width, t_height, 1 },
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
        vk::DescriptorPoolSize{ .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = 1u },
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
    cache::Handle<graphics::Model>                    t_model
) -> std::packaged_task<RenderModel(vk::CommandBuffer)>
{
    MappedBuffer index_staging_buffer{
        create_staging_buffer(t_allocator, std::span{ t_model->indices() })
    };
    Buffer index_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eIndexBuffer,
        static_cast<uint32_t>(std::span{ t_model->indices() }.size_bytes())
    ) };

    MappedBuffer vertex_staging_buffer{
        create_staging_buffer(t_allocator, std::span{ t_model->vertices() })
    };
    Buffer       vertex_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ t_model->vertices() }.size_bytes()),
        16
    ) };
    MappedBuffer vertex_uniform{ create_buffer<vk::DeviceAddress>(t_allocator) };

    std::vector nodes_with_mesh{
        t_model->nodes() | std::views::filter([](const graphics::Model::Node& node) {
            return node.mesh_index.has_value();
        })
        | std::views::transform([](const graphics::Model::Node& node) {
              return std::cref(node);
          })
        | std::ranges::to<std::vector>()
    };
    std::vector<glm::mat4> transforms(nodes_with_mesh.size());
    std::ranges::for_each(
        nodes_with_mesh,
        [&transforms](const graphics::Model::Node& node) {
            transforms.at(node.mesh_index.value()) = node.matrix();
        }
    );
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

    vk::UniqueDescriptorSet base_descriptor_set{
        create_base_descriptor_set<vk::DeviceAddress, vk::DeviceAddress>(
            t_device,
            t_descriptor_set_layouts[0],
            t_descriptor_pool,
            vertex_uniform.get(),
            transform_uniform.get()
        )
    };

    std::vector<MappedBuffer> image_staging_buffers{
        t_model->images()
        | std::views::transform([&](const graphics::Model::Image& image) {
              return create_staging_buffer(
                  t_allocator, image->data(), static_cast<uint32_t>(image->size())
              );
          })
        | std::ranges::to<std::vector>()
    };
    std::vector<Image> images{
        t_model->images()
        | std::views::transform([&](const graphics::Model::Image& image) {
              return create_image(
                  t_allocator,
                  image->width(),
                  image->height(),
                  image->format(),
                  vk::ImageTiling::eOptimal,
                  vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
              );
          })
        | std::ranges::to<std::vector>()
    };
    std::vector<vk::UniqueImageView> image_views{
        std::views::zip(
            images | std::views::transform(&Image::get),
            t_model->images()
                | std::views::transform([](const graphics::Model::Image& image) {
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

    vk::UniquePipeline pipeline{ create_pipeline(t_device, t_pipeline_create_info) };

    return std::packaged_task<RenderModel(vk::CommandBuffer)>{
        [device                = t_device,
         index_staging_buffer  = auto{ std::move(index_staging_buffer) },
         index_buffer          = auto{ std::move(index_buffer) },
         vertex_staging_buffer = auto{ std::move(vertex_staging_buffer) },
         vertex_buffer         = auto{ std::move(vertex_buffer) },
         vertex_uniform        = auto{ std::move(vertex_uniform) },
         transform_buffer_size =
             static_cast<uint32_t>(std::span{ transforms }.size_bytes()),
         transform_staging_buffer = auto{ std::move(transform_staging_buffer) },
         transform_buffer         = auto{ std::move(transform_buffer) },
         transform_uniform        = auto{ std::move(transform_uniform) },
         base_descriptor_set      = auto{ std::move(base_descriptor_set) },
         image_staging_buffers    = auto{ std::move(image_staging_buffers) },
         images                   = auto{ std::move(images) },
         image_views              = auto{ std::move(image_views) },
         image_descriptor_set     = auto{ std::move(image_descriptor_set) },
         samplers                 = auto{ std::move(samplers) },
         sampler_descriptor_set   = auto{ std::move(sampler_descriptor_set) },
         pipeline                 = auto{ std::move(pipeline) },
         model                    = auto{ std::move(t_model
         ) }](const vk::CommandBuffer t_transfer_command_buffer) mutable -> RenderModel {
            t_transfer_command_buffer.copyBuffer(
                index_staging_buffer.get(),
                index_buffer.get(),
                std::array{ vk::BufferCopy{ .size = static_cast<uint32_t>(
                                                std::span{ model->indices() }.size_bytes()
                                            ) } }
            );

            t_transfer_command_buffer.copyBuffer(
                vertex_staging_buffer.get(),
                vertex_buffer.get(),
                std::array{ vk::BufferCopy{ .size = static_cast<uint32_t>(
                                                std::span{ model->vertices() }.size_bytes()
                                            ) } }
            );

            t_transfer_command_buffer.copyBuffer(
                transform_staging_buffer.get(),
                transform_buffer.get(),
                std::array{ vk::BufferCopy{ .size = transform_buffer_size } }
            );

            for (auto&& [buffer, texture_image, image] :
                 std::views::zip(image_staging_buffers, images, model->images()))
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
                    image->width(),
                    image->height()
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
                                std::move(base_descriptor_set),
                                std::move(images),
                                std::move(image_views),
                                std::move(image_descriptor_set),
                                std::move(samplers),
                                std::move(sampler_descriptor_set),
                                std::move(pipeline),
                                std::move(model) };
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
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .size       = sizeof(PushConstants),
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

    t_graphics_command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, m_pipeline.get()
    );

    for (const auto& [mesh, mesh_index] : std::views::zip(
             m_model->meshes(), std::views::iota(0u, m_model->meshes().size())
         ))
    {
        PushConstants push_constants{ .transform_index = mesh_index };
        t_graphics_command_buffer.pushConstants(
            t_pipeline_layout,
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(PushConstants),
            &push_constants
        );

        for (const auto& primitive : mesh.primitives) {
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
    vk::UniqueDescriptorSet&&          t_base_descriptor_set,
    std::vector<Image>&&               t_images,
    std::vector<vk::UniqueImageView>&& t_image_views,
    vk::UniqueDescriptorSet&&          t_image_descriptor_set,
    std::vector<vk::UniqueSampler>&&   t_samplers,
    vk::UniqueDescriptorSet&&          t_sampler_descriptor_set,
    vk::UniquePipeline&&               t_pipeline,
    cache::Handle<graphics::Model>&&   t_model
)
    : m_index_buffer{ std::move(t_index_buffer) },
      m_vertex_buffer{ std::move(t_vertex_buffer) },
      m_vertex_uniform{ std::move(t_vertex_uniform) },
      m_transform_buffer{ std::move(t_transform_buffer) },
      m_transform_uniform{ std::move(t_transform_uniform) },
      m_base_descriptor_set{ std::move(t_base_descriptor_set) },
      m_images{ std::move(t_images) },
      m_image_views{ std::move(t_image_views) },
      m_image_descriptor_set{ std::move(t_image_descriptor_set) },
      m_samplers{ std::move(t_samplers) },
      m_sampler_descriptor_set{ std::move(t_sampler_descriptor_set) },
      m_pipeline{ std::move(t_pipeline) },
      m_model{ std::move(t_model) }
{
    vk::BufferDeviceAddressInfo buffer_device_address_info{
        .buffer = m_vertex_buffer.get(),
    };
    m_vertex_buffer_address = t_device.getBufferAddress(buffer_device_address_info);

    buffer_device_address_info.buffer = m_transform_buffer.get();
    m_transform_buffer_address = t_device.getBufferAddress(buffer_device_address_info);

    m_vertex_uniform.set(m_vertex_buffer_address);
    m_transform_uniform.set(m_transform_buffer_address);
}

}   // namespace core::renderer
