#pragma once

#include <vulkan/vulkan.hpp>

#include "core/utility/lifetime_bound.hpp"

namespace core::renderer::base {

class Allocator;

class Buffer {
public:
    Buffer() = default;

    [[nodiscard]]
    auto get() const [[lifetime_bound]] -> vk::Buffer;

    [[nodiscard]]
    auto size() const -> vk::DeviceSize;

    auto reset() -> void;

private:
    friend Allocator;

    vk::UniqueBuffer m_buffer;
    vk::DeviceSize   m_size{};

    Buffer(vk::UniqueBuffer&& buffer, vk::DeviceSize buffer_size) noexcept;
};

}   // namespace core::renderer::base
