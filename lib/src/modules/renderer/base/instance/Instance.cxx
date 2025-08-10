module;


#include <VkBootstrap.h>

module ddge.modules.renderer.base.instance.Instance;

namespace ddge::renderer::base {

Instance::Instance(const vkb::Instance& instance) noexcept : m_instance{ instance } {}

Instance::Instance(Instance&& other) noexcept
    : m_instance{ std::exchange(other.m_instance, {}) }
{}

Instance::~Instance()
{
    vkb::destroy_instance(m_instance);
}

auto Instance::operator*() const noexcept -> vk::Instance
{
    return m_instance.instance;
}

auto Instance::operator->() const noexcept -> const vk::Instance*
{
    return reinterpret_cast<const vk::Instance*>(&m_instance.instance);
}

auto Instance::get() const noexcept -> vk::Instance
{
    return m_instance.instance;
}

auto Instance::debug_messenger() const noexcept -> vk::DebugUtilsMessengerEXT
{
    return m_instance.debug_messenger;
}

Instance::operator vkb::Instance() const noexcept
{
    return m_instance;
}

}   // namespace ddge::renderer::base
