#pragma once

#include <vulkan/vulkan.hpp>

class GraphicsPipelineBuilder {
public:
    [[nodiscard]] auto build() -> vk::UniquePipeline;
};
