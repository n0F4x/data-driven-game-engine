#pragma once

#include <utility>

#include <vulkan/vulkan.hpp>

#include "Device.hpp"
#include "Instance.hpp"

namespace engine::vulkan {

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
        : m_owner{ std::exchange(t_other.m_owner, nullptr) },
          m_handle{ std::exchange(t_other.m_handle, nullptr) }
    {}

    ~Wrapped()
    {
        destroy();
    }

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(Wrapped&& t_other) noexcept -> Wrapped&
    {
        if (this != &t_other) {
            destroy();

            std::swap(m_owner, t_other.m_owner);
            std::swap(m_handle, t_other.m_handle);
        }
        return *this;
    }

    [[nodiscard]] auto operator*() const noexcept -> Handle
    {
        return m_handle;
    }

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto destroy() noexcept -> void
    {
        if (m_handle) {
            m_owner.destroy(m_handle);
        }
        m_handle = nullptr;
        m_owner  = nullptr;
    }

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Owner  m_owner;
    Handle m_handle;
};

using CommandPool         = Wrapped<vk::CommandPool>;
using DebugUtilsMessenger = Wrapped<vk::DebugUtilsMessengerEXT, vk::Instance>;
using DescriptorPool      = Wrapped<vk::DescriptorPool>;
using DescriptorSetLayout = Wrapped<vk::DescriptorSetLayout>;
using Fence               = Wrapped<vk::Fence>;
using Framebuffer         = Wrapped<vk::Framebuffer>;
using Image               = Wrapped<vk::Image>;
using ImageView           = Wrapped<vk::ImageView>;
using Pipeline            = Wrapped<vk::Pipeline>;
using PipelineLayout      = Wrapped<vk::PipelineLayout>;
using Semaphore           = Wrapped<vk::Semaphore>;
using ShaderModule        = Wrapped<vk::ShaderModule>;
using Surface             = Wrapped<vk::SurfaceKHR, vk::Instance>;
using RenderPass          = Wrapped<vk::RenderPass>;

}   // namespace engine::vulkan
