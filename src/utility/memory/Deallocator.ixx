module;

#include <concepts>

export module utility.memory.Deallocator;

import utility.meta.concepts.allocator;

namespace util {

export template <typename Allocator_T>
    requires(::util::meta::generic_allocator_c<Allocator_T> || ::util::meta::allocator_c<Allocator_T>)
class Deallocator {
public:
    template <typename UAllocator_T>
        requires std::constructible_from<Allocator_T, UAllocator_T>
    constexpr explicit Deallocator(UAllocator_T&& allocator);

    template <typename T>
    constexpr auto operator()(T* pointer, size_t n = 1) -> void;

private:
    Allocator_T m_allocator;
};

}   // namespace util

template <typename Allocator_T>
    requires(::util::meta::generic_allocator_c<Allocator_T> || ::util::meta::allocator_c<Allocator_T>)
template <typename UAllocator_T>
    requires std::constructible_from<Allocator_T, UAllocator_T>
constexpr util::Deallocator<Allocator_T>::Deallocator(UAllocator_T&& allocator)
    : m_allocator{ std::forward<UAllocator_T>(allocator) }
{}

template <typename Allocator_T>
    requires(::util::meta::generic_allocator_c<Allocator_T> || ::util::meta::allocator_c<Allocator_T>)
template <typename T>
constexpr auto util::Deallocator<Allocator_T>::operator()(T* const pointer, const size_t n)
    -> void
{
    m_allocator.deallocate(pointer, n);
}
