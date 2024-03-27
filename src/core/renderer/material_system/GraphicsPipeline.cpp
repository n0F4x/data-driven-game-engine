#include "GraphicsPipeline.hpp"

#include "GraphicsPipelineBuilder.hpp"

namespace core::renderer {

auto GraphicsPipeline::hash(const GraphicsPipelineBuilder& t_builder) noexcept -> size_t
{
    return hash_value(t_builder);
}

GraphicsPipeline::GraphicsPipeline(
    const vk::Device                      t_device,
    const vk::GraphicsPipelineCreateInfo& t_create_info
)
    : m_pipeline{ t_device.createGraphicsPipelineUnique(nullptr, t_create_info).value }
{}

auto GraphicsPipeline::get() const noexcept -> vk::Pipeline
{
    return m_pipeline.get();
}

}   // namespace core::renderer
