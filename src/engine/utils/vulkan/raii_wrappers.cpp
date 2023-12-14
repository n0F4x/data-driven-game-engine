module;

#include <utility>

#include <vulkan/vulkan.hpp>

module engine.utils.vulkan.raii_wrappers;

namespace engine::vulkan {

template <typename Handle, typename Owner>
Wrapped<Handle, Owner>::Wrapped(Owner t_owner, Handle t_handle)
    : m_owner{ t_owner },
      m_handle{ t_handle }
{}

template <typename Handle, typename Owner>
Wrapped<Handle, Owner>::Wrapped(Wrapped&& t_other) noexcept
    : m_owner{ std::exchange(t_other.m_owner, nullptr) },
      m_handle{ std::exchange(t_other.m_handle, nullptr) }
{}

template <typename Handle, typename Owner>
Wrapped<Handle, Owner>::~Wrapped()
{
    destroy();
}

template <typename Handle, typename Owner>
auto Wrapped<Handle, Owner>::operator=(Wrapped&& t_other) noexcept -> Wrapped&
{
    if (this != &t_other) {
        destroy();

        std::swap(m_owner, t_other.m_owner);
        std::swap(m_handle, t_other.m_handle);
    }
    return *this;
}

template <typename Handle, typename Owner>
[[nodiscard]] auto Wrapped<Handle, Owner>::operator*() const noexcept -> Handle
{
    return m_handle;
}

template <typename Handle, typename Owner>
auto Wrapped<Handle, Owner>::destroy() noexcept -> void
{
    if (m_handle) {
        m_owner.destroy(m_handle);
    }
    m_handle = nullptr;
    m_owner  = nullptr;
}

}   // namespace engine::vulkan
