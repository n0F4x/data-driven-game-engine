module;

#include "utility/lifetime_bound.hpp"

export module ddge.modules.renderer.base.resources.Buffer;

import vulkan_hpp;

namespace ddge::renderer::base {

export class Buffer {
public:
    Buffer() = default;
    Buffer(vk::UniqueBuffer&& buffer, vk::DeviceSize buffer_size) noexcept;

    [[nodiscard]]
    auto get() const [[lifetime_bound]] -> vk::Buffer;

    [[nodiscard]]
    auto size() const -> vk::DeviceSize;

    auto reset() -> void;

private:
    vk::UniqueBuffer m_buffer;
    vk::DeviceSize   m_size{};
};

}   // namespace ddge::renderer::base
