#pragma once

#include <vulkan/vulkan.hpp>

#include "raii_wrappers-Device.hpp"
#include "raii_wrappers-Instance.hpp"

namespace engine::vulkan {

template <typename Handle, typename Owner = vk::Device>
class Wrapped {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Wrapped(Owner t_owner, Handle t_handle);
    Wrapped(Wrapped&& t_other) noexcept;
    ~Wrapped();

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Wrapped&& t_other) noexcept -> Wrapped&;
    [[nodiscard]] auto operator*() const noexcept -> Handle;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto destroy() noexcept -> void;

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
using Swapchain           = Wrapped<vk::SwapchainKHR>;
using RenderPass          = Wrapped<vk::RenderPass>;

}   // namespace engine::vulkan

#include "raii_wrappers.inl"
