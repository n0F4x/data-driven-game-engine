#include "Scene.hpp"

#include "core/renderer/base/descriptor_pool/Builder.hpp"

#include "Builder.hpp"

namespace core::renderer {

auto Scene::create() noexcept -> Builder
{
    return Builder{};
}

auto Scene::draw(
    vk::CommandBuffer       t_graphics_command_buffer,
    const graphics::Camera& t_camera
) const -> void
{
    m_global_buffer.set(GlobalUniformBlock{ .camera = t_camera });

    for (const auto& model : m_models) {
        model.draw(t_graphics_command_buffer, m_pipeline_layout.get());
    }
}

Scene::Scene(
    vk::UniqueDescriptorSetLayout&& t_global_descriptor_set_layout,
    vk::UniqueDescriptorSetLayout&& t_model_descriptor_set_layout,
    vk::UniquePipelineLayout&&      t_pipeline_layout,
    DescriptorPool&&                t_descriptor_pool,
    MappedBuffer&&                  t_global_buffer,
    vk::UniqueDescriptorSet&&       t_global_descriptor_set,
    std::vector<RenderModel2>&&     t_models
) noexcept
    : m_global_descriptor_set_layout(std::move(t_global_descriptor_set_layout)),
      m_model_descriptor_set_layout(std::move(t_model_descriptor_set_layout)),
      m_pipeline_layout{ std::move(t_pipeline_layout) },
      m_descriptor_pool{ std::move(t_descriptor_pool) },
      m_global_buffer{ std::move(t_global_buffer) },
      m_global_descriptor_set{ std::move(t_global_descriptor_set) },
      m_models(std::move(t_models))
{}

}   // namespace core::renderer
