#include "Builder.hpp"

#include "core/renderer/base/descriptor_pool/Builder.hpp"

using namespace core;
using namespace core::renderer;

[[nodiscard]]
static auto create_global_descriptor_set_layout(const vk::Device device
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

    return device.createDescriptorSetLayoutUnique(create_info);
}

[[nodiscard]]
static auto max_image_count(const std::vector<Scene::Builder::ModelInfo>& models
) noexcept -> uint32_t
{
    return static_cast<uint32_t>(std::ranges::max_element(
                                     models,
                                     {},
                                     [](const Scene::Builder::ModelInfo& model) {
                                         return model.handle->images().size();
                                     }
    )
                                     ->handle->images()
                                     .size());
}

[[nodiscard]]
static auto max_sampler_count(const std::vector<Scene::Builder::ModelInfo>& models
) noexcept -> uint32_t
{
    return static_cast<uint32_t>(std::ranges::max_element(
                                     models,
                                     {},
                                     [](const Scene::Builder::ModelInfo& model) {
                                         return model.handle->samplers().size();
                                     }
    )
                                     ->handle->samplers()
                                     .size());
}

static auto request_descriptors(
    const gltf::Model&       model,
    DescriptorPool::Builder& descriptor_pool_builder
) -> void
{
    descriptor_pool_builder.request_descriptor_sets(RenderModel::descriptor_set_count());
    descriptor_pool_builder.request_descriptors(
        RenderModel::descriptor_pool_sizes(RenderModel::DescriptorSetLayoutCreateInfo{
            .max_image_count   = static_cast<uint32_t>(model.images().size()),
            .max_sampler_count = static_cast<uint32_t>(model.samplers().size()),
        })
    );
}

[[nodiscard]]
static auto create_descriptor_pool(
    const vk::Device                              device,
    const std::vector<Scene::Builder::ModelInfo>& models
) -> DescriptorPool
{
    auto builder{ DescriptorPool::create() };

    builder.request_descriptor_sets(1);
    builder.request_descriptors(vk::DescriptorPoolSize{
        .type            = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1,
    });

    std::ranges::for_each(
        models,
        [&](const gltf::Model& model) { request_descriptors(model, builder); },
        [](const Scene::Builder::ModelInfo& model_info) -> const gltf::Model& {
            return *model_info.handle;
        }
    );

    return builder.build(device);
}

[[nodiscard]]
static auto create_pipeline_layout(
    const vk::Device                               device,
    const std::span<const vk::DescriptorSetLayout> layouts
) -> vk::UniquePipelineLayout
{
    std::array push_constant_ranges{ RenderModel::push_constant_range() };

    const vk::PipelineLayoutCreateInfo pipeline_layout_create_info{
        .setLayoutCount         = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts            = layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(push_constant_ranges.size()),
        .pPushConstantRanges    = push_constant_ranges.data(),
    };

    return device.createPipelineLayoutUnique(pipeline_layout_create_info);
}

template <typename GlobalUniformBlock>
[[nodiscard]]
static auto create_global_buffer(const Allocator& allocator) -> MappedBuffer
{
    constexpr vk::BufferCreateInfo buffer_create_info = {
        .size  = sizeof(GlobalUniformBlock),
        .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    return allocator.allocate_mapped_buffer(buffer_create_info);
}

template <typename GlobalUniformBlock>
[[nodiscard]]
static auto create_global_descriptor_set(
    const vk::Device              device,
    const vk::DescriptorSetLayout layout,
    const vk::DescriptorPool      pool,
    const vk::Buffer              global_buffer
) -> vk::UniqueDescriptorSet
{
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .descriptorPool     = pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &layout,
    };
    auto descriptor_sets{ device.allocateDescriptorSetsUnique(descriptor_set_allocate_info
    ) };

    const vk::DescriptorBufferInfo buffer_info{
        .buffer = global_buffer,
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

    device.updateDescriptorSets(1, &write_descriptor_set, 0, nullptr);

    return std::move(descriptor_sets.front());
}

namespace core::renderer {

auto Scene::Builder::set_cache(cache::Cache& cache) noexcept -> Scene::Builder&
{
    m_cache = cache;
    return *this;
}

auto Scene::Builder::add_model(
    const cache::Handle<gltf::Model>& model,
    const Effect&                     effect
) -> Builder&
{
    m_models.emplace_back(model, effect);
    return *this;
}

auto Scene::Builder::add_model(cache::Handle<gltf::Model>&& model, const Effect& effect)
    -> Builder&
{
    m_models.emplace_back(std::move(model), effect);
    return *this;
}

auto Scene::Builder::build(
    vk::Device       device,
    const Allocator& allocator,
    vk::RenderPass   render_pass
) const -> std::packaged_task<Scene(vk::CommandBuffer)>
{
    cache::Cache temp_cache{};

    vk::UniqueDescriptorSetLayout global_descriptor_set_layout{
        create_global_descriptor_set_layout(device)
    };

    std::array model_descriptor_set_layouts{ RenderModel::create_descriptor_set_layouts(
        device,
        RenderModel::DescriptorSetLayoutCreateInfo{
            .max_image_count   = max_image_count(m_models),
            .max_sampler_count = max_sampler_count(m_models),
        }
    ) };

    vk::UniquePipelineLayout pipeline_layout{ create_pipeline_layout(
        device,
        std::array{
            global_descriptor_set_layout.get(),
            model_descriptor_set_layouts[0].get(),
            model_descriptor_set_layouts[1].get(),
            model_descriptor_set_layouts[2].get(),
        }
    ) };

    DescriptorPool descriptor_pool{ create_descriptor_pool(device, m_models) };

    MappedBuffer global_buffer{ create_global_buffer<Scene::ShaderScene>(allocator) };

    vk::UniqueDescriptorSet global_descriptor_set{
        create_global_descriptor_set<Scene::ShaderScene>(
            device,
            global_descriptor_set_layout.get(),
            descriptor_pool.get(),
            global_buffer.get()
        )
    };

    std::vector model_loaders{
        m_models | std::views::transform([&](const ModelInfo& model_info) {
            return RenderModel::create_loader(
                device,
                allocator,
                std::array{
                    model_descriptor_set_layouts[0].get(),
                    model_descriptor_set_layouts[1].get(),
                    model_descriptor_set_layouts[2].get(),
                },
                RenderModel::PipelineCreateInfo{ .effect      = model_info.effect,
                                                 .layout      = pipeline_layout.get(),
                                                 .render_pass = render_pass },
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
         ) }](vk::CommandBuffer transfer_command_buffer) mutable -> Scene {
            return Scene(
                std::move(global_descriptor_set_layout),
                std::move(model_descriptor_set_layouts),
                std::move(pipeline_layout),
                std::move(descriptor_pool),
                std::move(global_buffer),
                std::move(global_descriptor_set),
                model_loaders
                    | std::views::transform(
                        [transfer_command_buffer](
                            std::packaged_task<RenderModel(vk::CommandBuffer)>& model_task
                        ) {
                            std::invoke(model_task, transfer_command_buffer);
                            return model_task.get_future().get();
                        }
                    )
                    | std::ranges::to<std::vector>()
            );
        }
    };
}

}   // namespace core::renderer
