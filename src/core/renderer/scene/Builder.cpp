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

[[nodiscard]]
static auto max_image_count(const std::vector<Scene::Builder::ModelInfo>& t_models
) noexcept -> uint32_t
{
    return static_cast<uint32_t>(std::ranges::max_element(
                                     t_models,
                                     {},
                                     [](const Scene::Builder::ModelInfo& model) {
                                         return model.handle->images().size();
                                     }
    )
                                     ->handle->images()
                                     .size());
}

[[nodiscard]]
static auto max_sampler_count(const std::vector<Scene::Builder::ModelInfo>& t_models
) noexcept -> uint32_t
{
    return static_cast<uint32_t>(std::ranges::max_element(
                                     t_models,
                                     {},
                                     [](const Scene::Builder::ModelInfo& model) {
                                         return model.handle->samplers().size();
                                     }
    )
                                     ->handle->samplers()
                                     .size());
}

static auto request_descriptors(
    const gltf::Model&       t_model,
    DescriptorPool::Builder& t_descriptor_pool_builder
) -> void
{
    t_descriptor_pool_builder.request_descriptor_sets(RenderModel::descriptor_set_count());
    t_descriptor_pool_builder.request_descriptors(
        RenderModel::descriptor_pool_sizes(RenderModel::DescriptorSetLayoutCreateInfo{
            .max_image_count   = static_cast<uint32_t>(t_model.images().size()),
            .max_sampler_count = static_cast<uint32_t>(t_model.samplers().size()),
        })
    );
}

[[nodiscard]]
static auto create_descriptor_pool(
    const vk::Device                              t_device,
    const std::vector<Scene::Builder::ModelInfo>& t_models
) -> DescriptorPool
{
    auto builder{ DescriptorPool::create() };

    builder.request_descriptor_sets(1);
    builder.request_descriptors(vk::DescriptorPoolSize{
        .type            = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1,
    });

    std::ranges::for_each(
        t_models,
        [&](const gltf::Model& model) { request_descriptors(model, builder); },
        [](const Scene::Builder::ModelInfo& model_info) -> const gltf::Model& {
            return *model_info.handle;
        }
    );

    return builder.build(t_device);
}

[[nodiscard]]
static auto create_pipeline_layout(
    const vk::Device                               t_device,
    const std::span<const vk::DescriptorSetLayout> t_layouts
) -> vk::UniquePipelineLayout
{
    std::array push_constant_ranges{ RenderModel::push_constant_range() };

    const vk::PipelineLayoutCreateInfo pipeline_layout_create_info{
        .setLayoutCount         = static_cast<uint32_t>(t_layouts.size()),
        .pSetLayouts            = t_layouts.data(),
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

    return t_allocator.allocate_mapped_buffer(buffer_create_info);
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
        .descriptorCount = 1,
        .descriptorType  = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo     = &buffer_info,
    };

    t_device.updateDescriptorSets(1, &write_descriptor_set, 0, nullptr);

    return std::move(descriptor_sets.front());
}

namespace core::renderer {

auto Scene::Builder::set_cache(cache::Cache& t_cache) noexcept -> Scene::Builder&
{
    m_cache = t_cache;
    return *this;
}

auto Scene::Builder::add_model(
    const cache::Handle<gltf::Model>& t_model,
    const Effect&                     t_effect
) -> Builder&
{
    m_models.emplace_back(t_model, t_effect);
    return *this;
}

auto Scene::Builder::add_model(
    cache::Handle<gltf::Model>&& t_model,
    const Effect&                t_effect
) -> Builder&
{
    m_models.emplace_back(std::move(t_model), t_effect);
    return *this;
}

auto Scene::Builder::build(
    vk::Device       t_device,
    const Allocator& t_allocator,
    vk::RenderPass   t_render_pass
) const -> std::packaged_task<Scene(vk::CommandBuffer)>
{
    cache::Cache temp_cache{};

    vk::UniqueDescriptorSetLayout global_descriptor_set_layout{
        create_global_descriptor_set_layout(t_device)
    };

    std::array model_descriptor_set_layouts{ RenderModel::create_descriptor_set_layouts(
        t_device,
        RenderModel::DescriptorSetLayoutCreateInfo{
            .max_image_count   = max_image_count(m_models),
            .max_sampler_count = max_sampler_count(m_models),
        }
    ) };

    vk::UniquePipelineLayout pipeline_layout{ create_pipeline_layout(
        t_device,
        std::array{
            global_descriptor_set_layout.get(),
            model_descriptor_set_layouts[0].get(),
            model_descriptor_set_layouts[1].get(),
            model_descriptor_set_layouts[2].get(),
        }
    ) };

    DescriptorPool descriptor_pool{ create_descriptor_pool(t_device, m_models) };

    MappedBuffer global_buffer{ create_global_buffer<Scene::ShaderScene>(t_allocator) };

    vk::UniqueDescriptorSet global_descriptor_set{
        create_global_descriptor_set<Scene::ShaderScene>(
            t_device,
            global_descriptor_set_layout.get(),
            descriptor_pool.get(),
            global_buffer.get()
        )
    };

    std::vector model_loaders{
        m_models | std::views::transform([&](const ModelInfo& model_info) {
            return RenderModel::create_loader(
                t_device,
                t_allocator,
                std::array{
                    model_descriptor_set_layouts[0].get(),
                    model_descriptor_set_layouts[1].get(),
                    model_descriptor_set_layouts[2].get(),
                },
                RenderModel::PipelineCreateInfo{ .effect      = model_info.effect,
                                                 .layout      = pipeline_layout.get(),
                                                 .render_pass = t_render_pass },
                descriptor_pool.get(),
                model_info.handle,
                m_cache.value_or(temp_cache)
            );
        })
        | std::ranges::to<std::vector>()
    };

    return std::packaged_task<Scene(vk::CommandBuffer)>{
        [global_descriptor_set_layout = auto{ std::move(global_descriptor_set_layout) },
         model_descriptor_set_layouts = auto{ std::move(model_descriptor_set_layouts) },
         pipeline_layout              = auto{ std::move(pipeline_layout) },
         descriptor_pool              = auto{ std::move(descriptor_pool) },
         global_buffer                = auto{ std::move(global_buffer) },
         global_descriptor_set        = auto{ std::move(global_descriptor_set) },
         model_loaders                = auto{ std::move(model_loaders
         ) }](vk::CommandBuffer t_transfer_command_buffer) mutable -> Scene {
            return Scene(
                std::move(global_descriptor_set_layout),
                std::move(model_descriptor_set_layouts),
                std::move(pipeline_layout),
                std::move(descriptor_pool),
                std::move(global_buffer),
                std::move(global_descriptor_set),
                model_loaders
                    | std::views::transform(
                        [t_transfer_command_buffer](
                            std::packaged_task<RenderModel(vk::CommandBuffer)>& model_task
                        ) {
                            std::invoke(model_task, t_transfer_command_buffer);
                            return model_task.get_future().get();
                        }
                    )
                    | std::ranges::to<std::vector>()
            );
        }
    };
}

}   // namespace core::renderer
