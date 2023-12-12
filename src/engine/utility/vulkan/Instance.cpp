#include "Instance.hpp"

#include <utility>

namespace engine::vulkan {

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
    : m_instance{ std::exchange(t_other.m_instance, nullptr) },
      m_debug_messenger{ std::exchange(t_other.m_debug_messenger, nullptr) }
{}

Instance::~Instance() noexcept
{
    destroy();
}

auto Instance::operator=(Instance&& t_other) noexcept -> Instance&
{
    if (this != &t_other) {
        destroy();

        std::swap(m_instance, t_other.m_instance);
        std::swap(m_debug_messenger, t_other.m_debug_messenger);
    }
    return *this;
}

auto Instance::operator*() const noexcept -> vk::Instance
{
    return m_instance;
}

auto Instance::operator->() const noexcept -> const vk::Instance*
{
    return &m_instance;
}

auto Instance::destroy() noexcept -> void
{
    if (m_instance) {
        if (m_debug_messenger) {
            m_instance.destroy(m_debug_messenger);
        }
        m_instance.destroy();
    }
    m_debug_messenger = nullptr;
    m_instance        = nullptr;
}

}   // namespace engine::vulkan
