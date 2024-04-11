#pragma once

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include "core/cache/Cache.hpp"
#include "core/renderer/base/allocator/Allocator.hpp"
#include "core/renderer/material_system/VertexInputStateBuilder.hpp"

#include "Model.hpp"
#include "RenderModel.hpp"

namespace core::renderer {

class RenderModelLoader {
public:
    explicit RenderModelLoader(
        vk::Device           t_device,
        const Allocator&     t_allocator,
        tl::optional<cache::Cache&> t_cache = {}
    ) noexcept;

    [[nodiscard]] auto load(
        const Model&                   t_model,
        vk::CommandBuffer              t_transfer_command_buffer,
        const VertexInputStateBuilder& t_vertex_input_state
    ) -> tl::optional<RenderModel>;

private:
    vk::Device                              m_device;
    std::reference_wrapper<const Allocator> m_allocator;
    tl::optional<cache::Cache&>                    m_cache;
};

}   // namespace core::renderer
