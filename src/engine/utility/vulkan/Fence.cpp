#include "Fence.hpp"

namespace engine::utils::vulkan {

////////////////////////////////
///--------------------------///
///  Fence   IMPLEMENTATION  ///
///--------------------------///
////////////////////////////////

Fence::Fence(vk::Device t_device, vk::Fence t_fence) noexcept
    : m_device{ t_device },
      m_fence{ t_fence }
{}

Fence::Fence(Fence&& t_other) noexcept
    : m_device{ t_other.m_device },
      m_fence{ t_other.m_fence }
{
    t_other.m_fence = nullptr;
}

Fence::~Fence() noexcept
{
    if (m_fence) {
        m_device.destroy(m_fence);
    }
}

auto Fence::operator*() const noexcept -> vk::Fence
{
    return m_fence;
}

}   // namespace engine::utils::vulkan
