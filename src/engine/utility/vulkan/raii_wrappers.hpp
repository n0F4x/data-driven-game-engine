#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

namespace engine::utils::vulkan {

template <typename Handle, typename Owner = vk::Device>
class Wrapper {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Wrapper(Owner t_owner, Handle t_handle)
        : m_owner{ t_owner },
          m_handle{ t_handle }
    {}

    Wrapper(Wrapper&& t_other) noexcept
        : m_owner{ t_other.m_owner },
          m_handle{ t_other.m_handle }
    {
        t_other.m_handle = nullptr;
    }

    ~Wrapper()
    {
        if (m_handle) {
            m_owner.destroy(m_handle);
        }
    }

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(Wrapper&&) noexcept -> Wrapper& = default;

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

using Buffer       = Wrapper<vk::Buffer>;
using CommandPool  = Wrapper<vk::CommandPool>;
using Fence        = Wrapper<vk::Fence>;
using Framebuffer  = Wrapper<vk::Framebuffer>;
using ShaderModule = Wrapper<vk::ShaderModule>;
using Surface      = Wrapper<vk::SurfaceKHR, vk::Instance>;

}   // namespace engine::utils::vulkan
