#pragma once

#include <functional>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <fastgltf/core.hpp>

#include "engine/common/Cache.hpp"
#include "engine/renderer/base/Allocator.hpp"

#include "RenderModel.hpp"

namespace engine::renderer {

class RenderModelLoader {
public:
    using Model = fastgltf::Asset;

    explicit RenderModelLoader(
        vk::Device           t_device,
        const Allocator&     t_allocator,
        tl::optional<Cache&> t_cache = {}
    ) noexcept;

    [[nodiscard]] auto
        load(const Model& t_model, vk::CommandBuffer t_transfer_command_buffer)
            -> tl::optional<RenderModel>;

private:
    vk::Device                              m_device;
    std::reference_wrapper<const Allocator> m_allocator;
    tl::optional<Cache&>                    m_cache;
};

}   // namespace engine::renderer
