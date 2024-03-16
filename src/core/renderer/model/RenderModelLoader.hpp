#pragma once

#include <functional>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include "core/common/Cache.hpp"
#include "core/renderer/base/Allocator.hpp"

#include "Model.hpp"
#include "RenderModel.hpp"

namespace core::renderer {

class RenderModelLoader {
public:
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

}   // namespace core::renderer
