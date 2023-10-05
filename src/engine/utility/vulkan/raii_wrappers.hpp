#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

namespace engine::utils::vulkan {

template <typename Handle, typename Owner = vk::Device>
class Wrapped {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Wrapped(Owner t_owner, Handle t_handle)
        : m_owner{ t_owner },
          m_handle{ t_handle }
    {}

    Wrapped(Wrapped&& t_other) noexcept
        : m_owner{ t_other.m_owner },
          m_handle{ t_other.m_handle }
    {
        t_other.m_handle = nullptr;
    }

    ~Wrapped()
    {
        if (m_handle) {
            m_owner.destroy(m_handle);
        }
    }

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(Wrapped&&) noexcept -> Wrapped& = default;

    [[nodiscard]] auto operator*() const noexcept -> Handle
    {
        return m_handle;
    }

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Owner  m_owner;
    Handle m_handle;
};

using Buffer       = Wrapped<vk::Buffer>;
using CommandPool  = Wrapped<vk::CommandPool>;
using Fence        = Wrapped<vk::Fence>;
using Framebuffer  = Wrapped<vk::Framebuffer>;
using ShaderModule = Wrapped<vk::ShaderModule>;
using Surface      = Wrapped<vk::SurfaceKHR, vk::Instance>;

}   // namespace engine::utils::vulkan
