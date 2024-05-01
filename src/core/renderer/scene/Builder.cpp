#include "Builder.hpp"

#include "core/renderer/base/descriptor_pool/Builder.hpp"

using namespace core;
using namespace core::renderer;

[[nodiscard]]
static auto create_global_descriptor_set_layout(const vk::Device t_device
) -> vk::UniqueDescriptorSetLayout
{
    constexpr static std::array bindings{
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 0,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eVertex
                        | vk::ShaderStageFlagBits::eFragment },
    };

    constexpr static vk::DescriptorSetLayoutCreateInfo create_info{
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings    = bindings.data()
    };

    return t_device.createDescriptorSetLayoutUnique(create_info);
}

static auto request_descriptors(
    const graphics::Model&,
    DescriptorPool::Builder& t_descriptor_pool_builder
) -> void
{
    t_descriptor_pool_builder.request_descriptors(RenderModel2::descriptor_pool_sizes());
}

[[nodiscard]]
static auto create_descriptor_pool(
    const vk::Device                              t_device,
    const std::vector<Scene::Builder::ModelInfo>& t_models
) -> DescriptorPool
{
    auto builder{ DescriptorPool::create() };

    builder.request_descriptors(vk::DescriptorPoolSize{
        .type            = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1u,
    });

    std::ranges::for_each(
        t_models,
        [&](const graphics::Model& model) { request_descriptors(model, builder); },
        [](const Scene::Builder::ModelInfo& model_info) -> const graphics::Model& {
            return *model_info.handle;
        }
    );

    return builder.build(t_device, 2);
}

[[nodiscard]]
static auto create_pipeline_layout(
    const vk::Device        t_device,
    vk::DescriptorSetLayout t_global_descriptor_set_layout,
    vk::DescriptorSetLayout t_model_descriptor_set_layout
) -> vk::UniquePipelineLayout
{
    std::array descriptor_set_layouts{ t_global_descriptor_set_layout,
                                       t_model_descriptor_set_layout };
    std::array push_constant_ranges{ RenderModel2::push_constant_range() };

    const vk::PipelineLayoutCreateInfo pipeline_layout_create_info{
        .setLayoutCount         = static_cast<uint32_t>(descriptor_set_layouts.size()),
        .pSetLayouts            = descriptor_set_layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(push_constant_ranges.size()),
        .pPushConstantRanges    = push_constant_ranges.data(),
    };

    return t_device.createPipelineLayoutUnique(pipeline_layout_create_info);
}

template <typename GlobalUniformBlock>
[[nodiscard]]
static auto create_global_buffer(const Allocator& t_allocator) -> MappedBuffer
{
    constexpr vk::BufferCreateInfo buffer_create_info = {
        .size  = sizeof(GlobalUniformBlock),
        .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    return t_allocator.create_mapped_buffer(buffer_create_info);
}

template <typename GlobalUniformBlock>
[[nodiscard]]
static auto create_global_descriptor_set(
    const vk::Device              t_device,
    const vk::DescriptorSetLayout t_layout,
    const vk::DescriptorPool      t_pool,
    const vk::Buffer              t_global_buffer
) -> vk::UniqueDescriptorSet
{
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .descriptorPool     = t_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &t_layout,
    };
    auto descriptor_sets{
        t_device.allocateDescriptorSetsUnique(descriptor_set_allocate_info)
    };

    const vk::DescriptorBufferInfo buffer_info{
        .buffer = t_global_buffer,
        .offset = 0,
        .range  = sizeof(GlobalUniformBlock),
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

namespace core::renderer {

auto Scene::Builder::add_model(
    const cache::Handle<graphics::Model>& t_model,
    const Effect&                         t_effect
) -> Builder&
{
    m_models.emplace_back(t_model, t_effect);
    return *this;
}

auto Scene::Builder::add_model(
    cache::Handle<graphics::Model>&& t_model,
    const Effect&                    t_effect
) -> Builder&
{
    m_models.emplace_back(std::move(t_model), t_effect);
    return *this;
}

auto Scene::Builder::build(
    vk::Device        t_device,
    const Allocator&  t_allocator,
    vk::CommandBuffer t_transfer_command_buffer,
    vk::RenderPass    t_render_pass
) const -> Scene
{
    vk::UniqueDescriptorSetLayout global_descriptor_set_layout{
        create_global_descriptor_set_layout(t_device)
    };

    vk::UniqueDescriptorSetLayout model_descriptor_set_layout{
        RenderModel2::create_descriptor_set_layout(t_device)
    };

    vk::UniquePipelineLayout pipeline_layout{ create_pipeline_layout(
        t_device, global_descriptor_set_layout.get(), model_descriptor_set_layout.get()
    ) };

    DescriptorPool descriptor_pool{ create_descriptor_pool(t_device, m_models) };

    MappedBuffer global_buffer{
        create_global_buffer<Scene::GlobalUniformBlock>(t_allocator)
    };

    return Scene(
        std::move(global_descriptor_set_layout),
        std::move(model_descriptor_set_layout),
        std::move(pipeline_layout),
        std::move(descriptor_pool),
        std::move(global_buffer),
        create_global_descriptor_set<Scene::GlobalUniformBlock>(
            t_device,
            global_descriptor_set_layout.get(),
            descriptor_pool.get(),
            global_buffer.get()
        ),
        m_models | std::views::transform([&](const ModelInfo& model_info) {
            return RenderModel2::load(
                t_device,
                t_allocator,
                model_descriptor_set_layout.get(),
                RenderModel2::PipelineCreateInfo{ .effect      = model_info.effect,
                                                  .layout      = pipeline_layout.get(),
                                                  .render_pass = t_render_pass },
                descriptor_pool.get(),
                t_transfer_command_buffer,
                model_info.handle
            );
        }) | std::ranges::to<std::vector>()
    );
}

}   // namespace core::renderer
