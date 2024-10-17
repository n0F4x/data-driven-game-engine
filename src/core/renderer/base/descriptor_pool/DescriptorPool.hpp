#pragma once

#include <vulkan/vulkan.hpp>

namespace core::renderer::base {

class DescriptorPool {
public:
    class Builder;

    [[nodiscard]]
    static auto create() noexcept -> Builder;

    explicit DescriptorPool(vk::UniqueDescriptorPool&& descriptor_pool) noexcept;

    [[nodiscard]]
    auto get() const noexcept -> vk::DescriptorPool;

private:
    vk::UniqueDescriptorPool m_descriptor_pool;
};

}   // namespace core::renderer
