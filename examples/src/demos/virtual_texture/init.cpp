#include "init.hpp"

#include <source_location>

#include <spdlog/spdlog.h>

#include <base/init.hpp>
#include <core/renderer/base/device/Device.hpp>
#include <core/renderer/material_system/GraphicsPipelineBuilder.hpp>

#include "Vertex.hpp"
#include "VirtualTexture.hpp"

auto demo::init::create_descriptor_set_layout(const vk::Device device)
    -> vk::UniqueDescriptorSetLayout
{
    constexpr static std::array bindings = {
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 0,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eVertex  },
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 1,
                                       .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 2,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 3,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment },
    };

    constexpr static vk::DescriptorSetLayoutCreateInfo create_info{
        .bindingCount = bindings.size(),
        .pBindings    = bindings.data(),
    };

    return device.createDescriptorSetLayoutUnique(create_info);
}

auto demo::init::create_pipeline_layout(
    const vk::Device                               device,
    const std::span<const vk::DescriptorSetLayout> descriptor_set_layouts
) -> vk::UniquePipelineLayout
{
    const vk::PipelineLayoutCreateInfo create_info{
        .setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size()),
        .pSetLayouts    = descriptor_set_layouts.data(),
    };

    return device.createPipelineLayoutUnique(create_info);
}

[[nodiscard]]
static auto find_supported_format(
    const vk::PhysicalDevice          physical_device,
    const std::span<const vk::Format> candidates,
    const vk::ImageTiling             tiling,
    const vk::FormatFeatureFlags      features
) -> vk::Format
{
    for (const auto format : candidates) {
        vk::FormatProperties format_properties;
        physical_device.getFormatProperties(format, &format_properties);

        if ((tiling == vk::ImageTiling::eLinear
             && (format_properties.linearTilingFeatures & features) == features)
            || (tiling == vk::ImageTiling::eOptimal
                && (format_properties.optimalTilingFeatures & features) == features))
        {
            return format;
        }
    }
    throw std::runtime_error{ "Failed to find supported format!" };
}

