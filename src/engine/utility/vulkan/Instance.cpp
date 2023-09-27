#include "Instance.hpp"

#include "helpers.hpp"

namespace engine::utils::vulkan {

///////////////////////////////////
///-----------------------------///
///  Instance   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

Instance::Instance(
    vk::Instance               t_instance,
    vk::DebugUtilsMessengerEXT t_debug_messenger
) noexcept
    : m_instance{ t_instance },
      m_debug_messenger{ t_debug_messenger }
{}

Instance::Instance(Instance&& t_other) noexcept
    : m_instance{ t_other.m_instance },
      m_debug_messenger{ t_other.m_debug_messenger }
{
    t_other.m_instance        = nullptr;
    t_other.m_debug_messenger = nullptr;
}

Instance::~Instance() noexcept
{
    if (m_instance) {
        if (m_debug_messenger) {
            m_instance.destroy(m_debug_messenger);
        }
        m_instance.destroy();
    }
}

auto Instance::operator*() const noexcept -> vk::Instance
{
    return m_instance;
}

auto Instance::operator->() const noexcept -> const vk::Instance*
{
    return &m_instance;
}

}   // namespace engine::utils::vulkan
