module;

#include <future>
#include <optional>

#include <glm/ext/matrix_float4x4.hpp>

export module core.renderer.scene.Scene;

import vulkan_hpp;

import core.cache.Cache;
import core.cache.Handle;

import core.gfx.Camera;
import core.gltf.Model;
import core.gltf.RenderModel;

import core.renderer.base.device.Device;
import core.renderer.base.allocator.Allocator;
import core.renderer.base.descriptor_pool.DescriptorPool;
import core.renderer.resources.RandomAccessBuffer;

namespace core::renderer {

export class Scene {
public:
    class Builder;

    [[nodiscard]]
    static auto create() noexcept -> Builder;

    auto update(
        const gfx::Camera&               camera,
        const renderer::base::Allocator& allocator,
        vk::Queue                        sparse_queue,
        vk::CommandBuffer                transfer_command_buffer
    ) -> void;

    auto draw(vk::CommandBuffer graphics_command_buffer, const gfx::Camera& camera) const
        -> void;

private:
    struct ShaderScene {
        struct Camera {
            glm::vec4 position;
            glm::mat4 view;
            glm::mat4 projection;
        };

        Camera camera;
    };

    friend Builder;

    vk::UniqueDescriptorSetLayout                m_global_descriptor_set_layout;
    std::array<vk::UniqueDescriptorSetLayout, 3> m_model_descriptor_set_layouts;
    vk::UniquePipelineLayout                     m_pipeline_layout;
    base::DescriptorPool                         m_descriptor_pool;

    resources::RandomAccessBuffer<ShaderScene> m_global_buffer;
    vk::UniqueDescriptorSet                    m_global_descriptor_set;

    std::vector<gltf::RenderModel> m_models;

    explicit Scene(
        vk::UniqueDescriptorSetLayout&&                global_descriptor_set_layout,
        std::array<vk::UniqueDescriptorSetLayout, 3>&& model_descriptor_set_layouts,
        vk::UniquePipelineLayout&&                     pipeline_layout,
        base::DescriptorPool&&                         descriptor_pool,
        resources::RandomAccessBuffer<ShaderScene>&&   global_buffer,
        vk::UniqueDescriptorSet&&                      global_descriptor_set,
        std::vector<gltf::RenderModel>&&               models
    ) noexcept;
};

class Scene::Builder {
public:
    auto set_cache(cache::Cache& cache) noexcept -> Builder&;

    auto add_model(const cache::Handle<const gltf::Model>& model) -> Builder&;
    auto add_model(cache::Handle<const gltf::Model>&& model) -> Builder&;

    [[nodiscard]]
    auto build(
        const base::Device&    device,
        const base::Allocator& allocator,
        vk::RenderPass         render_pass,
        bool                   use_virtual_images
    ) const -> std::packaged_task<Scene(vk::CommandBuffer)>;

private:
    std::optional<std::reference_wrapper<cache::Cache>> m_cache;
    std::vector<cache::Handle<const gltf::Model>>       m_models;
};

}   // namespace core::renderer
