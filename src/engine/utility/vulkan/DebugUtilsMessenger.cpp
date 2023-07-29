#include "DebugUtilsMessenger.hpp"

namespace engine::vulkan {

//////////////////////////////////////////////
///----------------------------------------///
///  DebugUtilsMessenger   IMPLEMENTATION  ///
///----------------------------------------///
//////////////////////////////////////////////

DebugUtilsMessenger::DebugUtilsMessenger(
    vk::Instance               t_instance,
    vk::DebugUtilsMessengerEXT t_messenger
) noexcept
    : m_instance{ t_instance },
      m_messenger{ t_messenger }
{}

DebugUtilsMessenger::DebugUtilsMessenger(DebugUtilsMessenger&& t_other) noexcept
    : m_instance{ t_other.m_instance },
      m_messenger{ t_other.m_messenger }
{
    t_other.m_messenger = nullptr;
}

DebugUtilsMessenger::~DebugUtilsMessenger() noexcept
{
    if (m_messenger) {
        m_instance.destroy(m_messenger);
    }
}

auto DebugUtilsMessenger::operator*() const noexcept
    -> vk::DebugUtilsMessengerEXT
{
    return m_messenger;
}

}   // namespace engine::vulkan
