module;

#include <concepts>

export module utility.memory.Deallocator;

import utility.meta.concepts.allocator;

namespace util {

export template <::util::meta::allocator_c Allocator_T>
class Deallocator {
public:
    template <typename UAllocator_T>
        requires std::constructible_from<Allocator_T, UAllocator_T>
    constexpr explicit Deallocator(UAllocator_T&& allocator);

    template <typename T>
    constexpr auto operator()(T* pointer) -> void;

private:
    Allocator_T m_allocator;
};

}   // namespace util

template <::util::meta::allocator_c Allocator_T>
template <typename UAllocator_T>
    requires std::constructible_from<Allocator_T, UAllocator_T>
constexpr util::Deallocator<Allocator_T>::Deallocator(UAllocator_T&& allocator)
    : m_allocator{ std::forward<UAllocator_T>(allocator) }
{}

template <::util::meta::allocator_c Allocator_T>
template <typename T>
constexpr auto util::Deallocator<Allocator_T>::operator()(T* pointer) -> void
{
    m_allocator.deallocate(pointer);
}
