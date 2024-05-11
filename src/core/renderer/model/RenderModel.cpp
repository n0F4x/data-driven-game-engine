#include "RenderModel.hpp"

#include "core/renderer/material_system/GraphicsPipelineBuilder.hpp"

using namespace core;
using namespace core::renderer;

struct PushConstants {
    uint32_t transform_index;
};

[[nodiscard]]
static auto create_descriptor_set_layout(const vk::Device t_device
) -> vk::UniqueDescriptorSetLayout
{
    const std::array bindings{
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 0,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eVertex },
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 1,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eVertex },
    };
    const vk::DescriptorSetLayoutCreateInfo create_info{
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings    = bindings.data(),
    };

    return t_device.createDescriptorSetLayoutUnique(create_info);
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
static auto create_descriptor_set(
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

    std::array<vk::WriteDescriptorSet, 2> write_descriptor_sets{
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

template <typename T>
[[nodiscard]]
static auto create_staging_buffer(
    const Allocator&                          t_allocator,
    const std::span<T>                        t_data,
    const std::optional<const vk::DeviceSize> t_min_alignment = std::nullopt
) -> MappedBuffer
{
    const uint32_t             size{ static_cast<uint32_t>(t_data.size_bytes()) };
    const vk::BufferCreateInfo staging_buffer_create_info = {
        .size  = size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    return t_min_alignment
        .transform([&](const vk::DeviceSize min_alignment) {
            return t_allocator.create_mapped_buffer_with_alignment(
                staging_buffer_create_info, min_alignment, t_data.data()
            );
        })
        .value_or(
            t_allocator.create_mapped_buffer(staging_buffer_create_info, t_data.data())
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

namespace core::renderer {

auto RenderModel::descriptor_pool_sizes() -> std::vector<vk::DescriptorPoolSize>
{
    return std::vector<vk::DescriptorPoolSize>{
        vk::DescriptorPoolSize{
                               .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = 1u,
                               },
        vk::DescriptorPoolSize{
                               .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = 1u,
                               }
    };
}

auto RenderModel::create_loader(
    vk::Device                            t_device,
    const Allocator&                      t_allocator,
    const vk::DescriptorSetLayout         t_descriptor_set_layout,
    const PipelineCreateInfo&             t_pipeline_create_info,
    const vk::DescriptorPool              t_descriptor_pool,
    const cache::Handle<graphics::Model>& t_model
) -> std::packaged_task<RenderModel(vk::CommandBuffer)>
{
    MappedBuffer vertex_uniform{ create_buffer<vk::DeviceAddress>(t_allocator) };
    MappedBuffer transform_uniform{ create_buffer<vk::DeviceAddress>(t_allocator) };
    vk::UniqueDescriptorSet descriptor_set{
        create_descriptor_set<vk::DeviceAddress, vk::DeviceAddress>(
            t_device,
            t_descriptor_set_layout,
            t_descriptor_pool,
            vertex_uniform.get(),
            transform_uniform.get()
        )
    };

    vk::UniquePipeline pipeline{ create_pipeline(t_device, t_pipeline_create_info) };

    MappedBuffer vertex_staging_buffer{
        create_staging_buffer(t_allocator, std::span{ t_model->vertices() })
    };
    Buffer vertex_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ t_model->vertices() }.size_bytes()),
        16
    ) };

    MappedBuffer index_staging_buffer{
        create_staging_buffer(t_allocator, std::span{ t_model->indices() })
    };
    Buffer index_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eIndexBuffer,
        static_cast<uint32_t>(std::span{ t_model->indices() }.size_bytes())
    ) };

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
    Buffer transform_buffer{ create_gpu_only_buffer(
        t_allocator,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        static_cast<uint32_t>(std::span{ transforms }.size_bytes())
    ) };

    return std::packaged_task<RenderModel(vk::CommandBuffer)>{
        [device                = t_device,
         vertex_uniform        = auto{ std::move(vertex_uniform) },
         transform_uniform     = auto{ std::move(transform_uniform) },
         descriptor_set        = auto{ std::move(descriptor_set) },
         pipeline              = auto{ std::move(pipeline) },
         vertex_staging_buffer = auto{ std::move(vertex_staging_buffer) },
         vertex_buffer         = auto{ std::move(vertex_buffer) },
         index_staging_buffer  = auto{ std::move(index_staging_buffer) },
         index_buffer          = auto{ std::move(index_buffer) },
         transform_buffer_size =
             static_cast<uint32_t>(std::span{ transforms }.size_bytes()),
         transform_staging_buffer = auto{ std::move(transform_staging_buffer) },
         transform_buffer         = auto{ std::move(transform_buffer) },
         model = auto{ t_model }](const vk::CommandBuffer t_transfer_command_buffer
        ) mutable -> RenderModel {
            vk::BufferCopy copy_region{
                .size = static_cast<uint32_t>(std::span{ model->vertices() }.size_bytes())
            };
            t_transfer_command_buffer.copyBuffer(
                vertex_staging_buffer.get(), vertex_buffer.get(), 1, &copy_region
            );

            copy_region = {
                .size = static_cast<uint32_t>(std::span{ model->indices() }.size_bytes())
            };
            t_transfer_command_buffer.copyBuffer(
                index_staging_buffer.get(), index_buffer.get(), 1, &copy_region
            );

            copy_region = { .size = transform_buffer_size };
            t_transfer_command_buffer.copyBuffer(
                transform_staging_buffer.get(), transform_buffer.get(), 1, &copy_region
            );

            return RenderModel{ device,
                                std::move(vertex_uniform),
                                std::move(transform_uniform),
                                std::move(descriptor_set),
                                std::move(pipeline),
                                std::move(vertex_buffer),
                                std::move(index_buffer),
                                std::move(transform_buffer),
                                std::move(model) };
        }
    };
}

auto RenderModel::create_descriptor_set_layout(const vk::Device t_device
) -> vk::UniqueDescriptorSetLayout
{
    return ::create_descriptor_set_layout(t_device);
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
        m_descriptor_set.get(),
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
    const vk::Device                 t_device,
    MappedBuffer&&                   t_vertex_uniform,
    MappedBuffer&&                   t_transform_uniform,
    vk::UniqueDescriptorSet&&        t_descriptor_set,
    vk::UniquePipeline&&             t_pipeline,
    Buffer&&                         t_vertex_buffer,
    Buffer&&                         t_index_buffer,
    Buffer&&                         t_transform_buffer,
    cache::Handle<graphics::Model>&& t_model
)
    : m_vertex_uniform{ std::move(t_vertex_uniform) },
      m_transform_uniform{ std::move(t_transform_uniform) },
      m_descriptor_set(std::move(t_descriptor_set)),
      m_pipeline{ std::move(t_pipeline) },
      m_vertex_buffer(std::move(t_vertex_buffer)),
      m_index_buffer(std::move(t_index_buffer)),
      m_transform_buffer(std::move(t_transform_buffer)),
      m_model(std::move(t_model))
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
