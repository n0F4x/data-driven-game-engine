#pragma once

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class Fence {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Fence(vk::Device t_device, vk::Fence t_fence) noexcept;
    Fence(Fence&&) noexcept;
    ~Fence() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Fence&&) noexcept -> Fence& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Fence;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::Device m_device;
    vk::Fence  m_fence;
};

}   // namespace engine::vulkan