[[nodiscard]]
static auto find_depth_format(const vk::PhysicalDevice physical_device) -> vk::Format
{
    using enum vk::Format;
    return ::find_supported_format(
        physical_device,
        std::array{ eD32Sfloat, eD32SfloatS8Uint, eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

auto demo::init::create_depth_image(
    const vk::PhysicalDevice               physical_device,
    const core::renderer::base::Allocator& allocator,
    const vk::Extent2D                     swapchain_extent
) -> core::renderer::resources::Image
{
    const vk::ImageCreateInfo image_create_info = {
        .imageType   = vk::ImageType::e2D,
        .format      = ::find_depth_format(physical_device),
        .extent      = vk::Extent3D{ swapchain_extent.width, swapchain_extent.height, 1 },
        .mipLevels   = 1,
        .arrayLayers = 1,
        .samples     = vk::SampleCountFlagBits::e1,
        .tiling      = vk::ImageTiling::eOptimal,
        .usage       = vk::ImageUsageFlagBits::eDepthStencilAttachment,
    };

    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage    = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.f,
    };

    return core::renderer::resources::Image{ allocator,
                                             image_create_info,
                                             allocation_create_info };
}

auto demo::init::create_depth_image_view(
    const core::renderer::base::Device& device,
    const vk::Image                     depth_image
) -> vk::UniqueImageView
{
    const vk::ImageViewCreateInfo image_view_create_info{
        .image    = depth_image,
        .viewType = vk::ImageViewType::e2D,
        .format   = ::find_depth_format(device.physical_device()),
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                                      .baseMipLevel   = 0,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1 },
    };

    return device->createImageViewUnique(image_view_create_info);
}

[[nodiscard]]
static auto create_program(
    const vk::Device       device,
    const std::string_view fragment_shader_file_name
) -> core::renderer::Program
{
    static const std::filesystem::path shader_path{
        std::filesystem::path{ std::source_location::current().file_name() }.parent_path()
        / "shaders"
    };
    static const std::filesystem::path vertex_shader_path{ shader_path / "main.vert.spv" };
    static const std::filesystem::path fragment_shader_path{
        shader_path / fragment_shader_file_name
    };

    return core::renderer::Program{
        core::renderer::Shader{
            core::cache::make_handle<const core::renderer::ShaderModule>(
                core::renderer::ShaderModule::load(device, vertex_shader_path)
            ) },
        core::renderer::Shader{
            core::cache::make_handle<const core::renderer::ShaderModule>(
                core::renderer::ShaderModule::load(device, fragment_shader_path)
            ) }
    };
}

auto demo::init::create_pipeline(
    const vk::Device         device,
    const vk::PipelineLayout layout,
    const vk::Format         surface_format,
    const vk::Format         depth_format,
    const std::string_view   fragment_shader_file_name
) -> vk::UniquePipeline
{
    const vk::PipelineRenderingCreateInfoKHR dynamic_rendering_create_info{
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &surface_format,
        .depthAttachmentFormat   = depth_format,
    };

    return core::renderer::GraphicsPipelineBuilder{
        ::create_program(device, fragment_shader_file_name)
    }
        .set_layout(layout)
        .add_vertex_layout(core::renderer::VertexLayout{ sizeof(Vertex),
                                                         vk::VertexInputRate::eVertex }
                               .add_attribute(core::renderer::VertexAttribute{
                                   .location = 0,
                                   .format   = vk::Format::eR32G32B32Sfloat,
                                   .offset   = 0,
                               })
                               .add_attribute(core::renderer::VertexAttribute{
                                   .location = 1,
                                   .format   = vk::Format::eR32G32Sfloat,
                                   .offset   = offsetof(Vertex, uv),
                               }))
        .set_primitive_topology(vk::PrimitiveTopology::eTriangleList)
        .set_cull_mode(vk::CullModeFlagBits::eNone)
        .build(device, &dynamic_rendering_create_info);
}

auto demo::init::create_camera_buffer(const core::renderer::base::Allocator& allocator)
    -> core::renderer::resources::RandomAccessBuffer<Camera>
{
    constexpr vk::BufferCreateInfo buffer_create_info = {
        .size = sizeof(Camera), .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    return core::renderer::resources::RandomAccessBuffer<Camera>{ allocator,
                                                                  buffer_create_info };
}

auto demo::init::create_virtual_image(
    const core::renderer::base::Device&    device,
    const core::renderer::base::Allocator& allocator,
    std::unique_ptr<core::image::Image>&&  source
) -> core::gfx::resources::VirtualImage
{
    const vk::UniqueCommandPool command_pool{ examples::base::init::create_command_pool(
        device.get(), device.info().get_queue_index(vkb::QueueType::graphics).value()
    ) };
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = command_pool.get(),
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    const vk::CommandBuffer command_buffer{
        device->allocateCommandBuffers(command_buffer_allocate_info).front()
    };
    constexpr static vk::CommandBufferBeginInfo begin_info{};
    command_buffer.begin(begin_info);

    core::gfx::resources::VirtualImage::Loader virtual_image_loader{
        device.physical_device(), device.get(), allocator, *source
    };

    core::gfx::resources::VirtualImage virtual_image{ std::move(virtual_image_loader)(
        device.physical_device(),
        device.info().get_queue(vkb::QueueType::graphics).value(),
        command_buffer,
        core::renderer::base::Image::State{
            .stage_mask  = vk::PipelineStageFlagBits::eFragmentShader,
            .access_mask = vk::AccessFlagBits::eShaderRead,
            .layout      = vk::ImageLayout::eShaderReadOnlyOptimal,
        }
    ) };

    command_buffer.end();

    const vk::SubmitInfo submit_info{
        .commandBufferCount = 1,
        .pCommandBuffers    = &command_buffer,
    };
    vk::UniqueFence fence{ device->createFenceUnique({}) };

    static_cast<vk::Queue>(device.info().get_queue(vkb::QueueType::graphics).value())
        .submit(submit_info, fence.get());

    std::ignore =
        device->waitForFences(std::array{ fence.get() }, vk::True, 100'000'000'000);
    device->resetCommandPool(command_pool.get());

    return virtual_image;
}

auto demo::init::create_virtual_image_sampler(const core::renderer::base::Device& device)
    -> vk::UniqueSampler
{
    const vk::PhysicalDeviceLimits limits{
        device.physical_device().getProperties().limits
    };

    const vk::SamplerCreateInfo sampler_create_info{
        .magFilter        = vk::Filter::eNearest,
        .minFilter        = vk::Filter::eNearest,
        .mipmapMode       = vk::SamplerMipmapMode::eLinear,
        .addressModeU     = vk::SamplerAddressMode::eClampToEdge,
        .addressModeV     = vk::SamplerAddressMode::eClampToEdge,
        .addressModeW     = vk::SamplerAddressMode::eRepeat,
        .anisotropyEnable = vk::True,
        .maxAnisotropy    = limits.maxSamplerAnisotropy,
        .maxLod           = vk::LodClampNone,
    };

    return device->createSamplerUnique(sampler_create_info);
}

auto demo::init::create_virtual_texture_info_buffer(
    const core::renderer::base::Allocator&    allocator,
    const core::gfx::resources::VirtualImage& virtual_image
) -> core::renderer::resources::RandomAccessBuffer<VirtualTextureInfo>
{
    constexpr vk::BufferCreateInfo buffer_create_info = {
        .size  = sizeof(VirtualTextureInfo),
        .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    core::renderer::resources::RandomAccessBuffer<VirtualTextureInfo>
        virtual_texture_info_buffer{ allocator, buffer_create_info };

    const glm::uvec2 base_extent{
        virtual_image.image().extent().width,
        virtual_image.image().extent().height,
    };
    const glm::uvec2 image_granularity{
        virtual_image.sparse_properties().formatProperties.imageGranularity.width,
        virtual_image.sparse_properties().formatProperties.imageGranularity.height,
    };

    virtual_texture_info_buffer.set(VirtualTextureInfo{
        .baseExtent  = base_extent,
        .granularity = image_granularity,
        .block_count = static_cast<uint32_t>(virtual_image.blocks().size()),
    });

    SPDLOG_DEBUG("Block count: {}", static_cast<uint32_t>(virtual_image.blocks().size()));

    return virtual_texture_info_buffer;
}

auto demo::init::create_virtual_texture_blocks_buffer(
    const core::renderer::base::Allocator&    allocator,
    const core::gfx::resources::VirtualImage& virtual_image
) -> core::renderer::resources::Buffer
{
    const vk::BufferCreateInfo buffer_create_info = {
        .size  = virtual_image.blocks().size_bytes(),
        .usage = vk::BufferUsageFlagBits::eStorageBuffer
               | vk::BufferUsageFlagBits::eShaderDeviceAddress
    };

    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
               | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    core::renderer::resources::Buffer virtual_texture_blocks_buffer{
        allocator, buffer_create_info, allocation_create_info
    };

    const std::vector<uint32_t> virtual_texture_blocks(virtual_image.blocks().size());
    core::renderer::base::copy(
        virtual_texture_blocks.data(),
        core::renderer::base::CopyRegion{
            .allocation = virtual_texture_blocks_buffer.allocation(),
        },
        virtual_texture_blocks.size() * sizeof(uint32_t)
    );

    return virtual_texture_blocks_buffer;
}

auto demo::init::create_virtual_texture_blocks_uniform(
    const vk::Device                       device,
    const core::renderer::base::Allocator& allocator,
    const vk::Buffer                       virtual_blocks_buffer
) -> core::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>
{
    constexpr vk::BufferCreateInfo buffer_create_info = {
        .size = sizeof(vk::DeviceAddress), .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    const vk::BufferDeviceAddressInfo buffer_device_address_info{
        .buffer = virtual_blocks_buffer,
    };

    core::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>
        virtual_texture_block_uniform{ allocator, buffer_create_info };

    virtual_texture_block_uniform.set(device.getBufferAddress(buffer_device_address_info));

    return virtual_texture_block_uniform;
}

auto demo::init::create_debug_texture_descriptor_set(
    const vk::Device              device,
    const vk::DescriptorSetLayout descriptor_set_layout,
    const vk::DescriptorPool      descriptor_pool,
    const vk::Buffer              camera_buffer,
    const vk::ImageView           image_view,
    const vk::Sampler             sampler
) -> vk::UniqueDescriptorSet
{
    const vk::DescriptorSetAllocateInfo allocate_info{
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &descriptor_set_layout,
    };

    std::vector<vk::UniqueDescriptorSet> descriptor_sets{
        device.allocateDescriptorSetsUnique(allocate_info)
    };

    const vk::DescriptorBufferInfo buffer_info{
        .buffer = camera_buffer,
        .offset = 0,
        .range  = sizeof(Camera),
    };

    const vk::DescriptorImageInfo image_info{
        .sampler     = sampler,
        .imageView   = image_view,
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
    };

    const std::array write_descriptor_sets{
        vk::WriteDescriptorSet{.dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 0,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &buffer_info },
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 1,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
                               .pImageInfo      = &image_info }
    };

    device.updateDescriptorSets(write_descriptor_sets, {});

    return std::move(descriptor_sets.front());
}

auto demo::init::create_virtual_texture_descriptor_set(
    const vk::Device              device,
    const vk::DescriptorSetLayout descriptor_set_layout,
    const vk::DescriptorPool      descriptor_pool,
    const vk::Buffer              camera_buffer,
    const VirtualTexture&         virtual_texture,
    const vk::Buffer              virtual_texture_info_buffer,
    const vk::Buffer              virtual_texture_blocks_uniform
) -> vk::UniqueDescriptorSet
{
    const vk::DescriptorSetAllocateInfo allocate_info{
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &descriptor_set_layout,
    };

    std::vector<vk::UniqueDescriptorSet> descriptor_sets{
        device.allocateDescriptorSetsUnique(allocate_info)
    };

    const vk::DescriptorBufferInfo buffer_info{
        .buffer = camera_buffer,
        .range  = sizeof(Camera),
    };

    const vk::DescriptorImageInfo image_info{
        .sampler     = virtual_texture.sampler(),
        .imageView   = virtual_texture.view(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
    };

    const vk::DescriptorBufferInfo virtual_texture_info_buffer_info{
        .buffer = virtual_texture_info_buffer,
        .range  = sizeof(VirtualTextureInfo),
    };

    const vk::DescriptorBufferInfo virtual_texture_blocks_buffer_info{
        .buffer = virtual_texture_blocks_uniform,
        .range  = sizeof(virtual_texture.get().blocks().size_bytes()),
    };

    const std::array write_descriptor_sets{
        vk::WriteDescriptorSet{.dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 0,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &buffer_info                       },
        vk::WriteDescriptorSet{
                               .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 1,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
                               .pImageInfo      = &image_info                        },
        vk::WriteDescriptorSet{ .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 2,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &virtual_texture_info_buffer_info  },
        vk::WriteDescriptorSet{ .dstSet          = descriptor_sets.front().get(),
                               .dstBinding      = 3,
                               .descriptorCount = 1,
                               .descriptorType  = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo     = &virtual_texture_blocks_buffer_info }
    };

    device.updateDescriptorSets(write_descriptor_sets, {});

    return std::move(descriptor_sets.front());
}
