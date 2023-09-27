#include "ShaderModule.hpp"

namespace engine::utils::vulkan {

////////////////////////////////
///--------------------------///
///  Fence   IMPLEMENTATION  ///
///--------------------------///
////////////////////////////////

ShaderModule::ShaderModule(
    vk::Device       t_device,
    vk::ShaderModule t_shader_module
) noexcept
    : m_device{ t_device },
      m_shader_module{ t_shader_module }
{}

ShaderModule::ShaderModule(ShaderModule&& t_other) noexcept
    : m_device{ t_other.m_device },
      m_shader_module{ t_other.m_shader_module }
{
    t_other.m_shader_module = nullptr;
}

ShaderModule::~ShaderModule() noexcept
{
    if (m_shader_module) {
        m_device.destroy(m_shader_module);
    }
}

auto ShaderModule::operator*() const noexcept -> vk::ShaderModule
{
    return m_shader_module;
}

}   // namespace engine::utils::vulkan
