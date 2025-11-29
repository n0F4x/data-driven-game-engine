module;

#include <concepts>
#include <cstdint>

export module ddge.utility.memory.Deallocator;

import ddge.utility.meta.concepts.allocator;
import ddge.utility.meta.concepts.generic_allocator;

namespace ddge::util {

export template <typename Allocator_T>
    requires(meta::generic_allocator_c<Allocator_T> || meta::allocator_c<Allocator_T>)
class Deallocator {
public:
    template <typename UAllocator_T>
        requires std::constructible_from<Allocator_T, UAllocator_T&&>
    constexpr explicit Deallocator(UAllocator_T&& allocator);

    template <typename T>
    constexpr auto operator()(T* pointer, std::size_t n = 1) -> void;

private:
    Allocator_T m_allocator;
};

}   // namespace ddge::util

template <typename Allocator_T>
    requires(
        ddge::util::meta::generic_allocator_c<Allocator_T>
        || ddge::util::meta::allocator_c<Allocator_T>
    )
template <typename UAllocator_T>
    requires std::constructible_from<Allocator_T, UAllocator_T&&>
constexpr ddge::util::Deallocator<Allocator_T>::Deallocator(UAllocator_T&& allocator)
    : m_allocator{ std::forward<UAllocator_T>(allocator) }
{}

template <typename Allocator_T>
    requires(
        ddge::util::meta::generic_allocator_c<Allocator_T>
        || ddge::util::meta::allocator_c<Allocator_T>
    )
template <typename T>
constexpr auto ddge::util::Deallocator<Allocator_T>::operator()(
    T* const          pointer,
    const std::size_t n
) -> void
{
    m_allocator.deallocate(pointer, n);
}
