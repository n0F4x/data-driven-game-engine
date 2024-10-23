#include "Scene.hpp"

#include "core/renderer/base/descriptor_pool/Builder.hpp"

#include "Builder.hpp"

namespace core::renderer {

auto Scene::create() noexcept -> Builder
{
    return Builder{};
}

auto Scene::draw(
    const vk::CommandBuffer graphics_command_buffer,
    const gfx::Camera& camera
) const -> void
{
    m_global_buffer.set(ShaderScene{
        .camera = ShaderScene::Camera{ .position   = glm::vec4{ camera.position(), 1 },
                                      .view       = camera.view(),
                                      .projection = camera.projection() },
    });
    graphics_command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_pipeline_layout.get(),
        0,
        m_global_descriptor_set.get(),
        nullptr
    );

    for (const auto& model : m_models) {
        model.draw(graphics_command_buffer, m_pipeline_layout.get());
    }
}

Scene::Scene(
    vk::UniqueDescriptorSetLayout&&                global_descriptor_set_layout,
    std::array<vk::UniqueDescriptorSetLayout, 3>&& model_descriptor_set_layouts,
    vk::UniquePipelineLayout&&                     pipeline_layout,
    base::DescriptorPool&&                               descriptor_pool,
    resources::RandomAccessBuffer<ShaderScene>&&                    global_buffer,
    vk::UniqueDescriptorSet&&                      global_descriptor_set,
    std::vector<gltf::RenderModel>&&               models
) noexcept
    : m_global_descriptor_set_layout(std::move(global_descriptor_set_layout)),
      m_model_descriptor_set_layouts(std::move(model_descriptor_set_layouts)),
      m_pipeline_layout{ std::move(pipeline_layout) },
      m_descriptor_pool{ std::move(descriptor_pool) },
      m_global_buffer{ std::move(global_buffer) },
      m_global_descriptor_set{ std::move(global_descriptor_set) },
      m_models(std::move(models))
{}

}   // namespace core::renderer
