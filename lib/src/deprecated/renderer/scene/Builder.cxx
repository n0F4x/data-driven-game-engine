module;

#include <algorithm>
#include <future>
#include <ranges>
#include <vector>

module ddge.deprecated.renderer.scene.Scene;

import ddge.deprecated.renderer.base.device.Device;
import ddge.deprecated.renderer.base.descriptor_pool.DescriptorPool;
import ddge.deprecated.renderer.model.ModelLayout;

using namespace ddge;
using namespace ddge::renderer;

[[nodiscard]]
static auto create_global_descriptor_set_layout(const vk::Device device)
    -> vk::UniqueDescriptorSetLayout
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
static auto max_image_count(
    const std::span<const cache::Handle<const gltf::Model>> models
) noexcept -> uint32_t
{
    return static_cast<uint32_t>(std::ranges::max_element(
                                     models,
                                     {},
                                     [](const cache::Handle<const gltf::Model>& model) {
                                         return model->images().size();
                                     }
    )
                                     ->get()
                                     ->images()
                                     .size());
}

[[nodiscard]]
static auto max_sampler_count(
    const std::span<const cache::Handle<const gltf::Model>> models
) noexcept -> uint32_t
{
    return static_cast<uint32_t>(std::ranges::max_element(
                                     models,
                                     {},
                                     [](const cache::Handle<const gltf::Model>& model) {
                                         return model->samplers().size();
                                     }
    )
                                     ->get()
                                     ->samplers()
                                     .size());
}

static auto request_descriptors(
    const gltf::Model&             model,
    base::DescriptorPool::Builder& descriptor_pool_builder
) -> void
{
    descriptor_pool_builder.request_descriptor_sets(ModelLayout::descriptor_set_count());
    descriptor_pool_builder.request_descriptors(
        ModelLayout::descriptor_pool_sizes(
            ModelLayout::DescriptorSetLayoutCreateInfo{
                .max_image_count   = static_cast<uint32_t>(model.images().size()),
                .max_sampler_count = static_cast<uint32_t>(model.samplers().size()),
            }
        )
    );
}

