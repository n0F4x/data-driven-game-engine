#include "RenderModel2.hpp"

#include "core/renderer/material_system/GraphicsPipelineBuilder.hpp"

using namespace core;
using namespace core::renderer;

struct UniformBlock {
    vk::DeviceAddress vertex_buffer_address;
    vk::DeviceAddress transform_buffer_address;
};

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
    };
    //    constexpr static std::array binding_flags{
    //        vk::DescriptorBindingFlags{ vk::DescriptorBindingFlagBits::ePartiallyBound
    //                                    |
    //                                    vk::DescriptorBindingFlagBits::eUpdateAfterBind
    //                                    },
    //        vk::DescriptorBindingFlags{ vk::DescriptorBindingFlagBits::ePartiallyBound
    //                                    |
    //                                    vk::DescriptorBindingFlagBits::eUpdateAfterBind
    //                                    },
    //    };
    //
    //    vk::DescriptorSetLayoutBindingFlagsCreateInfo binding_flags_create_info{
    //        .bindingCount  = static_cast<uint32_t>(binding_flags.size()),
    //        .pBindingFlags = binding_flags.data(),
    //    };

    vk::DescriptorSetLayoutCreateInfo create_info{
        //        .pNext        = &binding_flags_create_info,
        //        .flags        =
        //        vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
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

template <typename UniformBlock>
[[nodiscard]]
static auto create_descriptor_set(
    const vk::Device              t_device,
    const vk::DescriptorSetLayout t_descriptor_set_layout,
    const vk::DescriptorPool      t_descriptor_pool,
    const vk::Buffer              t_buffer
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

    const vk::DescriptorBufferInfo buffer_info{
        .buffer = t_buffer,
        .offset = 0,
        .range  = sizeof(UniformBlock),
    };

    const vk::WriteDescriptorSet write_descriptor_set{
        .dstSet          = descriptor_sets.front().get(),
        .dstBinding      = 0,
        .descriptorCount = 1u,
        .descriptorType  = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo     = &buffer_info,
    };

    t_device.updateDescriptorSets(1, &write_descriptor_set, 0, nullptr);

    return std::move(descriptor_sets.front());
}

[[nodiscard]]
static auto create_pipeline(
    const vk::Device                        t_device,
    const RenderModel2::PipelineCreateInfo& t_create_info
) -> vk::UniquePipeline
{
    GraphicsPipelineBuilder builder{ t_create_info.effect };

    builder.set_layout(t_create_info.layout);
    builder.set_render_pass(t_create_info.render_pass);

    return builder.build(t_device);
}

template <typename T>
[[nodiscard]]
static auto create_gpu_only_buffer(
    const Allocator&           t_allocator,
    const vk::CommandBuffer    t_copy_command_buffer,
    const vk::BufferUsageFlags t_usage_flags,
    const std::span<T>         t_data,
    const VkDeviceSize         t_min_alignment = 8
) -> Buffer
{
    uint32_t                   size{ static_cast<uint32_t>(t_data.size_bytes()) };
    const vk::BufferCreateInfo staging_buffer_create_info = {
        .size  = size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };
    const auto staging_buffer{ t_allocator.create_mapped_buffer(
        staging_buffer_create_info, t_data.data(), t_min_alignment
    ) };

    const vk::BufferCreateInfo buffer_create_info = {
        .size = size, .usage = t_usage_flags | vk::BufferUsageFlagBits::eTransferDst
    };
    Buffer buffer{ t_allocator.create_buffer(
        buffer_create_info,
        VmaAllocationCreateInfo{
            .usage = VMA_MEMORY_USAGE_AUTO,
        },
        t_min_alignment
    ) };

    vk::BufferCopy copy_region{ .size = size };
    t_copy_command_buffer.copyBuffer(staging_buffer.get(), buffer.get(), 1, &copy_region);

    return buffer;
}

namespace core::renderer {

auto RenderModel2::descriptor_pool_sizes() -> std::vector<vk::DescriptorPoolSize>
{
    return std::vector<vk::DescriptorPoolSize>{
        vk::DescriptorPoolSize{ .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = 1u }
    };
}

auto RenderModel2::load(
    vk::Device                     t_device,
    const Allocator&               t_allocator,
    const vk::DescriptorSetLayout  t_descriptor_set_layout,
    const PipelineCreateInfo&      t_pipeline_create_info,
    const vk::DescriptorPool       t_descriptor_pool,
    vk::CommandBuffer              t_transfer_command_buffer,
    cache::Handle<graphics::Model> t_model
) -> RenderModel2
{
    MappedBuffer uniform_buffer{ create_buffer<UniformBlock>(t_allocator) };

    auto vertex_buffer{ create_gpu_only_buffer(
        t_allocator,
        t_transfer_command_buffer,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        std::span{ t_model->vertices() },
        16
    ) };

    auto index_buffer{ create_gpu_only_buffer(
        t_allocator,
        t_transfer_command_buffer,
        vk::BufferUsageFlagBits::eIndexBuffer,
        std::span{ t_model->indices() }
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
    auto transform_buffer{ create_gpu_only_buffer(
        t_allocator,
        t_transfer_command_buffer,
        vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eShaderDeviceAddress,
        std::span{ transforms }
    ) };

    return RenderModel2{
        t_device,
        std::move(uniform_buffer),
        create_descriptor_set<UniformBlock>(
            t_device, t_descriptor_set_layout, t_descriptor_pool, uniform_buffer.get()
        ),
        create_pipeline(t_device, t_pipeline_create_info),
        std::move(vertex_buffer),
        std::move(index_buffer),
        std::move(transform_buffer),
        std::move(t_model)
    };
}

auto RenderModel2::create_descriptor_set_layout(const vk::Device t_device
) noexcept -> vk::UniqueDescriptorSetLayout
{
    return ::create_descriptor_set_layout(t_device);
}

auto RenderModel2::push_constant_range() noexcept -> vk::PushConstantRange
{
    return vk::PushConstantRange{
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .size       = sizeof(PushConstants),
    };
}

auto RenderModel2::draw(
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

RenderModel2::RenderModel2(
    const vk::Device                 t_device,
    MappedBuffer&&                   t_uniform_buffer,
    vk::UniqueDescriptorSet&&        t_descriptor_set,
    vk::UniquePipeline&&             t_pipeline,
    Buffer&&                         t_vertex_buffer,
    Buffer&&                         t_index_buffer,
    Buffer&&                         t_transform_buffer,
    cache::Handle<graphics::Model>&& t_model
)
    : m_uniform_buffer{ std::move(t_uniform_buffer) },
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

    m_uniform_buffer.set(UniformBlock{
        .vertex_buffer_address    = m_vertex_buffer_address,
        .transform_buffer_address = m_transform_buffer_address });
}

}   // namespace core::renderer
