#include "Allocator.hpp"

namespace core::vma {

Allocator::Allocator(const VmaAllocator t_allocator) noexcept : m_allocator{ t_allocator }
{}

Allocator::Allocator(Allocator&& t_other) noexcept
    : Allocator{ std::exchange(t_other.m_allocator, nullptr) }
{}

Allocator::~Allocator() noexcept
{
    if (m_allocator != nullptr) {
        vmaDestroyAllocator(m_allocator);
    }
}

auto Allocator::operator*() const noexcept -> VmaAllocator
{
    return m_allocator;
}

auto Allocator::operator->() const noexcept -> const VmaAllocator*
{
    return &m_allocator;
}

auto Allocator::get() const noexcept -> VmaAllocator
{
    return m_allocator;
}

}   // namespace core::vma