[[nodiscard]]
static auto create_descriptor_pool(
    const vk::Device                                        device,
    const std::span<const cache::Handle<const gltf::Model>> models
) -> base::DescriptorPool
{
    auto builder{ base::DescriptorPool::create() };

    builder.request_descriptor_sets(1);
    builder.request_descriptors(
        vk::DescriptorPoolSize{
            .type            = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 1,
        }
    );

    std::ranges::for_each(
        models,
        [&](const gltf::Model& model) { request_descriptors(model, builder); },
        [](const cache::Handle<const gltf::Model>& model) -> const gltf::Model& {
            return *model;
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
    std::array push_constant_ranges{ ModelLayout::push_constant_range() };

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
static auto create_global_buffer(const base::Allocator& allocator)
    -> resources::RandomAccessBuffer<GlobalUniformBlock>
{
    constexpr vk::BufferCreateInfo buffer_create_info = {
        .size  = sizeof(GlobalUniformBlock),
        .usage = vk::BufferUsageFlagBits::eUniformBuffer
    };

    return resources::RandomAccessBuffer<GlobalUniformBlock>{ allocator,
                                                              buffer_create_info };
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
    vk::UniqueDescriptorSet result{ std::move(
        device.allocateDescriptorSetsUnique(descriptor_set_allocate_info).front()
    ) };

    const vk::DescriptorBufferInfo buffer_info{
        .buffer = global_buffer,
        .offset = 0,
        .range  = sizeof(GlobalUniformBlock),
    };

    const vk::WriteDescriptorSet write_descriptor_set{
        .dstSet          = result.get(),
        .dstBinding      = 0,
        .descriptorCount = 1,
        .descriptorType  = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo     = &buffer_info,
    };

    device.updateDescriptorSets(1, &write_descriptor_set, 0, nullptr);

    return result;
}

namespace ddge::renderer {

auto Scene::Builder::set_cache(cache::Cache& cache) noexcept -> Scene::Builder&
{
    m_cache = cache;
    return *this;
}

auto Scene::Builder::add_model(const cache::Handle<const gltf::Model>& model) -> Builder&
{
    m_models.push_back(model);
    return *this;
}

auto Scene::Builder::add_model(cache::Handle<const gltf::Model>&& model) -> Builder&
{
    m_models.push_back(std::move(model));
    return *this;
}

auto Scene::Builder::build(
    const base::Device&    device,
    const base::Allocator& allocator,
    const vk::RenderPass   render_pass,
    const bool             use_virtual_images
) const -> std::packaged_task<Scene(vk::CommandBuffer)>
{
    cache::Cache temp_cache{};

    vk::UniqueDescriptorSetLayout global_descriptor_set_layout{
        create_global_descriptor_set_layout(device.get())
    };

    std::array model_descriptor_set_layouts{ ModelLayout::create_descriptor_set_layouts(
        device.get(),
        ModelLayout::DescriptorSetLayoutCreateInfo{
            .max_image_count   = max_image_count(m_models),
            .max_sampler_count = max_sampler_count(m_models),
        }
    ) };

    vk::UniquePipelineLayout pipeline_layout{ create_pipeline_layout(
        device.get(),
        std::array{
            global_descriptor_set_layout.get(),
            model_descriptor_set_layouts[0].get(),
            model_descriptor_set_layouts[1].get(),
            model_descriptor_set_layouts[2].get(),
        }
    ) };

    base::DescriptorPool descriptor_pool{ create_descriptor_pool(device.get(), m_models) };

    resources::RandomAccessBuffer<Scene::ShaderScene> global_buffer{
        create_global_buffer<Scene::ShaderScene>(allocator)
    };

    vk::UniqueDescriptorSet global_descriptor_set{
        create_global_descriptor_set<Scene::ShaderScene>(
            device.get(),
            global_descriptor_set_layout.get(),
            descriptor_pool.get(),
            global_buffer.buffer()
        )
    };

    std::vector model_loaders{
        m_models
        | std::views::transform([&](const cache::Handle<const gltf::Model>& model) {
              return gltf::RenderModel::create_loader(
                  device,
                  allocator,
                  std::array{
                      model_descriptor_set_layouts[0].get(),
                      model_descriptor_set_layouts[1].get(),
                      model_descriptor_set_layouts[2].get(),
                  },
                  gltf::RenderModel::PipelineCreateInfo{
                      .layout      = pipeline_layout.get(),
                      .render_pass = render_pass,
                  },
                  descriptor_pool.get(),
                  model,
                  m_cache.value_or(temp_cache),
                  use_virtual_images
              );
          })
        | std::ranges::to<std::vector>()
    };

    return std::packaged_task{ [global_descriptor_set_layout =
                                    std::move(global_descriptor_set_layout),
                                model_descriptor_set_layouts =
                                    std::move(model_descriptor_set_layouts),
                                pipeline_layout       = std::move(pipeline_layout),
                                descriptor_pool       = std::move(descriptor_pool),
                                global_buffer         = std::move(global_buffer),
                                global_descriptor_set = std::move(global_descriptor_set),
                                model_loaders         = std::move(model_loaders)](
                                   const vk::CommandBuffer transfer_command_buffer
                               ) mutable -> Scene {
        return Scene{
            std::move(global_descriptor_set_layout),
            std::move(model_descriptor_set_layouts),
            std::move(pipeline_layout),
            std::move(descriptor_pool),
            std::move(global_buffer),
            std::move(global_descriptor_set),
            model_loaders
                | std::views::transform(
                    [transfer_command_buffer](
                        std::packaged_task<gltf::RenderModel(vk::CommandBuffer)>& model_task
                    ) {
                        std::invoke(model_task, transfer_command_buffer);
                        return model_task.get_future().get();
                    }
                )
                | std::ranges::to<std::vector>()
        };
    } };
}

}   // namespace ddge::renderer
