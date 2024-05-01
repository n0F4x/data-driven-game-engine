#pragma once

#include <vulkan/vulkan.hpp>

namespace core::renderer {

class GraphicsPipelineBuilder;

class GraphicsPipeline {
public:
    [[nodiscard]]
    static auto hash(const GraphicsPipelineBuilder& t_builder) noexcept -> size_t;

    explicit GraphicsPipeline(
        vk::Device                            t_device,
        const vk::GraphicsPipelineCreateInfo& t_create_info
    );

    [[nodiscard]]
    auto get() const noexcept -> vk::Pipeline;

private:
    vk::UniquePipeline m_pipeline;
};

}   // namespace core::renderer
