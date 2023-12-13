#include "Instance.hpp"

#include <utility>

namespace engine::vulkan {

///////////////////////////////////
///-----------------------------///
///  Instance   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

Instance::Instance(vk::Instance t_instance) noexcept : m_instance{ t_instance }
{}

Instance::Instance(Instance&& t_other) noexcept
    : m_instance{ std::exchange(t_other.m_instance, nullptr) }
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
    m_instance.destroy();
    m_instance = nullptr;
}

}   // namespace engine::vulkan
